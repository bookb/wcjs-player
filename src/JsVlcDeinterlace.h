#pragma once

#include <string>

#include <napi.h>
#include "helpers.h"

class JsVlcPlayer; //#include "JsVlcPlayer.h"

class JsVlcDeinterlace :
        public NobiObjectWrap<JsVlcDeinterlace> {
public:
    static void initJsApi(Napi::Env env);

    static Napi::FunctionReference _jsConstructor;

    void enable(const std::string &mode);

    void disable();

public:
    JsVlcDeinterlace(const Napi::CallbackInfo &info);

private:
    JsVlcPlayer *_jsPlayer;
};
