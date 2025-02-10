#include "util.hpp"
#include "message.hpp"
#include "format.hpp"
#include "Sing.hpp"
#include "logger.hpp"
#include <iostream>
#include <vector>
#include <string>

// void text()
// {
//     size_t time = zkjlog::util::Data::getTime();
//     std::string path = "./linux/log/util/text";
//     if(zkjlog::util::File::exists("./util.hpp")) std::cout<<"util.hpp is exist";
//     zkjlog::util::File::create_directory("./abc/bc/c/linux");
//     std::string Path = zkjlog::util::File::path("./abc/bc/c/linux");
//     std::cout<<Path<<std::endl;
// }

// int main()
// {
//     text();
//     return 0;
// }

// 对格式化规则字符串进行解析
// bool parsePattern(const std::string &_pattern)
// {
//     // adawds%%[%d{%H:%M:%S}][%t][%c][%f:%l][%p]%T%m%n
//     size_t len = _pattern.size(), pos = 0;
//     std::vector<std::pair<std::string, std::string>> fmt_order;
//     std::string key, val;
//     while (pos < len)
//     {
//         if (_pattern[pos] != '%')
//         {
//             val.push_back(_pattern[pos++]);
//             continue;
//         }
//         if (_pattern[pos] == '%' && _pattern[pos + 1] == '%')
//         {
//             val.push_back(_pattern[pos]);
//             pos += 2;
//             continue;
//         }
//         // val -> adawds%%[
//         if(!val.empty()||!key.empty()) fmt_order.push_back(std::make_pair("", val));
//         val.clear();key.clear();
//         pos += 1; // 这个时候走到第一个%后的标识符的位置
//         if (pos == len)
//         {
//             std::cout << "_pattern 的格式错误" << std::endl;
//             return false;
//         }
//         key.push_back(_pattern[pos]);
//         if (_pattern[pos + 1] == '{')
//         {
//             // 说明此时的表示符有子格式
//             pos += 2;
//             while (_pattern[pos] != '}' && pos < len)
//                 val.push_back(_pattern[pos++]);
//             // 此时说明子格式已经存到val里了   %H:%M:%S
//             fmt_order.push_back(std::make_pair(key, val));
//             key.clear();
//             val.clear();
//             continue;
//         }
//         else
//         {
//             fmt_order.push_back(std::make_pair(key, "")); pos++;
//             key.clear();
//             val.clear();
//             continue;
//         }
//     }
//     for (auto tmp : fmt_order)
//         std::cout << tmp.first << ":" << tmp.second << std::endl;
//     return true;
// }

// int main()
// {
//     parsePattern("adawds%%[%d{%H:%M:%S}][%t][%c][%f:%l][%p]%T%m%n");
//     return 0;
// }



void text1()
{
    zkjlog::formatter mat;
    zkjlog::LogMsg msg(zkjlog::LogLevel::value::INFO, 53, "utiltext.cc", "root", "功能化测试...");
    std::string str = mat.format(msg);
    //zkjlog::LogSink::ptr lsg = zkjlog::SinkFactory::create<zkjlog::StdoutSink>();
    zkjlog::LogSink::ptr lsg2 = zkjlog::SinkFactory::create<zkjlog::FileSink>("./log.txt");
    zkjlog::LogSink::ptr lsg3 = zkjlog::SinkFactory::create<zkjlog::RollSink>("./logfile/", 1024 * 1024);
    // lsg->log(str.c_str(), str.size());
    // lsg2->log(str.c_str(), str.size());
    std::vector<zkjlog::LogSink::ptr> sinks = {lsg2,lsg3};
    zkjlog::formatter::ptr fmt(new zkjlog::formatter("[%d{%H:%M:%S}][%t][%c][%f:%l][%p]%T%m%n"));//单参数构造函数支持隐式类型转换
    std::string logger_name = "sync_logger";
    zkjlog::SyncLogger::ptr Ger(new zkjlog::SyncLogger(logger_name,zkjlog::LogLevel::value::WARN,fmt,sinks));
    Ger->debug(__FILE__,__LINE__,"%s","测试日志");
    Ger->info(__FILE__,__LINE__,"%s","测试日志");
    Ger->warn(__FILE__,__LINE__,"%s","测试日志");
    Ger->error(__FILE__,__LINE__,"%s","测试日志");
    Ger->fatal(__FILE__,__LINE__,"%s","测试日志");

    // size_t cursize = 0;
    // size_t count = 0;
    // while (cursize < 1024 * 1024 * 10)
    // {
    //     std::string tmp = str + std::to_string(count++);
    //     lsg3->log(tmp.c_str(), tmp.size());
    //     cursize += tmp.size();
    // }

    size_t cur_size = 0,count = 0;
    while(cur_size < 1024*1024*10)
    {
        Ger->fatal(__FILE__,__LINE__,"%s","测试日志");
        cur_size += 20;
    }
}


void text2()
{
    zkjlog::formatter mat;
    zkjlog::LogMsg msg(zkjlog::LogLevel::value::INFO, 53, "utiltext.cc", "root", "功能化测试...");
    std::string str = mat.format(msg);
    zkjlog::LogSink::ptr lsg2 = zkjlog::SinkFactory::create<zkjlog::FileSink>("./log.txt");
    zkjlog::LogSink::ptr lsg3 = zkjlog::SinkFactory::create<zkjlog::RollSink>("./logfile/", 1024 * 1024);
    std::vector<zkjlog::LogSink::ptr> sinks = {lsg2,lsg3};
    zkjlog::formatter::ptr fmt(new zkjlog::formatter("[%d{%H:%M:%S}][%t][%c][%f:%l][%p]%T%m%n"));//单参数构造函数支持隐式类型转换
    std::string logger_name = "sync_logger";
    zkjlog::SyncLogger::ptr Ger(new zkjlog::SyncLogger(logger_name,zkjlog::LogLevel::value::WARN,fmt,sinks));
    Ger->debug(__FILE__,__LINE__,"%s","测试日志");
    Ger->info(__FILE__,__LINE__,"%s","测试日志");
    Ger->warn(__FILE__,__LINE__,"%s","测试日志");
    Ger->error(__FILE__,__LINE__,"%s","测试日志");
    Ger->fatal(__FILE__,__LINE__,"%s","测试日志");
    size_t cur_size = 0;
    while(cur_size < 1024*1024*10)
    {
        Ger->fatal(__FILE__,__LINE__,"%s","测试日志");
        cur_size += 20;
    }
}

int main()
{
    text2();
    return 0;
}