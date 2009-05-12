//author:Yang Hongyang<burnef@gmail.com>

#include "liblrc.h"
#include "common.h"
#include <unistd.h>
#include <stdlib.h>

#define DEFAULT_WIDTH 300
#define DEFAULT_HEIGHT 350
#define DEFAULT_LABELHEIGHT 20

sLrcPlugin lrcPlug;
gint maxWidth = DEFAULT_WIDTH;
gint height = DEFAULT_HEIGHT;
static gint labelheight = DEFAULT_LABELHEIGHT;

GtkWidget *window = NULL;
GtkWidget *layout = NULL;
GtkWidget *vbox = NULL;
GtkWidget *event = NULL;
//GtkWidget *popupmenu = NULL;

gboolean isgrab = FALSE;
gboolean isminimized = FALSE;
gboolean winstatechanged = FALSE;
gint grabbeginy = 0;
gint moveoldpixel = 0;
gchar* lastshowline = NULL;

gboolean
lrcplugin_init()
{
	lrcPlug.lrc = g_slice_new0(sLrcInfo);
	if (lrcPlug.lrc == NULL) {
		LDEBUG("No memory!\n");
		return FALSE;
	}
	lrcPlug.playerstate = STOPPED;
	lrcPlug.songchanged = TRUE;
	lrcPlug.lrcredowned = FALSE;
	lrcPlug.currsong = NULL;
	lrcPlug.currsongpath = NULL;
	lrcPlug.currlrcname = NULL;
	lrcPlug.totaltime = 0;
	lrcPlug.lastplaytime = 0;
	lrcPlug.currplaytime = 0;
	
	lrcPlug.init_lyrcs = linit_lyrcs;
	lrcPlug.get_curr_lrcname = lget_curr_lrcname;
	lrcPlug.get_player_state = lget_player_state;
	lrcPlug.get_music_state = lget_music_state;
	lrcPlug.set_play_time = lset_play_time;

	lrcPlug.get_avalible_lrc_num = lget_avalible_lrc_num;
	lrcPlug.get_avalible_lrc_info = lget_avalible_lrc_info;
	lrcPlug.get_lrc_by_index = lget_lrc_by_index;

	return TRUE;
}

GtkWidget*
creat_label(gchar *str)
{
	GtkWidget *label;

	label = gtk_label_new(str);
	gtk_label_set_single_line_mode((GtkLabel*)label,TRUE);
	gtk_label_set_justify((GtkLabel*)label,GTK_JUSTIFY_CENTER);

	return label;
}

void
set_label_size(GtkWidget* label, gpointer data)
{
	gint height;

	if (!g_strcmp0(data,"default"))
		gtk_widget_set_size_request(label,DEFAULT_WIDTH,-1);
	else {
		height = *(gint*)data;
		gtk_widget_set_size_request(label,maxWidth,height);
	}
}
/*
gint
get_label_height()
{
	GList* labels;
	GtkWidget *label;
	PangoLayout *playout = NULL;
        gint theight;

	labels = gtk_container_get_children(GTK_CONTAINER(vbox));
	label = labels->data;
        playout = gtk_label_get_layout((GtkLabel*)label);
        if(playout){
                pango_layout_get_pixel_size(playout, NULL, &theight);
        }
//	gtk_widget_get_size_request(vbox,NULL,&theight);
	LDEBUG("%d\n",(gint)(1.2*theight));
	return 1.2*theight;
}*/

void
creat_lyrcs(gpointer line,gpointer data)
{
	GtkWidget *label;
	PangoLayout *playout = NULL;
	gint twidth;

	label = creat_label(((sLrcLine*)line)->lyrc);
	playout = gtk_label_get_layout((GtkLabel*)label);
	if(playout){
		pango_layout_get_pixel_size(playout, &twidth, NULL);
	}
	if (twidth > maxWidth && twidth > DEFAULT_WIDTH)
		maxWidth = twidth;
	gtk_box_pack_start((GtkBox*)vbox,label,FALSE,FALSE,0);
}

