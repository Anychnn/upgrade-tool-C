//
// Created by anyang on 2017/7/4.

#include <iostream>
#include "diff/DiffManager.h"
#include "util/DiffUtil.h"
#include <map>
#include <dirent.h>
#include <fstream>
#include "util/common.h"
#include "patch/PatchManager.h"
#include "fileutils.h"
typedef unsigned char TByte;

int main(int argc, const char *argv[]) {
    using namespace std;
//
//    //TODO 处理路径尾部的/ 如果没有 需要加上 路径需要考虑操作系统
    char * oldDir = "/Users/kaifashi/Documents/DirDiffPatch/test_resources/HJD_V3.4.0/";
    char * newDir = "/Users/kaifashi/Documents/DirDiffPatch/test_resources/HJD_V3.4.3/";
////	std::string oldDir = string_format("%s%s%s", getenv("HOME"), FILE_SEPERATOR.c_str(), "utilProjects/testdata/old/");
////    std::string newDir = string_format("%s%s%s", getenv("HOME"), FILE_SEPERATOR.c_str(), "utilProjects/testdata/new/");
//
    char * patchFilePath="/Users/kaifashi/Documents/DirDiffPatch/test_resources/patch.patch";
    DiffManager diffManager;
    std::vector<TByte> diff_bytes;
    //获取diff数据
    diffManager.diffDir(oldDir, newDir, &diff_bytes);

    writeFile(diff_bytes,((std::string)oldDir+"../patch.patch").data());

    PatchManager patchManager;
    //设置basePath路径
    patchManager.setBasePath(oldDir);

    //设置temp输出路径
    string tempOutPutPath= (string)oldDir + ".." + FILE_SEPERATOR + "temp"+FILE_SEPERATOR;
    patchManager.setTmpOutputPath((char *) tempOutPutPath.data());

    //patch到dir
    patchManager.patchDir(patchFilePath);
//    cout<<oldDir<<endl;
    std::cout<<"diff-patch完毕"<<std::endl;


//    char * testDir = "/Users/kaifashi/Documents/DirDiffPatch/test_resources/temp/test/test.txt";
//    PatchUtil::createFileOrDir(testDir);
    return 0;
}
