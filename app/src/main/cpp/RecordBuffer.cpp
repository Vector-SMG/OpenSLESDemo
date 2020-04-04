//
// Created by ivan on 2020/3/1.
//

#include "RecordBuffer.h"

RecordBuffer::RecordBuffer(int bufferSize) {
    buffer=new short*[2];
    for(int i=0;i<2;i++){
        buffer[i]=new short[bufferSize];
    }
}

RecordBuffer::~RecordBuffer() {

}

short* RecordBuffer::getRecordBuffer() {
    index++;
    if(index>1){
        index=0;
    }
    return buffer[index];
}

short *RecordBuffer::getNowBuffer() {
    return buffer[index];
}
