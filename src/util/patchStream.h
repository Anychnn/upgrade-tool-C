//
//  filestream.h
//  differ
//
//  Created by 陈卓 on 2017/7/14.
//  Copyright © 2017年 4A Volcano. All rights reserved.
//

#ifndef filestream_h
#define filestream_h

#include <iostream>
#include "patch.h"
#include "PatchManager.h"

#define IS_USES_PATCH_STREAM

inline static hpatch_StreamPos_t getFilePos64(FILE *file) {
    fpos_t pos;
    int rt = fgetpos(file, &pos); //support 64bit?
#if defined(__linux) || defined(__linux__)
    return pos.__pos;
#else //windows macosx
    return pos;
#endif
}

inline void setFilePos64(FILE *file, hpatch_StreamPos_t seekPos) {
    fpos_t pos;
#if defined(__linux) || defined(__linux__)
    memset(&pos, 0, sizeof(pos));
    pos.__pos=seekPos; //safe?
#else //windows macosx
    pos = seekPos;
#endif
    int rt = fsetpos(file, &pos); //support 64bit?
}

struct TFilesStreamInput : public hpatch_TStreamInput {
    TFilesStreamInput(char **oldRefFiles, int oldRefFilesCount, CString dir) : streamPos(0) {
        hpatch_StreamPos_t totalLength = 0;//记录总长度
        BytesVector fileListBytes;

        int *fileSizeList = new int[oldRefFilesCount];
        for (int i = 0; i < oldRefFilesCount; i++) {
            CString readPath;
            readPath += dir;
            readPath += oldRefFiles[i];
            FILE *file = fopen(readPath.c_str(), "rb");
            fseek(file, 0, SEEK_END);
            fpos_t pos;
            fgetpos(file, &pos);
            fileSizeList[i] = pos;
            fclose(file);

            totalLength += pos;
//            std::cout << pos << std::endl;
        }
//        std::cout<<"total:"<<totalLength<<std::endl;
        this->streamHandle = this;
        this->streamSize = totalLength;
        this->read = read_file;
        this->fileSizeList = fileSizeList;
        this->dir = dir;
        this->oldFilesCount = oldRefFilesCount;
        this->oldRefFiles = oldRefFiles;
    }
    
    void getPosToFile(unsigned long long readFromPos,int *indexOfFile,int *indexOfBytes){
        for(int i=0;i<oldFilesCount;i++){
            if(readFromPos>=fileSizeList[i]){
                readFromPos-=fileSizeList[i];
            }else{
                *indexOfFile=i;
                *indexOfBytes=readFromPos;
//                std::cout<<readFromPos<<std::endl;
                return;
            }
        }
    }

    static long read_file(hpatch_TStreamInputHandle streamHandle, const hpatch_StreamPos_t readFromPos,
                          unsigned char *out_data, unsigned char *out_data_end) {
        TFilesStreamInput &fileStreamInput = *(TFilesStreamInput *) streamHandle;
//        std:cout<<fileStreamInput.oldFilesCount<<std::endl;
        int *fileSizeList = fileStreamInput.fileSizeList;
        long needToRead = out_data_end - out_data;
        int indexOfFile;
        int indexOfBytes;
        fileStreamInput.getPosToFile(readFromPos, &indexOfFile, &indexOfBytes);
        
        long readed = 0;
        while (needToRead > 0) {
            CString filePath;
            filePath+=fileStreamInput.dir;
            filePath+=fileStreamInput.oldRefFiles[indexOfFile];
//            cout<<filePath<<endl;
            FILE *file = fopen(filePath.c_str(), "rb");
            fseek(file, indexOfBytes, SEEK_SET);
            int currentFileLeft = fileSizeList[indexOfFile] - indexOfBytes;
            if (needToRead >= currentFileLeft) {
                //该文件能读完
                int currentFileReaded = fread(out_data + readed, 1, currentFileLeft, file);
                indexOfFile++;
                indexOfBytes = 0;
                needToRead -= currentFileReaded;
                readed += currentFileReaded;
            } else {
                //该文件只能读一部分 needToRead比这部分数据要小
                int currentFileReaded = fread(out_data + readed, 1, needToRead, file);
                needToRead = 0;
                readed += currentFileReaded;
                indexOfBytes += currentFileReaded;
            }
            fclose(file);
        }

        fileStreamInput.indexOfFile=indexOfFile;
        fileStreamInput.indexOfBytes=indexOfBytes;
        return readed;
    }

    int *fileSizeList;
    char **oldRefFiles;             //这个数据是从PatchManager传入的
    int oldFilesCount = 0;
    int indexOfFile = 0;
    int indexOfBytes = 0;
    CString dir;                    //文件夹路径
    hpatch_StreamPos_t streamPos;      //输入流的位置
    ~TFilesStreamInput() {
        delete[] fileSizeList;
    }
};

