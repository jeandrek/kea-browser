iconsdir = $(datadir)/icons/hicolor

nobase_dist_icons_DATA = 16x16/apps/kea.png \
												 22x22/apps/kea.png \
												 24x24/apps/kea.png \
												 32x32/apps/kea.png \
												 48x48/apps/kea.png

install-data-hook: update-icon-cache
uninstall-hook: update-icon-cache
update-icon-cache:
	gtk-update-icon-cache -f -t $(iconsdir)
