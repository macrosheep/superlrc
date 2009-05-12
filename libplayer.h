//author:Yang Hongyang<burnef@gmail.com>

#ifndef _LIBPLAYER_H
#define _LIBPLAYER_H

#include <glib.h>
#include "common.h"

typedef struct splayercontrol{
	PlayerStateEnum (*player_get_state)(void);
	gboolean (*player_get_currsong)(gchar** name,gchar** path);
	gint (*player_get_totaltime)(void);
	gint (*player_get_currplaytime)(void);
	gboolean (*player_set_currplaytime_second)(gint time);
	gboolean (*init)(void);
} sPlayerControl;

extern sPlayerControl rhythmbox_player;
extern sPlayerControl audacious_player;

#endif