struct TFileStreamInput : public hpatch_TStreamInput {
    explicit TFileStreamInput(const char *fileName) : m_file(0), m_offset(0), m_fpos(0) {
        m_file = fopen(fileName, "rb");
        if (m_file == 0) {
            std::cout << "Patch file not found: " << fileName << std::endl;
            exit(1);
        }
        fseek(m_file, 0, SEEK_END);
        hpatch_StreamPos_t file_length = getFilePos64(m_file);
        fseek(m_file, 0, SEEK_SET);
        hpatch_TStreamInput::streamHandle = this;
        hpatch_TStreamInput::streamSize = file_length;
        hpatch_TStreamInput::read = read_file;
    }

    static long read_file(hpatch_TStreamInputHandle streamHandle, const hpatch_StreamPos_t readFromPos,
                          unsigned char *out_data, unsigned char *out_data_end) {
        TFileStreamInput &fileStreamInput = *(TFileStreamInput *) streamHandle;
        hpatch_StreamPos_t curPos = fileStreamInput.m_offset + readFromPos;
        if (fileStreamInput.m_fpos != curPos) {
            setFilePos64(fileStreamInput.m_file, curPos);
        }
        size_t readed = fread(out_data, 1, (size_t) (out_data_end - out_data), fileStreamInput.m_file);
        fileStreamInput.m_fpos = curPos + readed;
        return (long) readed;
    }

    void setHeadSize(TUInt headSize) {
        m_offset = headSize;
        hpatch_TStreamInput::streamSize -= headSize;
    }

    FILE *m_file;
    TUInt m_offset;
    hpatch_StreamPos_t m_fpos;

    ~TFileStreamInput() {
        if (m_file != 0) fclose(m_file);
    }
};


struct TFileStreamOutput : public hpatch_TStreamOutput {
    TFileStreamOutput(PatchInfo *patchInfoPtr, int patchInfoCount, const CString dir) {
        hpatch_StreamPos_t totalLength = 0;

        for (int i = 0; i < patchInfoCount; i++) {
            totalLength += patchInfoPtr[i].size;
            //在输出之前先将文件夹或者文件创建
            CString output = dir;
            output += patchInfoPtr[i].newPath;
            PatchUtil::createFileOrDir(output.c_str());
            PatchUtil::createFileOrDir(output.c_str());
        }
        this->patchInfoCount = patchInfoCount;
        this->dir = dir;
        this->patchInfoPtr = patchInfoPtr;
        this->indexOfFile = 0;
        this->indexOfBytes = 0;
        hpatch_TStreamOutput::streamHandle = this;
        hpatch_TStreamOutput::streamSize = totalLength;
        hpatch_TStreamOutput::write = write_file;
    }

    //将输出到多个文件这多个流合成一个流
    static long write_file(hpatch_TStreamInputHandle streamHandle, const hpatch_StreamPos_t writeToPos,
                           const unsigned char *data, const unsigned char *data_end) {
        using namespace std;
        long needToWrite = data_end - data;
        TFileStreamOutput *output = (TFileStreamOutput *) streamHandle;
        PatchInfo *patchInfoPtr = output->patchInfoPtr;
        int indexOfFile = output->indexOfFile;
        int indexOfBytes = output->indexOfBytes;
        long writed = 0;

        BytesVector fileBytes;        //文件数据
        auto it = data;
        bool fileBytesWrited = false;

        while (it != data_end) {
            if (indexOfBytes < patchInfoPtr[indexOfFile].size) {
                fileBytes.push_back(*it);
                fileBytesWrited = false;
                indexOfBytes++;
                it++;
                writed++;
            } else if (indexOfBytes == patchInfoPtr[indexOfFile].size) {
//                cout << "writeFile:" << output->dir + output->patchInfoPtr[indexOfFile].newPath << endl;
                CString filePath;
                filePath += output->dir;
                filePath += patchInfoPtr[indexOfFile].newPath;
                PatchUtil::writeToFileEnd(filePath.c_str(), fileBytes);
                indexOfFile++;
                indexOfBytes = 0;
                fileBytes.clear();
                fileBytesWrited = true;
            } else {
                //cannot reach
                exit(-1);
            }
        }
        if (!fileBytes.empty() && !fileBytesWrited) {
//            cout << "writeFile:" << output->dir + output->patchInfoPtr[indexOfFile].newPath << endl;
            CString filePath;
            filePath += output->dir;
            filePath += patchInfoPtr[indexOfFile].newPath;
            PatchUtil::writeToFileEnd(filePath.c_str(), fileBytes);
            if (indexOfBytes == patchInfoPtr[indexOfFile].size) {
                indexOfFile++;
                indexOfBytes = 0;
            }
        }
        output->indexOfFile = indexOfFile;
        output->indexOfBytes = indexOfBytes;
        return (long) writed;
    }

    CString dir;
    PatchInfo *patchInfoPtr;
    int patchInfoCount;
    int indexOfFile;                              //当前正在输出的文件的下标
    int indexOfBytes;                         //当前正在输出的字节相对于当前文件的位置

    ~TFileStreamOutput() {

    }
};


#endif /* filestream_h */
