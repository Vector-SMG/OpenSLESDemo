//
// Created by ivan on 2020-02-23.
//

#include "OpenSLESPlayer.h"

#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,"OpenSLESPlayer",FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"OpenSLESPlayer",FORMAT,##__VA_ARGS__);

FILE *pcmFile;
void *buffer;
uint8_t *out_buffer;
SLAndroidSimpleBufferQueueItf playerBufferQueueItf;

OpenSLESPlayer::OpenSLESPlayer(const char *path) {
    pcmFile = fopen(path, "r");
    out_buffer = (uint8_t *) malloc(44100 * 2 * 2);
}

SLresult OpenSLESPlayer::createEngine() {
    //创建引擎对象
    //1.对象地址，用于传出对象
    //2.配置参数数量
    //3.配置参数，枚举数组
    //4.支持的接口数量
    //5.具体的要支持的接口
    //6.具体的要支持的接口时开放的还是关闭的
    SLresult result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("slCreateEngine failed,result=%d", result);
        return result;
    }
    //实例化
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("engineObject Realize failed,result=%d", result);
        return result;
    }
    //获取引擎对象接口
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("engineObject GetInterface failed,result=%d", result);
        return result;
    }
    return result;
}

SLDataSource OpenSLESPlayer::createDataSource() {
    //数据源简单缓冲队列定位器
    SLDataLocator_AndroidSimpleBufferQueue dataSourceLocator = {
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
            2
    };
    //PCM 数据源格式
    SLDataFormat_PCM dataSourceFormat = {
            SL_DATAFORMAT_PCM,
            2,
            SL_SAMPLINGRATE_44_1,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN
    };
    SLDataSource dataSource;
    dataSource.pLocator = malloc(sizeof(SLDataLocator_AndroidSimpleBufferQueue));
    *(static_cast<SLDataLocator_AndroidSimpleBufferQueue *>(dataSource.pLocator)) = dataSourceLocator;

    dataSource.pFormat = malloc(sizeof(SLDataFormat_PCM));
    *(static_cast<SLDataFormat_PCM *>(dataSource.pFormat)) = dataSourceFormat;
    return dataSource;
}

SLresult OpenSLESPlayer::createDataSink() {
    SLInterfaceID interfaceIDs[1] = {SL_IID_ENVIRONMENTALREVERB};
    SLboolean requiredInterfaces[1] = {SL_BOOLEAN_FALSE};

    SLresult result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject,
                                                       1, interfaceIDs, requiredInterfaces);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("CreateOutputMix failed, result=%d", result);
        return result;
    }

    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("outputMixObject Realize failed, result=%d", result);
        return result;
    }

    //针对数据接收器的输出混合定位器(混音器)
    SLDataLocator_OutputMix dataSinkLocator = {
            SL_DATALOCATOR_OUTPUTMIX,//定位器类型
            outputMixObject //输出混合
    };
    this->dataSink.pLocator = malloc(sizeof(SLDataLocator_OutputMix));
    *(static_cast<SLDataLocator_OutputMix *>( this->dataSink.pLocator)) = dataSinkLocator;
    //输出
    this->dataSink.pFormat = NULL;
    return result;
}


