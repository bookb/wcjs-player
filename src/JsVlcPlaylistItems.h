#pragma once

#include <napi.h>

#include "helpers.h"

class JsVlcPlayer; //#include "JsVlcPlayer.h"

class JsVlcPlaylistItems :
        public NobiObjectWrap<JsVlcPlaylistItems> {
public:
    static void initJsApi(Napi::Env env);

    Napi::Value item(const Napi::CallbackInfo &info);

    unsigned count();

    void clear();

    bool remove(unsigned idx);

public:
    static Napi::FunctionReference _jsConstructor;

    JsVlcPlaylistItems(const Napi::CallbackInfo &info);

private:
    JsVlcPlayer *_jsPlayer;
};
