#pragma once

#include <string>

#include <napi.h>
#include "helpers.h"

class JsVlcPlayer; //#include "JsVlcPlayer.h"

class JsVlcAudio :
        public NobiObjectWrap<JsVlcAudio> {
public:
    static void initJsApi(Napi::Env env);

    static Napi::FunctionReference _jsConstructor;

    std::string description(uint32_t index);

    unsigned count();

    int track();

    void setTrack(int);

    bool muted();

    void setMuted(bool muted);

    unsigned volume();

    void setVolume(unsigned);

    int channel();

    void setChannel(unsigned);

    int delay();

    void setDelay(int);

    void toggleMute();

public:
    JsVlcAudio(const Napi::CallbackInfo &info);

private:
    JsVlcPlayer *_jsPlayer;
};
