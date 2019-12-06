#pragma once

#include <napi.h>
#include "helpers.h"

#include <libvlc_wrapper/include/vlc_player.h>

class JsVlcPlayer; //#include "JsVlcPlayer.h"

class JsVlcMedia :
        public NobiObjectWrap<JsVlcMedia> {
public:
    static void initJsApi(Napi::Env env);

    std::string artist();

    std::string genre();

    std::string copyright();

    std::string album();

    std::string trackNumber();

    std::string description();

    std::string rating();

    std::string date();

    std::string URL();

    std::string language();

    std::string nowPlaying();

    std::string publisher();

    std::string encodedBy();

    std::string artworkURL();

    std::string trackID();

    std::string mrl();

    bool parsed();

    void parse();

    void parseAsync();

    std::string title();

    void setTitle(const std::string &);

    std::string setting();

    void setSetting(const std::string &);

    bool disabled();

    void setDisabled(bool);

    double duration();

public:
    JsVlcMedia(const Napi::CallbackInfo &info);

    static Napi::FunctionReference _jsConstructor;

protected:
    vlc::media get_media() { return _media; };

private:
    std::string meta(libvlc_meta_t e_meta);

    void setMeta(libvlc_meta_t e_meta, const std::string &);

    JsVlcPlayer *_jsPlayer;
    vlc::media _media;
};
