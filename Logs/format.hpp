#ifndef __MY_FORMAT__
#define __MY_FORMAT__

#include "message.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <memory>
#include <ctime>
#include <cassert>

namespace zkjlog
{
    // 抽象格式化子项基类
    class FormatItem
    {
    public:
        using pte = std::shared_ptr<FormatItem>;
        virtual void format(std::ostream &out, const LogMsg &msg) = 0;
    };
    // 派生格式化子项子类--消息，等级，时间，文件名，行号，线程ID，日志器名，制表符，换行，其他
    class payformat : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMsg &msg) override
        {
            out << msg._payload;
        }
    };
    class levelformat : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMsg &msg) override
        {
            out << zkjlog::LogLevel::toString(msg._level);
        }
    };
    class timeformat : public FormatItem
    {
    private:
        std::string _format;

    public:
        timeformat(const std::string &format = "%H:%M:%S") : _format(format)
        {
            if (format.empty())
                _format = "%H:%M:%S"; // H,M,S是strftime这个接口里给定的格式H用小时替换，M用分替换，S用秒替换
        }
        virtual void format(std::ostream &out, const LogMsg &msg)
        {
            time_t t = msg._ctime;
            struct tm lt;
            localtime_r(&t, &lt);
            char tmp[128];
            strftime(tmp, 127, _format.c_str(), &lt);
            out << tmp;
        }
    };
    class fileformat : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMsg &msg) override
        {
            out << msg._file;
        }
    };
    class lineformat : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMsg &msg) override
        {
            out << msg._line;
        }
    };
    class tidformat : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMsg &msg) override
        {
            out << msg._tid;
        }
    };
    class loggerformat : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMsg &msg) override
        {
            out << msg._logger;
        }
    };
    class tabformat : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMsg &msg) override
        {
            out << "\t";
        }
    };
    class nlineformat : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMsg &msg) override
        {
            out << "\n";
        }
    };
    class otherformat : public FormatItem
    {
    public:
        otherformat(const std::string &str) : _str(str) {}

    public:
        void format(std::ostream &out, const LogMsg &msg) override
        {
            out << _str;
        }

    private:
        std::string _str;
    };
    /*
        %d 表示日期 ， 包含子格式{%H:%M:%S}
        %t 表示线程ID
        %c 表示日志器名称
        %f 表示源码文件名
        %l 表示源码行号
        %p 表示日志等级
        %T 表示制表符缩进
        %m 表示主体消息
        %n 表示换行
    */
    class formatter
    {
    public:
        using ptr = std::shared_ptr<formatter>;
        formatter(const std::string &pattern = "[%d{%H:%M:%S}][%t][%c][%f:%l][%p]%T%m%n")
            : _pattern(pattern)
        {
            //assert(parsePattern()); // 规则字符串解析不能失败
            assert(parsePattern(_pattern));
        }

        // 对msg进行格式化
        void format(std::ostream &out, LogMsg &msg)
        {
            for (auto item : _items)
                item->format(out, msg);
        }
        std::string format(LogMsg &msg)
        {
            std::stringstream ss;
            format(ss, msg);
            return ss.str();
        }
    private:
        // 对格式化规则字符串进行解析
        bool parsePattern(const std::string &_pattern)
        {
            // adawds%%[%d{%H:%M:%S}][%t][%c][%f:%l][%p]%T%m%n
            size_t len = _pattern.size(), pos = 0;
            std::vector<std::pair<std::string, std::string>> fmt_order;
            std::string key, val;
            while (pos < len)
            {
                if (_pattern[pos] != '%')
                {
                    val.push_back(_pattern[pos++]);
                    continue;
                }
                if (_pattern[pos] == '%' && _pattern[pos + 1] == '%')
                {
                    val.push_back(_pattern[pos]);
                    pos += 2;
                    continue;
                }
                // val -> adawds%%[
                if (!val.empty() || !key.empty())
                    fmt_order.push_back(std::make_pair("", val));
                val.clear();
                key.clear();
                pos += 1; // 这个时候走到第一个%后的标识符的位置
                if (pos == len)
                {
                    std::cout << "_pattern 的格式错误" << std::endl;
                    return false;
                }
                key.push_back(_pattern[pos]);
                if (_pattern[pos + 1] == '{')
                {
                    // 说明此时的表示符有子格式
                    pos += 2;
                    while (_pattern[pos] != '}' && pos < len) val.push_back(_pattern[pos++]);
                    // 此时说明子格式已经存到val里了   %H:%M:%S
                    fmt_order.push_back(std::make_pair(key, val));
                    if(pos < len) pos ++;
                    key.clear();
                    val.clear();
                    continue;
                }
                else
                {
                    fmt_order.push_back(std::make_pair(key, ""));
                    pos++;
                    key.clear();
                    val.clear();
                    continue;
                }
            }
            for (auto tmp : fmt_order) _items.push_back(createItem(tmp.first,tmp.second));
            //     std::cout << tmp.first << ":" << tmp.second << std::endl;
            return true;
        }

    private:
        // 根据不同的格式化字符创建不同的格式化子项对象
        FormatItem::pte createItem(const std::string &key, const std::string &val)
        {
            /*
                %d 表示日期 ， 包含子格式{%H:%M:%S}
                %t 表示线程ID
                %c 表示日志器名称
                %f 表示源码文件名
                %l 表示源码行号
                %p 表示日志等级
                %T 表示制表符缩进
                %m 表示主体消息
                %n 表示换行
            */
            if (key == "d")
                return std::make_shared<timeformat>(val);
            if (key == "t")
                return std::make_shared<tidformat>();
            if (key == "c")
                return std::make_shared<loggerformat>();
            if (key == "f")
                return std::make_shared<fileformat>();
            if (key == "l")
                return std::make_shared<lineformat>();
            if (key == "p")
                return std::make_shared<levelformat>();
            if (key == "T")
                return std::make_shared<tabformat>();
            if (key == "m")
                return std::make_shared<payformat>();
            if (key == "n")
                return std::make_shared<nlineformat>();
            return std::make_shared<otherformat>(val);
        }

    private:
        std::string _pattern; // 格式化规则字符串
        std::vector<FormatItem::pte> _items;
    };
}

#endif