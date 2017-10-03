/*
  Kea Browser
  Copyright (C) 2015 Kea Browser

  This file is part of Kea Browser.

  Kea Browser is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Kea Browser is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Kea Browser.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

#include "../config.h"
#include "protocols.h"

void make_tab(GtkNotebook *notebook, char *uri);
void remove_tab(GtkWidget *widget, gpointer data);
void change_current_tab(GtkWidget *widget, int index, gpointer data);
void go(GtkWidget *widget, gpointer data);
void go_back(GtkWidget *widget, gpointer data);
void go_forward(GtkWidget *widget, gpointer data);
void web_view_load_changed(GtkWidget *widget, WebKitLoadEvent load_event, gpointer data);
void web_view_close(GtkWidget *widget, GtkWidget *window);
gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data);

GtkWidget *main_window;
GtkWidget *entry_url_bar;
GtkWidget *spinner_loading;
GtkWidget *tabs;
WebKitSettings *settings;
int kiosk = 0;

int
main(int argc, char **argv)
{
  char *start_page = NULL;
  GtkBuilder *builder;

  gtk_init(&argc, &argv);

  for(int i = 1; i < argc; i++) {
    if(argv[i][0] != '-') {
      start_page = argv[i];
      continue;
    }

    if(g_strcmp0(argv[i], "--kiosk") == 0) { // kiosk mode
      kiosk = 1;
    } else {
      printf("%s: unrecognized option '%s'\n", argv[0], argv[i]);
    }
  }
  if(!start_page || start_page[0] == '\0')
    start_page = (char *)"http://jonathan50.github.io/kea-browser/";

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
  tabs            = GTK_WIDGET(gtk_builder_get_object(builder, "tabs"));

  gtk_builder_add_callback_symbols(builder, "go", G_CALLBACK(go),
                                   "go_back", G_CALLBACK(go_back),
                                   "go_forward", G_CALLBACK(go_forward),
                                   "change_current_tab", G_CALLBACK(change_current_tab),
                                   "delete_event", G_CALLBACK(delete_event), NULL);
  gtk_builder_connect_signals(builder, NULL);
  g_object_unref(G_OBJECT(builder));

  // make the first tab
  make_tab(GTK_NOTEBOOK(tabs), start_page);

  gtk_widget_show(main_window);

  if(kiosk)
    gtk_window_fullscreen(GTK_WINDOW(main_window));

  gtk_main();

  return 0;
}

// make a new tab
void
make_tab(GtkNotebook *notebook, char *uri)
{
  GtkWidget *web_view;
  GtkWidget *label;
  GtkWidget *close_button;

  web_view = webkit_web_view_new();

  register_schemes(webkit_web_context_get_default());

  settings = webkit_web_view_get_settings(WEBKIT_WEB_VIEW(web_view));
  webkit_settings_set_user_agent_with_application_details(settings, "Kea", VERSION);

  g_signal_connect(web_view, "close", G_CALLBACK(web_view_close), NULL);
  g_signal_connect(web_view, "load-changed", G_CALLBACK(web_view_load_changed), NULL);

  label = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
  gtk_box_pack_start(GTK_BOX(label), gtk_label_new(""),
                     TRUE, TRUE, 0);
  close_button = gtk_button_new_from_icon_name("gtk-close",
                                               GTK_ICON_SIZE_MENU);
  gtk_button_set_relief(GTK_BUTTON(close_button), GTK_RELIEF_NONE);
  g_signal_connect(close_button, "clicked", G_CALLBACK(remove_tab), NULL);
  gtk_box_pack_start(GTK_BOX(label), close_button,
                     FALSE, FALSE, 0);

  gtk_widget_show(web_view);
  gtk_widget_show_all(label);
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), web_view, label);

  webkit_web_view_load_uri(WEBKIT_WEB_VIEW(web_view), uri);
}

// remove a tab
void
remove_tab(GtkWidget *widget, gpointer data)
{
  // grrr
}

// update URL bar on switching tab
void
change_current_tab(GtkWidget *widget, int index, gpointer data)
{
  gtk_entry_set_text(GTK_ENTRY(entry_url_bar),
                     webkit_web_view_get_uri(WEBKIT_WEB_VIEW(gtk_notebook_get_nth_page(GTK_NOTEBOOK(tabs),
                                                                                       index))));
}

// navigate to a page
void
go(GtkWidget *widget, gpointer data)
{
  char uri[MAX_URI_SIZE];
  WebKitWebView *web_view;
  // get current WebKitWebView
  web_view = WEBKIT_WEB_VIEW(gtk_notebook_get_nth_page(GTK_NOTEBOOK(tabs),
                                                       gtk_notebook_get_current_page(GTK_NOTEBOOK(tabs))));
  g_strlcpy(uri, gtk_entry_get_text(GTK_ENTRY(entry_url_bar)), MAX_URI_SIZE);
  // if the URL is "", just reload the current page
  if(g_strcmp0("", uri) == 0) {
    g_strlcpy(uri, webkit_web_view_get_uri(web_view), MAX_URI_SIZE);
  }

  webkit_web_view_load_uri(web_view, uri);
}

// go backwards/forwards
void
go_back(GtkWidget *widget, gpointer data) {
  WebKitWebView *web_view;
  // get current WebKitWebView
  web_view = WEBKIT_WEB_VIEW(gtk_notebook_get_nth_page(GTK_NOTEBOOK(tabs),
                                                       gtk_notebook_get_current_page(GTK_NOTEBOOK(tabs))));
  if(webkit_web_view_can_go_back(WEBKIT_WEB_VIEW(web_view))) {
    webkit_web_view_go_back(WEBKIT_WEB_VIEW(web_view));
  }
}
void
go_forward(GtkWidget *widget, gpointer data) {
  WebKitWebView *web_view;
  // get current WebKitWebView
  web_view = WEBKIT_WEB_VIEW(gtk_notebook_get_nth_page(GTK_NOTEBOOK(tabs),
                                                       gtk_notebook_get_current_page(GTK_NOTEBOOK(tabs))));
  if(webkit_web_view_can_go_forward(WEBKIT_WEB_VIEW(web_view))) {
    webkit_web_view_go_forward(WEBKIT_WEB_VIEW(web_view));
  }
}

// update the URL bar and tab title on navigation
void
web_view_load_changed(GtkWidget *widget, WebKitLoadEvent load_event, gpointer data)
{
  // if this is not the active tab do nothing
  if(gtk_notebook_page_num(GTK_NOTEBOOK(tabs), widget) !=
     gtk_notebook_get_current_page(GTK_NOTEBOOK(tabs)))
    return;

  switch(load_event) {
  case WEBKIT_LOAD_STARTED:
    gtk_spinner_start(GTK_SPINNER(spinner_loading));
  case WEBKIT_LOAD_REDIRECTED:
    gtk_entry_set_text(GTK_ENTRY(entry_url_bar),
                       webkit_web_view_get_uri(WEBKIT_WEB_VIEW(widget)));
    break;
  case WEBKIT_LOAD_FINISHED:
    gtk_spinner_stop(GTK_SPINNER(spinner_loading));
    break;
  }

  const char *title;
  title = webkit_web_view_get_title(WEBKIT_WEB_VIEW(widget));
  if(title == NULL)
    title = webkit_web_view_get_uri(WEBKIT_WEB_VIEW(widget));

  GtkWidget *label;
  label = gtk_notebook_get_tab_label(GTK_NOTEBOOK(tabs), widget);
  gtk_label_set_text(GTK_LABEL(gtk_container_get_children(GTK_CONTAINER(label))->data),
                     title);
}

// close the window/tab from JavaScript
void
web_view_close(GtkWidget *widget, GtkWidget *window)
{
  gtk_notebook_remove_page(GTK_NOTEBOOK(tabs),
                           gtk_notebook_page_num(GTK_NOTEBOOK(tabs), widget));
}

gboolean
delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
  if(kiosk)
    return TRUE;
  return FALSE;
}
