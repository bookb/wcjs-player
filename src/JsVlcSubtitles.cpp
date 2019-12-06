#include "JsVlcSubtitles.h"

#include "JsVlcPlayer.h"

Napi::FunctionReference JsVlcSubtitles::_jsConstructor;

void JsVlcSubtitles::initJsApi(Napi::Env env) {
    Napi::HandleScope scope(env);
    // VlcSubtitles
    Napi::Function func = DefineClass(env, "JsVlcSubtitles", {
            CLASS_RO_ACCESSOR("count", &JsVlcSubtitles::count),
            CLASS_RW_ACCESSOR("track", &JsVlcSubtitles::track, &JsVlcSubtitles::setTrack),
            CLASS_RW_ACCESSOR("delay", &JsVlcSubtitles::delay, &JsVlcSubtitles::setDelay),

            CLASS_METHOD("description", &JsVlcSubtitles::description),
            CLASS_METHOD("load", &JsVlcSubtitles::load),

    });

    _jsConstructor = Napi::Persistent(func);
    _jsConstructor.SuppressDestruct();
}


JsVlcSubtitles::JsVlcSubtitles(const Napi::CallbackInfo &info) :
        NobiObjectWrap(info) {
    JsVlcPlayer *jsPlayer = JsVlcPlayer::UnwrapThis(info);
    _jsPlayer = jsPlayer;
}

std::string JsVlcSubtitles::description(uint32_t index) {
    vlc_player &p = _jsPlayer->player();

    std::string name;

    libvlc_track_description_t *rootDesc =
            libvlc_video_get_spu_description(p.get_mp());
    if (!rootDesc)
        return name;

    unsigned count = libvlc_video_get_spu_count(p.get_mp());
    if (count && index < count) {
        libvlc_track_description_t *desc = rootDesc;
        for (; index && desc; --index) {
            desc = desc->p_next;
        }

        if (desc && desc->psz_name) {
            name = desc->psz_name;
        }
    }
    libvlc_track_description_list_release(rootDesc);

    return name;
}

unsigned JsVlcSubtitles::count() {
    return _jsPlayer->player().subtitles().track_count();
}

int JsVlcSubtitles::track() {
    return _jsPlayer->player().subtitles().get_track();
}

void JsVlcSubtitles::setTrack(int track) {
    return _jsPlayer->player().subtitles().set_track(track);
}

int JsVlcSubtitles::delay() {
    return static_cast<int>( _jsPlayer->player().subtitles().get_delay());
}

void JsVlcSubtitles::setDelay(int delay) {
    _jsPlayer->player().subtitles().set_delay(delay);
}

bool JsVlcSubtitles::load(const std::string &path) {
    return _jsPlayer->player().subtitles().load(path);
}
