/*  superlrc
 *  Copyright (C) 2005-2007  Yang Hongyang<burnef@gmail.com>.
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
