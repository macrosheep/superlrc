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

#include "libplayer.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

PlayerStateEnum
audacious_get_state()
{
	FILE *stream;
	gchar buf[10];
	PlayerStateEnum state;
	gint i;

	memset(buf,0,sizeof(buf));

	stream = popen("audtool --playback-status 2>/dev/null","r");
	fread(buf, sizeof(gchar),sizeof(buf)-1,stream);

	if (!g_strcmp0(buf,"playing\n")) {
		state = PLAYING;
		goto end;
	} else if (!g_strcmp0(buf,"stopped\n")) {
		state = STOPPED;
		goto end;
	} else if (!g_strcmp0(buf,"paused\n")) {
		state = PAUSED;
		goto end;
	} else {
		state = STOPPED;
		goto end;
	}
end:
	pclose(stream);
	return state;
}

gboolean
audacious_get_currsong(gchar** name,gchar** path)
{
	FILE *stream;
	gchar buf[400],**tmp,*fullname;
	gint i;

	memset(buf,0,sizeof(buf));

	stream = popen("audtool --current-song-filename","r");
	fread(buf, sizeof(gchar),sizeof(buf)-1,stream);

	fullname = g_uri_unescape_string(buf,NULL);
	if (fullname)
		tmp = g_strsplit_set(fullname,":.",3);
	else {
		pclose(stream);
		return FALSE;
	}
	if (*tmp[1]) {
		*name = g_path_get_basename(tmp[1]);
		*path = g_path_get_dirname(tmp[1]);
	} else {
		g_free(fullname);
		g_strfreev(tmp);
		pclose(stream);
		return FALSE;
	}
	g_free(fullname);
	g_strfreev(tmp);
	pclose(stream);
	return TRUE;
}

gint
audacious_get_totaltime()
{
	FILE *stream;
	gchar buf[10];
	gint i,len;
	gint totaltime = 0;

	memset(buf,0,sizeof(buf));

	stream = popen("audtool --current-song-length-seconds","r");
	fread(buf, sizeof(gchar),sizeof(buf)-1,stream);

	len = strlen(buf)-1;
	for (i=0;i<len;i++) {
		if ((len-i-1)==0)
			totaltime = totaltime + g_ascii_digit_value(buf[i]);
		else
			totaltime = totaltime + g_ascii_digit_value(buf[i])*pow(10,(len-i-1));
	}

	pclose(stream);
	return totaltime;
}

gint
audacious_get_currplaytime()
{
	FILE *stream;
	gchar buf[10];
	gint i,len;
	gint currtime = 0;

	memset(buf,0,sizeof(buf));

	stream = popen("audtool --current-song-output-length-seconds","r");
	fread(buf, sizeof(gchar),sizeof(buf)-1,stream);

	len = strlen(buf)-1;
	for (i=0;i<len;i++) {
		if ((len-i-1)==0)
			currtime = currtime + g_ascii_digit_value(buf[i]);
		else
			currtime = currtime + g_ascii_digit_value(buf[i])*pow(10,(len-i-1));
	}

	pclose(stream);
	return currtime;
}

gboolean
audacious_set_currplaytime_second(gint time)
{
	gchar *cmd;
	cmd = g_strdup_printf("%s%d","audtool --playback-seek ",time);
	system(cmd);
	g_free(cmd);
	return TRUE;
}

gboolean
audacious_init(void)
{
//test if player is running,if not ,return FALSE;
	if (audacious_get_state() == STOPPED)
		return FALSE;

	return TRUE;
}

sPlayerControl audacious_player = {
	.player_get_state = audacious_get_state,
	.player_get_currsong = audacious_get_currsong,
	.player_get_totaltime = audacious_get_totaltime,
	.player_get_currplaytime = audacious_get_currplaytime,
	.player_set_currplaytime_second = audacious_set_currplaytime_second,
	.init = audacious_init,
};
