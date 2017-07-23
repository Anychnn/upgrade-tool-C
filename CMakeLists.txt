cmake_minimum_required(VERSION 3.7)
project(update-tool)


set(CMAKE_CXX_STANDARD 11)
#set(SOURCE_FILES src/main.cpp src/file.h src/diff.h)
file(GLOB SRCS src/*.cpp)
file(GLOB HEADS src/*.h)



#aux_source_directory(src)

include_directories(lib/HDiffPatch/libHDiffPatch)
include_directories(lib)
file(GLOB TEST "*/h" "*.cpp")
set(SOURCE_FILES
        src/hash.h src/pat_file.h
        src/util/DiffUtil.h src/util/fileutils.h  src/util/fileutils.cpp
        src/util/patchStream.h
        src/util/PatchUtil.h
        src/patch/PatchManager.h
        src/stl/BytesVector.h
        src/util/types.h
        src/patch/PatchManager.cpp src/patch/PatchManager.h src/diff/DiffManager.cpp src/diff/DiffManager.h src/stl/BytesVector.h src/stl/BytesVector.h src/stl/CString.h src/stl/CString.cpp)
#aux_source_directory(HDiffPatch SOURCE_FILES)

#add_executable(TestC ${SOURCE_FILES})
#add_executable(TestC ${SRCS} ${HEADS} ${SOURCE_FILES})
add_executable(TestC ${SRCS} ${HEADS} ${SOURCE_FILES} ${TEST})