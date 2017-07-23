//
// Created by anyang on 2017/7/7.
//

#include "DiffManager.h"
#include "../pat_file.h"
#include "../util/DiffUtil.h"
#include <iostream>
#include "patch.h"
#include "stdarg.h"
#include <assert.h>
#include "sys/stat.h"
#include <map>
#include "fileutils.h"


//生成map<hash，文件相对路径>
void createFileHashMap(const std::vector<std::string> pathList, const std::string dir,
                       std::multimap<THashValue, std::string> *fileHashMap) {
    int a = 0;
    for (int i = 0; i < pathList.size(); i++) {
        std::vector<TByte> fileBytes;
        readFile(fileBytes, pathList[i].data());
        const THashValue fileBytesHash = hash(fileBytes);
        std::string relativePath = pathList[i];
        relativePath = relativePath.replace(relativePath.begin(), relativePath.begin() + dir.size(), "");
        fileHashMap->insert(std::make_pair(fileBytesHash, relativePath));
    }
}

/**
 * 输出Move/Copy部分数据
 *
 * 类型标记(1byte)
 * oldRelativePathLength (4byte)
 * oldRelativePath
 * newRelativePathLength (4byte)
 * newRelativePath
 * @param oldRelativePath
 * @param newRelativePath
 * @param fileInfoBytes
 */
void outMoveCopyData(std::string oldRelativePath, std::string newRelativePath, std::vector<TByte> *fileInfoBytes) {
    fileInfoBytes->push_back(MOVE_COPY);
    DiffUtil::outInt(oldRelativePath.size(), *fileInfoBytes);
    DiffUtil::outString(oldRelativePath, *fileInfoBytes);
    DiffUtil::outInt(newRelativePath.size(), *fileInfoBytes);
    DiffUtil::outString(newRelativePath, *fileInfoBytes);
}

/**
 * 输出Patch部分数据
 *
 * 类型标记(1byte)
 * newPathLength(4byte)
 * newPath
 * newFileDataLength(4byte)
 *
 * @param newRelativePath
 * @param newFileSize
 * @param fileInfoBytes
 */
void outPatchData(std::string newRelativePath,int newFileSize,std::vector<TByte> *fileInfoBytes){
    fileInfoBytes->push_back(PATCH);
    DiffUtil::outInt(newRelativePath.size(), *fileInfoBytes);
    DiffUtil::outString(newRelativePath, *fileInfoBytes);
    DiffUtil::outInt(newFileSize, *fileInfoBytes);
}


/**
 * 1.获取oldDir和newDir下的文件目录
 * 2.根据目录获取oldFileMap和newFileMao<hash,文件相对路径>
 * 3.遍历newFileMap，如果在oldFileMap中有key相同的值，说明文件数据相同，文件名可能不同，这部分数据作为copy/move部分存入fileInfoBytes
 * 3.newFileMap中剩余部分文件数据合并为newFileBytes，另将这部分数据作为patch部分，存入fileInfoBytes中
 * 4.将oldRefFiles，fileInfoBytes，patch数据按照顺序存入outBytes中
 * @param oldDirPath 旧版文件路径
 * @param newDirPath 新版文件路径
 * @param outBytes 补丁包数据
 */
