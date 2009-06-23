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

enum {
	INDEX_COLUMN,
	TITLE_COLUMN,
	ARTIST_COLUMN,
	ALBUM_COLUMN,
	N_COLUMNS,
};

gint
double_click_handler(GtkWidget *list, GdkEventButton *event, gpointer lrcPlug)
{
	GtkTreeSelection *select;
	GtkTreeIter iter;
	GtkTreeModel *model;
	gint index;

	if (event->type==GDK_2BUTTON_PRESS) {
		select = gtk_tree_view_get_selection(GTK_TREE_VIEW(list));
		if (gtk_tree_selection_get_selected (select,&model,&iter)) {
			gtk_tree_model_get (model,&iter,INDEX_COLUMN,&index,-1);
			((sLrcPlugin *)lrcPlug)->get_lrc_by_index((sLrcPlugin *)lrcPlug,index);
			((sLrcPlugin *)lrcPlug)->lrcredowned = TRUE;
		}
	}

	return FALSE;
}

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
	gtk_widget_set_size_request(downwnd,-1,200);
	store = gtk_list_store_new(N_COLUMNS,G_TYPE_INT,G_TYPE_STRING,\
					G_TYPE_STRING,G_TYPE_STRING);

//	gtk_list_store_append(store, &iter);

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

	gtk_signal_connect(GTK_OBJECT(list),
			"button_press_event",
			GTK_SIGNAL_FUNC(double_click_handler),
			lrcPlug);

	gtk_widget_show_all(downwnd);

	lrcPlug->get_avalible_lrc_num(lrcPlug,&num);

//	LDEBUG("%s %d\n",lrcPlug->currsong,num);

	for (i = 1;i<= num;i++) {
		if (lrcPlug->get_avalible_lrc_info(lrcPlug,&title,&artist,&album,i)) {
//			LDEBUG("%s %s %s\n",title,artist,album);
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
