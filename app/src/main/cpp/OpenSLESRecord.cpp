//
// Created by ivan on 2020/2/28.
//

#include "OpenSLESRecord.h"
#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,"OpenSLESPlayer",FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"OpenSLESPlayer",FORMAT,##__VA_ARGS__);

static unsigned recorderSize = 4096;

static void RecordCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void *context) {
    LOGI("录制大小:%d",recorderSize)

    OpenSLESRecord *recorder = (OpenSLESRecord *) context;

    if (recorder->recorderBuffer != NULL) {
        fwrite(recorder->recorderBuffer->getNowBuffer(), 1, recorderSize,
               recorder->pcmFile);
    }
    if (recorder->finished) {
        (*recorder->recorderRecorder)->SetRecordState(recorder->recorderRecorder, SL_RECORDSTATE_STOPPED);
        //刷新缓冲区，关闭流
        fclose(recorder->pcmFile);
        //释放内存
        delete recorder->recorderBuffer;
        recorder->recorderBuffer=NULL;
        LOGI("停止录音");
    } else{
        (*bufferQueue)->Enqueue(bufferQueue,recorder->recorderBuffer->getRecordBuffer(),recorderSize);
    }
}

void OpenSLESRecord::StartRecord(const char *pcmPath, int sampleRate, int channels, int bitRate) {
    pcmFile = fopen(pcmPath, "w");

    recorderBuffer=new RecordBuffer(recorderSize);

    //1.调用全局方法创建一个引擎对象(OpenSL ES入口)
    SLresult result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    if (result != SL_RESULT_SUCCESS) {
        return;
    }
    //2.实例化这个对象
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        return;
    }
    //3.从这个对象获取引擎接口
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    if (result != SL_RESULT_SUCCESS) {
        return;
    }
    //4.设置IO设备
    SLDataLocator_IODevice ioDevice = {
            SL_DATALOCATOR_IODEVICE,//类型
            SL_IODEVICE_AUDIOINPUT,//device类型 选择音频输入类型
            SL_DEFAULTDEVICEID_AUDIOINPUT,//deviceID
            NULL //device实例
    };
    SLDataSource dataSource = {
            &ioDevice,//SLDataLocator_IODevice配置输入
            NULL//输入格式，采集的并不需要
    };
    //5.设置输出buffer队列
    SLDataLocator_AndroidSimpleBufferQueue bufferQueue = {
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,//类型，这里只能是这个常量
            2 //buffer的数量
    };
    //6.设置输出数据的格式
    SLDataFormat_PCM pcmFormat = {
            SL_DATAFORMAT_PCM,
            2,
            SL_SAMPLINGRATE_44_1,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN
    };
    SLDataSink audioSink = {
            &bufferQueue,//SLDataFormat_PCM配置输出
            &pcmFormat//输出数据格式
    };
    SLAndroidSimpleBufferQueueItf recorderBufferQueue;//Buffer接口

    //7.创建录制的对象
    const SLInterfaceID id[1] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
    const SLboolean req[1] = {SL_BOOLEAN_TRUE};
    result = (*engineEngine)->CreateAudioRecorder(engineEngine,        //引擎接口
                                                  &recorderObject,   //录制对象地址，用于传出对象
                                                  &dataSource,          //输入配置
                                                  &audioSink,         //输出配置
                                                  1,                  //支持的接口数量
                                                  id,                 //具体的要支持的接口
                                                  req                 //具体的要支持的接口是开放的还是关闭的
    );
    if (SL_RESULT_SUCCESS != result) {
        LOGE("CreateAudioRecorder failed,result=%d", result);
        return;
    }
    //8.实例化这个录制对象
    result = (*recorderObject)->Realize(recorderObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) {
        LOGE("Realize failed,result=%d", result);
        return;
    }
    //9.获取录制接口
    (*recorderObject)->GetInterface(recorderObject, SL_IID_RECORD, &recorderRecorder);
    //10.获取Buffer接口
    (*recorderObject)->GetInterface(recorderObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
                                    &recorderBufferQueue);
    //11.注册录制回调
    result = (*recorderBufferQueue)->RegisterCallback(recorderBufferQueue,
                                                      RecordCallback, this);
    if (SL_RESULT_SUCCESS != result) {
        LOGE("RegisterCallback failed,result=%d", result);
        return;
    }
    finished = false;
    //12.写入一帧音频数据
    result = (*recorderBufferQueue)->Enqueue(recorderBufferQueue, recorderBuffer->getRecordBuffer(),
                                             recorderSize);
    if (SL_RESULT_SUCCESS != result) {
        LOGE("Enqueue failed,result=%d", result);
        return;
    }
    //13.开始录音
    (*recorderRecorder)->SetRecordState(recorderRecorder, SL_RECORDSTATE_RECORDING);
}

void OpenSLESRecord::StopRecord() {
    if (NULL != recorderRecorder) {
        finished = true;
    }
}

