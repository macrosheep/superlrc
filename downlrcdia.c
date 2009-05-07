#include "common.h"

gboolean
downlrcdia_quit(GtkWidget* downwnd, GdkEvent *event, gpointer data)
{
	gtk_widget_destroy(downwnd);
}

gboolean
redownload_lrc_dia(sLrcPlugin *lrcPlug)
{
	gint num,i;
	gchar *title = NULL;
	gchar *artist = NULL;
	gchar *album = NULL;

	lrcPlug->get_avalible_lrc_num(lrcPlug,&num);

	LDEBUG("%s %d\n",lrcPlug->currsong,num);

	for (i = 1;i<= num;i++) {
		if (lrcPlug->get_avalible_lrc_info(lrcPlug,&title,&artist,&album,i))
			LDEBUG("%s %s %s\n",title,artist,album);
	}

//	lrcPlug->get_lrc_by_index(lrcPlug,num);
	GtkWidget *downwnd = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	GtkWidget *label = gtk_label_new("testtttttttttttttttttttttttttttttt");

	gtk_container_add (GTK_CONTAINER (downwnd), label);

	gtk_signal_connect(GTK_OBJECT(downwnd),"delete-event",\
				GTK_SIGNAL_FUNC(downlrcdia_quit),NULL);

	gtk_widget_show_all(downwnd);

	return TRUE;
}
