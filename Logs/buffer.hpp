/*实现异步日志缓冲区*/
#include "util.hpp"
#include <vector>
#include <cassert>
namespace zkjlog
{
    #define DEFAULT_BUFFER_SIZE (10*1024*1024)
    #define THRESHOLD_BUFFER_SIZE (100*1024*1024)
    #define INCREMENT_BUFFER_SIZE (10*1024*1024)
    class Buffer{
    public:
        Buffer():_buffer(DEFAULT_BUFFER_SIZE),_writer_idx(0),_reader_idx(0){}
        //向缓冲区写入数据
        void push(const char* data,size_t len)
        {
            //缓冲区剩余空间不足的情况：扩容
            //1.固定大小，则直接返回
            // if(len>writeABleSize()) return ; 
            //2.动态空间，用于极限性能测试--扩容
            // if(len > writeABleSize()) _buffer.resize(_buffer.size()+len);
            ensureEnoughSize(len);
            //1.将数据拷贝进缓冲区
            std::copy(data,data+len,&_buffer);
            //2.将当前写入位置向后偏移
            moveWriter(len);
        }
        //返回可读数据的起始地址
        const char* begin(){return &_buffer[_reader_idx];}
        //返回可读数据的长度
        size_t readAbleSize(){
            //当前缓冲区的设计思想是双缓冲区，处理完就交换，所以不存在空间的循环使用
            return (_reader_idx-_writer_idx);
            }
        //对读写指针进行向后偏移操作
        void moveWriter(size_t len)
        {
            assert((len+_writer_idx) <= _buffer.size());
            _writer_idx+=len;
        }
        void moveReader(size_t len)
        {
            assert(len <= readAbleSize());
            _reader_idx += len;
        }
        //重置读写位置，初始化缓冲区
        void reset()
        {
            _writer_idx = 0;//缓冲区所有空间都是空闲的
            _reader_idx = 0;//与_writer_idx相同表示没有数据可读
        }
        //对Buffer实现交换操作
        void swap(Buffer& buffer)
        {
            _buffer.swap(buffer._buffer);
            std::swap(_writer_idx,buffer._writer_idx);
            std::swap(_reader_idx,buffer._reader_idx);
        }
        //判断缓冲区是否为空
        bool empty(){return _writer_idx == _reader_idx;}
         //缓冲区剩余大小即可写空间大小
        size_t writeABleSize(){return (_buffer.size() - _writer_idx);}
    private:
        //对空间进行扩容
        void ensureEnoughSize(size_t len){
            if(len<=writeABleSize()) return ;//不需要扩容
            size_t new_size = 0;
            if(_buffer.size() < THRESHOLD_BUFFER_SIZE) new_size = _buffer.size()*2;//小于阈值翻倍增长
            else new_size = _buffer.size()+INCREMENT_BUFFER_SIZE;
            _buffer.resize(new_size);
        }
       
    private:
        std::vector<char> _buffer;
        size_t _reader_idx; //当前可读数据的指针--本质是下标
        size_t _writer_idx; //当前可读数据的指针
    };
}