dnl Process this file with autoconf to produce a configure script.  Based on the SDL_image 2.0 configure.in
AC_INIT(Splat, 0.0)
AC_CONFIG_SRCDIR(src/splat.c)
AM_INIT_AUTOMAKE

dnl Set various version strings - taken gratefully from the GTk sources

# Making releases:
#   MICRO_VERSION += 1;
#   INTERFACE_AGE += 1;
#   BINARY_AGE += 1;
# if any functions have been added, set INTERFACE_AGE to 0.
# if backwards compatibility has been broken,
# set BINARY_AGE and INTERFACE_AGE to 0.

MAJOR_VERSION=0
MINOR_VERSION=0
MICRO_VERSION=0
INTERFACE_AGE=0
BINARY_AGE=0
VERSION=$MAJOR_VERSION.$MINOR_VERSION.$MICRO_VERSION

AC_SUBST(MAJOR_VERSION)
AC_SUBST(MINOR_VERSION)
AC_SUBST(MICRO_VERSION)
AC_SUBST(INTERFACE_AGE)
AC_SUBST(BINARY_AGE)
AC_SUBST(VERSION)

# libtool versioning
LT_INIT([win32-dll])

LT_RELEASE=$MAJOR_VERSION.$MINOR_VERSION
LT_CURRENT=`expr $MICRO_VERSION - $INTERFACE_AGE`
LT_REVISION=$INTERFACE_AGE
LT_AGE=`expr $BINARY_AGE - $INTERFACE_AGE`

AC_SUBST(LT_RELEASE)
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)

dnl Detect the canonical build and host environments
AC_CANONICAL_HOST

dnl Setup compiler and linker flags
CFLAGS='-fvisibility=internal -std=c99 -fno-common -Wall -I$(srcdir)/include'
LDFLAGS='-lGL -lGLU'

dnl Check for tools
AC_PROG_LIBTOOL
AC_PROG_CC
AC_PROG_OBJC
AC_C_INLINE
AC_PROG_INSTALL
AC_PROG_MAKE_SET
if test -z "$host_alias"; then
    hostaliaswindres=
else
    hostaliaswindres="$host_alias-windres"
fi
AC_CHECK_PROGS(WINDRES, [windres $hostaliaswindres $host_os-windres])

case "$host" in
    *-*-beos*)
        ac_default_prefix=/boot/develop/tools/gnupro
        ;;
    *-*-cygwin* | *-*-mingw32*)
        if test "$build" != "$host"; then # cross-compiling
            # Default cross-compile location
            ac_default_prefix=/usr/local/cross-tools/i386-mingw32
        else
            # Look for the location of the tools and install there
            if test "$BUILD_PREFIX" != ""; then
                ac_default_prefix=$BUILD_PREFIX
            fi
        fi
        if test x$WINDRES != x; then
            use_version_rc=true
        fi
        ;;
esac
AM_CONDITIONAL(USE_VERSION_RC, test x$use_version_rc = xtrue)

dnl set this to use on systems that use lib64 instead of lib
base_libdir=`echo \${libdir} | sed 's/.*\/\(.*\)/\1/; q'`

dnl Function to find a library in the compiler search path
find_lib()
{
    gcc_bin_path=[`$CC -print-search-dirs 2>/dev/null | fgrep programs: | sed 's/[^=]*=\(.*\)/\1/' | sed 's/:/ /g'`]
    gcc_lib_path=[`$CC -print-search-dirs 2>/dev/null | fgrep libraries: | sed 's/[^=]*=\(.*\)/\1/' | sed 's/:/ /g'`]
    env_lib_path=[`echo $LIBS $LDFLAGS $* | sed 's/-L[ ]*//g'`]
    if test "$cross_compiling" = yes; then
        host_lib_path=""
    else
        host_lib_path="/usr/$base_libdir /usr/local/$base_libdir"
    fi
    for path in $gcc_bin_path $gcc_lib_path $env_lib_path $host_lib_path; do
        lib=[`ls -- $path/$1 2>/dev/null | sort | sed 's/.*\/\(.*\)/\1/; q'`]
        if test x$lib != x; then
            echo $lib
            return
        fi
    done
}

dnl Check for SDL
SDL_VERSION=2.0.0
AC_SUBST(SDL_VERSION)
AM_PATH_SDL2($SDL_VERSION,
            :,
           AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!])
)
CFLAGS="$CFLAGS $SDL_CFLAGS"
LIBS="$LIBS $SDL_LIBS"

AC_SUBST([WINDRES])

OBJCFLAGS=$CFLAGS

# Finally create all the generated files
AC_OUTPUT([
Makefile
Splat.spec
Splat.pc
])
