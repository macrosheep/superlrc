//author:Yang Hongyang<burnef@gmail.com>

#ifndef _LIBLRC_H
#define _LIBLRC_H

#include "common.h"

gboolean linit_lyrcs(gchar *filename, sLrcInfo * lrcinfo);
gboolean lget_player_state(sLrcPlugin* lrcPlug);
gboolean lget_music_state(sLrcPlugin* lrcPlug);
gboolean lget_curr_lrcname(sLrcPlugin* lrcPlug);
gboolean lset_play_time(gint time);

#endif
