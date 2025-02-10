#ifndef __MY_SING__
#define __MY_SING__
// #pragma once

#include "util.hpp"
#include <string>
#include <sstream>
#include <memory>
#include <utility>
#include <fstream>
#include <ctime>
#include <cassert>

namespace zkjlog
{
    class LogSink
    {
    public:
        using ptr = std::shared_ptr<LogSink>;
        virtual void log(const char *data, size_t len) = 0;
        virtual ~LogSink() {}
        LogSink() {}
    };
    // 标准输出
    class StdoutSink : public LogSink
    {
    public:
        void log(const char *data, size_t len)
        {
            std::cout.write(data, len);
        }
    };

    // 固定文件输出
    class FileSink : public LogSink
    {
    public:
        // 构造函数需要完成通过文件名打开文件并保存文件的句柄
        FileSink(const std::string &pathname)
            : _pathname(pathname)
        {
            _ofs.open(_pathname, std::ios::binary | std::ios::app);
            assert(_ofs);
        }
        // 完成向指定文件写入
        void log(const char *data, size_t len)
        {
            _ofs.write(data, len);
            assert(_ofs.good());
        }

    private:
        std::string _pathname;
        std::ofstream _ofs;
    };

    // 滚动文件输出
    class RollSink : public LogSink
    {
    public:
        RollSink(const std::string &basename, size_t file_size_max)
            : _basename(basename), _file_size_max(file_size_max), _name_cunt(0)
        {
            std::string filename = CreateNewFile();
            util::File::create_directory(_basename); // 创建对应的目录
            _ofs.open(filename, std::ios::app | std::ios::binary);
            assert(_ofs.is_open());
        }
        // 判断文件的大小，并判断是否创建新文件
        void log(const char *data, size_t len)
        {
            if (_cur_fsize + len >= _file_size_max)
            {
                _cur_fsize = 0;
                std::string filename = CreateNewFile();
                _ofs.close();
                _ofs.open(filename, std::ios::binary | std::ios::app);
                assert(_ofs.is_open());
            }
            _ofs.write(data, len);
            assert(_ofs.good());
            _cur_fsize += len;
        }

    private:
        // 创建写入文件
        std::string CreateNewFile()
        {
            time_t t = util::Data::getTime();
            struct tm lt;
            localtime_r(&t, &lt);
            std::stringstream ss;
            ss << _basename;
            ss << lt.tm_year + 1900;
            ss << lt.tm_mon + 1;
            ss << lt.tm_yday;
            ss << lt.tm_hour;
            ss << std::to_string(_name_cunt++);
            ss << ".log";
            return ss.str();
        }

    private:
        size_t _name_cunt;
        std::string _basename; // 类似log/log-20241030/
        size_t _file_size_max; // 文件最大的大小
        size_t _cur_fsize;     // 目前文件大小
        std::ofstream _ofs;
    };
    // 这里模拟实现通过时间来控制滚动文件
    enum class TimeGap
    {
        GAP_SECOND,
        GAP_MINUTE,
        GAP_HOUR,
        GAP_DAY,
    };

    // 滚动文件输出
    class RollSink2 : public zkjlog::LogSink
    {
    public:
        RollSink2(const std::string &pathname, TimeGap time_gap)
            : _pathname(pathname), _time_gap(time_gap)
        {
            switch (time_gap)
            {
            case TimeGap::GAP_SECOND:
                _gap_size = 1;
                break;
            case TimeGap::GAP_MINUTE:
                _gap_size = 60;
                break;
            case TimeGap::GAP_HOUR:
                _gap_size = 3600;
                break;
            case TimeGap::GAP_DAY:
                _gap_size = 3600 * 24;
                break;
            }
            _cur_gap = zkjlog::util::Data::getTime() % _cur_gap;
            std::string filename = createNewFile();
            zkjlog::util::File::create_directory(filename);
            _ofs.open(_pathname, std::ios::binary | std::ios::app);
            assert(_ofs.good());
        }
        // 判断文件的大小，并判断是否创建新文件
        void log(const char *data, size_t len)
        {
            time_t cur = zkjlog::util::Data::getTime();
            if (cur % _gap_size != _cur_gap)
            {
                _ofs.close();
                std::string filename = createNewFile();
                zkjlog::util::File::create_directory(filename);
                _ofs.open(_pathname, std::ios::binary | std::ios::app);
                assert(_ofs.good());
            }
            _ofs.write(data, len);
            assert(_ofs.good());
        }

    private:
        std::string createNewFile()
        {
            time_t t = zkjlog::util::Data::getTime();
            struct tm lt;
            localtime_r(&t, &lt);
            std::stringstream ss;
            ss << _pathname;
            ss << lt.tm_year + 1900;
            ss << lt.tm_mon + 1;
            ss << lt.tm_yday;
            ss << lt.tm_hour;
            ss << std::to_string(_cur_gap++);
            ss << ".log";
            return ss.str();
        }

    private:
        TimeGap _time_gap;
        std::string _pathname;
        std::ofstream _ofs;
        size_t _cur_gap;  // 当前是第几个时间段
        size_t _gap_size; // 时间段的大小
    };

    class SinkFactory
    {
        // public:
        //     // 这个写法违反了开闭原则
        //     static LogSink::ptr create0(int type)
        //     {
        //         switch (type)
        //         {
        //         case 1:
        //             return std::make_shared<StdoutSink>();
        //         case 2:
        //             return std::make_shared<FileSink>();
        //         case 3:
        //             return std::make_shared<RollSink>();
        //         default:
        //             return std::make_shared<LogSink>();
        //         }
        //         return std::make_shared<LogSink>();
        //     }

    public:
        template <typename SinkType, typename... Args>
        static LogSink::ptr create(Args &&...args)
        {
            return std::make_shared<SinkType>(std::forward<Args>(args)...);
        }
    };
}

#endif
