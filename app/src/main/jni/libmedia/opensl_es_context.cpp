//
// Created by jetson on 2021/3/13.
//

#include "opensl_es_context.h"
#include "CommonTools.h"
#define LOG_TAG "liyaoliopensles"

OpenSLESContext* OpenSLESContext::mInstance = new OpenSLESContext();

void OpenSLESContext::init() {
    LOGI("createEngine");
    SLresult result = createEngine();
    if (SL_RESULT_SUCCESS == result) {
        result = RealizeObject(mEngineObject);
        if (SL_RESULT_SUCCESS == result) {
            result = GetEngineInterface();
        }
    }
}

OpenSLESContext::OpenSLESContext() {
    mIsInited = false;
}

OpenSLESContext::~OpenSLESContext() {
}

OpenSLESContext* OpenSLESContext::GetInstance() {
    if (!mInstance->mIsInited) {
        mInstance->init();
        mInstance->mIsInited = true;
    }
    return mInstance;
}