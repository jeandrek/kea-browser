/*
  Kea Browser
  Copyright (C) 2015-2017 Kea Browser

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
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

#include "../config.h"
#include "protocols.h"

static void
about_protocol_request(WebKitURISchemeRequest *request, gpointer data)
{
  GInputStream *stream;
  size_t length;
  const char *path;
  char *contents;
  char filename[64];

  g_object_ref(request);

  path = webkit_uri_scheme_request_get_path(request);
  g_strlcpy(filename, DATA_DIR "/", 64);
  if (path[0] != '\0')
    g_strlcat(filename, path, 64);
  else
    g_strlcat(filename, "version.html", 64);

  GError *err = NULL;
  if(!g_file_get_contents(filename, &contents, &length, &err)) {
    g_warning("%s", err->message);
    webkit_uri_scheme_request_finish_error(request, err);
    g_error_free(err);
    return;
  }
  stream = g_memory_input_stream_new_from_data(contents, length, g_free);

  webkit_uri_scheme_request_finish(request, stream, length, "text/html");
  g_object_unref(G_OBJECT(stream));
}

void
register_schemes(WebKitWebContext *context)
{
  webkit_web_context_register_uri_scheme(context, "kea-about",
                                         about_protocol_request,
                                         NULL, NULL);
}
