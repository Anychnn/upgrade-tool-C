//
//  main.cpp
//  patchTests
//
//  Created by 陈卓 on 2017/7/12.
//  Copyright © 2017年 4A Volcano. All rights reserved.
//

#include <iostream>
#include "gtest/gtest.h"
#include "util/PatchUtil.h"
#include "util/common.h"

using namespace std;

u_long GetIteratorPos(TByteIterator *it){
    return (u_long)&**it;
}

vector<TByte> *makeBytesVector(TByte *bytes, int len){
    vector<TByte> *vec = new vector<TByte>(bytes, bytes + len);
    return vec;
}

void VerifyInt(TByteIterator *currentPos, int benchmarkInt){
    u_long beginPos = GetIteratorPos(currentPos);
    u_long shouldEndPos = GetIteratorPos(currentPos) + 4;
    int result = readInt(currentPos);
    cout << "<Int> Iter Move @ " << beginPos << " -> " << GetIteratorPos(currentPos) << endl;
    EXPECT_EQ(benchmarkInt, result);
    EXPECT_EQ(shouldEndPos, GetIteratorPos(currentPos));
}

void VerifyByte(TByteIterator *currentPos, TByte benchmarkByte){
    u_long beginPos = GetIteratorPos(currentPos);
    u_long shouldEndPos = GetIteratorPos(currentPos) + 1;
    TByte result = readByte(currentPos);
    cout << "<Byte> Iter Move @ " << beginPos << " -> " << GetIteratorPos(currentPos) << endl;
    EXPECT_EQ(benchmarkByte, result);
    EXPECT_EQ(shouldEndPos, GetIteratorPos(currentPos));
}

void VerifyString(TByteIterator *currentPos, int len, string benchmarkString){
    u_long beginPos = GetIteratorPos(currentPos);
    u_long shouldEndPos = GetIteratorPos(currentPos) + len;
    string result = readString(currentPos, len);
    cout << "<String> Iter Move @ " << beginPos << " -> " << GetIteratorPos(currentPos) << endl;
    EXPECT_EQ(benchmarkString, result);
    EXPECT_EQ(shouldEndPos, GetIteratorPos(currentPos));
}

void VerifyPatchFileInfo(PatchInfo *info, string expectedNewPath, int expectedSize){
    EXPECT_EQ(expectedNewPath, info->newPath);
    EXPECT_EQ(expectedSize, info->size);
}

void VerifyMoveCopyFileInfo(const MoveCopyInfo *info, string expectedOldPath, string expectedNewPath){
    EXPECT_EQ(expectedOldPath, info->oldPath);
    EXPECT_EQ(expectedNewPath, info->newPath);
}

/*
 * 测试读取Int值，测试数据共8字节，包含两个Int值
 * 同时检验读取结果是否正确以及指针偏移是否正确
 */
TEST(PatchTests, ReadIntTest){
    TByte arrBytes[] = {4,3,2,1,1,2,3,4};
    vector<TByte> *bytes = makeBytesVector(arrBytes, sizeof(arrBytes));
    TByteIterator it = bytes->begin();
    VerifyInt(&it, 16909060);
    VerifyInt(&it, 67305985);
    delete bytes;
}

/*
 * 测试读取Byte值，验证值是否正确读书，指针前进是否正确
 */
TEST(PatchTest, ReadByteTest){
    TByte arrBytes[] = {0,1,127};
    vector<TByte> *bytes = makeBytesVector(arrBytes, sizeof(arrBytes));
    TByteIterator it = bytes->begin();
    VerifyByte(&it, 0);
    VerifyByte(&it, 1);
    VerifyByte(&it, 127);
    delete bytes;
}

/*
 * 测试读取String值，分两次获取完整字符串
 * 每次获取子字符串，都会测试结果和指针偏移是否正确
 */
