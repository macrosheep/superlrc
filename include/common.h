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


#ifndef _MYLRC_H
#define _MYLRC_H

#include <glib.h>
#include <gtk/gtk.h>
#include "debuglog.h"
#include "libplayer.h"

typedef struct {
	gint time;
	gchar *lyrc;
} sLrcLine;

typedef struct {
	gchar *title;
	gchar *artist;
	gint length;
	GPtrArray *lyrcs;
} sLrcInfo;

typedef struct slrcplugin{
	sLrcInfo* lrc;
	PlayerStateEnum playerstate;
	gboolean songchanged;
	gboolean lrcredowned;
	gchar* currsong;
	gchar* currsongpath;
	gchar* currlrcname;
	gint totaltime;
	gint currplaytime;
	gint lastplaytime;

	gboolean (*init_lyrcs)(gchar *filename, sLrcInfo* lrcinfo);
	gboolean (*get_curr_lrcname)(struct slrcplugin* lrcPlug);
	gboolean (*get_player_state)(struct slrcplugin* lrcPlug);
	gboolean (*get_music_state)(struct slrcplugin* lrcPlug);
	gboolean (*set_play_time)(gint time);
	//used for redownload lrcs
	gboolean (*get_avalible_lrc_num)(struct slrcplugin* lrcPlug,gint* num);
	gboolean (*get_avalible_lrc_info)(struct slrcplugin* lrcPlug, gchar** title, gchar** artist, gchar** album, gint index);
	gboolean (*get_lrc_by_index)(struct slrcplugin* lrcPlug,gint index);
} sLrcPlugin;

#endif