gboolean
creat_lrc_layout()
{
	GtkWidget *label;

	if (vbox) {
		gtk_widget_destroy(vbox);
		vbox = gtk_vbox_new(TRUE,0);
	} else
		vbox = gtk_vbox_new(TRUE,0);

	if (!lrcPlug.lrc->lyrcs) {
		label = creat_label("No lrc found!");
		gtk_box_pack_start((GtkBox*)vbox,label,FALSE,FALSE,0);
		gtk_container_add(GTK_CONTAINER(event),vbox);
		gtk_layout_move((GtkLayout*)layout,event,0,(height/2));
		gtk_container_foreach(GTK_CONTAINER(vbox),set_label_size,"default");
		gtk_widget_set_size_request(window,DEFAULT_WIDTH,DEFAULT_HEIGHT);
		gtk_widget_show_all (window);
	} else {
		maxWidth = DEFAULT_WIDTH;
		g_ptr_array_foreach(lrcPlug.lrc->lyrcs,creat_lyrcs,NULL);
//		gint labelheight = get_label_height();
		gtk_container_foreach(GTK_CONTAINER(vbox),set_label_size,&labelheight);
		gtk_container_add(GTK_CONTAINER(event),vbox);
		gtk_layout_move((GtkLayout*)layout,event,0,(height/2+35));
		gtk_widget_set_size_request(window,maxWidth,height);
		gtk_widget_show_all (window);
	}

	return TRUE;
}

gboolean
set_curr_label_color(gint n)
{
	GList* labels;
	GtkWidget* currlabel,*prevlabel,*label;
	GdkColor color;
	gint i;

	if ((lrcPlug.currplaytime - lrcPlug.lastplaytime) < 0 || \
		(lrcPlug.currplaytime - lrcPlug.lastplaytime) > 1.5 || \
			lrcPlug.playerstate == STOPPED || isgrab || \
				winstatechanged) {
		labels = gtk_container_get_children(GTK_CONTAINER(vbox));
		for (i=0; i<lrcPlug.lrc->length; i++) {
			label = labels->data;
			gtk_widget_modify_fg(label,GTK_STATE_NORMAL,NULL);
			labels = g_list_next(labels);
		}
		if (winstatechanged)
			winstatechanged = FALSE;
	}
		

	labels = gtk_container_get_children(GTK_CONTAINER(vbox));
	for (i=0; i<n; i++)
		labels = g_list_next(labels);
	currlabel = labels->data;
	if (g_list_previous(labels)) {
		prevlabel = g_list_previous(labels)->data;
		gtk_widget_modify_fg(prevlabel,GTK_STATE_NORMAL,NULL);
	}
	gdk_color_parse ("green",&color);
	gtk_widget_modify_fg(currlabel,GTK_STATE_NORMAL,&color);
}

gint
pixel_to_time(gint pixel)
{
	gint n,delta,dtime;
	sLrcLine *line,*next;

	n = pixel/labelheight;
	delta = pixel%labelheight;
	if (n < 0)
		return 0;
	if (n == 0) {
		line = g_ptr_array_index(lrcPlug.lrc->lyrcs,0);
		return delta * line->time / labelheight;
	}
	if (n == lrcPlug.lrc->length) {
		line = g_ptr_array_index(lrcPlug.lrc->lyrcs,n-1);
		return delta * (lrcPlug.totaltime - line->time) / labelheight;
	}
	if (n > lrcPlug.lrc->length)
		return lrcPlug.totaltime;
	line = g_ptr_array_index(lrcPlug.lrc->lyrcs,n-1);
	next = g_ptr_array_index(lrcPlug.lrc->lyrcs,n);

	dtime = delta * (next->time - line->time)/labelheight;
	return (line->time + dtime);
}

gint
time_to_pixel(void)
{
	gint n,currtime,nexttime;
	sLrcLine* line;

	if (!lrcPlug.lrc->lyrcs)
		return 0;
	for (n=0; n<lrcPlug.lrc->length; n++) {
		line = g_ptr_array_index(lrcPlug.lrc->lyrcs,n);
		if (line->time > lrcPlug.currplaytime) {
			nexttime = line->time;
			break;
		}
	}
	if (n == 0) {
		line = g_ptr_array_index(lrcPlug.lrc->lyrcs,0);
		set_curr_label_color(n);
		return n*labelheight + lrcPlug.currplaytime \
			*labelheight/line->time;
	}

	if (n == lrcPlug.lrc->length) {
		set_curr_label_color(n-1);
		return n*labelheight + (lrcPlug.currplaytime - line->time) \
			*labelheight/(lrcPlug.totaltime - line->time);
	}
	
	n -= 1;
//	LDEBUG("%d\n",n);
	line = g_ptr_array_index(lrcPlug.lrc->lyrcs,n);
	currtime = line->time;
//	LDEBUG("%d,%d,%d\n",lrcPlug.currplaytime,currtime,nexttime);
	set_curr_label_color(n);

	return (n+1)*labelheight + (lrcPlug.currplaytime - currtime)*labelheight \
			/(nexttime - currtime);
}

