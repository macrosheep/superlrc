//author:Yang Hongyang<burnef@gmail.com>

#ifndef _MYLRC_H
#define _MYLRC_H

#include <glib.h>
#include <gtk/gtk.h>
#include "debuglog.h"

typedef enum {
	PLAYING,
	PAUSED,
	STOPPED,
} PlayerStateEnum;

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
} sLrcPlugin;

#endif
