#include "JsVlcPlayer.h"

static Napi::Object Init(Napi::Env env, Napi::Object exports) {
    return JsVlcPlayer::initJsApi(env, exports);
}

// Register and initialize native add-on
// module name -> init handler
NODE_API_MODULE(player_bindings, Init)

