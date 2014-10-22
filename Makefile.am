# Makefile.am for the Splat render library.  Based on the SDL_image 2.0 Makefile.am

lib_LTLIBRARIES = libsplatgl.la

libsplatglincludedir = $(includedir)/SplatGL
libsplatglinclude_HEADERS =	\
	include/splat.h

libsplatgl_la_SOURCES =	\
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
libsplatgl_la_LDFLAGS = 	\
	-no-undefined		\
	-release $(LT_RELEASE)	\
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) -Wl,version.o
libsplatgl_la_DEPENDENCIES = version.o
else
libsplatgl_la_LDFLAGS = 	\
	-no-undefined		\
	-release $(LT_RELEASE)	\
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)
endif

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = splatgl.pc

%.o : %.rc
	$(WINDRES) $< $@

# Rule to build tar-gzipped distribution package
$(PACKAGE)-$(VERSION).tar.gz: distcheck

# Rule to build RPM distribution package
rpm: $(PACKAGE)-$(VERSION).tar.gz
	rpmbuild -ta $(PACKAGE)-$(VERSION).tar.gz

