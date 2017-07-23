//
//  hash.h
//  apk_diff
//
//  Created by housisong on 2017/6/11.
//  Copyright © 2017年 housisong. All rights reserved.
//

#ifndef hash_h
#define hash_h

#include <string.h>
#include <ostream>


#define _MD5_HASH
//#define _zlib_CRC32_HASH

#ifdef _MD5_HASH
#include "md5/md5.h"
typedef md5_byte_t UInt8;
typedef struct THashValue{
    UInt8  digest[16];
    //重载比较符号
    friend bool operator<(const THashValue&x,const THashValue &y){
        const UInt8 *dx=x.digest;
        const UInt8 *dy=y.digest;
        for(int i=0;i< sizeof(THashValue);i++){
            if(dx[i]<dy[i]){
                return true;
            }else if(dx[i]==dy[i]){
                continue;
            }else{
                return false;
            }
        }
        return false;
    }

    friend std::ostream &operator<<(std::ostream &os,const THashValue hashValue){
        for(int i=0;i< sizeof(THashValue);i++){
            os<<(int)hashValue.digest[i]<<"\t";
        }
        return os;
    }
} THashValue;



#define THashState  md5_state_t
#define hash_init   md5_init
#define hash_append md5_append
#define hash_finish md5_finish

#endif
#ifdef _zlib_CRC32_HASH
#include "zlib.h"
typedef Bytef UInt8;

typedef struct THashValue{
    UInt8 digest[4];
} THashValue;

#define THashState  uLong
inline static void hash_init(THashState* hs){  *hs=crc32(0,0,0); }
inline static void hash_append(THashState* hs,const UInt8* data_begin,int len){
    *hs=crc32(*hs,data_begin,len); }
inline static void hash_finish(THashState* hs,UInt8* pCrc32){
    THashState crc32=*hs;
    pCrc32[0]=(UInt8)crc32;
    pCrc32[1]=(UInt8)(crc32>>8);
    pCrc32[2]=(UInt8)(crc32>>16);
    pCrc32[3]=(UInt8)(crc32>>24);
}
#endif


static const THashValue kEmptyHashValue={0};

inline static THashValue hash(const UInt8* data_begin,const UInt8* data_end){
    THashState hs;
    hash_init(&hs);
    hash_append(&hs,data_begin,(int)(data_end-data_begin));
    THashValue hv;
    hash_finish(&hs,&hv.digest[0]);
    return hv;
}


inline static void copyHash(UInt8* dst,const THashValue* srcHash){
    memcpy(dst,&srcHash->digest[0],sizeof(THashValue));
}

inline static void copyHash(THashValue* dstHash,const UInt8* buf_begin){
    memcpy(&dstHash->digest[0],buf_begin, sizeof(THashValue));
}

inline static int hash_is_equal(const THashValue* x,const THashValue* y){//result bool
    return 0==memcmp(x,y,sizeof(THashValue));
}


#endif /* hash_h */
