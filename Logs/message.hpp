#ifndef __MY_MESSAGGE__
#define __MY_MESSAGGE__

#include "level.hpp"
#include "util.hpp"
#include <iostream>
#include <string>
#include <thread> //用c++的线程库是为了平台的兼容性

namespace zkjlog
{
    struct LogMsg{
            size_t _count;  //计数器
            time_t _ctime; // 日志产生的时间戳
            LogLevel::value _level;  //日志等级
            size_t _line; // 行号
            std::thread::id _tid; //线程ID
            std::string _file;  //源文件名
            std::string _logger;//日志器名字
            std::string _payload;//有效消息数据
            LogMsg(LogLevel::value level,size_t line,std::string file,std::string logger,std::string payload)
            :_ctime(util::Data::getTime()),_level(level),_line(line),_tid(std::this_thread::get_id()),_file(file),_logger(logger),_payload(payload)
            {}
    };
}

#endif