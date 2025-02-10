#include "logger.hpp"

namespace zkjlog
{
    //1.提供获取指定日志器的全局接口
    logger::ptr getLogger(const std::string &name)
    {
        return zkjlog::LoggerManager::getInstance().getLogger(name);
    }
    logger::ptr rootLogger()
    {
        return zkjlog::LoggerManager::getInstance().rootLogger();
    }
    //2.使用宏函数对日志器的接口进行代理
    #define debug(fmt, ...) debug(__FILE__,__LINE__,fmt,##__VA_ARGS__)
    #define info(fmt, ...) info(__FILE__,__LINE__,fmt,##__VA_ARGS__)
    #define warn(fmt, ...) warn(__FILE__,__LINE__,fmt,##__VA_ARGS__)
    #define error(fmt, ...) error(__FILE__,__LINE__,fmt,##__VA_ARGS__)
    #define fatal(fmt, ...) fatal(__FILE__,__LINE__,fmt,##__VA_ARGS__)

    //3.提供宏函数，直接通过默认日志器进行日志的标准输出打印
    #define DEBUG(fmt,...) zkjlog::rootLogger()->debug(fmt,##__VA_ARGS__)
    #define INFO(fmt,...) zkjlog::rootLogger()->info(fmt,##__VA_ARGS__)
    #define WARN(fmt,...) zkjlog::rootLogger()->warn(fmt,##__VA_ARGS__)
    #define ERROR(fmt,...) zkjlog::rootLogger()->error(fmt,##__VA_ARGS__)
    #define FATAL(fmt,...) zkjlog::rootLogger()->fatal(fmt,##__VA_ARGS__)

}   