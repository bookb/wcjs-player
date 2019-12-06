#include "JsVlcPlayer.h"

#include <string.h>

#include "JsVlcInput.h"
#include "JsVlcAudio.h"
#include "JsVlcVideo.h"
#include "JsVlcSubtitles.h"
#include "JsVlcPlaylist.h"

#if V8_MAJOR_VERSION > 4 || \
    (V8_MAJOR_VERSION == 4 && V8_MINOR_VERSION > 4) || \
    (V8_MAJOR_VERSION == 4 && V8_MINOR_VERSION == 4 && V8_BUILD_NUMBER >= 26)

#define USE_ARRAY_BUFFER 1

#endif

#undef min
#undef max

const char *JsVlcPlayer::callbackNames[] =
        {
                "FrameSetup",
                "FrameReady",
                "FrameCleanup",

                "MediaChanged",
                "NothingSpecial",
                "Opening",
                "Buffering",
                "Playing",
                "Paused",
                "Stopped",
                "Forward",  // 10
                "Backward",
                "EndReached",
                "EncounteredError",

                "TimeChanged",
                "PositionChanged",
                "SeekableChanged",
                "PausableChanged",
                "LengthChanged",

                "LogMessage",  // 19
                "Emit"
        };

Napi::FunctionReference JsVlcPlayer::_jsConstructor;
std::set<JsVlcPlayer *> JsVlcPlayer::_instances;

///////////////////////////////////////////////////////////////////////////////
struct JsVlcPlayer::CallbackData : public JsVlcPlayer::AsyncData {
    CallbackData(JsVlcPlayer::Callbacks_e callback) :
            callback(callback) {}

    void process(JsVlcPlayer *);

    const JsVlcPlayer::Callbacks_e callback;
};

void JsVlcPlayer::CallbackData::process(JsVlcPlayer *jsPlayer) {
    jsPlayer->callCallback(callback);
}

///////////////////////////////////////////////////////////////////////////////
struct JsVlcPlayer::LibvlcEvent : public JsVlcPlayer::AsyncData {
    LibvlcEvent(const libvlc_event_t &libvlcEvent) :
            libvlcEvent(libvlcEvent) {}

    void process(JsVlcPlayer *);

    const libvlc_event_t libvlcEvent;
};

void JsVlcPlayer::LibvlcEvent::process(JsVlcPlayer *jsPlayer) {
    Napi::HandleScope scope(jsPlayer->Env());
    jsPlayer->handleLibvlcEvent(libvlcEvent);
}

///////////////////////////////////////////////////////////////////////////////
struct JsVlcPlayer::LibvlcLogEvent : public JsVlcPlayer::AsyncData {
    LibvlcLogEvent(int level, const std::string &message, const std::string &format) :
            level(level), message(message), format(format) {}

    void process(JsVlcPlayer *);

    const int level;
    std::string message;
    std::string format;
};

void JsVlcPlayer::LibvlcLogEvent::process(JsVlcPlayer *jsPlayer) {
    Napi::Env env = jsPlayer->Env();
    Napi::HandleScope scope(env);
    //
    Napi::Value jsLevel = ToJsValue(env, level);
    Napi::Value jsMessage = ToJsValue(env, message.c_str());
    Napi::Value jsFormat = ToJsValue(env, format.c_str());
    //
    log("JsVlcPlayer::LibvlcLogEvent::process...\n");
    //
    jsPlayer->callCallback(CB_LogMessage, {jsLevel, jsMessage, jsFormat});
}

///////////////////////////////////////////////////////////////////////////////
#define SET_CALLBACK_PROPERTY(name, callback) \
            InstanceAccessor(name, \
                   &JsVlcPlayer::getJsCallback, \
                   reinterpret_cast<InstanceSetterCallback>(&JsVlcPlayer::setJsCallback), \
                   napi_default, \
                   (void *)callbackNames[callback])

