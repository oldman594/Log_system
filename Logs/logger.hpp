/*完成日志器模块
    1.抽象日志器基类
    2.派生出不同的子类（同步日志器 & 异步日志器）
*/
#ifndef __MY_LOGGER__
#define __MY_LOGGER__
// #define _GNU_SOURCE
#include "util.hpp"
#include "format.hpp"
#include "level.hpp"
#include "message.hpp"
#include "Sing.hpp"
#include "loper.hpp"
#include <atomic>
#include <mutex>
#include <pthread.h>
#include <cstdarg>
#include <memory>
#include <unordered_map>

namespace zkjlog
{
    class logger
    {
    public:
        using ptr = std::shared_ptr<logger>;
        logger(std::string &logegr_name, LogLevel::value limit_level, formatter::ptr &formatter, std::vector<LogSink::ptr> &sinks)
            : _logger_name(logegr_name), _limit_level(limit_level), _formatter(formatter), _sinks(sinks.begin(), sinks.end())
        {
        }
        const std::string &name() { return _logger_name; }
        void debug(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            // 通过传入的参数构造出一个日志消息对象，得到格式化后的日志消息字符串 -- 然后进行落地输出
            // 1.判断当前的日志是否达到了输出等级
            if (LogLevel::value::DEBUG < _limit_level)
                return;
            // 2.对fmt格式化字符串和不定参进行字符串组织，得到的日志消息的字符串
            va_list ap;
            va_start(ap, fmt);
            char *tmp;
            int ret = vasprintf(&tmp, fmt.c_str(), ap);
            if (ret == -1)
            {
                std::cout << "vasprintf fail ..." << std::endl;
                return;
            }
            va_end(ap);
            serialize(LogLevel::value::DEBUG, file, line, tmp);
            free(tmp);
        }
        void info(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            // 通过传入的参数构造出一个日志消息对象，得到格式化后的日志消息字符串 -- 然后进行落地输出
            // 1.判断当前的日志是否达到了输出等级
            if (LogLevel::value::INFO < _limit_level)
                return;
            // 2.对fmt格式化字符串和不定参进行字符串组织，得到的日志消息的字符串
            va_list ap;
            va_start(ap, fmt);
            char *tmp;
            int ret = vasprintf(&tmp, fmt.c_str(), ap);
            if (ret == -1)
            {
                std::cout << "vasprintf fail ..." << std::endl;
                return;
            }
            va_end(ap);
            serialize(LogLevel::value::INFO, file, line, tmp);
            free(tmp);
        }
        void warn(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            // 通过传入的参数构造出一个日志消息对象，得到格式化后的日志消息字符串 -- 然后进行落地输出
            // 1.判断当前的日志是否达到了输出等级
            if (LogLevel::value::WARN < _limit_level)
                return;
            // 2.对fmt格式化字符串和不定参进行字符串组织，得到的日志消息的字符串
            va_list ap;
            va_start(ap, fmt);
            char *tmp;
            int ret = vasprintf(&tmp, fmt.c_str(), ap);
            if (ret == -1)
            {
                std::cout << "vasprintf fail ..." << std::endl;
                return;
            }
            va_end(ap);
            serialize(LogLevel::value::WARN, file, line, tmp);
            free(tmp);
        }
        void error(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            // 通过传入的参数构造出一个日志消息对象，得到格式化后的日志消息字符串 -- 然后进行落地输出
            // 1.判断当前的日志是否达到了输出等级
            if (LogLevel::value::ERROR < _limit_level)
                return;
            // 2.对fmt格式化字符串和不定参进行字符串组织，得到的日志消息的字符串
            va_list ap;
            va_start(ap, fmt);
            char *tmp;
            int ret = vasprintf(&tmp, fmt.c_str(), ap);
            if (ret == -1)
            {
                std::cout << "vasprintf fail ..." << std::endl;
                return;
            }
            va_end(ap);
            serialize(LogLevel::value::ERROR, file, line, tmp);
            free(tmp);
        }

