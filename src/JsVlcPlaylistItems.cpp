#include "JsVlcPlaylistItems.h"

#include "JsVlcPlayer.h"
#include "JsVlcMedia.h"

Napi::FunctionReference JsVlcPlaylistItems::_jsConstructor;

void JsVlcPlaylistItems::initJsApi(Napi::Env env) {
    JsVlcMedia::initJsApi(env);
    Napi::HandleScope scope(env);
    // VlcPlaylistItems
    Napi::Function func = DefineClass(env, "JsVlcPlaylistItems", {

            CLASS_RO_ACCESSOR("count", &JsVlcPlaylistItems::count),
            CLASS_METHOD("clear", &JsVlcPlaylistItems::clear),
            CLASS_METHOD("remove", &JsVlcPlaylistItems::remove),
            InstanceMethod("item", &JsVlcPlaylistItems::item),

    });

    _jsConstructor = Napi::Persistent(func);
    _jsConstructor.SuppressDestruct();

}


JsVlcPlaylistItems::JsVlcPlaylistItems(const Napi::CallbackInfo &info) :
        NobiObjectWrap(info) {
    JsVlcPlayer *jsPlayer = JsVlcPlayer::UnwrapThis(info);
    _jsPlayer = jsPlayer;
}

Napi::Value JsVlcPlaylistItems::item(const Napi::CallbackInfo &info) {
    log("JsVlcPlaylistItems::item\n");
    uint32_t index = info[0].As<Napi::Number>().Uint32Value();
    vlc::media media = _jsPlayer->player().get_media(index);
    std::initializer_list<napi_value> args = {
            _jsPlayer->Value(),
            Napi::External<vlc::media>::New(Env(), const_cast<vlc::media *>( &media ))
    };
    //Napi::EscapableHandleScope scope(info.Env());
    //scope.Escape();
    return JsVlcMedia::create(info, args);
}

unsigned JsVlcPlaylistItems::count() {
    return _jsPlayer->player().item_count();
}

void JsVlcPlaylistItems::clear() {
    return _jsPlayer->player().clear_items();
}

bool JsVlcPlaylistItems::remove(unsigned int idx) {
    return _jsPlayer->player().delete_item(idx);
}
