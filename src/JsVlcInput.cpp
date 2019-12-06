#include "JsVlcInput.h"

#include "JsVlcPlayer.h"
#include "JsVlcDeinterlace.h"

Napi::FunctionReference JsVlcInput::_jsConstructor;

void JsVlcInput::initJsApi(Napi::Env env) {
    JsVlcDeinterlace::initJsApi(env);
    Napi::HandleScope scope(env);
    // VlcInput
    Napi::Function func = DefineClass(env, "JsVlcInput", {
            CLASS_RO_ACCESSOR("length", &JsVlcInput::length),
            CLASS_RO_ACCESSOR("fps", &JsVlcInput::fps),
            CLASS_RO_ACCESSOR("state", &JsVlcInput::state),
            CLASS_RO_ACCESSOR("hasVout", &JsVlcInput::hasVout),
            CLASS_RW_ACCESSOR("position",
                              &JsVlcInput::position,
                              &JsVlcInput::setPosition),
            CLASS_RW_ACCESSOR("time",
                              &JsVlcInput::time,
                              &JsVlcInput::setTime),
            CLASS_RW_ACCESSOR("rate",
                              &JsVlcInput::rate,
                              &JsVlcInput::setRate),
    });

    _jsConstructor = Napi::Persistent(func);
    _jsConstructor.SuppressDestruct();

}

JsVlcInput::JsVlcInput(const Napi::CallbackInfo &info) :
        NobiObjectWrap<JsVlcInput>(info) {
    log("JsVlcInput Constructor...\n");
    JsVlcPlayer *jsPlayer = JsVlcPlayer::UnwrapThis(info);
    _jsPlayer = jsPlayer;
}

double JsVlcInput::length() {
    return static_cast<double>( _jsPlayer->player().playback().get_length());
}

double JsVlcInput::fps() {
    return _jsPlayer->player().playback().get_fps();
}

unsigned JsVlcInput::state() {
    return _jsPlayer->player().get_state();
}

bool JsVlcInput::hasVout() {
    return _jsPlayer->player().video().has_vout();
}

double JsVlcInput::position() {
    return _jsPlayer->player().playback().get_position();
}

void JsVlcInput::setPosition(double position) {
    _jsPlayer->player().playback().set_position(static_cast<float>( position ));
}

double JsVlcInput::time() {
    return static_cast<double>( _jsPlayer->player().playback().get_time());
}

void JsVlcInput::setTime(double time) {
    return _jsPlayer->player().playback().set_time(static_cast<libvlc_time_t>( time ));
}

double JsVlcInput::rate() {
    return _jsPlayer->player().playback().get_rate();
}

void JsVlcInput::setRate(double rate) {
    _jsPlayer->player().playback().set_rate(static_cast<float>( rate ));
}

