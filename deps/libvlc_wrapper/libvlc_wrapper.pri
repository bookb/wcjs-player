include( libvlc-sdk/libvlc-sdk.pri )

CONFIG += c++11

HEADERS += $$PWD/include/vlc_vmem.h \
    $$PWD/include/vlc_audio.h \
    $$PWD/include/vlc_basic_player.h \
    $$PWD/include/vlc_helpers.h \
    $$PWD/include/vlc_player.h \
    $$PWD/include/vlc_subtitles.h \
    $$PWD/include/vlc_playback.h \
    $$PWD/include/vlc_video.h \
    $$PWD/include/vlc_media.h \
    $$PWD/include/callbacks_holder.h

SOURCES += $$PWD/src/vlc_vmem.cpp \
    $$PWD/src/vlc_audio.cpp \
    $$PWD/src/vlc_basic_player.cpp \
    $$PWD/src/vlc_helpers.cpp \
    $$PWD/src/vlc_player.cpp \
    $$PWD/src/vlc_subtitles.cpp \
    $$PWD/src/vlc_playback.cpp \
    $$PWD/src/vlc_video.cpp\
    $$PWD/src/vlc_media.cpp

!android {
    HEADERS += $$PWD/src/vlc_media_list_player.h

    SOURCES += $$PWD/src/vlc_media_list_player.cpp
}

INCLUDEPATH += $$PWD