//void JsVlcPlayer::initJsApi( const v8::Local<v8::Object>& exports )
Napi::Object JsVlcPlayer::initJsApi(Napi::Env env, Napi::Object exports) {

    //node::AtExit([] ( void* ) { JsVlcPlayer::closeAll(); });
    atexit([](void) { JsVlcPlayer::closeAll(); });

    // open a new scope
    Napi::HandleScope scope(env);

    log("JsVlcPlayer::initJsApi...\n");

    JsVlcInput::initJsApi(env);
    JsVlcAudio::initJsApi(env);
    JsVlcVideo::initJsApi(env);
    JsVlcSubtitles::initJsApi(env);
    JsVlcPlaylist::initJsApi(env);

    Napi::String vlcVersion = ToJsValue(env, libvlc_get_version()).ToString();
    Napi::String vlcChangeset = ToJsValue(env, libvlc_get_changeset()).ToString();

    //  utf8name: Name of the JavaScript constructor function;
    //  this is not required to be the same as the C++ class name,
    //  though it is recommended for clarity.

    Napi::Function func = DefineClass(env, "JsVlcPlayer", {
            InstanceValue("RV32", Napi::Number::New(env, static_cast<int>( PixelFormat::RV32 ))),
            InstanceValue("I420", Napi::Number::New(env, static_cast<int>( PixelFormat::RV32 ))),
            InstanceValue("NothingSpecial", Napi::Number::New(env, libvlc_NothingSpecial)),
            InstanceValue("Opening", Napi::Number::New(env, libvlc_Opening)),
            InstanceValue("Buffering", Napi::Number::New(env, libvlc_Buffering)),
            InstanceValue("Playing", Napi::Number::New(env, libvlc_Playing)),
            InstanceValue("Paused", Napi::Number::New(env, libvlc_Paused)),
            InstanceValue("Playing", Napi::Number::New(env, libvlc_Playing)),
            InstanceValue("Stopped", Napi::Number::New(env, libvlc_Stopped)),
            InstanceValue("Ended", Napi::Number::New(env, libvlc_Ended)),
            InstanceValue("Error", Napi::Number::New(env, libvlc_Error)),

            InstanceValue("vlc_version", vlcVersion),
            InstanceValue("vlc_changeset", vlcChangeset),

            SET_CALLBACK_PROPERTY("onFrameSetup", CB_FrameSetup),
            SET_CALLBACK_PROPERTY("onFrameReady", CB_FrameReady),
            SET_CALLBACK_PROPERTY("onFrameCleanup", CB_FrameCleanup),

            SET_CALLBACK_PROPERTY("onMediaChanged", CB_MediaPlayerMediaChanged),
            SET_CALLBACK_PROPERTY("onNothingSpecial", CB_MediaPlayerNothingSpecial),
            SET_CALLBACK_PROPERTY("onOpening", CB_MediaPlayerOpening),
            SET_CALLBACK_PROPERTY("onBuffering", CB_MediaPlayerBuffering),
            SET_CALLBACK_PROPERTY("onPlaying", CB_MediaPlayerPlaying),
            SET_CALLBACK_PROPERTY("onPaused", CB_MediaPlayerPaused),
            SET_CALLBACK_PROPERTY("onForward", CB_MediaPlayerForward),
            SET_CALLBACK_PROPERTY("onBackward", CB_MediaPlayerBackward),
            SET_CALLBACK_PROPERTY("onEncounteredError", CB_MediaPlayerEncounteredError),
            SET_CALLBACK_PROPERTY("onEndReached", CB_MediaPlayerEndReached),
            SET_CALLBACK_PROPERTY("onStopped", CB_MediaPlayerStopped),

            SET_CALLBACK_PROPERTY("onTimeChanged", CB_MediaPlayerTimeChanged),
            SET_CALLBACK_PROPERTY("onPositionChanged", CB_MediaPlayerPositionChanged),
            SET_CALLBACK_PROPERTY("onSeekableChanged", CB_MediaPlayerSeekableChanged),
            SET_CALLBACK_PROPERTY("onPausableChanged", CB_MediaPlayerPausableChanged),
            SET_CALLBACK_PROPERTY("onLengthChanged", CB_MediaPlayerLengthChanged),

            SET_CALLBACK_PROPERTY("onLogMessage", CB_LogMessage),
            SET_CALLBACK_PROPERTY("onEmit", CB_Emit),

            CLASS_METHOD("playing", &JsVlcPlayer::playing),
            CLASS_METHOD("length", &JsVlcPlayer::length),
            CLASS_METHOD("state", &JsVlcPlayer::state),

            CLASS_METHOD("input", &JsVlcPlayer::input),
            CLASS_METHOD("audio", &JsVlcPlayer::audio),
            CLASS_METHOD("video", &JsVlcPlayer::video),
            CLASS_METHOD("subtitles", &JsVlcPlayer::subtitles),
            CLASS_METHOD("playlist", &JsVlcPlayer::playlist),

            CLASS_METHOD("videoFrame", &JsVlcPlayer::getVideoFrame),
            CLASS_METHOD("events", &JsVlcPlayer::getEventEmitter),

            CLASS_RW_ACCESSOR("pixelFormat", &JsVlcPlayer::pixelFormat, &JsVlcPlayer::setPixelFormat),
            CLASS_RW_ACCESSOR("position", &JsVlcPlayer::position, &JsVlcPlayer::setPosition),
            CLASS_RW_ACCESSOR("time", &JsVlcPlayer::time, &JsVlcPlayer::setTime),
            CLASS_RW_ACCESSOR("volume", &JsVlcPlayer::volume, &JsVlcPlayer::setVolume),
            CLASS_RW_ACCESSOR("mute", &JsVlcPlayer::muted, &JsVlcPlayer::setMuted),

            CLASS_METHOD("play", &JsVlcPlayer::play),
            CLASS_METHOD("pause", &JsVlcPlayer::pause),
            CLASS_METHOD("togglePause", &JsVlcPlayer::togglePause),
            CLASS_METHOD("stop", &JsVlcPlayer::stop),
            CLASS_METHOD("pause", &JsVlcPlayer::pause),
            CLASS_METHOD("toggleMute", &JsVlcPlayer::toggleMute),
            CLASS_METHOD("close", &JsVlcPlayer::close),
    });

    // Create a persistent reference to the class constructor. This will allow
    // a function called on a class prototype and a function
    // called on instance of a class to be distinguished from each other.
    _jsConstructor = Napi::Persistent(func);
    // Call the SuppressDestruct() method on the static data prevent the calling
    // to this destructor to reset the reference when the environment is no longer
    // available.
    _jsConstructor.SuppressDestruct();
    //
    exports.Set("VlcPlayer", func);
    exports.Set("createPlayer", func);
    exports.Set("vlc_version", vlcVersion);
    exports.Set("vlc_changeset", vlcChangeset);

    return exports;
}

