//
//  PatchUtil.hpp
//  patcher
//
//  Created by 陈卓 on 2017/7/13.
//  Copyright © 2017年 4A Volcano. All rights reserved.
//

#ifndef PatchUtil_h
#define PatchUtil_h

#include <iostream>
#include "types.h"
#include "../stl/BytesVector.h"
#include "../stl/CString.h"
#include <fstream>
#include <sys/stat.h>
#include "dirent.h"

namespace PatchUtil {
    using namespace std;

    static CString readString(TByte *&tBytePtr, int len) {
        if (len < 0) exit(-1);
        CString res;
        for (int i = 0; i < len; i++) {
            unsigned char ch=*tBytePtr;
            CString temp(ch);
            res += temp;
            tBytePtr++;
        }
        return res;
    }

    static TByte readByte(TByte *&tBytePtr) {
        TByte res = *tBytePtr;
        tBytePtr++;
        return res;
    }

    static int readInt(TByte *&tBytePtr) {
        int num = 0;
        num |= *tBytePtr;
        num |= (*(tBytePtr + 1) << 8);
        num |= (*(tBytePtr + 2) << 16);
        num |= (*(tBytePtr + 3) << 24);
        tBytePtr += 4;
        return num;
    }

    static CString readName(TByte *&tBytePtr) {
        int nameLen = readInt(tBytePtr);
        return readString(tBytePtr, nameLen);
    }

    static bool isFile(const char *filePath) {
        DIR * dir;
        dir = opendir(filePath);
        if (dir != NULL) return false; //是文件夹
        if (std::ifstream(filePath)) {
            closedir(dir);
            return true;
        }
        return false;
    }


    static bool isDirectory(const char *filePath) {
        DIR * dir;
        dir = opendir(filePath);
        if (dir == NULL) return false;
        auto dirent = readdir(dir);
        if (dirent->d_type == DT_DIR) {
            closedir(dir);
            return true;
        }
        return false;
    }

    static bool readFile(BytesVector &bytesVector, const char *filePath) {
        if (isDirectory(filePath)) return false;
        std::ifstream file(filePath, std::ios::in | std::ios::binary | std::ios::ate);
        std::streampos file_length = file.tellg();
        file.seekg(0, std::ios::beg);
        size_t needRead = (size_t) file_length;
        if ((file_length < 0) || ((std::streamsize) needRead != (std::streamsize) file_length)) {
            file.close();
            return false;
        }
        bytesVector.resize(needRead);
        char *begin = (char *) &(bytesVector[0]);
        file.read(begin, needRead);
        std::streamsize readed = file.gcount();
        file.close();
        if ((std::streamsize) needRead != readed)
            return false;
        return true;
    }

    static void writeFile(BytesVector &res, const char *filePath) {
        std::ofstream file(filePath, std::ios::out | std::ios::binary | std::ios::trunc);
        char *begin = (char *) &(res[0]);
        file.write(begin, res.size());
        file.close();
    }


    //将res输出到out中
    static void outVector(BytesVector &res, BytesVector &out) {
        for (int i = 0; i < res.size(); i++) {
            out.push_back(res[i]);
        }
    }

    static void writeToFileEnd(const char *filePath, BytesVector &fileBytes) {
        using namespace std;
        BytesVector tempBytes;
        PatchUtil::readFile(tempBytes, filePath);
        PatchUtil::outVector(fileBytes, tempBytes);
        PatchUtil::writeFile(tempBytes, filePath);
    }



    /**
     * 文件操作
     * @param str
     * @param suffix
     * @return
     */
    static bool has_suffix(const char *str, const char *suffix) {
        int strLen = strlen(str);
        int suffixLen = strlen(suffix);

        char *strSuffix = new char[suffixLen + 1];
        for (int i = 0; i < suffixLen; i++) {
            strSuffix[i]=str[i + strLen - suffixLen];
        }

        return strLen >= suffixLen &&
               strcmp(strSuffix, suffix) == 0;
    }

    static int findIndexInCharArray(const char *arr,char ch,int begin){
        int arrLen=strlen(arr);
        for(int i=begin;i<arrLen;i++){
            if(arr[i]==ch){
                return i;
            }
        }
        return -1;
    }
    static char* subString(const char * str,int begin,int len){
        char * res=new char[len+1];
        for(int i=0;i<len;i++){
            res[i]=str[begin+i];
        }
        res[len]='\0';
        return res;
    }

    static void createFileOrDir(const char *filePath) {
        using namespace std;
        int firstPos = 0;
        int pathLen=strlen(filePath);
        while (true) {
            int index = findIndexInCharArray(filePath,FILE_SEPERATOR_CHAR,firstPos);
            if (index == -1) {
                if (firstPos < pathLen) {
                    //如果不是以分隔符结束，则创建文件
//                    cout<<"make file"<<endl;
                    std::ofstream outFile;
                    outFile.open(filePath);
                    outFile.close();
                }
                break;
            }
            const char *tempPath=subString(filePath,0,index+1);
            if (isDirectory(tempPath)) {
                    //cout<<"isDir.."<<endl;
            } else if (isFile(tempPath)) {
                    //cout<<"isFile.."<<endl;
            } else {
                if (has_suffix(tempPath, FILE_SEPERATOR)) {
                    //cout<<"makeDir.."<<endl;
                    mkdir(tempPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                }
            }
            
//            cout<<tempPath<<endl;
            delete [] tempPath;
            firstPos = index + 1;
        }
    }
}


#endif /* PatchUtil_h */
