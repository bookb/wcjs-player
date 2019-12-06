#pragma once

#include <napi.h>

#include "helpers.h"

#include <libvlc_wrapper/include/vlc_player.h>

class JsVlcPlayer; //#include "JsVlcPlayer.h"

class JsVlcPlaylist :
        public NobiObjectWrap<JsVlcPlaylist> {
public:
    enum class PlaybackMode {
        Normal = vlc::mode_normal,
        Loop = vlc::mode_loop,
        Single = vlc::mode_single,
    };

    static void initJsApi(Napi::Env env);

    unsigned itemCount();

    bool isPlaying();

    int currentItem();

    void setCurrentItem(unsigned);

    unsigned mode();

    void setMode(unsigned);

    int add(const std::string &mrl);

    int addWithOptions(const std::string &mrl, const std::vector<std::string> &options);

    void play();

    bool playItem(unsigned idx);

    void pause();

    void togglePause();

    void stop();

    void next();

    void prev();

    void clear();

    bool removeItem(unsigned idx);

    void advanceItem(unsigned idx, int count);

    Napi::Object items();

public:
    static Napi::FunctionReference _jsConstructor;

    JsVlcPlaylist(const Napi::CallbackInfo &info);

private:
    Napi::Object _jsItems;
    JsVlcPlayer *_jsPlayer;

};