void JsVlcPlayer::closeAll() {
    log("JsVlcPlayer closeAll...\n");
    // 清理静态变量
    for (JsVlcPlayer *p : _instances) {
        p->close();
    }
}

JsVlcPlayer::JsVlcPlayer(const Napi::CallbackInfo &info) :
        NobiObjectWrap<JsVlcPlayer>(info),
        _libvlc(nullptr) {
    _instances.insert(this);
    log(std::time(0), ":JsVlcPlayer Constructor...\n");

    uv_loop_t *loop = uv_default_loop();

    uv_async_init(loop,
                  &_async,
                  [](uv_async_t *handle) {
                      if (handle->data) {
                          reinterpret_cast<JsVlcPlayer *>( handle->data )->handleAsync();
                      }
                  }
    );
    _async.data = this;

    uv_timer_init(loop, &_errorTimer);
    _errorTimer.data = this;

    log("initLibvlc...\n");

    Napi::Array vlcOpts = {};
    if (info[0].IsArray()) {
        vlcOpts = info[0].As<Napi::Array>();
    }

    initLibvlc(vlcOpts);

    _player.set_playback_mode(vlc::mode_normal);

    if (_libvlc && _player.open(_libvlc)) {
        _player.register_callback(this);
        VlcVideoOutput::open(&_player.basic_player());
    } else {
        assert(false);
    }

    log("_jsInput...\n");

    _jsInput = JsVlcInput::create(info, {this->Value()});
    _jsAudio = JsVlcAudio::create(info, {this->Value()});
    _jsVideo = JsVlcVideo::create(info, {this->Value()});
    _jsSubtitles = JsVlcSubtitles::create(info, {this->Value()});
    _jsPlaylist = JsVlcPlaylist::create(info, {this->Value()});

}

