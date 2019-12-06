#pragma once

#include <string>

#include <napi.h>

#include "helpers.h"

class JsVlcPlayer; //#include "JsVlcPlayer.h"

class JsVlcSubtitles :
        public NobiObjectWrap<JsVlcSubtitles> {
public:
    static void initJsApi(Napi::Env env);

    std::string description(uint32_t index);

    unsigned count();

    int track();

    void setTrack(int);

    int delay();

    void setDelay(int);

    bool load(const std::string &path);

public:
    static Napi::FunctionReference _jsConstructor;

    JsVlcSubtitles(const Napi::CallbackInfo &info);

private:
    JsVlcPlayer *_jsPlayer;
};
