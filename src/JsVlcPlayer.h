#pragma once

#include <memory>
#include <deque>
#include <set>

#include <napi.h>
#include <uv.h>
#include <NapiHelpers.h>

#include "helpers.h"

#include <libvlc_wrapper/include/vlc_player.h>
#include <libvlc_wrapper/include/vlc_vmem.h>

#include "VlcVideoOutput.h"

class JsVlcPlayer :
        public NobiObjectWrap<JsVlcPlayer>,
        private VlcVideoOutput,
        private vlc::media_player_events_callback {
    enum Callbacks_e {
        CB_FrameSetup = 0,
        CB_FrameReady,
        CB_FrameCleanup,

        CB_MediaPlayerMediaChanged,
        CB_MediaPlayerNothingSpecial,
        CB_MediaPlayerOpening,
        CB_MediaPlayerBuffering,
        CB_MediaPlayerPlaying,
        CB_MediaPlayerPaused,
        CB_MediaPlayerStopped,
        CB_MediaPlayerForward,
        CB_MediaPlayerBackward,
        CB_MediaPlayerEndReached,
        CB_MediaPlayerEncounteredError,

        CB_MediaPlayerTimeChanged,
        CB_MediaPlayerPositionChanged,
        CB_MediaPlayerSeekableChanged,
        CB_MediaPlayerPausableChanged,
        CB_MediaPlayerLengthChanged,

        CB_LogMessage,
        CB_Emit,

        CB_Max,
    };

    static const char *callbackNames[CB_Max];

public:

    JsVlcPlayer(const Napi::CallbackInfo &info);

    ~JsVlcPlayer();

    static Napi::FunctionReference _jsConstructor;

    //static void initJsApi( const v8::Local<v8::Object>& exports );
    static Napi::Object initJsApi(Napi::Env env, Napi::Object exports);

    static int indexCallbackByName(const char *name);

    Napi::Value getJsCallback(const Napi::CallbackInfo &info);

    void setJsCallback(const Napi::CallbackInfo &info, const Napi::Value &value);

    bool playing();

    double length();

    unsigned state();

    Napi::Value getVideoFrame();

    Napi::Function getEventEmitter();

    unsigned pixelFormat();

    void setPixelFormat(unsigned);

    double position();

    void setPosition(double);

    double time();

    void setTime(double);

    unsigned volume();

    void setVolume(unsigned);

    bool muted();

    void setMuted(bool);

    bool play(const std::string &mrl);

    void pause();

    void togglePause();

    void stop();

    void toggleMute();

    Napi::Object input();

    Napi::Object audio();

    Napi::Object video();

    Napi::Object subtitles();

    Napi::Object playlist();

    vlc::player &player() { return _player; }

    void close();

public:
    Napi::ObjectReference _this;
private:
    struct AsyncData {
        virtual void process(JsVlcPlayer *) = 0;
    };

    struct CallbackData;
    struct LibvlcEvent;
    struct LibvlcLogEvent;

    static void closeAll();

    void initLibvlc(const Napi::Array &vlcOpts);

    void handleAsync();

    //could come from worker thread
    void media_player_event(const libvlc_event_t *);

    static void log_event_wrapper(void *, int, const libvlc_log_t *, const char *, va_list);

    void log_event(int, const libvlc_log_t *, const char *, va_list);

    void handleLibvlcEvent(const libvlc_event_t &);

    void currentItemEndReached();

    void callCallback(
            Callbacks_e callback,
            const std::initializer_list<napi_value> &list = {});

protected:
    void *onFrameSetup(const RV32VideoFrame &) override;

    void *onFrameSetup(const I420VideoFrame &) override;

    void onFrameReady() override;

    void onFrameCleanup() override;

private:
    static std::set<JsVlcPlayer *> _instances;
    libvlc_instance_t *_libvlc;
    vlc::player _player;

    uv_async_t _async;
    std::mutex _asyncDataGuard;
    std::deque<std::unique_ptr<AsyncData> > _asyncData;

    Napi::FunctionReference _emit;

    Napi::Reference<Napi::Value> _jsFrameBuffer;

    Napi::FunctionReference _jsCallbacks[CB_Max];

    Napi::Object _jsInput;
    Napi::Object _jsAudio;
    Napi::Object _jsVideo;
    Napi::Object _jsSubtitles;
    Napi::Object _jsPlaylist;

    uv_timer_t _errorTimer;
};