void JsVlcPlayer::initLibvlc(const Napi::Array &vlcOpts) {
    if (_libvlc) {
        assert(false);
        libvlc_release(_libvlc);
        _libvlc = nullptr;
    }

    if (vlcOpts.IsEmpty() || vlcOpts.Length() == 0) {
        _libvlc = libvlc_new(0, nullptr);
    } else {
        std::deque<std::string> opts;
        std::vector<const char *> libvlcOpts;

        for (unsigned i = 0;
             i < std::min<unsigned>(vlcOpts.Length(), std::numeric_limits<short>::max());
             ++i) {
            std::string opt = Napi::String::From(Env(), vlcOpts[i].ToString()).Utf8Value();
            if (opt.length()) {
                auto it = opts.emplace(opts.end(), opt);
                // log(ToJsValue(Env(),it->c_str()));
                libvlcOpts.push_back(it->c_str());
            }
        }

        _libvlc = libvlc_new(static_cast<int>( libvlcOpts.size()), libvlcOpts.data());
    }

    if (_libvlc) {
        libvlc_log_set(_libvlc, JsVlcPlayer::log_event_wrapper, this);
    }
}

JsVlcPlayer::~JsVlcPlayer() {
    log(std::time(0), ":JsVlcPlayer Destructor...\n");
    close();
    _instances.erase(this);
}

void JsVlcPlayer::close() {
    log("JsVlcPlayer Closing...\n");

    VlcVideoOutput::close();
    _player.unregister_callback(this);
    _player.close();

    _async.data = nullptr;
    if (_async.loop) {
        uv_close(reinterpret_cast<uv_handle_t *>( &_async ), 0);
    }
    _errorTimer.data = nullptr;
    uv_timer_stop(&_errorTimer);

    if (_libvlc) {
        libvlc_release(_libvlc);
        _libvlc = nullptr;
    }
}

void JsVlcPlayer::media_player_event(const libvlc_event_t *e) {
    _asyncDataGuard.lock();
    _asyncData.emplace_back(new LibvlcEvent(*e));
    _asyncDataGuard.unlock();
    uv_async_send(&_async);
}

void JsVlcPlayer::log_event_wrapper(void *data, int level, const libvlc_log_t *ctx, const char *fmt, va_list args) {
    ((JsVlcPlayer *) data)->log_event(level, ctx, fmt, args);
}

#ifndef _MSC_VER

inline int _vscprintf(const char *format, va_list argptr) {
    return vsnprintf(nullptr, 0, format, argptr);
}

#endif

void JsVlcPlayer::log_event(int level, const libvlc_log_t *ctx, const char *fmt, va_list args) {
    va_list argsCopy;
    va_copy(argsCopy, args);
    int messageSize = _vscprintf(fmt, argsCopy);
    va_end(argsCopy);

    // If the format string is bad, there is nothing we'll ever be able to do.
    if (messageSize <= 0)
        return;

    std::string message(messageSize + 1, '\0');
    // vsnprintf is a bit of a mess in Microsoft-land, older versions do not guarantee termination.
    vsnprintf(&message[0], message.size(), fmt, args);
    while ('\0' == message[message.size() - 1])
        message.resize(message.size() - 1);

    _asyncDataGuard.lock();
    _asyncData.emplace_back(new LibvlcLogEvent(level, message, fmt));
    _asyncDataGuard.unlock();

    uv_async_send(&_async);
}

void JsVlcPlayer::handleAsync() {
    while (!_asyncData.empty()) {
        std::deque<std::unique_ptr<AsyncData> > tmpData;
        _asyncDataGuard.lock();
        _asyncData.swap(tmpData);
        _asyncDataGuard.unlock();
        for (const auto &i: tmpData) {
            i->process(this);

            //events queue could be very long...
            if (VlcVideoOutput::isFrameReady()) {
                onFrameReady();
            }
        }
    }
}

