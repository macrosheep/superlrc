/*  superlrc
 *  Copyright (C) 2009-2011  Yang Hongyang<burnef@gmail.com>.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; under version 3 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses>.
 */

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
