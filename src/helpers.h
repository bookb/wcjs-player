#pragma once

#include <iostream>
#include <time.h>

#include <napi.h>

#include <NapiHelpers.h>

template<>
inline std::vector<std::string> FromJsValue<std::vector<std::string>>(const Napi::Value &value) {
    std::vector<std::string> buf = {};
    Napi::Array abuf = value.As<Napi::Array>();
    for (unsigned i = 0; i < (unsigned) (abuf.Length()); ++i) {
        buf.push_back(abuf.Get(i).ToString());
    }
    return buf;
}

//////

template<typename C, typename ... A, size_t ... I>
Napi::Value NobiCallMethod(
        void (C::* method)(const Napi::Env &env, A ...),
        const Napi::CallbackInfo &info,
        StaticSequence<I ...>) {
    Napi::HandleScope scope(info.Env());

    C *instance = Napi::ObjectWrap<C>::Unwrap(info.This().As<Napi::Object>());

    (instance->*method)(
            info.Env(),
            AdjustValue<A>(
                    FromJsValue<
                            typename std::conditional<
                                    std::is_same<A, const char *>::value ||
                                    std::is_same<A, const unsigned char *>::value,
                                    std::string,
                                    typename std::remove_const<
                                            typename std::remove_reference<A>::type>::type
                            >::type
                    >(info[I])) ...);

    return Napi::Value();
}

template<typename R, typename C, typename ... A, size_t ... I>
Napi::Value NobiCallMethod(
        R (C::* method)(A ...),
        const Napi::CallbackInfo &info,
        StaticSequence<I ...>) {
    Napi::HandleScope scope(info.Env());

    C *instance = Napi::ObjectWrap<C>::Unwrap(info.This().As<Napi::Object>());

    return
            ToJsValue(info.Env(),
                      (instance->*method)(
                              AdjustValue<A>(
                                      FromJsValue<
                                              typename std::conditional<
                                                      std::is_same<A, const char *>::value ||
                                                      std::is_same<A, const unsigned char *>::value,
                                                      std::string,
                                                      typename std::remove_const<
                                                              typename std::remove_reference<A>::type>::type
                                              >::type
                                      >(info[I])) ...));
}

template<typename R, typename C, typename ... A, size_t ... I>
Napi::Value NobiCallMethod(
        R (C::* method)(const Napi::CallbackInfo &info, A ...),
        const Napi::CallbackInfo &info,
        StaticSequence<I ...>) {
    Napi::HandleScope scope(info.Env());

    C *instance = Napi::ObjectWrap<C>::Unwrap(info.This().As<Napi::Object>());

    return
            ToJsValue(info.Env(),
                      (instance->*method)(
                              info.Env(),
                              AdjustValue<A>(
                                      FromJsValue<
                                              typename std::conditional<
                                                      std::is_same<A, const char *>::value ||
                                                      std::is_same<A, const unsigned char *>::value,
                                                      std::string,
                                                      typename std::remove_const<
                                                              typename std::remove_reference<A>::type>::type
                                              >::type
                                      >(info[I])) ...));
}

template<typename R, typename C, typename ... A>
Napi::Value NobiCallMethod(
        R (C::* method)(A ...),
        const Napi::CallbackInfo &info) {
    typedef typename MakeStaticSequence<sizeof ... (A)>::SequenceType SequenceType;
    return NobiCallMethod(method, info, SequenceType());
}

template<typename R, typename C, typename ... A>
Napi::Value NobiCallMethod(
        R (C::* method)(const Napi::CallbackInfo &info, A ...),
        const Napi::CallbackInfo &info) {
    typedef typename MakeStaticSequence<sizeof ... (A)>::SequenceType SequenceType;
    return NobiCallMethod(method, info, SequenceType());
}

/////

inline napi_property_descriptor ClassAccessor(
        const char *utf8name,
        NapiCallbackType getter,
        NapiCallbackType setter = nullptr,
        napi_property_attributes attributes = napi_default,
        void *data = nullptr) {
    napi_property_descriptor descriptor{};
    descriptor.utf8name = utf8name;
    descriptor.getter = getter;
    descriptor.setter = setter;
    descriptor.attributes = attributes;
    descriptor.data = data;
    return descriptor;
}

#define CLASS_RW_ACCESSOR(name, getter, setter) \
    ClassAccessor(name, \
        [] (napi_env env, napi_callback_info info) -> napi_value { \
            return CallMethod(getter, env, info); \
        }, \
        [] (napi_env env, napi_callback_info info) -> napi_value { \
            return CallMethod(setter, env, info); \
        } \
    )

#define CLASS_RO_ACCESSOR(name, getter) \
    ClassAccessor(name, \
        [] (napi_env env, napi_callback_info info) -> napi_value { \
            return CallMethod(getter, env, info); \
        } \
    )

/////////////////////////

template<typename T>
class NobiObjectWrap : public Napi::ObjectWrap<T> {
public:
    NobiObjectWrap(const Napi::CallbackInfo &callbackInfo) : Napi::ObjectWrap<T>::ObjectWrap(callbackInfo) {};

    static T *UnwrapThis(const Napi::CallbackInfo &info) {
        return NobiObjectWrap<T>::Unwrap(info[0].As<Napi::Object>());
    }

public:
    static Napi::Object create(const Napi::CallbackInfo &info, const std::initializer_list<napi_value> &args) {
        Napi::HandleScope scope(info.Env());
        Napi::Object instance = T::_jsConstructor.New(args);  // 创建实例
        return instance;
    }

protected:
    // static Napi::FunctionReference _jsConstructor;
    template<typename A>
    static void log(const A &a);

    template<typename A, typename... Args>
    static void log(const A &a, const Args &... args);

private:
    static Napi::FunctionReference _jsConstructor;
};

template<typename T>
template<typename A>
void NobiObjectWrap<T>::log(const A &a) {
    std::cout << a;
}

template<typename T>
template<typename A, typename... Args>
void NobiObjectWrap<T>::log(const A &a, const Args &... args) {
    std::cout << a;
    log(args...);
}

