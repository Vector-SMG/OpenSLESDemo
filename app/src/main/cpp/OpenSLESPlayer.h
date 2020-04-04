//
// Created by ivan on 2020-02-23.
//

#ifndef OPENSLESDEMO_OPENSLESPLAYER_H
#define OPENSLESDEMO_OPENSLESPLAYER_H

#include <jni.h>
#include <string>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <android/log.h>

class OpenSLESPlayer {
public:
    OpenSLESPlayer(const char *path);

    void startPlay();

    void stopPlay();

private:
    //1.创建引擎
    SLresult createEngine();

    //2.设置混音器,用于声音的输出
    SLresult createDataSink();

    //3.设置dataSource，用于数据的传入
    SLDataSource createDataSource();

    //4.创建播放器
    SLresult createAudioPlayer();

    //5.设置缓冲队列和回调函数
    SLresult createEnqueue();

    //6.设置回调函数和播放状态，并启动回调函数.
    SLresult setCallbackAndPlayStatus();

private:
    SLObjectItf engineObject;
    SLEngineItf engineEngine;

    SLObjectItf playerObject;
    SLPlayItf playerPlay;
    SLObjectItf outputMixObject;

    SLDataSink dataSink;
};


#endif //OPENSLESDEMO_OPENSLESPLAYER_H