void
scroll_layout(gint pixel)
{
	gtk_layout_move((GtkLayout*)layout,event,0,(height/2-pixel));
}

gchar*
get_line(void)
{
	gint n;
	sLrcLine* line;

	if (!lrcPlug.lrc->lyrcs)
		return NULL;
	for (n=0; n<lrcPlug.lrc->length; n++) {
		line = g_ptr_array_index(lrcPlug.lrc->lyrcs,n);
		if (line->time > lrcPlug.currplaytime) {
			break;
		}
	}

	if (n == 0 || n == lrcPlug.lrc->length)
		return line->lyrc;

	line = g_ptr_array_index(lrcPlug.lrc->lyrcs,(n-1));
	return line->lyrc;
}

void
player_stoped()
{
//	set_curr_label_color(0);
	scroll_layout(0);
}

void
scroll_lyrcs()
{
	gint pixel;

//	labelheight = get_label_height();
	pixel = time_to_pixel();
	scroll_layout(pixel);
}

gboolean
set_label_color_pixel(gint pixel,gboolean set)
{
	GList* labels;
	GtkWidget *label;
	gint n,i;
	GdkColor color;

	n = pixel/labelheight;
	if (n <= 1 || n > lrcPlug.lrc->length) return FALSE;
	labels = gtk_container_get_children(GTK_CONTAINER(vbox));
	for (i=0; i<(n-1); i++)
		labels = g_list_next(labels);
	label = labels->data;
	if (set) {
		gdk_color_parse ("green",&color);
		gtk_widget_modify_fg(label,GTK_STATE_NORMAL,&color);
	} else {
		gtk_widget_modify_fg(label,GTK_STATE_NORMAL,NULL);
	}

	return TRUE;
}

gboolean
grab_lrc(GtkWidget* vbox, GdkEventButton *event, gpointer data)
{
	if (event->button == 3) {
		gtk_menu_popup(GTK_MENU(data),NULL,NULL,NULL,NULL,
				event->button,event->time);
		return TRUE;
	}
	if (event->button != 1 || !lrcPlug.lrc->lyrcs || isgrab)
		return TRUE;
	isgrab = TRUE;
	grabbeginy = (gint)event->y;
}

gboolean
release_lrc(GtkWidget* vbox, GdkEventButton *event, gpointer data)
{
	if (!isgrab || !moveoldpixel) {
		grabbeginy = 0;
		moveoldpixel = 0;
		isgrab = FALSE;
		return TRUE;
	}
	gint time;
//	gint labelheight = get_label_height();
	time = pixel_to_time(moveoldpixel);
	lrcPlug.set_play_time(time);
	grabbeginy = 0;
	moveoldpixel = 0;
	isgrab = FALSE;
}

gboolean
move_lrc(GtkWidget* vbox, GdkEventButton *event, gpointer data)
{
	gint pixel;
	if (!isgrab)
		return TRUE;
//	gint labelheight = get_label_height();
	gint delta = (gint)(event->y) - grabbeginy;
	if (delta < -labelheight*lrcPlug.lrc->length || delta > labelheight*lrcPlug.lrc->length)
		return FALSE;
	if (!moveoldpixel) {
		gint moveoldpixel = time_to_pixel();
	}
	set_label_color_pixel(moveoldpixel,FALSE);
	pixel = moveoldpixel - delta;
	moveoldpixel = pixel;
	set_label_color_pixel(moveoldpixel,TRUE);
	scroll_layout(moveoldpixel);
}

gboolean
minimized_lyric_show(gchar* line)
{
	pid_t child = fork();
	if (child == -1)
	{
		LDEBUG("Fork Error!%s:%d\n",__FILE__ , __LINE__);
		return FALSE;
	}
	else if (child == 0)
	{
		execl("./scripts/lrcshow", "lrcshow", "W", line, (char * )0);
		exit(0);
	}
	return TRUE;
}

gboolean
window_minimized_lyric_show()
{
	gchar* line = get_line();
	if (line == NULL)
		return FALSE;

	if (!lastshowline)
		lastshowline = line;
	if (g_strcmp0(lastshowline,line)) {
		lastshowline = line;
		minimized_lyric_show(line);
	}
	return TRUE;
}

