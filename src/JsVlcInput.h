#pragma once

#include <string>

#include <napi.h>
#include "helpers.h"

class JsVlcPlayer; //#include "JsVlcPlayer.h"

class JsVlcInput :
        public NobiObjectWrap<JsVlcInput> {
public:
    static void initJsApi(Napi::Env env);

    static Napi::FunctionReference _jsConstructor;

    double length();

    double fps();

    unsigned state();

    bool hasVout();

    double position();

    void setPosition(double);

    double time();

    void setTime(double);

    double rate();

    void setRate(double);


public:
    JsVlcInput(const Napi::CallbackInfo &info);

private:
    JsVlcPlayer *_jsPlayer;
};
