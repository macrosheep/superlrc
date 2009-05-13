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

#include "libplayer.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

//#define CAL_TIME(a,b,c,d) ((g_ascii_digit_value(a)*10+g_ascii_digit_value(b))*60+ \
                                g_ascii_digit_value(c)*10 + g_ascii_digit_value(d))

PlayerStateEnum
rhythmbox_get_state()
{
	FILE *stream;
	gchar buf[50];
	PlayerStateEnum state;
	gint i;
//to do: set 0?another method
	for (i=0;i<50;i++) {
		buf[i] = '\0';
	}

	stream = popen("rhythmbox-client --print-playing --no-start 2>/dev/null","r");
	fread(buf, sizeof(gchar),sizeof(buf)-1,stream);

	if (!g_strcmp0(buf,"Not playing\n") || !g_strcmp0(buf,"")) {
		state = STOPPED;
		goto end;
	} else {
		state = PLAYING;
		goto end;
	}
end:
	pclose(stream);
	return state;
}

gboolean
rhythmbox_get_currsong(gchar** name,gchar** path)
{
	FILE *stream;
	gchar buf[400],**tmp,*fullname;
	gint i;
//to do: set 0?another method
	for (i=0;i<400;i++) {
		buf[i] = '\0';
	}
	stream = popen("dbus-send --session --print-reply --dest=org.gnome.Rhythmbox /org/gnome/Rhythmbox/Player org.gnome.Rhythmbox.Player.getPlayingUri 2>/dev/null|tail -n 1|awk '{print $2}'","r");
	fread(buf, sizeof(gchar),sizeof(buf)-1,stream);

	if(!g_strcmp0(buf,"")) {
		*name = *path = "";
		return FALSE;
	}
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
rhythmbox_get_totaltime()
{
/*	FILE *stream;
	gchar buf[10];
	gchar **tmp;
	gint i,len;
	gint totaltime = 0;*/
//to do: set 0?another method
//	for (i=0;i<10;i++) {
//		buf[i] = '\0';
//	}

/*	stream = popen("rhythmbox-client --print-playing-format %td","r");	
	fread(buf, sizeof(gchar),sizeof(buf)-1,stream);

	LDEBUG("TOTALTIME:%s\n",buf);
	if (strlen(buf) <= 1) {
		pclose(stream);
		return 0 ;
	}

	tmp = g_strsplit_set(buf,":",2);

	if (strlen(*tmp) == 1)
		totaltime = CAL_TIME('0',tmp[0][0],tmp[1][0],tmp[1][1]);
	else if (strlen(*tmp) == 2)
		totaltime = CAL_TIME(tmp[0][0],tmp[0][1],tmp[1][0],tmp[1][1]);

	g_strfreev(tmp);

	pclose(stream);*/
//	return totaltime;
	return 0;
}

gint
rhythmbox_get_currplaytime()
{
	FILE *stream;
	gchar buf[10];
//	gchar **tmp;
	gint i,len;
	gint currtime = 0;
//to do: set 0?another method
	for (i=0;i<10;i++) {
		buf[i] = '\0';
	}

	stream = popen("dbus-send --session --print-reply --dest=org.gnome.Rhythmbox /org/gnome/Rhythmbox/Player org.gnome.Rhythmbox.Player.getElapsed 2>/dev/null |tail -n 1|awk \'{print $2}\'","r");	
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
rhythmbox_set_currplaytime_second(gint time)
{
        gchar *cmd;
        cmd = g_strdup_printf("%s%d%s","dbus-send --session --print-reply --dest=org.gnome.Rhythmbox /org/gnome/Rhythmbox/Player org.gnome.Rhythmbox.Player.setElapsed uint32:",time," >/dev/null");
        system(cmd);
        g_free(cmd);
        return TRUE;
}

gboolean
rhythmbox_init(void)
{
	if (rhythmbox_get_state() == STOPPED)
		return FALSE;

	return TRUE;
}

sPlayerControl rhythmbox_player = {
	.player_get_state = rhythmbox_get_state,
	.player_get_currsong = rhythmbox_get_currsong,
	.player_get_totaltime = rhythmbox_get_totaltime,
	.player_get_currplaytime = rhythmbox_get_currplaytime,
	.player_set_currplaytime_second = rhythmbox_set_currplaytime_second,
	.init = rhythmbox_init,
};
