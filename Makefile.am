AUTOMAKE_OPTIONS=foreign std-options subdir-objects

ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = libsplat.la

libsplatincludedir = $(includedir)/splat
libsplatinclude_HEADERS = \
    include/splat.h \
    include/csplat.h

libsplat_la_SOURCES = \
    src/canvas.cpp \
    src/dcanvas.cpp

if USE_VERSION_RC
libsplat_la_LDFLAGS =      \
        -no-undefined           \
        -release $(LT_RELEASE)  \
        -version-info $(MAJOR_VERSION):$(MINOR_VERSION):$(REVISION) -Wl,version.o
libsplat_la_DEPENDENCIES = version.o
else
libsplat_la_LDFLAGS =      \
        -no-undefined           \
        -release $(LT_RELEASE)  \
        -version-info $(MAJOR_VERSION):$(MINOR_VERSION):$(REVISION)
endif

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = splat.pc

%.o : %.rc
        $(WINDRES) $< $@

## Rule to build tar-gzipped distribution package
#$(PACKAGE)-$(VERSION).tar.gz: distcheck
#
## Rule to build RPM distribution package
#rpm: $(PACKAGE)-$(VERSION).tar.gz
#        rpmbuild -ta $(PACKAGE)-$(VERSION).tar.gz
