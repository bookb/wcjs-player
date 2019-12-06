#include "JsVlcDeinterlace.h"

#include "JsVlcPlayer.h"

Napi::FunctionReference JsVlcDeinterlace::_jsConstructor;

void JsVlcDeinterlace::initJsApi(Napi::Env env) {
    Napi::HandleScope scope(env);
    // className: VlcDeinterlace
    Napi::Function func = DefineClass(env, "JsVlcDeinterlace", {
            CLASS_METHOD("enable", &JsVlcDeinterlace::enable),
            CLASS_METHOD("disable", &JsVlcDeinterlace::disable)
    });

    _jsConstructor = Napi::Persistent(func);
    _jsConstructor.SuppressDestruct();
}

JsVlcDeinterlace::JsVlcDeinterlace(const Napi::CallbackInfo &info) :
        NobiObjectWrap<JsVlcDeinterlace>(info) {
    log("JsVlcDeinterlace Constructor...\n");
    JsVlcPlayer *jsPlayer = JsVlcPlayer::UnwrapThis(info);
    _jsPlayer = jsPlayer;
}

void JsVlcDeinterlace::enable(const std::string &mode) {
    libvlc_video_set_deinterlace(_jsPlayer->player().get_mp(), mode.c_str());
}

void JsVlcDeinterlace::disable() {
    libvlc_video_set_deinterlace(_jsPlayer->player().get_mp(), nullptr);
}