void DiffManager::diffDir(const char* oldDirPath, const char * newDirPath, std::vector<TByte> *outBytes) {
    using namespace std;

    vector<string> oldFilePathList;
    listFileRecursively(oldDirPath, oldFilePathList);
    multimap<THashValue, string> oldFileMap;
    createFileHashMap(oldFilePathList, oldDirPath, &oldFileMap);

    vector<string> newFilePathList;
    listFileRecursively(newDirPath, newFilePathList);
    multimap<THashValue, string> newFileMap;
    createFileHashMap(newFilePathList, newDirPath, &newFileMap);
    cout << "---------------------" << endl;
    vector<TByte> fileInfoBytes;

    vector<TByte> newFilesBytes;
    int move_copy_count=0;
    int patch_count=0;
    for (auto it = newFileMap.begin(); it != newFileMap.end();) {
        auto oldIt = oldFileMap.find(it->first);
        if (oldIt != oldFileMap.end()) {
//            cout << "newPath:" << it->second << endl;
            string newPath = it->second;
            int fileCount = oldFileMap.count(it->first);
            for (int i = 0; i < fileCount; i++) {
//                cout << "oldPath:" << oldIt->second << endl;
                string oldPath = oldIt->second;
                //输出MOVE/COPY的数据
                outMoveCopyData(oldPath, newPath, &fileInfoBytes);
                oldIt++;
                move_copy_count++;
            }
            newFileMap.erase(it++);
        } else {
            vector<TByte> fileBytes;
            readFile(fileBytes, newDirPath + it->second);
            DiffUtil::outVector(fileBytes, newFilesBytes);
            //cout << "new file:"<<it->second <<"    size:"<<fileBytes.size()<< endl;
            //输出patch数据
            outPatchData(it->second,fileBytes.size(),&fileInfoBytes);
            it++;
            patch_count++;
        }
    }


    vector<TByte> oldFilesBytes;
    vector<TByte> oldRefFilesPathBytes;
    for (auto it = oldFileMap.begin(); it != oldFileMap.end(); it++) {
        vector<TByte> fileBytes;
        readFile(fileBytes, oldDirPath + it->second);
        DiffUtil::outVector(fileBytes, oldFilesBytes);
        //生成oldRefFiles路径列表
        DiffUtil::outInt(it->second.size(), oldRefFilesPathBytes);
        DiffUtil::outString(it->second, oldRefFilesPathBytes);
    }

    vector<TByte> diffBytes;
    DiffUtil::doDiff(&newFilesBytes, &oldFilesBytes, &diffBytes);


    //输出diff的数据

    //1.将oldRefFiles部分的长度存入头部
    vector<TByte> oldRefFilesCountBytes=DiffUtil::intToBytes(oldFileMap.size());
    oldRefFilesPathBytes.insert(oldRefFilesPathBytes.begin(),oldRefFilesCountBytes.begin(),
                                oldRefFilesCountBytes.end());
    vector<TByte> oldRefFilesLengthBytes = DiffUtil::intToBytes(oldRefFilesPathBytes.size());
    oldRefFilesPathBytes.insert(oldRefFilesPathBytes.begin(), oldRefFilesLengthBytes.begin(),
                                oldRefFilesLengthBytes.end());


    //2.将fileInfo长度插入fileInfo头部
    vector<TByte> patchCountBytes=DiffUtil::intToBytes(patch_count);
    vector<TByte> moveCopyCountBytes=DiffUtil::intToBytes(move_copy_count);
    fileInfoBytes.insert(fileInfoBytes.begin(), patchCountBytes.begin(), patchCountBytes.end());
    fileInfoBytes.insert(fileInfoBytes.begin(), moveCopyCountBytes.begin(), moveCopyCountBytes.end());
    cout<<"patchCount:"<<patch_count<<endl;
    cout<<"moveCopyCount:"<<move_copy_count<<endl;


    vector<TByte> fileInfoLengthBytes = DiffUtil::intToBytes(fileInfoBytes.size());
    fileInfoBytes.insert(fileInfoBytes.begin(), fileInfoLengthBytes.begin(), fileInfoLengthBytes.end());

    //将oldRefFiles数据输出
    DiffUtil::outVector(oldRefFilesPathBytes, *outBytes);
    //将fileInfo数据输出
    DiffUtil::outVector(fileInfoBytes, *outBytes);
    //patch数据输出
    DiffUtil::outVector(diffBytes, *outBytes);
    
    cout << "newFilesBytesSize:" << newFilesBytes.size() << endl;
    cout << "oldFilesBytesSize:" << oldFilesBytes.size() << endl;
    cout<<"oldRefInfoSize:"<<oldRefFilesPathBytes.size()<<endl;
    cout << "diffSize:" << diffBytes.size() << endl;
}


