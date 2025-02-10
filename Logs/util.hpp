// #pragma once

#ifndef __MY_UTIL__
#define __MY_UTIL__

/*实用工具类的实现
    1.获取系统时间
    2.判断文件是否存在
    3.获取文件所在路径
    4.创建目录
*/

#include <iostream>
#include <string>
#include <ctime>
#include <sys/stat.h>
#include <sys/types.h>
namespace zkjlog
{
    namespace util
    {
        class Data
        {
        public:
            static time_t getTime()
            {
                return time(nullptr);
            }
        };
        class File
        {
        public:
            static bool exists(const std::string &name) // 判断文件是否存在
            {
                struct stat st;
                return stat(name.c_str(), &st) == 0; // 这个接口是参看文件属性的，如果文件存在必然有属性
            }

            static std::string path(const std::string &name) // 获取文件所在路径
            {
                //   abc/bc/c/text.txt
                if (name.empty())
                    return ".";
                size_t pos = name.rfind("/");
                if (pos == std::string::npos)
                    return ".";
                return name.substr(0, pos + 1);
            }

            static void create_directory(const std::string &path) // 创建目录
            {
                //   1 abc/bc/c/text.txt    2 text.txt  3 .   4 ..
                if (path.empty())
                    return;
                if (exists(path))
                    return;
                size_t pos = 0, idx = 0;
                while (idx < path.size())
                {
                    pos = path.find("/",idx);
                    if(pos == std::string::npos)
                    {
                        mkdir(path.c_str(),0755);
                        break;
                    }
                    std::string sstr = path.substr(0, pos);
                    if (exists(sstr)){
                        idx = pos + 1;
                        continue;
                    }
                    // else{
                    //     mkdir(sstr.c_str(),0755);
                    //     idx =pos+1;
                    //     continue;
                    // }

                    if (sstr == "." || sstr == "..")
                    {
                        idx = pos + 1;
                        continue;
                    }
                    mkdir(sstr.c_str(),0755);   //0755为默认权限
                    idx = pos+1;
                }
                return ;
            }
        };
    }
}


#endif