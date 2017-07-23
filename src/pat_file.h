//
//  pat_file.h
//  apk_diff
//
//  Created by housisong on 2017/6/18.
//  Copyright © 2017年 housisong. All rights reserved.
//

#ifndef pat_file_h
#define pat_file_h

#include "hash.h"
#include <vector>
static const long kVersion1='p' | ('a'<<8)  | ('t'<<16)  | (1<<24);
static const int  kHeadSize1=4*4+3*sizeof(THashValue);

/*
struct pat_head_1 {//按小端格式储存;
    int32       version;  // 4byte
    int32       fileSize; // 4byte
    THashValue  fileHash; //16byte
    int32       oldSize;  // 4byte
    THashValue  oldHash;  //16byte
    int32       newSize;  // 4byte
    THashValue  newHash;  //16byte
};
*/

inline static THashValue hash(const std::vector<UInt8>& data){
    const UInt8* begin=data.empty()?0:(&data[0]);
    return hash(begin,begin+data.size());
}

inline static void outHash(std::vector<UInt8>& out,const THashValue& data){
    const UInt8* p=&data.digest[0];
    out.insert(out.end(),p,p+sizeof(THashValue));
}


inline static void outData4(std::vector<UInt8>& out,size_t data){
    out.push_back((UInt8)data);
    out.push_back((UInt8)(data>>8));
    out.push_back((UInt8)(data>>16));
    out.push_back((UInt8)(data>>24));
}
inline static void outSize4(std::vector<UInt8>& out,size_t size){
    if ((size>>31)!=0)
        throw std::runtime_error("outSize4: size>=2^31 erorr!"); //数值超界;
    outData4(out,size);
}

#endif /* pat_file_h */
