#include <jni.h>
#include <string>
#include "SLES/OpenSLES.h"
#include "SLES/OpenSLES_Android.h"
#include "android/log.h"
#define LOGE(FORMAT,...) __android_log_print(ANDROID_LOG_ERROR,"chencl",FORMAT,##__VA_ARGS__);
#define LOGI(FORMAT,...) __android_log_print(ANDROID_LOG_INFO,"chencl",FORMAT,##__VA_ARGS__);
// 初始化引擎
SLObjectItf engineObject = NULL;
SLEngineItf engineEngine = NULL;

//初始化混音器
SLObjectItf outputMixObject = NULL;
SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;
SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
//pcm
SLObjectItf pcmPlayerObject = NULL;
SLPlayItf pcmPlayerPlay = NULL;

SLAndroidSimpleBufferQueueItf pcmSimpleBufferQueueItf = NULL;
FILE *pcmFile;
void *buffer;
uint8_t *out_buffer;

int getPCMData(void **pcm) {
    int size = 0;
    while (!feof(pcmFile)) {
         size  = fread(out_buffer,1,44100 * 2 * 2,pcmFile);
        if (out_buffer == NULL) {
            LOGE("数据读取完毕。。。");
            break;
        } else {
            LOGE("读取数据");
        }
        *pcm = out_buffer;
        break;
    }
    return size;
}

void pcmBufferCallBack(SLAndroidSimpleBufferQueueItf bf,void *context) {
  int size  =  getPCMData(&buffer);
  if (buffer!= NULL) {
      (*pcmSimpleBufferQueueItf)->Enqueue(pcmSimpleBufferQueueItf,buffer,size);
  }
}

extern "C" JNIEXPORT void JNICALL
Java_com_zhidao_open_1sl_MainActivity_playPcm(JNIEnv *env, jobject thiz, jstring url) {

    const char *urll = env->GetStringUTFChars(url,0);

    pcmFile = fopen(urll,"r");
    if (pcmFile == NULL) {
        LOGE("文件打开失败。。。");
        return;
    }
    LOGE("文件打开成功 :)");
    out_buffer = (uint8_t *)malloc(44100 * 2 * 2);
    LOGE("初始化引擎 :)");
    slCreateEngine(&engineObject, 0, 0, 0, 0, 0);
    (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);

    const SLInterfaceID mdis[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean mreq[1] = {SL_BOOLEAN_FALSE};
    (*engineEngine)->CreateOutputMix(engineEngine,&outputMixObject,1,mdis,mreq);
    (*outputMixObject)->Realize(outputMixObject,SL_BOOLEAN_FALSE);
    (*outputMixObject)->GetInterface(outputMixObject,SL_IID_ENVIRONMENTALREVERB,&outputMixEnvironmentalReverb);
    (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(outputMixEnvironmentalReverb,&reverbSettings);
    LOGE("初始化MixEnvironmental :)");

    SLDataLocator_OutputMix  outputMix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};


    SLDataLocator_AndroidSimpleBufferQueue androidBufferQueue ={SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,2};
    SLDataFormat_PCM pcm={
            SL_DATAFORMAT_PCM,
            2,
            SL_SAMPLINGRATE_44_1,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN
    };

    LOGE("初始化MixEnvironmental 111 :)");
    SLDataSource  slDataSource = {&androidBufferQueue,&pcm};
    SLDataSink audioSink = {&outputMix,NULL};
    LOGE("初始化MixEnvironmental 22222 :)");
    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    (*engineEngine)->CreateAudioPlayer(engineEngine,&pcmPlayerObject,&slDataSource,&audioSink,3,ids,req);
    (*pcmPlayerObject)->Realize(pcmPlayerObject,SL_BOOLEAN_FALSE);
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject,SL_IID_PLAY,&pcmPlayerPlay);
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject,SL_IID_BUFFERQUEUE,&pcmSimpleBufferQueueItf);


    LOGE("初始化pcmPlayer :)");
    (*pcmSimpleBufferQueueItf)->RegisterCallback(pcmSimpleBufferQueueItf,pcmBufferCallBack,NULL);
    (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay,SL_PLAYSTATE_PLAYING);

     pcmBufferCallBack(pcmSimpleBufferQueueItf,NULL);
    LOGE("初始化pcmBufferCallBack :)");
    env->ReleaseStringUTFChars(url,urll);
}
