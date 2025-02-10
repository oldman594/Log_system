#ifndef __MY_LEVEL__
#define __MY_LEVEL__

namespace zkjlog{
    class LogLevel{
    public:
        enum class value{
            UNKNOW = 0,
            DEBUG,
            INFO,
            WARN,
            ERROR,
            FATAL,
            OFF
        };

        static const char* toString(zkjlog::LogLevel::value level)
        {
            //写法1
            // switch (level)
            // {
            //     case zkjlog::LogLevel::value::DEBUG : return "DEBUG" ;
            //     case zkjlog::LogLevel::value::INFO : return "INFO" ;
            //     case zkjlog::LogLevel::value::WARN : return "WARN" ;
            //     case zkjlog::LogLevel::value::ERROR : return "ERROR" ;
            //     case zkjlog::LogLevel::value::FATAL : return "FATAL" ;
            //     case zkjlog::LogLevel::value::OFF : return "OFF" ;
            // }
            // return "UNKNOW"
            //写法2
            switch (level)
            {
                #define TOSTRING(name) #name
                case zkjlog::LogLevel::value::DEBUG : return TOSTRING(DEBUG);
                case zkjlog::LogLevel::value::INFO : return TOSTRING(INFO);
                case zkjlog::LogLevel::value::WARN : return TOSTRING(WARN);
                case zkjlog::LogLevel::value::ERROR : return TOSTRING(ERROR);
                case zkjlog::LogLevel::value::FATAL : return TOSTRING(FATAL);
                case zkjlog::LogLevel::value::OFF : return TOSTRING(OFF);
                #undef TOSTRING
                default : return "UNKNOW";
            }
            return "UNKNOW";
        }
    };
}



#endif