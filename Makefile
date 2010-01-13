CC := gcc
CXX := g++

CPPFLAGS := -ggdb -Wall -Wextra -Wno-unused-parameter

TAGLIB_CPPFLAGS := $(shell pkg-config --cflags taglib)
TAGLIB_LIBS := $(shell pkg-config --libs taglib)

prefix := /usr
libdir := $(prefix)/lib
version := $(shell ./get-version)

all:

libmtag.so: lib/mtag.o
libmtag.so: CPPFLAGS := $(CPPFLAGS) $(TAGLIB_CPPFLAGS) -I./lib -fPIC
libmtag.so: LIBS := $(LIBS) $(TAGLIB_LIBS)
libmtag.so: LDFLAGS := $(LDFLAGS) -Wl,-soname,libmtag.so.0

mtag: src/mtag.o | libmtag.so
mtag: CPPFLAGS := $(CPPFLAGS) -I./lib
mtag: LIBS := $(LIBS) -L./ -lmtag
binaries += mtag

tests/reader: tests/reader.o | libmtag.so
tests/reader: CPPFLAGS := $(CPPFLAGS) -I./lib
tests/reader: LIBS := $(LIBS) -L./ -lmtag
binaries += tests/reader

all: libmtag.so $(binaries)

libmtag.pc: libmtag.pc.in
	sed -e 's#@prefix@#$(prefix)#g' -e 's#@version@#$(version)#g' $< > $@

D = $(DESTDIR)

install: libmtag.so libmtag.pc
	mkdir -p $(D)/$(libdir)
	install -m 755 libmtag.so $(D)/$(libdir)/libmtag.so.0
	ln -sf libmtag.so.0 $(D)/$(libdir)/libmtag.so
	mkdir -p $(D)/$(prefix)/include/libmtag
	install -m 644 lib/mtag.h $(D)/$(prefix)/include/libmtag
	mkdir -p $(D)/$(libdir)/pkgconfig
	install -m 644 libmtag.pc $(D)/$(libdir)/pkgconfig/libmtag.pc

dist: base := libmtag-$(version)
dist:
	git archive --format=tar --prefix=$(base)/ HEAD > /tmp/$(base).tar
	mkdir -p $(base)
	echo $(version) > $(base)/.version
	chmod 664 $(base)/.version
	tar --append -f /tmp/$(base).tar --owner root --group root $(base)/.version
	rm -r $(base)
	gzip /tmp/$(base).tar

# pretty print
V = @
Q = $(V:y=)
QUIET_CC    = $(Q:@=@echo '   CC         '$@;)
QUIET_CXX   = $(Q:@=@echo '   CXX        '$@;)
QUIET_LINK  = $(Q:@=@echo '   LINK       '$@;)
QUIET_CLEAN = $(Q:@=@echo '   CLEAN      '$@;)

%.so::
	$(QUIET_LINK)$(CC) $(LDFLAGS) -shared -o $@ $^ $(LIBS)

$(binaries):
	$(QUIET_LINK)$(CC) $(LDFLAGS) $(LIBS) -o $@ $^

%.o:: %.cpp
	$(QUIET_CXX)$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -o $@ -c $<

%.o:: %.c
	$(QUIET_CC)$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -o $@ -c $<

clean:
	$(QUIET_CLEAN)$(RM) libmtag.so $(binaries) libmtag.pc \
		`find -name '*.[oad]'`

-include lib/*.d src/*.d tests/*.d
