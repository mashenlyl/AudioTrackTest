//
// Created by jetson on 2021/3/13.
//

#ifndef AUDIOTRACKTEST_OPENSL_ES_CONTEXT_H
#define AUDIOTRACKTEST_OPENSL_ES_CONTEXT_H

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "CommonTools.h"

class OpenSLESContext {
private:
    SLObjectItf mEngineObject;
    SLEngineItf mEngine;
    bool mIsInited;

    SLresult createEngine() {
        SLEngineOption engineOptions[] {
                {
                        (SLuint32) SL_ENGINEOPTION_THREADSAFE,
                        (SLuint32) SL_BOOLEAN_TRUE
                }
        };
        return slCreateEngine(&mEngineObject, ARRAY_LEN(engineOptions),
                engineOptions, 0,
                0, 0);
    };

    SLresult RealizeObject(SLObjectItf object) {
        return (*object)->Realize(object, SL_BOOLEAN_FALSE);
    }

    SLresult GetEngineInterface() {
        return (*mEngineObject)->GetInterface(mEngineObject, SL_IID_ENGINE, &mEngine);
    }

    OpenSLESContext();
    void init();
    static OpenSLESContext* mInstance;
public:
    static OpenSLESContext* GetInstance();
    virtual ~OpenSLESContext();
    SLEngineItf getEngine() {
        return mEngine;
    };
};


#endif //AUDIOTRACKTEST_OPENSL_ES_CONTEXT_H
