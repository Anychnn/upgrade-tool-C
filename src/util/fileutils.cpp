//
//  fileutils.cpp
//  patcher
//
//  Created by 陈卓 on 2017/7/14.
//  Copyright © 2017年 4A Volcano. All rights reserved.
//

#include "fileutils.h"

bool has_suffix(const std::string &str, const std::string &suffix) {
    return str.size() >= suffix.size() &&
    str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

void listFileRecursively(const std::string dirPtr, std::vector<std::string> &res) {
    using namespace std;
    string FILE_SEPERATOR = "/";
    DIR *dir;
    if ((dir = opendir(dirPtr.data())) == NULL) {
        //如果是文件 加入列表中
        res.push_back(dirPtr);
        //std::cout<<dirPtr<<std::endl;
        return;
    }
    struct dirent *ptr;
    bool hasFileInDir=false;    //当前文件夹下是否有文件
    while ((ptr = readdir(dir)) != NULL) {
        std::string path;
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0|| strcmp(ptr->d_name, SYS_FILE_SUFFIX) == 0) {
            continue;
        }
        hasFileInDir=true;
        if (has_suffix(dirPtr, FILE_SEPERATOR)) {
            path += dirPtr;
            path += ptr->d_name;
        } else {
            path += dirPtr;
            path += FILE_SEPERATOR;
            path += ptr->d_name;
        }
        //递归
        listFileRecursively(path.data(), res);
    }
    if(!hasFileInDir){
        if(has_suffix(dirPtr, FILE_SEPERATOR)){
            res.push_back(dirPtr);
        }else{
            res.push_back(dirPtr+FILE_SEPERATOR);
        }
    }
    
}

bool readFile(std::vector<TByte> &data, const std::string fileName) {
    if(isDirectory(fileName.data())) return false;
    std::ifstream file(fileName, std::ios::in | std::ios::binary | std::ios::ate);
    std::streampos file_length = file.tellg();
    file.seekg(0, std::ios::beg);
    size_t needRead = (size_t) file_length;
    if ((file_length < 0) || ((std::streamsize) needRead != (std::streamsize) file_length)) {
        file.close();
        return false;
    }
    data.resize(needRead);
    file.read((char *) &data[0], needRead);
    std::streamsize readed = file.gcount();
    file.close();
    if ((std::streamsize) needRead != readed)
        return false;
    return true;
}

void writeFile(const std::vector<TByte> &data, const char *fileName) {
    std::ofstream file(fileName, std::ios::out | std::ios::binary | std::ios::trunc);
    file.write((const char *) &data[0], data.size());
    file.close();
}


bool isFileOrDirExist(const char *filePath) {
    FILE *file = fopen(filePath, "rb");
    if (file == NULL) {
        return false;
    }
    fclose(file);
    return true;
}

void createFileOrDir(const char *filePath) {
    using namespace std;
    std::string path = filePath;
    int firstPos=0;
    while(true){
        int index=path.find(FILE_SEPERATOR,firstPos);
        if(index==-1){
            if(firstPos<path.size()){
                //                cout<<"make file"<<endl;
                std::ofstream outFile;
                outFile.open(filePath);
                outFile.close();
            }
            break;
        }
        string tempPath=path.substr(0,index+1);
        if(isDirectory(tempPath.data())){
            //            cout<<"isDir.."<<endl;
        }else if(isFile(tempPath.data())){
            //            cout<<"isFile.."<<endl;
        }else{
            if(has_suffix(tempPath,FILE_SEPERATOR)){
                //                cout<<"makeDir.."<<endl;
                mkdir(tempPath.data(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            }
        }
        //        cout<<tempPath<<endl;
        firstPos=index+1;
    }
    
}

bool isDirectory(const char *filePath) {
    DIR *dir;
    dir = opendir(filePath);
    if (dir == NULL) return false;
    auto dirent = readdir(dir);
    if (dirent->d_type == DT_DIR) {
        closedir(dir);
        return true;
    }
    return false;
}

bool isFile(const char *filePath) {
    DIR *dir;
    dir = opendir(filePath);
    if (dir != NULL) return false; //是文件夹
    if (std::ifstream(filePath)) {
        closedir(dir);
        return true;
    }
    return false;
}

void writeToFileEnd(const char * filePath, std::vector<TByte> fileBytes) {
    using namespace std;
    vector<TByte> oldFileBytes;
    readFile(oldFileBytes,filePath);
//    DiffU

}
