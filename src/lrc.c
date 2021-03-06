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

#include "common.h"
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define CAL_TIME(a,b,c,d) ((g_ascii_digit_value(a)*10+g_ascii_digit_value(b))*60+ \
				g_ascii_digit_value(c)*10 + g_ascii_digit_value(d))
#define CHARSET_NUM 4

static gchar *Charsets[CHARSET_NUM] = {
	"UTF-8",
	"GBK",
	"GB2312",
	"GB18380",
};

enum support_player {
	AUDACIOUS,
	RHYTHMBOX,
	N_PLAYER,
};

sPlayerControl *supplayer[N_PLAYER] = {
	&audacious_player,
	&rhythmbox_player,
};

sPlayerControl *player = &audacious_player; //default

static gint _compare_time(gconstpointer a, gconstpointer b)
{
	if ((*(sLrcLine **)a)->time < (*(sLrcLine **)b)->time)
		return -1;
	else if ((*(sLrcLine **)a)->time == (*(sLrcLine **)b)->time)
		return 0;
	else if ((*(sLrcLine **)a)->time > (*(sLrcLine **)b)->time)
		return 1;
}

gboolean
linit_lyrcs(gchar *filename, sLrcInfo * lrcinfo)
{
	GIOChannel* lrcfile;
	gchar *str;
	gchar *song;
	gchar **temp;
	gchar **split;
	gchar **tmp;
	gint i;
	sLrcLine *line;

	if (lrcinfo->lyrcs) {
		for (;lrcinfo->length > 0;(lrcinfo->length)--) {
			line = g_ptr_array_remove_index(lrcinfo->lyrcs,(lrcinfo->length-1));
			g_slice_free(sLrcLine,line);
		}
	} else
		lrcinfo->lyrcs = g_ptr_array_new();

	if (lrcinfo->length != 0) {
		LDEBUG("bug on!\n");
		g_ptr_array_free(lrcinfo->lyrcs,FALSE);
		lrcinfo->lyrcs = NULL;
		return FALSE;
	}

	lrcfile = g_io_channel_new_file(filename,"r",NULL);
	if (lrcfile == NULL) {
		LDEBUG("open file error!\n");
		g_ptr_array_free(lrcinfo->lyrcs,FALSE);
		lrcinfo->lyrcs = NULL;
		return FALSE;
	}
	for (i=0;i<CHARSET_NUM;i++) {
		g_io_channel_set_encoding (lrcfile, Charsets[i],NULL);
		if(g_io_channel_read_line(lrcfile, &str,NULL,NULL,NULL) == G_IO_STATUS_NORMAL) {
			goto do_strhandle;
		}
	}
	if (i == CHARSET_NUM) {
		LDEBUG("can't find file charset!\n");
		g_ptr_array_free(lrcinfo->lyrcs,FALSE);
		lrcinfo->lyrcs = NULL;
		return FALSE;
	}
	
	while (g_io_channel_read_line(lrcfile,&str,NULL,NULL,NULL) == G_IO_STATUS_NORMAL) {
do_strhandle:
		if (str != NULL) {
			split = g_strsplit(str, "]", 10); //first time split
			g_free(str);
			str = NULL;
			if (g_strv_length(split) == 1) {
				g_strfreev(split);
				continue;
			}
			temp = split;
			while (*temp != NULL) {
				str = *temp;
				temp++;
			}
			temp = g_strsplit_set(str,"\n\r",3);
			song = g_strdup(*temp);
			g_strfreev(temp);
			str = NULL;
			temp = split;
			while (*temp != NULL) {
				str = *temp;
				if (str[0] == '[') {
					tmp = g_strsplit_set(str,"[:.",4); //second split
					if (g_ascii_isdigit(tmp[1][0])) {
						line = g_slice_new0(sLrcLine);
						line->time = CAL_TIME(tmp[1][0], \
							tmp[1][1],tmp[2][0],tmp[2][1]);
						line->lyrc = song;
						g_ptr_array_add(lrcinfo->lyrcs, line);
						(lrcinfo->length)++;
					}
					g_strfreev(tmp);
				}
				temp++;
			}
			g_strfreev(split);
		}
	}

	if (lrcinfo->length == 0) {
		g_ptr_array_free(lrcinfo->lyrcs,FALSE);
		lrcinfo->lyrcs = NULL;
		return FALSE;
	}
	
	g_ptr_array_sort(lrcinfo->lyrcs,(GCompareFunc)_compare_time);

	return TRUE;
}

static gboolean
_init_player()
{
	gint i;
	for (i=0; i<N_PLAYER; i++) {
		if (supplayer[i]->init()) {
			player = supplayer[i];
			break;
		}
	}

	if (i >= N_PLAYER)
		return FALSE;

	return TRUE;
}

gboolean
lget_player_state(sLrcPlugin* lrcPlug)
{
	if (!player->init()) {
		if (!_init_player()) {
			lrcPlug->playerstate = STOPPED;
			return FALSE;
		}
	}

	lrcPlug->playerstate = player->player_get_state();
	
	return TRUE;
}

