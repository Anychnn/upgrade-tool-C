//
//  main.cpp
//  differTests
//
//  Created by 陈卓 on 2017/7/11.
//  Copyright © 2017年 4A Volcano. All rights reserved.
//

#include <iostream>
#include "gtest/gtest.h"
#include "Util.h"
#include "util/types.h"
#include "util/fileutils.h"
#include "DiffManager.h"
#include "PatchManager.h"
#include "DiffUtil.h"
 using namespace std;

//old 文件夹路径
char* oldDir = "/Users/kaifashi/Documents/DirDiffPatch/test_resources/HJD_V3.4.0/";
//new文件夹路径
char * newDir = "/Users/kaifashi/Documents/DirDiffPatch/test_resources/HJD_V3.4.3/";
//patch包的路径
char * patchFilePath="/Users/kaifashi/Documents/DirDiffPatch/test_resources/patch.patch";
//temp目录，对old进行patch的输出目录
char* tempDir = "/Users/kaifashi/Documents/DirDiffPatch/test_resources/temp/";

char* testdir = "/Users/kaifashi/Documents/DirDiffPatch/test_resources/test/";

//测试完成删除目录
bool deleteFilesAfterTest = true;

TEST(ExampleTests,SimepleAdd) {
        EXPECT_EQ(3, 1+2);
}

TEST(DiffTests, Int2ByteTest){
    vector<TByte> bytes = DiffUtil::intToBytes(16909060);
    EXPECT_EQ(bytes[0], 4);
    EXPECT_EQ(bytes[1], 3);
    EXPECT_EQ(bytes[2], 2);
    EXPECT_EQ(bytes[3], 1);
    vector<TByte> bytes2 = DiffUtil::intToBytes(6968);
}

TEST(PatchTest, BytesToInt){
    vector<TByte> bytes;
    bytes.push_back(4);
    bytes.push_back(3);
    bytes.push_back(2);
    bytes.push_back(1);
    EXPECT_EQ(16909060, DiffUtil::bytesToInt(bytes));
}

TEST(DiffPatchTest,DiffTest){
    DiffManager diffManager;
    std::vector<TByte> diff_bytes;
    //获取diff数据
    diffManager.diffDir(oldDir, newDir, &diff_bytes);
    //输出diff到文件
    writeFile(diff_bytes,patchFilePath);

    PatchManager patchManager;
    //设置basePath路径
    patchManager.setBasePath(oldDir);

    //设置temp输出路径
    patchManager.setTmpOutputPath(tempDir);

    //patch到dir
    patchManager.patchDir(patchFilePath);

    //检查patch后的文件是否相同
    vector<string> tempList;
    listFileRecursively(tempDir, tempList);
    vector<string> newDirList;
    listFileRecursively(newDir, newDirList);
    EXPECT_EQ(newDirList.size(), tempList.size());

    for(int i=0;i<tempList.size();i++){
        vector<TByte> tempBytes;
        readFile(tempBytes, tempList[i]);
        vector<TByte> newBytes;
        readFile(newBytes, newDirList[i]);
        for(int j=0;j<tempBytes.size();j++){
            EXPECT_EQ(tempBytes[j], newBytes[j]);
        }
        EXPECT_EQ(tempBytes.size(), newBytes.size());
    }

    if(deleteFilesAfterTest){
        //TODO 递归 删除所有文件和文件夹
        cout<<remove(tempDir)<<endl;
        remove(patchFilePath);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
