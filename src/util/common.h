//
//  common.h
//  patcher
//
//  Created by 陈卓 on 2017/7/14.
//  Copyright © 2017年 4A Volcano. All rights reserved.
//

#ifndef common_h
#define common_h

#include <string>
#include <vector>

std::string string_format( const std::string format, ... );

/**
 * 清除指针元素vector的模版函数，会释放其中每个指针元素的内存，并清空vector
 * @param vec 要清除的向量指针
 **/
template <typename T> void CleanPtrVector(std::vector<T*> *vec){
    for(int i = 0; i < vec->size(); i++){
        delete (*vec)[i];
    }
    std::vector<T*>().swap(*vec);
}

#endif /* common_h */
