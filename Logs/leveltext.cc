#include<iostream>
#include"level.hpp"

void text()
{
    const char* str = zkjlog::LogLevel::toString(zkjlog::LogLevel::value::DEBUG);
    std::cout<<str<<std::endl;
}

int main()
{
    text();
    return 0;
}