//
// Created by anyang on 2017/7/7.
//

#ifndef TESTC_DIFFUTIL_H
#define TESTC_DIFFUTIL_H

#include <vector>
#include <string>
#include "../hash.h"
#include "types.h"
#include "diff.h"

namespace DiffUtil{
   
    //将res输出到out中
    static void outVector(std::vector<TByte> &res, std::vector<TByte> &out){
        for (int i = 0; i < res.size(); i++) {
            out.push_back(res[i]);
        }
    }

    static std::vector<TByte> intToBytes(int num){
        std::vector<TByte> res;
        res.push_back((TByte) num);
        res.push_back((TByte) (num >> 8));
        res.push_back((TByte) (num >> 16));
        res.push_back((TByte) (num >> 24));
        return res;
    }

    static int bytesToInt(std::vector<TByte> bytes) {
         int num = 0;
         num |= bytes[0];
         num |= (bytes[1] << 8);
         num |= (bytes[2] << 16);
         num |= (bytes[3] << 24);
         return num;
     }


    //将字符串以bytes形式输出到out中
    static void outString(std::string str, std::vector<TByte> &out){
        const char *str_ptr = str.data();
        for (int i = 0; i < str.size(); i++) {
            out.push_back((TByte) str_ptr[i]);
        }
    }
    //输出int目前表示4个byte大小
    static void outInt(int num, std::vector<TByte> &out) {
        std::vector<TByte> intBytes = intToBytes(num);
        outVector(intBytes, out);
    }

    //对diff 进行简单封装
    static void doDiff(std::vector<TByte> *new_data,std::vector<TByte > *old_data,std::vector<TByte > *out){
        std::vector<TByte> oldBytes = *new_data;
        std::vector<TByte> newBytes = *old_data;
        const TByte *old_begin = &oldBytes[0];
        const TByte *new_begin = &newBytes[0];
        
        create_diff(old_begin, old_begin + oldBytes.size(), new_begin, new_begin + newBytes.size(), *out);
    }
}


#endif //TESTC_DIFFUTIL_H