void *JsVlcPlayer::onFrameSetup(const RV32VideoFrame &videoFrame) {

    if (0 == videoFrame.width() || 0 == videoFrame.height() || 0 == videoFrame.size()) {
        assert(false);
        return nullptr;
    }

    Napi::Uint8Array jsArray = Napi::Uint8Array::New(Env(), videoFrame.size());

    Napi::Number jsWidth = Napi::Number::New(Env(), videoFrame.width());
    Napi::Number jsHeight = Napi::Number::New(Env(), videoFrame.height());
    Napi::Number jsPixelFormat = Napi::Number::New(Env(), static_cast<int>( PixelFormat::RV32 ));

    jsArray.DefineProperties(
            {
                    Napi::PropertyDescriptor::Value("width", jsWidth),
                    Napi::PropertyDescriptor::Value("height", jsHeight),
                    Napi::PropertyDescriptor::Value("pixelFormat", jsPixelFormat),
                    Napi::PropertyDescriptor::Value("width", jsWidth),
            });

    _jsFrameBuffer.Reset(jsArray, 1);

    callCallback(CB_FrameSetup, {jsWidth, jsHeight, jsPixelFormat, jsArray});

    return jsArray.ArrayBuffer().Data();
}

void *JsVlcPlayer::onFrameSetup(const I420VideoFrame &videoFrame) {

    if (0 == videoFrame.width() || 0 == videoFrame.height() ||
        0 == videoFrame.uPlaneOffset() || 0 == videoFrame.vPlaneOffset() ||
        0 == videoFrame.size()) {
        assert(false);
        return nullptr;
    }
    Napi::HandleScope scope(Env());

    Napi::Uint8Array jsArray = Napi::Uint8Array::New(Env(), videoFrame.size());

    Napi::Number jsWidth = Napi::Number::New(Env(), videoFrame.width());
    Napi::Number jsHeight = Napi::Number::New(Env(), videoFrame.height());
    Napi::Number jsPixelFormat = Napi::Number::New(Env(), static_cast<int>( PixelFormat::I420 ));

    jsArray.DefineProperties(
            {
                    Napi::PropertyDescriptor::Value("width", jsWidth),
                    Napi::PropertyDescriptor::Value("height", jsHeight),
                    Napi::PropertyDescriptor::Value("pixelFormat", jsPixelFormat),
                    Napi::PropertyDescriptor::Value("uOffset",
                                                    ToJsValue(Env(), videoFrame.uPlaneOffset())),
                    Napi::PropertyDescriptor::Value("vOffset",
                                                    ToJsValue(Env(), videoFrame.vPlaneOffset()))
            });


    _jsFrameBuffer.Reset(jsArray, 1);

    callCallback(CB_FrameSetup, {jsWidth, jsHeight, jsPixelFormat, jsArray});

    return jsArray.ArrayBuffer().Data();
}

void JsVlcPlayer::onFrameReady() {

    Napi::HandleScope scope(Env());

    log("<<<onFrameReady: buffer is ", (_jsFrameBuffer.IsEmpty() ? "empty" : "ok"), "\n");

    assert(!_jsFrameBuffer.IsEmpty()); //FIXME! maybe it worth add condition here
    callCallback(CB_FrameReady, {Napi::Value::From(Env(), _jsFrameBuffer.Value())});
}

void JsVlcPlayer::onFrameCleanup() {
    log("onFrameCleanup\n");
    callCallback(CB_FrameCleanup);
}

