//author:Yang Hongyang<burnef@gmail.com>

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
//to do: set 0?another method
	for (i=0;i<10;i++) {
		buf[i] = '\0';
	}
	stream = popen("audtool --playback-status","r");	
	fread(buf, sizeof(gchar),sizeof(buf),stream);

	if (!g_strcmp0(buf,"playing\n")) {
		state = PLAYING;
		goto end;
	} else if (!g_strcmp0(buf,"stopped\n")) {
		state = STOPPED;
		goto end;
	} else if (!g_strcmp0(buf,"paused\n")) {
		state = PAUSED;
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
	gchar buf[100],**tmp,*fullname;
	gint i;
//to do: set 0?another method
	for (i=0;i<100;i++) {
		buf[i] = '\0';
	}
	stream = popen("audtool --current-song-filename","r");	
	fread(buf, sizeof(gchar),sizeof(buf),stream);

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
//to do: set 0?another method
	for (i=0;i<10;i++) {
		buf[i] = '\0';
	}

	stream = popen("audtool --current-song-length-seconds","r");	
	fread(buf, sizeof(gchar),sizeof(buf),stream);

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
//to do: set 0?another method
	for (i=0;i<10;i++) {
		buf[i] = '\0';
	}

	stream = popen("audtool --current-song-output-length-seconds","r");	
	fread(buf, sizeof(gchar),sizeof(buf),stream);

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
audacious_init(sPlayerControl* player)
{
//to do
//test if player is running,if not ,return FALSE;
	if (audacious_get_state() == STOPPED)
		return FALSE;

	player->player_get_state = audacious_get_state;
	player->player_get_currsong = audacious_get_currsong;
	player->player_get_totaltime = audacious_get_totaltime;
	player->player_get_currplaytime = audacious_get_currplaytime;
	player->player_set_currplaytime_second = audacious_set_currplaytime_second;

	return TRUE;
}