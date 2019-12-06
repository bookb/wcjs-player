/*******************************************************************************
* Copyright © 2015, Sergey Radionov <rsatom_gmail.com>
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*   1. Redistributions of source code must retain the above copyright notice,
*      this list of conditions and the following disclaimer.
*   2. Redistributions in binary form must reproduce the above copyright notice,
*      this list of conditions and the following disclaimer in the documentation
*      and/or other materials provided with the distribution.

* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
* OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "vlc_playback.h"

using namespace vlc;

playback::playback( vlc::basic_player& player )
    : _player( player )
{
}

float playback::get_rate()
{
    if( !_player.is_open() )
        return 1.f;

    return libvlc_media_player_get_rate( _player.get_mp() );
}

void playback::set_rate( float rate )
{
    if( !_player.is_open() )
        return;

    libvlc_media_player_set_rate( _player.get_mp(), rate );
}

float playback::get_position()
{
    if( !_player.is_open() )
        return 0.f;

    float p = libvlc_media_player_get_position( _player.get_mp() );

    return p < 0 ? 0 : p;
}

void playback::set_position( float p )
{
    if( !_player.is_open() )
        return;

    libvlc_media_player_set_position( _player.get_mp(), p );
}

libvlc_time_t playback::get_time()
{
    if( !_player.is_open() )
        return 0;

    libvlc_time_t t = libvlc_media_player_get_time( _player.get_mp() );

    return t < 0 ? 0 : t ;
}

void playback::set_time( libvlc_time_t t )
{
    if( !_player.is_open() )
        return;

    libvlc_media_player_set_time( _player.get_mp(), t );
}

libvlc_time_t playback::get_length()
{
    if( !_player.is_open() )
        return 0;

    libvlc_time_t t = libvlc_media_player_get_length( _player.get_mp() );

    return t < 0 ? 0 : t ;
}

float playback::get_fps()
{
    if( !_player.is_open() )
        return 0;

    if ( !_player.current_media() ){
        return 0;
    }
    libvlc_media_track_t **ppTracks;
    unsigned n = libvlc_media_tracks_get( _player.current_media().libvlc_media_t(), &ppTracks);
    if ( n <= 0 ){
        return 0;
    }
    unsigned fns = 0;
    unsigned fds = 0;
    float fps = 0;
    for ( unsigned i = 0; i < n; i++ ){
        libvlc_media_track_t* pTrack = ppTracks[i];
        if ( pTrack && libvlc_track_video ==  pTrack->i_type){
          libvlc_video_track_t* pVideoTrack = pTrack->video;
          fns += pVideoTrack->i_frame_rate_num;
          fds += pVideoTrack->i_frame_rate_den;
        }
    }

    if ( fds > 0 ){
        fps = (float)(fns)/ fds;
    }
    libvlc_media_tracks_release( ppTracks, n);

    return fps;
    // return libvlc_media_player_get_fps( _player.get_mp() );
}