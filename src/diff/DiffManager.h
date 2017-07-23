//
// Created by anyang on 2017/7/7.
//

#ifndef TESTC_DIFFMANAGER_H
#define TESTC_DIFFMANAGER_H

#include <vector>
#include <string>
#include "../util/types.h"
#include "../util/PatchUtil.h"
#include "../util/patchStream.h"
#include "../util/common.h"


class DiffManager {
public:
    void diffDir(const char* oldDirPath, const char * newDirPath, std::vector<TByte> *outBytes);
};


#endif //TESTC_DIFFMANAGER_H
