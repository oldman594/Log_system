/*实现异步工作器*/
#ifndef __M_LOOPER_H__
#define __M_LOOPER_H__

#include "buffer.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <memory>
#include <atomic>

namespace zkjlog
{
    using Functor = std::function<void(Buffer &)>;
    enum class AsyncType{
        ASYNC_SAFE,//安全状态，表示缓冲区满了则阻塞，避免资源耗尽的风险
        ASUNC_UNSAFE//不考虑资源耗尽的问题，无限扩容，常用于测试
    };
    class AsyncLooper
    {
    public:
        using ptr = std::shared_ptr<AsyncLooper>;
        AsyncLooper(const Functor &cb,AsyncType loop_type = AsyncType::ASYNC_SAFE) :_looper_type(loop_type), _stop(false), _thread(std::thread(&AsyncLooper::threadEntry, this)),_callBack(cb) {}
        ~AsyncLooper(){stop();}
        void stop()
        {
            _stop = true;//将退出标志设置为true
            _cond_con.notify_all();//唤醒所有的工作线程
            _thread.join();//等待工作线程的退出
        }
        void push(const char *data, size_t len)
        {
            //1.无限扩容-非安全   2.固定大小--生产缓冲区中数据满了就阻塞
            std::unique_lock<std::mutex> lock(_mutex);
            //条件变量控制，若缓冲区剩余空间的大小大于数据长度，则可以添加数据
            if(_looper_type == AsyncType::ASYNC_SAFE)
                _cond_pro.wait(lock,[&](){return _pro_buf.writeABleSize() >= len;});
            //能够走下来代表满足了条件，可以向缓冲区添加数据
            _pro_buf.push(data,len);
            //唤醒消费者对缓冲区中的数据进行处理
            _cond_con.notify_one();
        }

    private:
        //线程入口函数--对消费缓冲区中的数据进行处理，处理完毕后，初始化缓冲区，交换缓冲区
        void threadEntry() // 线程入口函数
        {
            while(!_stop)
            {
                //1.判断生产缓冲区有没有数据，有则交换，无则阻塞
                {
                    //加这个中括号的目的让lock的生命周期在中括号内
                    //当缓冲区交换完毕后就解锁（并不对数据的处理过程加锁保护）
                    std::unique_lock<std::mutex> lock(_mutex);
                    if(_stop || _pro_buf.empty()) break;
                    //若当前是推出前被唤醒，或者有数据被唤醒，则返回真，继续向下运行，否则重新陷入休眠
                    _cond_con.wait(lock,[&](){return _stop||!_pro_buf.empty();});
                    _con_buf.swap(_pro_buf);
                    //唤醒生产者
                    if(_looper_type == AsyncType::ASYNC_SAFE)
                        _cond_pro.notify_all();
                }
                //2.被唤醒后，对消费缓冲区进行数据处理
                _callBack(_con_buf);
                //3.初始化消费缓冲区
                _con_buf.reset();
            }
        }
    private:
        Functor _callBack; // 具体对缓冲区数据进行处理的回调函数，由异步工作器的使用者传入
    private:
        AsyncType _looper_type;  //
        std::atomic<bool> _stop; // 工作器停止标志
        Buffer _pro_buf;         // 生产缓冲区
        Buffer _con_buf;         // 消费缓冲区
        std::mutex _mutex;
        std::condition_variable _cond_pro;
        std::condition_variable _cond_con;
        std::thread _thread; // 异步日志器对应的工作线程
    };
}

#endif