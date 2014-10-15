# Makefile.am for the Splat render library.  Based on the SDL_image 2.0 Makefile.am

lib_LTLIBRARIES = libsplat.la

libsplatincludedir = $(includedir)/splat
libsplatinclude_HEADERS =	\
	include/splat.h

libsplat_la_SOURCES =	\
    src/canvas.c        \
    src/debug.c         \
    src/error.c         \
    src/image.c         \
    src/instance.c      \
    src/layer.c         \
    src/render.c        \
    src/splat.c

EXTRA_DIST =			\
	version.rc		\
	splat.spec		\
	gcc-fat.sh		\
	autogen.sh

if USE_VERSION_RC
libsplat_la_LDFLAGS = 	\
	-no-undefined		\
	-release $(LT_RELEASE)	\
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) -Wl,version.o
libsplat_la_DEPENDENCIES = version.o
else
libsplat_la_LDFLAGS = 	\
	-no-undefined		\
	-release $(LT_RELEASE)	\
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)
endif

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = splat.pc

%.o : %.rc
	$(WINDRES) $< $@

# Rule to build tar-gzipped distribution package
$(PACKAGE)-$(VERSION).tar.gz: distcheck

# Rule to build RPM distribution package
rpm: $(PACKAGE)-$(VERSION).tar.gz
	rpmbuild -ta $(PACKAGE)-$(VERSION).tar.gz

