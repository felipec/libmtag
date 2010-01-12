SUBDIRS = lib src tests

EXTRA_DIST = libmtag.pc.in build-aux/version-gen LICENSE

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libmtag.pc

dist-hook:
	echo $(VERSION) > $(distdir)/.version

ACLOCAL_AMFLAGS = -I m4
