bin_PROGRAMS = kea
kea_SOURCES = main.c
kea_CFLAGS = $(GTK30_CFLAGS) $(WEBKIT2GTK40_CFLAGS) -DDATA_DIR=\"$(pkgdatadir)\"
kea_LDADD = $(GTK30_LIBS) $(WEBKIT2GTK40_LIBS)

keadir = $(datadir)/kea
kea_DATA = gui.glade html/*