gboolean
lget_music_state(sLrcPlugin* lrcPlug)
{
	gchar *currsong = NULL;
	gchar *songpath = NULL;

	if (!player->init()) {
		if (!_init_player())
			return FALSE;
	}
	player->player_get_currsong(&currsong,&songpath);
	if (!currsong) {LDEBUG("can't get currsong!\n");return FALSE;}
	if (lrcPlug->currsong) {
		if (g_strcmp0(lrcPlug->currsong,currsong)) {
			lrcPlug->songchanged = TRUE;
			g_free(lrcPlug->currsong);
			g_free(lrcPlug->currsongpath);
			lrcPlug->currsong = currsong;
			lrcPlug->currsongpath = songpath;
			goto otherinfo;
		}
		g_free(currsong);
		currsong = NULL;
		g_free(songpath);
		songpath = NULL;
	} else {
		lrcPlug->currsong = currsong;
		lrcPlug->currsongpath = songpath;
	}
otherinfo:
	lrcPlug->totaltime = player->player_get_totaltime();
	lrcPlug->lastplaytime = lrcPlug->currplaytime;
	lrcPlug->currplaytime = player->player_get_currplaytime();

	return TRUE;
}

gboolean
lset_play_time(gint time)
{
	if (!player->init()) {
		if (!_init_player())
			return FALSE;
	}

	player->player_set_currplaytime_second(time);
}

static gboolean
_download_lyric(sLrcPlugin* lrcPlug, gint index)
{
	pid_t child = fork();
	if (child == -1)
	{
		LDEBUG("Fork Error!%s:%d\n",__FILE__ , __LINE__);
		return FALSE;
	}
	else if (child == 0)
	{
		char *arg1,*arg2;
		arg1 = g_strdup_printf("%s",lrcPlug->currsongpath);
		arg2 = g_strdup_printf("%s",lrcPlug->currsong);
		if (index) {
			char* arg3 = g_strdup_printf("%s","d");
			char* arg4 = g_strdup_printf("%d",index);
			execl("./scripts/lrcdownload", "lrcdownload", arg1,arg2,arg3,arg4, (char * )0);
			g_free(arg3);
			g_free(arg4);
		} else
			execl("./scripts/lrcdownload", "lrcdownload", arg1,arg2, (char * )0);
		g_free(arg1);
		g_free(arg2);
		exit(0);
	}
	LDEBUG("正在下载歌词......\n");
	int iState;
	int iCount = 30;
	while (iCount--)
	{
		pid_t pr = waitpid(child, &iState, WNOHANG);
		if (pr> 0)
		break;
		sleep(1);
	}
	if (iCount < 0)
	{
		system("killall lrcdownload");
		LDEBUG("下载歌词超时！\n");
		return FALSE;
	}
	return TRUE;
}

gboolean
lget_curr_lrcname(sLrcPlugin* lrcPlug)
{
	gchar *buf;
	gint i;

	if (lrcPlug->currlrcname) {
		g_free(lrcPlug->currlrcname);
		lrcPlug->currlrcname = NULL;
	}

	buf = g_strdup_printf("%s%s",(lrcPlug->currsong),".lrc");
	lrcPlug->currlrcname = g_build_filename(lrcPlug->currsongpath,buf,NULL);
	if (!g_file_test(lrcPlug->currlrcname, G_FILE_TEST_EXISTS)) {
		if (!_download_lyric(lrcPlug,0)) {
			g_free(buf);
			return FALSE;
		}
	}
	g_free(buf);

	return TRUE;
}

gboolean
lget_avalible_lrc_num(struct slrcplugin* lrcPlug,gint* num)
{
	FILE *stream;
	gchar buf[5],*cmd;
	gint i;

	if (!lrcPlug->currsong) {
		*num = 0;
		return FALSE;
	}

	memset(buf,0,sizeof(buf));

	cmd = g_strdup_printf("%s \"%s\" \"%s\" %s","./scripts/lrcdownload",lrcPlug->currsongpath,lrcPlug->currsong,"n");
	stream = popen(cmd,"r");
	g_free(cmd);
	fread(buf, sizeof(gchar),sizeof(buf)-1,stream);

	if (strlen(buf) > 2) {
		pclose(stream);
		return FALSE;
	}
//to do judge g_isdit();
	*num = g_ascii_digit_value(buf[0]);

	pclose(stream);
	return TRUE;
}

gboolean
lget_avalible_lrc_info(struct slrcplugin* lrcPlug, gchar** title, gchar** artist, gchar** album, gint index)
{
	FILE *stream;
	gchar buf[400],*cmd,**info,**temp,**tmp,*str;
	gint i;

	memset(buf,0,sizeof(buf));

	cmd = g_strdup_printf("%s \"%s\" \"%s\" %d","./scripts/lrcdownload",lrcPlug->currsongpath,lrcPlug->currsong,index);
	stream = popen(cmd,"r");
	g_free(cmd);
	fread(buf, sizeof(gchar),sizeof(buf)-1,stream);

	info = g_strsplit_set(buf, "\r\n", 6); //first time split
	temp = info;
	while (*temp != NULL) {
		str = *temp;
		if (str[0] == '[') {
			tmp = g_strsplit_set(str,"[:]",4); //second split
			if (!g_strcmp0(tmp[1],"ti"))
				*title = g_strdup(tmp[2]);
			else if (!g_strcmp0(tmp[1],"ar"))
				*artist = g_strdup(tmp[2]);
			else if (!g_strcmp0(tmp[1],"al"))
				*album = g_strdup(tmp[2]);
			g_strfreev(tmp);
		}
		temp++;
	}
	g_strfreev(info);
	pclose(stream);

	if (*title == NULL && *artist == NULL && *album == NULL)
		return FALSE;
	return TRUE;
}

gboolean
lget_lrc_by_index(struct slrcplugin* lrcPlug,gint index)
{
	_download_lyric(lrcPlug,index);

	return TRUE;
}