void JsVlcPlayer::handleLibvlcEvent(const libvlc_event_t &libvlcEvent) {

    Callbacks_e callback = CB_Max;
    log("<<<<<handleLibvlcEvent: ", libvlcEvent.type, "\n");

    Napi::HandleScope scope(Env());

    switch (libvlcEvent.type) {
        case libvlc_MediaPlayerMediaChanged:
            callback = CB_MediaPlayerMediaChanged;
            break;
        case libvlc_MediaPlayerNothingSpecial:
            callback = CB_MediaPlayerNothingSpecial;
            break;
        case libvlc_MediaPlayerOpening:
            callback = CB_MediaPlayerOpening;
            break;
        case libvlc_MediaPlayerBuffering: {
            callCallback(CB_MediaPlayerBuffering,
                         {Napi::Number::New(Env(), libvlcEvent.u.media_player_buffering.new_cache)});
            break;
        }
        case libvlc_MediaPlayerPlaying:
            callback = CB_MediaPlayerPlaying;
            break;
        case libvlc_MediaPlayerPaused:
            callback = CB_MediaPlayerPaused;
            break;
        case libvlc_MediaPlayerStopped:
            callback = CB_MediaPlayerStopped;
            break;
        case libvlc_MediaPlayerForward:
            callback = CB_MediaPlayerForward;
            break;
        case libvlc_MediaPlayerBackward:
            callback = CB_MediaPlayerBackward;
            break;
        case libvlc_MediaPlayerEndReached:
            callback = CB_MediaPlayerEndReached;
            uv_timer_stop(&_errorTimer);
            currentItemEndReached();
            break;
        case libvlc_MediaPlayerEncounteredError:
            callback = CB_MediaPlayerEncounteredError;
            //sometimes libvlc do some internal error handling
            //and sends EndReached after that,
            //so we have to wait it some time,
            //to not break playlist ligic.
            uv_timer_start(&_errorTimer,
                           [](uv_timer_t *handle) {
                               if (handle->data)
                                   static_cast<JsVlcPlayer *>( handle->data )->currentItemEndReached();
                           }, 1000, 0);
            break;
        case libvlc_MediaPlayerTimeChanged: {
            const double new_time =
                    static_cast<double>( libvlcEvent.u.media_player_time_changed.new_time );
            callCallback(CB_MediaPlayerTimeChanged,
                         {ToJsValue(Env(), static_cast<double>( new_time ))});
            break;
        }
        case libvlc_MediaPlayerPositionChanged: {
            callCallback(CB_MediaPlayerPositionChanged,
                         {ToJsValue(Env(), libvlcEvent.u.media_player_position_changed.new_position)});
            break;
        }
        case libvlc_MediaPlayerSeekableChanged: {
            callCallback(CB_MediaPlayerSeekableChanged,
                         {ToJsValue(Env(), libvlcEvent.u.media_player_seekable_changed.new_seekable != 0)});
            break;
        }
        case libvlc_MediaPlayerPausableChanged: {
            callCallback(CB_MediaPlayerPausableChanged,
                         {ToJsValue(Env(), libvlcEvent.u.media_player_pausable_changed.new_pausable != 0)});
            break;
        }
        case libvlc_MediaPlayerLengthChanged: {
            const double new_length =
                    static_cast<double>( libvlcEvent.u.media_player_length_changed.new_length );
            callCallback(CB_MediaPlayerLengthChanged, {ToJsValue(Env(), new_length)});
            break;
        }
    }

    if (callback != CB_Max) {
        callCallback(callback);
    }
}

void JsVlcPlayer::currentItemEndReached() {
    log("currentItemEndReached...\n");
    if (vlc::mode_single != player().get_playback_mode())
        player().next();
}

void JsVlcPlayer::callCallback(
        Callbacks_e callback,
        const std::initializer_list<napi_value> &list) {

    Napi::HandleScope scope(Env());

    if (!_jsCallbacks[callback].IsEmpty()) {
        Napi::Function callbackFunc = _jsCallbacks[callback].Value();
        log("callback: ", callback, "\n");
        callbackFunc.Call(list);
        /// TEST  _emit.Call({ToJsValue(Env(), "test"), *(list.begin() + 1)});
    }
}

Napi::Value JsVlcPlayer::getJsCallback(const Napi::CallbackInfo &info) {
    JsVlcPlayer *jsPlayer = UnwrapThis(info);
    Callbacks_e eventId = (Callbacks_e) (*(uint32_t *) info.Data());
    if (jsPlayer->_jsCallbacks[eventId].IsEmpty())
        return Napi::Value{};
    return jsPlayer->_jsCallbacks[eventId].Value();
}

