#include "JsVlcVideo.h"

#include "JsVlcPlayer.h"
#include "JsVlcDeinterlace.h"

Napi::FunctionReference JsVlcVideo::_jsConstructor;

void JsVlcVideo::initJsApi(Napi::Env env) {
    JsVlcDeinterlace::initJsApi(env);
    Napi::HandleScope scope(env);
    // VlcVideo
    Napi::Function func = DefineClass(env, "JsVlcVideo", {

            CLASS_RO_ACCESSOR("count", &JsVlcVideo::count),
            CLASS_RO_ACCESSOR("deinterlace", &JsVlcVideo::deinterlace),

            CLASS_RW_ACCESSOR("track", &JsVlcVideo::track, &JsVlcVideo::setTrack),

            CLASS_RW_ACCESSOR("contrast", &JsVlcVideo::contrast, &JsVlcVideo::setContrast),
            CLASS_RW_ACCESSOR("brightness", &JsVlcVideo::brightness, &JsVlcVideo::setBrightness),
            CLASS_RW_ACCESSOR("hue", &JsVlcVideo::hue, &JsVlcVideo::setHue),
            CLASS_RW_ACCESSOR("saturation", &JsVlcVideo::saturation, &JsVlcVideo::setSaturation),
            CLASS_RW_ACCESSOR("gamma", &JsVlcVideo::gamma, &JsVlcVideo::setGamma),

    });

    _jsConstructor = Napi::Persistent(func);
    _jsConstructor.SuppressDestruct();
}


JsVlcVideo::JsVlcVideo(const Napi::CallbackInfo &info) :
        NobiObjectWrap(info) {
    JsVlcPlayer *jsPlayer = JsVlcPlayer::UnwrapThis(info);
    _jsPlayer = jsPlayer;
    _jsDeinterlace = JsVlcDeinterlace::create(info, {jsPlayer->Value()});
}

unsigned JsVlcVideo::count() {
    return _jsPlayer->player().video().track_count();
}

int JsVlcVideo::track() {
    return _jsPlayer->player().video().get_track();
}

void JsVlcVideo::setTrack(unsigned track) {
    _jsPlayer->player().video().set_track(track);
}

double JsVlcVideo::contrast() {
    return _jsPlayer->player().video().get_contrast();
}

void JsVlcVideo::setContrast(double contrast) {
    _jsPlayer->player().video().set_contrast(static_cast<float>( contrast ));
}

double JsVlcVideo::brightness() {
    return _jsPlayer->player().video().get_brightness();
}

void JsVlcVideo::setBrightness(double brightness) {
    _jsPlayer->player().video().set_brightness(static_cast<float>( brightness ));
}

int JsVlcVideo::hue() {
    return _jsPlayer->player().video().get_hue();
}

void JsVlcVideo::setHue(int hue) {
    _jsPlayer->player().video().set_hue(hue);
}

double JsVlcVideo::saturation() {
    return _jsPlayer->player().video().get_saturation();
}

void JsVlcVideo::setSaturation(double saturation) {
    _jsPlayer->player().video().set_saturation(static_cast<float>( saturation ));
}

double JsVlcVideo::gamma() {
    return _jsPlayer->player().video().get_gamma();
}

void JsVlcVideo::setGamma(double gamma) {
    _jsPlayer->player().video().set_gamma(static_cast<float>( gamma ));
}

Napi::Object JsVlcVideo::deinterlace() {
    return _jsDeinterlace;
}
