#pragma once

#include <string>

#include <napi.h>

#include "helpers.h"

class JsVlcPlayer; //#include "JsVlcPlayer.h"

class JsVlcVideo :
        public NobiObjectWrap<JsVlcVideo> {
public:
    static Napi::FunctionReference _jsConstructor;

    static void initJsApi(Napi::Env env);

    unsigned count();

    int track();

    void setTrack(unsigned);

    double contrast();

    void setContrast(double);

    double brightness();

    void setBrightness(double);

    int hue();

    void setHue(int);

    double saturation();

    void setSaturation(double);

    double gamma();

    void setGamma(double);

    Napi::Object deinterlace();

public:
    JsVlcVideo(const Napi::CallbackInfo &info);

private:
    Napi::Object _jsDeinterlace;

    JsVlcPlayer *_jsPlayer;
};
