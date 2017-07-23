//
//  fileutils.h
//  patcher
//
//  Created by 陈卓 on 2017/7/14.
//  Copyright © 2017年 4A Volcano. All rights reserved.
//

#ifndef fileutils_h
#define fileutils_h

#include <vector>
#include <string>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include "types.h"

//遍历文件夹获取所有文件路径列表
void listFileRecursively(const std::string dirPtr, std::vector<std::string> &res) ;
bool readFile(std::vector<TByte>& data,const std::string fileName);
void writeFile(const std::vector<TByte>& data,const char* fileName);
bool isFileOrDirExist(const char *filePath);
//创建文件或者目录  如果是多层目录不存在 就创建多层目录
void createFileOrDir(const char *filePath);
bool isDirectory(const char *filePath);
bool isFile(const char *filePath);
//将数据写入文件尾部
void writeToFileEnd(const std::string,std::vector<TByte> fileBytes);
#endif /* fileutils_h */
