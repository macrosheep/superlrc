//author:Yang Hongyang<burnef@gmail.com>

#ifndef _LIBLRC_H
#define _LIBLRC_H

#include "common.h"

gboolean linit_lyrcs(gchar *filename, sLrcInfo * lrcinfo);
gboolean lget_player_state(sLrcPlugin* lrcPlug);
gboolean lget_music_state(sLrcPlugin* lrcPlug);
gboolean lget_curr_lrcname(sLrcPlugin* lrcPlug);
gboolean lset_play_time(gint time);

gboolean lget_avalible_lrc_num(struct slrcplugin* lrcPlug,gint* num);
gboolean lget_avalible_lrc_info(struct slrcplugin* lrcPlug, gchar** title, gchar** artist, gchar** album, gint index);
gboolean lget_lrc_by_index(struct slrcplugin* lrcPlug,gint index);

#endif
