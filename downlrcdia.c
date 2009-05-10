#include "common.h"

enum {
	INDEX_COLUMN,
	TITLE_COLUMN,
	ARTIST_COLUMN,
	ALBUM_COLUMN,
	N_COLUMNS,
};

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
	GtkListStore *store;
	GtkTreeIter iter;
	GtkWidget *list;
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer;
	GtkWidget *downwnd ;

	downwnd = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request(downwnd,-1,250);
	store = gtk_list_store_new(N_COLUMNS,G_TYPE_INT,G_TYPE_STRING,\
					G_TYPE_STRING,G_TYPE_STRING);

	gtk_list_store_append(store, &iter);

	list = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
	g_object_unref (G_OBJECT (store));

	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ("Index",renderer,"text", INDEX_COLUMN,NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (list), column);

	column = gtk_tree_view_column_new_with_attributes ("Title",renderer,"text", TITLE_COLUMN,NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (list), column);

	column = gtk_tree_view_column_new_with_attributes ("Artist",renderer,"text", ARTIST_COLUMN,NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (list), column);

	column = gtk_tree_view_column_new_with_attributes ("Album",renderer,"text", ALBUM_COLUMN,NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (list), column);

	gtk_container_add (GTK_CONTAINER (downwnd), list);

	gtk_signal_connect(GTK_OBJECT(downwnd),"delete-event",\
				GTK_SIGNAL_FUNC(downlrcdia_quit),NULL);
	gtk_widget_show_all(downwnd);

	lrcPlug->get_avalible_lrc_num(lrcPlug,&num);

	LDEBUG("%s %d\n",lrcPlug->currsong,num);

	for (i = 1;i<= num;i++) {
		if (lrcPlug->get_avalible_lrc_info(lrcPlug,&title,&artist,&album,i)) {
			LDEBUG("%s %s %s\n",title,artist,album);
			gtk_list_store_insert_with_values (store, &iter,i-1,
				INDEX_COLUMN, i,
				TITLE_COLUMN, title,
				ARTIST_COLUMN, artist,
				ALBUM_COLUMN, album,
				-1);
		}
	}

//	lrcPlug->get_lrc_by_index(lrcPlug,num);

	return TRUE;
}
