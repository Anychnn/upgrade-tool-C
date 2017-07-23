//
//  common.cpp
//  patcher
//
//  Created by 陈卓 on 2017/7/14.
//  Copyright © 2017年 4A Volcano. All rights reserved.
//

#include "common.h"

std::string string_format( const std::string format, ... )
{
    va_list args;
    va_start(args, format);
    size_t size = vsnprintf(NULL, 0, "%s%s%s", args ) + 1; // Extra space for '\0'
    va_end(args);
    
    va_start(args, format);
    std::unique_ptr<char[]> buf( new char[ size ] );
    vsnprintf( buf.get(), size, format.c_str(), args );
    va_end(args);
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}