SLresult OpenSLESPlayer::createAudioPlayer() {
    //需要的接口
    const SLInterfaceID ids[1] = {SL_IID_BUFFERQUEUE};
    const SLboolean req[1] = {SL_BOOLEAN_TRUE};
    SLDataSource dataSource = createDataSource();

//    //数据源简单缓冲队列定位器
//    SLDataLocator_AndroidSimpleBufferQueue dataSourceLocator = {
//            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
//            2
//    };
//    //PCM 数据源格式
//    SLDataFormat_PCM dataSourceFormat = {
//            SL_DATAFORMAT_PCM,
//            2,
//            SL_SAMPLINGRATE_44_1,
//            SL_PCMSAMPLEFORMAT_FIXED_16,
//            SL_PCMSAMPLEFORMAT_FIXED_16,
//            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
//            SL_BYTEORDER_LITTLEENDIAN
//    };
//    //输入数据源
//    SLDataSource dataSource = {
//            &dataSourceLocator,
//            &dataSourceFormat
//    };
    SLresult result = createDataSink();

//    SLresult result= createDataSink();
    if (result != SL_RESULT_SUCCESS) {
        return result;
    }
    result = (*engineEngine)->CreateAudioPlayer(
            engineEngine,
            &playerObject,
            &dataSource,
            &this->dataSink,
            1,
            ids,
            req
    );
    if (result != SL_RESULT_SUCCESS) {
        LOGE("CreateAudioPlayer failed,result=%d", result);
        return result;
    }
    result = (*playerObject)->Realize(playerObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("playerObject Realize() failed,result=%d", result);
        return result;
    }

    result = (*playerObject)->GetInterface(playerObject, SL_IID_PLAY, &playerPlay);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("playerObject GetInterface(SL_IID_PLAY) failed.");
        return result;
    }

    result = (*playerObject)->GetInterface(playerObject, SL_IID_BUFFERQUEUE,
                                           &playerBufferQueueItf);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("playerObject GetInterface(SL_IID_BUFFERQUEUE),result=%d", result);
        return result;
    }
    return result;
}

void getPcmData(void **pcm) {
    while (!feof(pcmFile)) {
        fread(out_buffer, 44100 * 2 * 2, 1, pcmFile);
        if (out_buffer == NULL) {
            LOGI("%s", "read end");
        } else {
            LOGI("%s", "reading");
        }
        *pcm = out_buffer;
        break;
    }
}

SLresult OpenSLESPlayer::createEnqueue() {
    SLresult result;
    getPcmData(&buffer);
    if (NULL != buffer) {
        result = (*playerBufferQueueItf)->Enqueue(playerBufferQueueItf, buffer,
                                                  44100 * 2 * 2);
        if (result != SL_RESULT_SUCCESS) {
            LOGE("playerBufferQueueItf Enqueue failed,result=%d", result);
            return result;
        }
    }
    return result;
}

void AudioPlayerCallback(SLAndroidSimpleBufferQueueItf bf, void *context) {
    getPcmData(&buffer);
    if (NULL != buffer) {
        SLresult result = (*playerBufferQueueItf)->Enqueue(playerBufferQueueItf, buffer,
                                                           44100 * 2 * 2);
        if (result != SL_RESULT_SUCCESS) {
            LOGE("playerBufferQueueItf Enqueue failed,result=%d", result);
            return;
        }
    }
}

SLresult OpenSLESPlayer::setCallbackAndPlayStatus() {
    SLresult result = (*playerBufferQueueItf)->RegisterCallback(playerBufferQueueItf,
                                                                AudioPlayerCallback, this);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("playerBufferQueueItf RegisterCallback failed,result=%d", result);
        return result;
    }

    result = (*playerPlay)->SetPlayState(playerPlay, SL_PLAYSTATE_PLAYING);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("SetPlayState playing failed,result=%d", result);
        return result;
    }
    return result;
}

void OpenSLESPlayer::startPlay() {
    SLresult result = this->createEngine();
    if (result != SL_RESULT_SUCCESS) {
        return;
    }
    result = this->createAudioPlayer();
    if (result != SL_RESULT_SUCCESS) {
        return;
    }
    result = this->createEnqueue();
    if (result != SL_RESULT_SUCCESS) {
        return;
    }
    result = this->setCallbackAndPlayStatus();
    if (result != SL_RESULT_SUCCESS) {
        return;
    }
    AudioPlayerCallback(playerBufferQueueItf, NULL);
}

void OpenSLESPlayer::stopPlay() {
    //stop
    if (playerPlay != NULL) {
        SLresult result = (*playerPlay)->SetPlayState(playerPlay, SL_PLAYSTATE_STOPPED);
        if (result != SL_RESULT_SUCCESS) {
            LOGE("SetPlayState stopped  failed,result=%d", result);
        }
    }
    //release
    if (playerObject != NULL) {
        (*playerObject)->Destroy(playerObject);
        playerObject = NULL;
        playerPlay = NULL;
        playerBufferQueueItf = NULL;
        outputMixObject = NULL;
    }
    if (engineObject != NULL) {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineEngine = NULL;
    }
}