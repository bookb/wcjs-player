#include "JsVlcMedia.h"

#include "JsVlcPlayer.h"

Napi::FunctionReference JsVlcMedia::_jsConstructor;

void JsVlcMedia::initJsApi(Napi::Env env) {
    Napi::HandleScope scope(env);
    // JsVlcMedia
    Napi::Function func = DefineClass(env, "JsVlcMedia", {
            CLASS_RO_ACCESSOR("artist", &JsVlcMedia::artist),
            CLASS_RO_ACCESSOR("genre", &JsVlcMedia::genre),
            CLASS_RO_ACCESSOR("copyright", &JsVlcMedia::copyright),
            CLASS_RO_ACCESSOR("album", &JsVlcMedia::album),
            CLASS_RO_ACCESSOR("trackNumber", &JsVlcMedia::trackNumber),
            CLASS_RO_ACCESSOR("description", &JsVlcMedia::description),
            CLASS_RO_ACCESSOR("rating", &JsVlcMedia::rating),
            CLASS_RO_ACCESSOR("date", &JsVlcMedia::date),
            CLASS_RO_ACCESSOR("URL", &JsVlcMedia::URL),
            CLASS_RO_ACCESSOR("language", &JsVlcMedia::language),
            CLASS_RO_ACCESSOR("nowPlaying", &JsVlcMedia::nowPlaying),
            CLASS_RO_ACCESSOR("publisher", &JsVlcMedia::publisher),
            CLASS_RO_ACCESSOR("encodedBy", &JsVlcMedia::encodedBy),
            CLASS_RO_ACCESSOR("artworkURL", &JsVlcMedia::artworkURL),
            CLASS_RO_ACCESSOR("trackID", &JsVlcMedia::trackID),
            CLASS_RO_ACCESSOR("mrl", &JsVlcMedia::mrl),

            CLASS_RO_ACCESSOR("parsed", &JsVlcMedia::parsed),
            CLASS_RO_ACCESSOR("duration", &JsVlcMedia::duration),

            CLASS_RW_ACCESSOR("title",
                              &JsVlcMedia::title,
                              &JsVlcMedia::setTitle),
            CLASS_RW_ACCESSOR("setting",
                              &JsVlcMedia::setting,
                              &JsVlcMedia::setSetting),
            CLASS_RW_ACCESSOR("disabled",
                              &JsVlcMedia::disabled,
                              &JsVlcMedia::setDisabled),

            CLASS_METHOD("parse", &JsVlcMedia::parse),
            CLASS_METHOD("parseAsync", &JsVlcMedia::parseAsync),

    });

    _jsConstructor = Napi::Persistent(func);
    _jsConstructor.SuppressDestruct();
}


JsVlcMedia::JsVlcMedia(const Napi::CallbackInfo &info) :
        NobiObjectWrap(info) {
    log(ToJsValue(Env(), "JsVlcMedia Constructor"));

    JsVlcPlayer *jsPlayer = JsVlcPlayer::UnwrapThis(info);
    vlc::media *media = Napi::ObjectWrap<vlc::media>::Unwrap(info[1].ToObject());
    _jsPlayer = jsPlayer;
    _media = *media;
}

std::string JsVlcMedia::meta(libvlc_meta_t e_meta) {
    return get_media().meta(e_meta);
}

void JsVlcMedia::setMeta(libvlc_meta_t e_meta, const std::string &meta) {
    return get_media().set_meta(e_meta, meta);
}

std::string JsVlcMedia::artist() {
    return meta(libvlc_meta_Artist);
}

std::string JsVlcMedia::genre() {
    return meta(libvlc_meta_Genre);
}

std::string JsVlcMedia::copyright() {
    return meta(libvlc_meta_Copyright);
}

std::string JsVlcMedia::album() {
    return meta(libvlc_meta_Album);
}

std::string JsVlcMedia::trackNumber() {
    return meta(libvlc_meta_TrackNumber);
}

std::string JsVlcMedia::description() {
    return meta(libvlc_meta_Description);
}

std::string JsVlcMedia::rating() {
    return meta(libvlc_meta_Rating);
}

std::string JsVlcMedia::date() {
    return meta(libvlc_meta_Date);
}

std::string JsVlcMedia::URL() {
    return meta(libvlc_meta_URL);
}

std::string JsVlcMedia::language() {
    return meta(libvlc_meta_Language);
}

std::string JsVlcMedia::nowPlaying() {
    return meta(libvlc_meta_NowPlaying);
}

std::string JsVlcMedia::publisher() {
    return meta(libvlc_meta_Publisher);
}

std::string JsVlcMedia::encodedBy() {
    return meta(libvlc_meta_EncodedBy);
}

std::string JsVlcMedia::artworkURL() {
    return meta(libvlc_meta_ArtworkURL);
}

std::string JsVlcMedia::trackID() {
    return meta(libvlc_meta_TrackID);
}

std::string JsVlcMedia::mrl() {
    return get_media().mrl();
}

bool JsVlcMedia::parsed() {
    return get_media().is_parsed();
}

void JsVlcMedia::parse() {
    return get_media().parse();
}

void JsVlcMedia::parseAsync() {
    return get_media().parse(true);
}

std::string JsVlcMedia::title() {
    return meta(libvlc_meta_Title);
}

void JsVlcMedia::setTitle(const std::string &title) {
    setMeta(libvlc_meta_Title, title);
}

std::string JsVlcMedia::setting() {
    vlc_player &p = _jsPlayer->player();

    int idx = p.find_media_index(get_media());
    if (idx >= 0) {
        return p.get_item_data(idx);
    }

    return std::string();
}

void JsVlcMedia::setSetting(const std::string &setting) {
    vlc_player &p = _jsPlayer->player();

    int idx = p.find_media_index(get_media());
    if (idx >= 0) {
        p.set_item_data(idx, setting);
    }
}

bool JsVlcMedia::disabled() {
    vlc_player &p = _jsPlayer->player();

    int idx = p.find_media_index(get_media());
    return idx < 0 ? false : p.is_item_disabled(idx);
}

void JsVlcMedia::setDisabled(bool disabled) {
    vlc_player &p = _jsPlayer->player();

    int idx = p.find_media_index(get_media());
    if (idx >= 0) {
        p.disable_item(idx, disabled);
    }
}

double JsVlcMedia::duration() {
    return static_cast<double>( _media.duration());
}