        void fatal(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            // 通过传入的参数构造出一个日志消息对象，得到格式化后的日志消息字符串 -- 然后进行落地输出
            // 1.判断当前的日志是否达到了输出等级
            if (LogLevel::value::FATAL < _limit_level)
                return;
            // 2.对fmt格式化字符串和不定参进行字符串组织，得到的日志消息的字符串
            va_list ap;
            va_start(ap, fmt);
            char *tmp;
            int ret = vasprintf(&tmp, fmt.c_str(), ap);
            if (ret == -1)
            {
                std::cout << "vasprintf fail ..." << std::endl;
                return;
            }
            va_end(ap);
            serialize(LogLevel::value::FATAL, file, line, tmp);
            free(tmp);
        }

    protected:
        void serialize(LogLevel::value level, const std::string &file, size_t line, char *str)
        {
            // 3.构造logmsg对象
            LogMsg msg(level, line, file, _logger_name, str);
            // 4.通过格式化工具对LogMsg进行格式化，得到格式化后的日志字符串
            std::stringstream ss;
            _formatter->format(ss, msg);
            // 5.进行日志落地
            log(ss.str().c_str(), ss.str().size());
        }

    protected:
        /*抽象接口完成实际的落地输出--不同的日志器会有不同的实际落地方式*/
        virtual void log(const char *data, size_t len) = 0;

    protected:
        std::mutex _mutex;
        std::string _logger_name;
        std::atomic<LogLevel::value> _limit_level; // 让_limit_level这个变量的访问是原子的（和加锁差不多但是成本低于加锁）
        formatter::ptr _formatter;
        std::vector<LogSink::ptr> _sinks;
    };

    class SyncLogger : public logger
    {
    public:
        SyncLogger(std::string &logegr_name, LogLevel::value limit_level, formatter::ptr &formatter, std::vector<LogSink::ptr> &sinks)
            : logger(logegr_name, limit_level, formatter, sinks)
        {
        }

    protected:
        // 同步日志器，是将日志直接落地模块句柄进行日志落地
        void log(const char *data, size_t len) override
        {
            std::unique_lock<std::mutex> lock(_mutex);
            if (_sinks.empty())
                return;
            for (auto sink : _sinks)
                sink->log(data, len);
        }
    };
    class AsyncLogger : public logger
    {
    public:
        AsyncLogger(std::string &logegr_name, LogLevel::value limit_level, formatter::ptr &formatter, std::vector<LogSink::ptr> &sinks, AsyncType looper_type)
            : logger(logegr_name, limit_level, formatter, sinks),
              _looper(std::make_shared<AsyncLooper>(std::bind(AsyncLogger::realLog, this, std::placeholders::_1), looper_type))
        {
        }
        void log(const char *data, size_t len) override // 将数据写入缓冲区
        {
            _looper->push(data, len);
        }
        // 设计一个实际落地函数（将缓冲区的数据落地）
        void realLog(Buffer &buf);

    private:
        zkjlog::AsyncLooper::ptr _looper;
    };

    /*使用建造者模式来建造日志器，而不是让用户直接去构造日志器，简化用户的使用复杂度*/
    // 1.抽象一个日志器建造者类（完成日志器对象所需零部件的构建 & 日志器的构建）
    //   1.设置日志器类型
    //   2.将不同类型日志器的创建放到同一个日志器建造者类中完成
    enum class LoggerType
    {
        LOGGER_SYNC,
        LOGGER_ASYNC,
    };
    class LoggerBuilder
    {
    public:
        LoggerBuilder()
            : _logger_type(LoggerType::LOGGER_SYNC),
              _limit_level(zkjlog::LogLevel::value::WARN),
              _looper_type(AsyncType::ASYNC_SAFE)
        {
        }

