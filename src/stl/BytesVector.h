//
// Created by anyang on 2017/7/19.
//

#ifndef UPDATE_TOOL_CVECTOR_H
#define UPDATE_TOOL_CVECTOR_H
class BytesVector {
public:
    explicit BytesVector(int initSize = 0) : theSize(initSize), theCapacity(initSize + SPARE_CAPACITY) {
        objects = new TByte[theCapacity];
    }

    ~BytesVector() {
        if(objects!=nullptr){
            delete[]objects;
        }
    }

    const BytesVector &operator=(const BytesVector &rhs) {
        if (this != &rhs) {
            delete[] objects;
            theSize = rhs.size();
            theCapacity = rhs.theCapacity;
            objects = new TByte[theCapacity];
            for (int k = 0; k < size(); k++) {
                objects[k] = rhs.objects[k];
            }
        }
        return *this;
    }

    void resize(int newSize) {
        if (newSize > theCapacity) {
            reserve(newSize * 2 + 1);
        }
        theSize = newSize;
    }

    void reserve(int newCapacity) {
        if (newCapacity < theSize) {
            return;
        }
        TByte *oldArray = objects;
        objects = new TByte[newCapacity];
        for (int k = 0; k < theSize; k++) {
            objects[k] = oldArray[k];
        }
        theCapacity = newCapacity;
        delete[]oldArray;
    }

    int size() const {
        return theSize;
    }

    bool empty() const { return size() == 0; }

    void clear(){
        theSize=0;
    }

    void push_back(const TByte &x) {
        if (theSize == theCapacity)
            reserve(2 * theCapacity + 1);
        objects[theSize++] = x;
    }

    void pop_back() { theSize--; }

    TByte &operator[](int index) { return objects[index]; }

    typedef TByte *iterator;

    iterator begin() { return &objects[0]; }

    iterator end() { return &objects[size()]; }

    enum {
        SPARE_CAPACITY = 16
    };

private:
    int theSize;
    int theCapacity;
    TByte *objects=nullptr;
};
#endif //UPDATE_TOOL_CVECTOR_H