int JsVlcPlayer::indexCallbackByName(const char *name) {
    int index = -1;
    if (!name) {
        return index;
    }
    unsigned length = sizeof(callbackNames) / sizeof(const char *);
    for (unsigned i = 0; i < CB_Max && i < length; i++) {
        if (0 == strcmp(callbackNames[i], name)) {
            index = i;
            break;
        }
    }
    return index;
}

void JsVlcPlayer::setJsCallback(const Napi::CallbackInfo &info,
                                const Napi::Value &value) {

    Napi::HandleScope scope(Env());

    log(ToJsValue(Env(), "JsVlcPlayer::setJsCallback"));

    int index = indexCallbackByName((char *) info.Data());
    if (index == -1) {
        return;
    }

    Callbacks_e eventId = (Callbacks_e) index;

    Napi::Function callbackFunc = value.As<Napi::Function>();
    if (CB_Emit == eventId) {
        _emit = Napi::Persistent(callbackFunc);  //必须是Persistent， 否则被垃圾回收 .Reset(callbackFunc);
    }
    if (!callbackFunc.IsEmpty()) {
        this->_jsCallbacks[eventId] = Napi::Persistent(callbackFunc);  // Reset --> Persistent
    }
}

bool JsVlcPlayer::playing() {
    return player().is_playing();
}

double JsVlcPlayer::length() {
    return static_cast<double>( player().playback().get_length());
}

unsigned JsVlcPlayer::state() {
    return player().get_state();
}

Napi::Value JsVlcPlayer::getVideoFrame() {
    return _jsFrameBuffer.Value();
}

Napi::Function JsVlcPlayer::getEventEmitter() {
    return _emit.Value();
}

unsigned JsVlcPlayer::pixelFormat() {
    return static_cast<unsigned>( VlcVideoOutput::pixelFormat());
}

void JsVlcPlayer::setPixelFormat(unsigned format) {
    switch (format) {
        case static_cast<unsigned>( PixelFormat::RV32 ):
            VlcVideoOutput::setPixelFormat(PixelFormat::RV32);
            break;
        case static_cast<unsigned>( PixelFormat::I420 ):
            VlcVideoOutput::setPixelFormat(PixelFormat::I420);
            break;
    }
}

double JsVlcPlayer::position() {
    return player().playback().get_position();
}

void JsVlcPlayer::setPosition(double position) {
    player().playback().set_position(static_cast<float>( position ));
}

double JsVlcPlayer::time() {
    return static_cast<double>( player().playback().get_time());
}

void JsVlcPlayer::setTime(double time) {
    player().playback().set_time(static_cast<libvlc_time_t>( time ));
}

unsigned JsVlcPlayer::volume() {
    return player().audio().get_volume();
}

void JsVlcPlayer::setVolume(unsigned volume) {
    player().audio().set_volume(volume);
}

bool JsVlcPlayer::muted() {
    return player().audio().is_muted();
}

void JsVlcPlayer::setMuted(bool mute) {
    player().audio().set_mute(mute);
}

bool JsVlcPlayer::play(const std::string &mrl) {
    vlc::player &p = player();

    if (mrl.length() == 0 || 0 == mrl.compare("undefined")) {
        log(ToJsValue(Env(), "JsVlcPlayer::play...1..."));
        p.play();
        return true;
    }
    log(ToJsValue(Env(), mrl));
    p.clear_items();
    const int idx = p.add_media(mrl.c_str());
    if (idx >= 0) {
        p.play(idx);
        return true;
    }

    return false;
}

void JsVlcPlayer::pause() {
    player().pause();
}

void JsVlcPlayer::togglePause() {
    player().togglePause();
}

void JsVlcPlayer::stop() {
    player().stop();
}

void JsVlcPlayer::toggleMute() {
    player().audio().toggle_mute();
}

Napi::Object JsVlcPlayer::input() {
    return _jsInput;
}

Napi::Object JsVlcPlayer::audio() {
    return _jsAudio;
}

Napi::Object JsVlcPlayer::video() {
    return _jsVideo;
}

Napi::Object JsVlcPlayer::subtitles() {
    return _jsSubtitles;
}

Napi::Object JsVlcPlayer::playlist() {
    return _jsPlaylist;
}