TEST(PatchTests, ReadStringTest){
    string testString = "I hate C++ 中文";
    const u_long stringLength = testString.length();
    const char *stringBytes = testString.c_str();
    vector<TByte> bytes(stringBytes, stringBytes + stringLength);
    
    cout << "string len: " << testString.length() << ", stringBytes len: " << sizeof(stringBytes) << endl;
    cout << "string bytes (len " << stringLength << "): " << endl;
    for(TByteIterator it = bytes.begin(); it != bytes.end(); it++){
        cout << *it << "(" << (int)*it << ")";
        if (it != bytes.end() - 1){
            cout << ", ";
        }
    }
    cout << endl;
    
    TByteIterator it = bytes.begin();
    VerifyString(&it, 6, "I hate");
    VerifyString(&it, (int)stringLength - 6, " C++ 中文");
}

TEST(PatchTests, ReadRefFilePathsTest){
    TByte arrBytes[] = {
        15, 0, 0, 0, // data length
        3, 0, 0, 0, 'a', 'b', 'c', // path 1
        4, 0, 0, 0, 'd', 'e', 'f', 'g' // path 2
    };
    vector<TByte> *bytes = makeBytesVector(arrBytes, sizeof(arrBytes));
    TByteIterator it = bytes->begin();
    vector<string> result;
    readRefFilePaths(&it, &result);
    delete bytes;
    
    EXPECT_EQ("abc", result[0]);
    EXPECT_EQ("defg", result[1]);
    
    vector<string>().swap(result);
}

vector<const MoveCopyInfo*> moveCopyInfos;
void CollectMoveCopyFilePathList(const MoveCopyInfo* moveCopyInfo, const PatchUtil* util){
    moveCopyInfos.push_back(moveCopyInfo);
}

TEST(PatchTests, ReadFileInfosTest){
    TByte arrBytes[] = {
        61, 0, 0, 0, // data length
        /******** FILE INFO 1 ********/
        1 /* PATCH */,
        3, 0, 0, 0, /* PATH SIZE */ 'a', 'b', 'c', /* PATH */
        4, 3, 2, 1, /* SIZE: 16909060 */
        /******** FILE INFO 2 ********/
        2 /* MOVE/COPY */,
        3, 0, 0, 0, /* OLD PATH SIZE */ 'u', 'v', 'w', /* OLD PATH */
        3, 0, 0, 0, /* NEW PATH SIZE */ 'x', 'y', 'z', /* NEW PATH */
        /******** FILE INFO 3 ********/
        1 /* PATCH */,
        3, 0, 0, 0, /* PATH SIZE */ 'n', 'e', 'w', /* PATH */
        1, 2, 3, 4, /* SIZE: 67305985 */
        /******** FILE INFO 4 ********/
        2 /* MOVE/COPY */,
        7, 0, 0, 0, /* OLD PATH SIZE */ 'i', '\'', 'm', ' ', 'b', 'a', 'd', /* OLD PATH */
        6, 0, 0, 0, /* NEW PATH SIZE */ 228, 184, 173, 230, 150, 135 /* NEW PATH */
    };
    vector<TByte> *bytes = makeBytesVector(arrBytes, sizeof(arrBytes));
    TByteIterator it = bytes->begin();
    
    string home = getenv("HOME");
    string sub = "testfir";
    string dir = home + sub;
    mkdir(dir.c_str(), 777);
    
    vector<PatchInfo*> patchInfoList;
    PatchUtil util("");
    MoveCopyCallbackPtr callbackPtr = &CollectMoveCopyFilePathList;
    readFileInfos(&it, &util, &patchInfoList, callbackPtr);
    delete bytes;
    
    VerifyPatchFileInfo(patchInfoList[0], "abc", 16909060);
    VerifyPatchFileInfo(patchInfoList[1], "new", 67305985);
    VerifyMoveCopyFileInfo(moveCopyInfos[0], "uvw", "xyz");
    VerifyMoveCopyFileInfo(moveCopyInfos[1], "i'm bad", "中文");
    
    CleanPtrVector(&patchInfoList);
    CleanPtrVector(&moveCopyInfos);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
