//
// Created by ivan on 2020-02-22.
//
#include <jni.h>
#include <string>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <android/log.h>
#import "OpenSLESPlayer.h"

#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,"haohao",FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"haohao",FORMAT,##__VA_ARGS__);


OpenSLESPlayer *openSlesPlayer;

extern "C"
JNIEXPORT void JNICALL
Java_com_cornucopia_openslesdemo_OpenSLESHelper_playPcmByOpenSLES(JNIEnv *env, jobject thiz,
                                                                  jstring path) {
    const char *audioPath = (*env).GetStringUTFChars(path, JNI_FALSE);
    openSlesPlayer = new OpenSLESPlayer(audioPath);
    openSlesPlayer->startPlay();
    (*env).ReleaseStringUTFChars(path,audioPath);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_cornucopia_openslesdemo_OpenSLESHelper_stopPlay(JNIEnv *env, jobject thiz) {
    if (openSlesPlayer != NULL) {
        openSlesPlayer->stopPlay();
    }
}
