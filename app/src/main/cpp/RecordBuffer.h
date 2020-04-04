//
// Created by ivan on 2020/3/1.
//

#ifndef OPENSLESDEMO_RECORDBUFFER_H
#define OPENSLESDEMO_RECORDBUFFER_H


class RecordBuffer {
public:
    short **buffer;
    int index=-1;
public:
    RecordBuffer(int bufferSize);
    ~RecordBuffer();
    //即将录入PCM数据的buffer
    short *getRecordBuffer();
    //当前录制好的PCM数据的buffer,可以写入文件
    short *getNowBuffer();
};


#endif //OPENSLESDEMO_RECORDBUFFER_H
