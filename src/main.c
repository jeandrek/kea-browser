/*
  Kea Browser
  Copyright (C) 2015 Kea Browser

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
//
// main.c
//

#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

#include "../config.h"
#include "protocols.h"

void go(GtkWidget *widget, gpointer data);
void go_back(GtkWidget *widget, gpointer data);
void go_forward(GtkWidget *widget, gpointer data);
void web_view_load_changed(GtkWidget *widget, WebKitLoadEvent load_event, gpointer data);
void web_view_close(GtkWidget *widget, GtkWidget *window);
gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data);

GtkWidget *main_window;
GtkWidget *entry_url_bar;
GtkWidget *spinner_loading;
GtkWidget *box;
GtkWidget *web_view;
WebKitWebContext *context;

int
main(int argc, char **argv)
{
  GtkBuilder *builder;

  gtk_init(&argc, &argv);

  // load the interface
  builder = gtk_builder_new();
  GError *err = NULL;
  if(!gtk_builder_add_from_file(builder, DATA_DIR "/gui.glade", &err)) {
    g_warning("%s", err->message);
    g_error_free(err);
    return 1;
  }

  // get the widgets
  main_window     = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
  entry_url_bar   = GTK_WIDGET(gtk_builder_get_object(builder, "entry_url_bar"));
  spinner_loading = GTK_WIDGET(gtk_builder_get_object(builder, "spinner_loading"));
  box             = GTK_WIDGET(gtk_builder_get_object(builder, "box"));

  gtk_builder_add_callback_symbols(builder, "go", G_CALLBACK(go),
                                   "go_back", G_CALLBACK(go_back),
                                   "go_forward", G_CALLBACK(go_forward), NULL);
  gtk_builder_connect_signals(builder, NULL);
  g_object_unref(G_OBJECT(builder));

  // create the webview
  web_view = webkit_web_view_new();

  context = webkit_web_context_get_default();
  register_schemes(context);

  g_signal_connect(web_view, "close", G_CALLBACK(web_view_close), NULL);
  g_signal_connect(web_view, "load-changed", G_CALLBACK(web_view_load_changed), NULL);

  gtk_box_pack_start(GTK_BOX(box), web_view,
                     TRUE, TRUE, 0);
  gtk_widget_show(web_view);

  gtk_widget_show(main_window);

  webkit_web_view_load_uri(WEBKIT_WEB_VIEW(web_view), "about:home.html");

  gtk_main();
  return 0;
}

// navigate to a page
void
go(GtkWidget *widget, gpointer data)
{
  char uri[MAX_URI_SIZE];
  g_strlcpy(uri, gtk_entry_get_text(GTK_ENTRY(entry_url_bar)), MAX_URI_SIZE);
  // if the URL is "", just reload the current page
  if(g_strcmp0("", uri) == 0) {
    g_strlcpy(uri, webkit_web_view_get_uri(WEBKIT_WEB_VIEW(web_view)), MAX_URI_SIZE);
  }

  webkit_web_view_load_uri(WEBKIT_WEB_VIEW(web_view), uri);
}

// go backwards/forwards
void
go_back(GtkWidget *widget, gpointer data) {
  if(webkit_web_view_can_go_back(WEBKIT_WEB_VIEW(web_view))) {
    webkit_web_view_go_back(WEBKIT_WEB_VIEW(web_view));
  }
}
void
go_forward(GtkWidget *widget, gpointer data) {
  if(webkit_web_view_can_go_forward(WEBKIT_WEB_VIEW(web_view))) {
    webkit_web_view_go_forward(WEBKIT_WEB_VIEW(web_view));
  }
}

// update the URL bar on navigation
void
web_view_load_changed(GtkWidget *widget, WebKitLoadEvent load_event, gpointer data)
{
  switch(load_event) {
  case WEBKIT_LOAD_STARTED:
    gtk_entry_set_text(GTK_ENTRY(entry_url_bar),
                       webkit_web_view_get_uri(WEBKIT_WEB_VIEW(web_view)));
    gtk_spinner_start(GTK_SPINNER(spinner_loading));
    break;
  case WEBKIT_LOAD_REDIRECTED:
    gtk_entry_set_text(GTK_ENTRY(entry_url_bar),
                       webkit_web_view_get_uri(WEBKIT_WEB_VIEW(web_view)));
    break;
  case WEBKIT_LOAD_FINISHED:
    gtk_spinner_stop(GTK_SPINNER(spinner_loading));
    break;
  }
}

// close the window from JavaScript
void
web_view_close(GtkWidget *widget, GtkWidget *window)
{
  gtk_widget_destroy(window);
}

gboolean
delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
  return FALSE;
}
