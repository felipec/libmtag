SUBDIRS = lib src tests

EXTRA_DIST = libmtag.pc.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libmtag.pc

dist-hook:
	echo $(VERSION) > $(distdir)/.version

ACLOCAL_AMFLAGS = -I m4