gboolean
smart_control()
{
	if (isgrab) return TRUE;

	lrcPlug.get_player_state(&lrcPlug);
	switch (lrcPlug.playerstate) {
		case PLAYING:
			break;
		case PAUSED:
			return TRUE;
			break;
		case STOPPED:
			player_stoped();
			return TRUE;
			break;
		default:
			return TRUE;
			break;
	}

	lrcPlug.get_music_state(&lrcPlug);
//	LDEBUG("%s\n",lrcPlug.currsong);
//	LDEBUG("%s\n",lrcPlug.currsongpath);
	if (lrcPlug.songchanged || lrcPlug.lrcredowned) {
		lrcPlug.get_curr_lrcname(&lrcPlug);
		lrcPlug.init_lyrcs(lrcPlug.currlrcname,lrcPlug.lrc);
		LDEBUG("%s\n",lrcPlug.currlrcname);
		creat_lrc_layout();

		if (lrcPlug.songchanged)
			lrcPlug.songchanged = FALSE;
		if (lrcPlug.lrcredowned)
			lrcPlug.lrcredowned = FALSE;
	}

	if (isminimized) {
		window_minimized_lyric_show();
		return TRUE;
	}
	scroll_lyrcs();
	return TRUE;
}

gboolean
main_quit(GtkWidget* window, GdkEvent *event, gpointer data)
{
	gtk_main_quit();
}

gboolean
window_state_handle(GtkWidget* window, GdkEventWindowState *event, gpointer data)
{
	switch (event->new_window_state) {
		case GDK_WINDOW_STATE_ICONIFIED:
		case GDK_WINDOW_STATE_BELOW:
			isminimized = TRUE;
			break;
		case GDK_WINDOW_STATE_ABOVE:
		case GDK_WINDOW_STATE_MAXIMIZED:
		case 0:
			isminimized = FALSE;
			winstatechanged = TRUE;
			lastshowline = NULL;
			break;
		default:
			break;
	}

	return TRUE;
}

static gboolean
redownload_lrc(GtkWidget *widget,GdkEvent *event)
{
	redownload_lrc_dia(&lrcPlug);
	return TRUE;
}

gboolean
creat_window()
{
	GtkWidget *popupmenu = NULL;
	GtkWidget *reloadlrc_item = NULL;

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request(window,DEFAULT_WIDTH,DEFAULT_HEIGHT);
	gtk_window_set_resizable((GtkWindow*)window,FALSE);

	popupmenu = gtk_menu_new();
	reloadlrc_item = gtk_menu_item_new_with_label("redownload lyric");

	gtk_menu_shell_append (GTK_MENU_SHELL(popupmenu),reloadlrc_item);
	g_signal_connect_swapped (G_OBJECT(reloadlrc_item),"activate",
					G_CALLBACK(redownload_lrc),NULL);

	gtk_widget_show(reloadlrc_item);

	layout = gtk_layout_new(NULL,NULL);
	event = gtk_event_box_new ();

	gtk_container_add (GTK_CONTAINER (window), layout);
	gtk_container_add (GTK_CONTAINER (layout), event);

	gtk_signal_connect(GTK_OBJECT(event),"button-press-event",\
					GTK_SIGNAL_FUNC(grab_lrc),GTK_OBJECT(popupmenu));
	gtk_signal_connect(GTK_OBJECT(event),"button-release-event",\
					GTK_SIGNAL_FUNC(release_lrc),NULL);
	gtk_signal_connect(GTK_OBJECT(event),"motion-notify-event",\
					GTK_SIGNAL_FUNC(move_lrc),NULL);
	gtk_signal_connect(GTK_OBJECT(window),"delete-event",\
					GTK_SIGNAL_FUNC(main_quit),NULL);
	gtk_signal_connect(GTK_OBJECT(window),"window-state-event",\
					GTK_SIGNAL_FUNC(window_state_handle),NULL);
//	g_signal_connect_swapped (G_OBJECT(event),"button-press-event",
//					G_CALLBACK(),G_OBJECT(popupmenu));
}

int
main(int argc, char **argv)
{
	gtk_init (&argc, &argv);

	if (!lrcplugin_init()) {
		LDEBUG("init error!\n");
		return -1;
	}

	creat_window();
	creat_lrc_layout();

	gtk_widget_show_all (window);

	g_timeout_add(1000,smart_control,NULL);

	gtk_main();
	return 0;
}
