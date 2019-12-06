#include "JsVlcAudio.h"

#include "JsVlcPlayer.h"

Napi::FunctionReference JsVlcAudio::_jsConstructor;

void JsVlcAudio::initJsApi(Napi::Env env) {

    Napi::HandleScope scope(env);
    // VlcVideo
    Napi::Function func = DefineClass(env, "JsVlcAudio", {
            InstanceValue("Error", ToJsValue(env, libvlc_AudioChannel_Error)),
            InstanceValue("Stereo", ToJsValue(env, libvlc_AudioChannel_Stereo)),
            InstanceValue("ReverseStereo", ToJsValue(env, libvlc_AudioChannel_RStereo)),
            InstanceValue("Left", ToJsValue(env, libvlc_AudioChannel_Left)),
            InstanceValue("Right", ToJsValue(env, libvlc_AudioChannel_Right)),
            InstanceValue("Dolby", ToJsValue(env, libvlc_AudioChannel_Dolbys)),

            CLASS_METHOD("description", &JsVlcAudio::description),
            CLASS_RO_ACCESSOR("count", &JsVlcAudio::count),
            CLASS_RW_ACCESSOR("mute", &JsVlcAudio::muted, &JsVlcAudio::setMuted),
            CLASS_RW_ACCESSOR("volume", &JsVlcAudio::volume, &JsVlcAudio::setVolume),
            CLASS_RW_ACCESSOR("channel", &JsVlcAudio::channel, &JsVlcAudio::setChannel),
            CLASS_RW_ACCESSOR("delay", &JsVlcAudio::delay, &JsVlcAudio::setDelay),

            CLASS_METHOD("toggleMute", &JsVlcAudio::toggleMute),

    });

    _jsConstructor = Napi::Persistent(func);
    _jsConstructor.SuppressDestruct();
}


JsVlcAudio::JsVlcAudio(const Napi::CallbackInfo &info) : NobiObjectWrap<JsVlcAudio>(info) {
    JsVlcPlayer *jsPlayer = JsVlcPlayer::UnwrapThis(info);
    _jsPlayer = jsPlayer;
}

std::string JsVlcAudio::description(uint32_t index) {
    vlc_player &p = _jsPlayer->player();

    std::string name;

    libvlc_track_description_t *rootTrackDesc =
            libvlc_audio_get_track_description(p.get_mp());
    if (!rootTrackDesc)
        return name;

    unsigned count = _jsPlayer->player().audio().track_count();
    if (count && index < count) {
        libvlc_track_description_t *trackDesc = rootTrackDesc;
        for (; index && trackDesc; --index) {
            trackDesc = trackDesc->p_next;
        }

        if (trackDesc && trackDesc->psz_name) {
            name = trackDesc->psz_name;
        }
    }
    libvlc_track_description_list_release(rootTrackDesc);

    return name;
}

unsigned JsVlcAudio::count() {
    return _jsPlayer->player().audio().track_count();
}

int JsVlcAudio::track() {
    return _jsPlayer->player().audio().get_track();
}

void JsVlcAudio::setTrack(int track) {
    _jsPlayer->player().audio().set_track(track);
}

int JsVlcAudio::delay() {
    return static_cast<int>( _jsPlayer->player().audio().get_delay());
}

void JsVlcAudio::setDelay(int delay) {
    _jsPlayer->player().audio().set_delay(delay);
}

bool JsVlcAudio::muted() {
    return _jsPlayer->player().audio().is_muted();
}

void JsVlcAudio::setMuted(bool muted) {
    _jsPlayer->player().audio().set_mute(muted);
}

unsigned JsVlcAudio::volume() {
    return _jsPlayer->player().audio().get_volume();
}

void JsVlcAudio::setVolume(unsigned volume) {
    _jsPlayer->player().audio().set_volume(volume);
}

int JsVlcAudio::channel() {
    return _jsPlayer->player().audio().get_channel();
}

void JsVlcAudio::setChannel(unsigned channel) {
    _jsPlayer->player().audio().set_channel((libvlc_audio_output_channel_t) channel);
}

void JsVlcAudio::toggleMute() {
    _jsPlayer->player().audio().toggle_mute();
}
