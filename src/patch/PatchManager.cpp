//
// Created by anyang on 2017/7/17.
//

#include "patch.h"
#include "PatchManager.h"
#include "../util/PatchUtil.h"
#include "../util/patchStream.h"

void createDir(CString path) {
    FILE *fp = NULL;
    fp = fopen(path.c_str(), "w");

    if (!fp) {
        mkdir(path.c_str(), 0775);
    } else {
        fclose(fp);
    }
}

void PatchManager::readRefFilesPaths(TByte *&tBytePtr) {
    using namespace std;
    int dataLen = PatchUtil::readInt(tBytePtr);
    TByte *dataEndPos = tBytePtr + dataLen;
    this->oldRefFilesCount = PatchUtil::readInt(tBytePtr);
    this->oldRefFilesPtr = new char *[this->oldRefFilesCount];
    cout << "oldRefFilesCount:" << oldRefFilesCount << endl;
    int oldRefIndex = 0;
    while (tBytePtr < dataEndPos) {
        int filePathLen = PatchUtil::readInt(tBytePtr);
        CString refFilePath = PatchUtil::readString(tBytePtr, filePathLen);

        oldRefFilesPtr[oldRefIndex] = new char[refFilePath.size() + 1];
        strcpy(oldRefFilesPtr[oldRefIndex], refFilePath.c_str());
        oldRefFilesPtr[oldRefIndex][refFilePath.size()] = '\0';
        oldRefIndex++;
//        cout<<oldRefFilesPtr[oldRefIndex-1]<<endl;
//        pathList.push_back(refFilePath);
//        cout << "引用的oldDir下的文件:" << refFilePath << endl;
    }

//    for(int i=0;i<oldRefFilesCount;i++){
//        cout<<oldRefFilesPtr[i]<<endl;
//    }
}

void PatchManager::readFileInfos(TByte *&tBytePtr) {
    using namespace std;
    int fileInfoLen = PatchUtil::readInt(tBytePtr);
    TByte *fileInfoEnd = tBytePtr + fileInfoLen;
    int moveCopyCount = PatchUtil::readInt(tBytePtr);
    int patchCount = PatchUtil::readInt(tBytePtr);

    if (this->moveCopyInfoPtr != nullptr) delete[]moveCopyInfoPtr;
    if (this->patchInfoPtr != nullptr) delete[]patchInfoPtr;

    this->moveCopyInfoPtr = new MoveCopyInfo[moveCopyCount];
    this->patchInfoPtr = new PatchInfo[patchCount];

    int moveCopyIndex = 0;
    int patchIndex = 0;
    while (tBytePtr < fileInfoEnd) {
        TByte action = PatchUtil::readByte(tBytePtr);
        if (action == PATCH) {
            //表明数据是patch结构  需要利用patch数据进行还原
            CString newFilePath = PatchUtil::readName(tBytePtr);
            int dataSize = PatchUtil::readInt(tBytePtr);

            PatchInfo *patchInfo = new PatchInfo(newFilePath, dataSize);
            patchInfoPtr[patchIndex++] = *patchInfo;

//            cout << "patch文件" << endl;
//            cout << "newFilePath:" << newFilePath << endl;
//            cout << "newFileSize:" << dataSize << endl;
        } else if (action == MOVE_COPY) {
            //后面数据的结构属于 move/copy
            CString oldPath = PatchUtil::readName(tBytePtr);
            CString newPath = PatchUtil::readName(tBytePtr);

            MoveCopyInfo *moveCopyInfo = new MoveCopyInfo(oldPath, newPath);
            moveCopyInfoPtr[moveCopyIndex++] = *moveCopyInfo;

//            cout << "复制移动文件" << endl;
//            cout << "oldPath:\t" << oldPath << endl;
//            cout << "newPath:\t" << newPath << endl;
        } else {
            //不应该进行到这里
            cout << "[ERR] 不支持的action: " << (int) action << endl;
            continue;
        }
    }
    this->moveCopyCount = moveCopyIndex;
    this->patchInfoCount = patchIndex;
}


void PatchManager::moveCopyFile() {
    using namespace std;
    for (int i = 0; i < moveCopyCount; i++) {
        BytesVector fileBytes;
        CString oldPath = getBasePath();
        oldPath += moveCopyInfoPtr[i].oldPath;
        CString newPath = getTmpOutputPath();
        newPath += moveCopyInfoPtr[i].newPath;

        PatchUtil::readFile(fileBytes, oldPath.c_str());
        
        PatchUtil::createFileOrDir(newPath.c_str());
        
        PatchUtil::writeFile(fileBytes, newPath.c_str());
    }
}


void PatchManager::patchDir(const char *patchFilePath) {
    using namespace std;
    BytesVector patchBytes;
    PatchUtil::readFile(patchBytes, patchFilePath);

    const TByte *patchBytesBegin = &patchBytes[0];
    const TByte *patchBytesEnd = &patchBytes[patchBytes.size() - 1];


    createDir(getTmpOutputPath());

    cout << "---------开始对dir进行patch---------------" << endl;
    TByte *byte_iter = (TByte *) patchBytesBegin;
    //解析refFilesPath部分
    readRefFilesPaths(byte_iter);
    //解析FileInfo
    readFileInfos(byte_iter);
    //复制移动/重命名文件
    moveCopyFile();

    //解析patchData部分
    cout << "--------解析patchData部分-------" << endl;
    u_long loadedSize = byte_iter - patchBytesBegin;
    cout << "patchDataSize: " << patchBytesEnd - byte_iter + 1 << endl;


    TFilesStreamInput oldsRefInput(this->oldRefFilesPtr, this->oldRefFilesCount, this->basePath);
    TFileStreamInput patchInput(patchFilePath);
    patchInput.setHeadSize(byte_iter - patchBytesBegin);
    TFileStreamOutput patchOutPut(patchInfoPtr, this->patchInfoCount, this->getTmpOutputPath());
    if (!patch_stream(&patchOutPut, &oldsRefInput, &patchInput)) {
        cout << "patch failed" << endl;
        exit(3);
    }

//    free memory
//    vector<string>().swap(oldRefFiles);
//    CleanPtrVector(&patchInfoList);
    //TODO delete moveCopyInfoList
}

char *PatchManager::getBasePath() const {
    return basePath;
}

void PatchManager::setBasePath(char *basePath) {
    PatchManager::basePath = basePath;
}

char *PatchManager::getTmpOutputPath() const {
    return tmpOutputPath;
}

void PatchManager::setTmpOutputPath(char *tmpOutputPath) {
    PatchManager::tmpOutputPath = tmpOutputPath;
}


