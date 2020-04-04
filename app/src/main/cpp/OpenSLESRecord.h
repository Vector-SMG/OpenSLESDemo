//
// Created by ivan on 2020/2/28.
//

#ifndef OPENSLESDEMO_OPENSLESRECORD_H
#define OPENSLESDEMO_OPENSLESRECORD_H

#include <jni.h>
#include <string>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <android/log.h>
#include "RecordBuffer.h"

class OpenSLESRecord {
private:
    SLObjectItf engineObject;
    SLEngineItf engineEngine;
    SLObjectItf recorderObject;

public:
    SLRecordItf recorderRecorder;
    RecordBuffer *recorderBuffer;
    FILE *pcmFile;
    bool finished = false;

    void  StartRecord(const char *pcmPath, int sampleRate, int channels, int bitRate);
    void  StopRecord();

};


#endif //OPENSLESDEMO_OPENSLESRECORD_H
