//
// Created by anyang on 2017/7/17.
//

#ifndef TESTC_PATCHMANAGER_H
#define TESTC_PATCHMANAGER_H


#include "../util/types.h"
#include "../stl/CString.h"

/*
 * 通过补丁文件patch出新文件的操作信息
 */
typedef struct PatchInfo {
    CString newPath;
    int size;

    PatchInfo(CString newPath="", int size=0){
        this->newPath = newPath;
        this->size = size;
    }
} PatchInfo;
/*
 * 移动或复制旧文件的操作信息
 */
typedef struct MoveCopyInfo {
    CString oldPath;
    CString newPath;

    MoveCopyInfo(CString oldPath="", CString newPath=""){
        this->oldPath = oldPath;
        this->newPath = newPath;
    }
} MoveCopyInfo;


class PatchManager {
public:

    void patchDir(const char *patchFilePath);

    char *getBasePath() const;

    void setBasePath(char *basePath);

    char *getTmpOutputPath() const;

    void setTmpOutputPath(char *tmpOutputPath);

    ~PatchManager(){
        if(this->moveCopyInfoPtr!= nullptr){
            delete []moveCopyInfoPtr;
        }
        if(this->patchInfoPtr!= nullptr){
            delete []patchInfoPtr;
        }
        if(this->oldRefFilesPtr!= nullptr){
            for(int i=0;i<oldRefFilesCount;i++){
                delete[] oldRefFilesPtr[i];
            }
            delete []oldRefFilesPtr;
        }
    }

private:
    char * basePath;
    char * tmpOutputPath;


    //引用的文件数据
    char **oldRefFilesPtr= nullptr;
    int oldRefFilesCount=0;

    //需要复制 重命名的文件
    MoveCopyInfo *moveCopyInfoPtr= nullptr;
    //patch的文件
    PatchInfo *patchInfoPtr= nullptr;
    int moveCopyCount=0;
    int patchInfoCount=0;

    void readRefFilesPaths(TByte *&tBytePtr);
    void readFileInfos(TByte *&tBytePtr) ;
    void moveCopyFile();

};


#endif //TESTC_PATCHMANAGER_H