        void buildLoggerType(LoggerType type) { _logger_type = type; }
        void buildEnableUnSafeAsync() { _looper_type = AsyncType::ASUNC_UNSAFE; }
        void buildLoggerName(std::string &name) { _logger_name = name; }
        void buildLoggerLevel(LogLevel::value level) { _limit_level = level; }
        void buildFormatter(const std::string &pattern)
        {
            _formatter = std::make_shared<formatter>(pattern);
        }
        template <typename SinkType, typename... Args>
        void buildSink(Args &&...args)
        {
            LogSink::ptr psink = SinkFactory::create<SinkType>(std::forward<Args>(args)...);
            _sinks.push_back(psink);
        }
        virtual zkjlog::logger::ptr build() = 0;

    protected:
        AsyncType _looper_type;
        LoggerType _logger_type;
        std::string _logger_name;
        LogLevel::value _limit_level;
        formatter::ptr _formatter;
        std::vector<LogSink::ptr> _sinks;
    };
    // 2.派生出具体的建造者类---局部日志器建造者 & 全局日志器建造者
    class LocalLoggerBuilder : public LoggerBuilder
    {
    public:
        logger::ptr build() override
        {
            assert(_logger_name.empty() == false); // 日志器名称必须有
            if (_formatter.get() == nullptr)
                _formatter = std::make_shared<formatter>();
            if (_sinks.empty())
                buildSink<StdoutSink>();
            if (_logger_type == LoggerType::LOGGER_ASYNC)
                return std::make_shared<AsyncLogger>(_logger_name, _limit_level, _formatter, _sinks, _looper_type);
            return std::make_shared<SyncLogger>(_logger_name, _limit_level, _formatter, _sinks);
        }
    };

    class LoggerManager
    {
    public:
        static LoggerManager &getInstance()
        {
            // 在C++11以后，针对静态局部变量，编译器在编译层面实现了线程安全
            // 当静态局部变量在没有构造完成之前，其他的线程进入就会阻塞
            static LoggerManager eton;
            return eton;
        }
        void addLogger(logger::ptr &logger)
        {
            if (hasLogger(logger->name()) == true)
                return;
            std::unique_lock<std::mutex> lock(_mutex);
            _loggers.insert(std::make_pair(logger->name(), logger));
        }
        bool hasLogger(const std::string &name)
        {
            auto it = _loggers.find(name);
            if (it == _loggers.end())
                return false;
            else
                return true;
        }
        logger::ptr getLogger(const std::string &name)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto it = _loggers.find(name);
            if (it == _loggers.end())
                return logger::ptr();
            return it->second;
        }
        logger::ptr rootLogger() { return _root_logger; }

    private:
        LoggerManager()
        {
            std::unique_ptr<zkjlog::LoggerBuilder> build(new zkjlog::LocalLoggerBuilder());
            std::string name = "root";
            build->buildLoggerName(name);
            _root_logger = build->build();
            _loggers.insert(std::make_pair("root", _root_logger));
        }

    private:
        std::mutex _mutex;
        logger::ptr _root_logger; // 默认日志器
        std::unordered_map<std::string, logger::ptr> _loggers;
    };


    //设计一个全局日志器的建造者--在局部的基础上增加了一个功能：将日志器添加到单例对象中
    class GlobalLoggerBuilder : public LoggerBuilder
    {
    public:
        logger::ptr build() override
        {
            assert(_logger_name.empty() == false); // 日志器名称必须有
            if (_formatter.get() == nullptr)
                _formatter = std::make_shared<formatter>();
            if (_sinks.empty())
                buildSink<StdoutSink>();
            logger::ptr logger;
            if (_logger_type == LoggerType::LOGGER_ASYNC)
                logger = std::make_shared<AsyncLogger>(_logger_name, _limit_level, _formatter, _sinks, _looper_type);
            else logger = std::make_shared<SyncLogger>(_logger_name, _limit_level, _formatter, _sinks);
            LoggerManager::getInstance()->addLogger(logger);
            return logger;
        }
    };
}
#endif