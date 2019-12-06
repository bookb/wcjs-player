#include "JsVlcPlaylist.h"

#include "JsVlcPlayer.h"
#include "JsVlcPlaylistItems.h"

Napi::FunctionReference JsVlcPlaylist::_jsConstructor;

void JsVlcPlaylist::initJsApi(Napi::Env env) {
    JsVlcPlaylistItems::initJsApi(env);
    Napi::HandleScope scope(env);
    //     constructorTemplate->SetClassName( String::NewFromUtf8( isolate, "VlcPlaylist", v8::NewStringType::kInternalized ).ToLocalChecked() );
    Napi::Function func = DefineClass(env, "JsVlcPlaylist", {
            InstanceValue("Normal", ToJsValue(env, PlaybackMode::Normal)),
            InstanceValue("Loop", ToJsValue(env, PlaybackMode::Loop)),
            InstanceValue("Single", ToJsValue(env, PlaybackMode::Single)),

            CLASS_RO_ACCESSOR("itemCount", &JsVlcPlaylist::itemCount),
            CLASS_RO_ACCESSOR("isPlaying", &JsVlcPlaylist::isPlaying),
            CLASS_RO_ACCESSOR("items", &JsVlcPlaylist::items),

            CLASS_RW_ACCESSOR("currentItem", &JsVlcPlaylist::currentItem, &JsVlcPlaylist::setCurrentItem),
            CLASS_RW_ACCESSOR("mode", &JsVlcPlaylist::mode, &JsVlcPlaylist::setMode),

            CLASS_METHOD("add", &JsVlcPlaylist::add),
            CLASS_METHOD("addWithOptions", &JsVlcPlaylist::addWithOptions),
            CLASS_METHOD("play", &JsVlcPlaylist::play),
            CLASS_METHOD("playItem", &JsVlcPlaylist::playItem),
            CLASS_METHOD("pause", &JsVlcPlaylist::pause),
            CLASS_METHOD("togglePause", &JsVlcPlaylist::togglePause),
            CLASS_METHOD("stop", &JsVlcPlaylist::stop),
            CLASS_METHOD("next", &JsVlcPlaylist::next),
            CLASS_METHOD("prev", &JsVlcPlaylist::prev),
            CLASS_METHOD("clear", &JsVlcPlaylist::clear),
            CLASS_METHOD("removeItem", &JsVlcPlaylist::removeItem),
            CLASS_METHOD("advanceItem", &JsVlcPlaylist::advanceItem),
    });

    _jsConstructor = Napi::Persistent(func);
    _jsConstructor.SuppressDestruct();

}


JsVlcPlaylist::JsVlcPlaylist(const Napi::CallbackInfo &info) :
        NobiObjectWrap(info) {
    JsVlcPlayer *jsPlayer = JsVlcPlayer::UnwrapThis(info);
    _jsPlayer = jsPlayer;
    _jsItems = JsVlcPlaylistItems::create(info, {jsPlayer->Value()});
}

unsigned JsVlcPlaylist::itemCount() {
    return _jsPlayer->player().item_count();
}

bool JsVlcPlaylist::isPlaying() {
    return _jsPlayer->player().is_playing();
}

unsigned JsVlcPlaylist::mode() {
    return static_cast<unsigned>( _jsPlayer->player().get_playback_mode());
}

void JsVlcPlaylist::setMode(unsigned mode) {
    vlc::player &p = _jsPlayer->player();

    switch (mode) {
        case static_cast<unsigned>( PlaybackMode::Normal ):
            p.set_playback_mode(vlc::mode_normal);
            break;
        case static_cast<unsigned>( PlaybackMode::Loop ):
            p.set_playback_mode(vlc::mode_loop);
            break;
        case static_cast<unsigned>( PlaybackMode::Single ):
            p.set_playback_mode(vlc::mode_single);
            break;
    }
}

int JsVlcPlaylist::currentItem() {
    return _jsPlayer->player().current_item();
}

void JsVlcPlaylist::setCurrentItem(unsigned idx) {
    _jsPlayer->player().set_current(idx);
}

int JsVlcPlaylist::add(const std::string &mrl) {
    return _jsPlayer->player().add_media(mrl.c_str());
}

int JsVlcPlaylist::addWithOptions(const std::string &mrl,
                                  const std::vector<std::string> &options) {
    std::vector<const char *> trusted_opts;
    trusted_opts.reserve(options.size());

    for (const std::string &opt: options) {
        trusted_opts.push_back(opt.c_str());
    }

    return _jsPlayer->player().add_media(mrl.c_str(),
                                         0, nullptr,
                                         static_cast<unsigned>( trusted_opts.size()),
                                         trusted_opts.data());
}

void JsVlcPlaylist::play() {
    _jsPlayer->player().play();
}

bool JsVlcPlaylist::playItem(unsigned idx) {
    return _jsPlayer->player().play(idx);
}

void JsVlcPlaylist::pause() {
    _jsPlayer->player().pause();
}

void JsVlcPlaylist::togglePause() {
    _jsPlayer->player().togglePause();
}

void JsVlcPlaylist::stop() {
    _jsPlayer->player().stop();
}

void JsVlcPlaylist::next() {
    _jsPlayer->player().next();
}

void JsVlcPlaylist::prev() {
    _jsPlayer->player().prev();
}

void JsVlcPlaylist::clear() {
    _jsPlayer->player().clear_items();
}

bool JsVlcPlaylist::removeItem(unsigned idx) {
    return _jsPlayer->player().delete_item(idx);
}

void JsVlcPlaylist::advanceItem(unsigned idx, int count) {
    _jsPlayer->player().advance_item(idx, count);
}

Napi::Object JsVlcPlaylist::items() {
    return _jsItems;
}
