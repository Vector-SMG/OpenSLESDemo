//
// Created by ivan on 2020-02-22.
//
#include <jni.h>
#include <string>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <android/log.h>
#import "OpenSLESPlayer.h"
#import "OpenSLESRecord.h"

#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,"haohao",FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"haohao",FORMAT,##__VA_ARGS__);


OpenSLESPlayer *openSlesPlayer;
OpenSLESRecord *openSlesRecord;


extern "C"
JNIEXPORT void JNICALL
Java_com_cornucopia_openslesdemo_OpenSLESHelper_playPcmByOpenSLES(JNIEnv *env, jobject thiz,
                                                                  jstring path) {
    const char *audioPath = (*env).GetStringUTFChars(path, JNI_FALSE);
    openSlesPlayer = new OpenSLESPlayer(audioPath);
    openSlesPlayer->startPlay();
    (*env).ReleaseStringUTFChars(path, audioPath);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_cornucopia_openslesdemo_OpenSLESHelper_stopPlay(JNIEnv *env, jobject thiz) {
    if (openSlesPlayer != NULL) {
        openSlesPlayer->stopPlay();
    }
    delete openSlesPlayer;
    openSlesPlayer = NULL;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_cornucopia_openslesdemo_OpenSLESHelper_recordVoiceToPcm(JNIEnv *env, jobject thiz,
                                                                 jstring path, jint sampleRate,
                                                                 jint channels,
                                                                 jint bitRate) {
    const char *pcmPath = (*env).GetStringUTFChars(path, JNI_FALSE);
    openSlesRecord = new OpenSLESRecord();
    openSlesRecord->StartRecord(pcmPath, sampleRate, channels, bitRate);
    (*env).ReleaseStringUTFChars(path, pcmPath);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_cornucopia_openslesdemo_OpenSLESHelper_stopRecord(JNIEnv *env, jobject thiz) {
    if (openSlesRecord != NULL) {
        openSlesRecord->StopRecord();
        openSlesRecord = NULL;
    }
}