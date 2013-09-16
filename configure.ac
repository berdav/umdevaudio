#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.60)
AC_INIT([View-OS], [0.8.2.1], [info@v2.cs.unibo.it], [umview])
AM_INIT_AUTOMAKE([foreign dist-bzip2 1.10])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([xmview/umview.c])
AC_CONFIG_HEADER([config.h])

AC_USE_SYSTEM_EXTENSIONS

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

AC_DEFINE_UNQUOTED(MODULES_EXT, "$shrext_cmds", [Extension of shared objects])

# Checks for libraries.
AC_CHECK_LIB([dl], [dlopen],, AC_MSG_ERROR([libdl missing]))
AC_CHECK_LIB([pthread], [pthread_key_create],, AC_MSG_ERROR([libpthread missing]))
# Is part of this package
AC_CHECK_LIB([vdeplug], [vde_recv], [enable_umdevtap=yes],
			 [enable_umdevtap=warn])
AC_CHECK_LIB([mhash], [mhash_init],, AC_MSG_ERROR([mhash missing]), [-lmhash])

#umpyew temporary disabled (not yet upgraded to 0.8)
#AM_PATH_PYTHON([2.4], [enable_umpyew=yes], [enable_umpyew=warn])
## define PYTHOPN_CFLAGS & PYTHON_LIBS
#PYTHON_CONFIG=`type -p python$PYTHON_VERSION-config`
#if test "$PYTHON_CONFIG" != ""; then
#	PYTHON_CFLAGS=`$PYTHON_CONFIG --includes`
#	PYTHON_LIBS=`$PYTHON_CONFIG --libs`
#else
#	PYTHON_CFLAGS=`$PYTHON $srcdir/python-config.py --includes`
#	PYTHON_LIBS=`$PYTHON $srcdir/python-config.py --libs`
#fi
#
#AC_SUBST(PYTHON_CFLAGS)
#AC_SUBST(PYTHON_LIBS)

# Checks for header files.
AC_FUNC_ALLOCA
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([fcntl.h limits.h stddef.h stdint.h stdlib.h string.h sys/ioctl.h sys/mount.h sys/param.h sys/socket.h sys/statfs.h sys/statvfs.h sys/time.h sys/capability.h unistd.h utime.h values.h],, AC_MSG_ERROR([required header file missing]))

AC_CHECK_HEADERS([fuse.h fuse/fuse.h], [enable_umfuse=yes], [enable_umfuse=warn], [#define _FILE_OFFSET_BITS 64])

AC_CHECK_HEADER([libvdeplug.h],, [enable_umdevtap=warn])

AC_CHECK_HEADER([lwipv6.h], [enable_um_lwip=yes], [enable_um_lwip=warn])

#CPPFLAGS_save="${CPPFLAGS}"
#CPPFLAGS="${PYTHON_CFLAGS}"
#AC_CHECK_HEADER([Python.h],, [enable_umpyew=warn])
#CPPFLAGS="${CPPFLAGS_save}"

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_UID_T
AC_C_INLINE
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_CHECK_MEMBERS([struct stat.st_blksize])
AC_STRUCT_ST_BLOCKS
AC_CHECK_MEMBERS([struct stat.st_rdev])
AC_HEADER_TIME
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_CHOWN
AC_FUNC_FORK
AC_PROG_GCC_TRADITIONAL
AC_HEADER_MAJOR
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_FUNC_STAT
AC_FUNC_STRCOLL
AC_FUNC_STRNLEN
AC_FUNC_UTIME_NULL
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([atexit ftruncate lchown memmove mempcpy memset mkdir mkfifo rmdir setenv strchr strdup strerror strrchr utime],, AC_MSG_ERROR([required functions missing]))
AC_CHECK_FUNCS([process_vm_readv])
AC_CHECK_FUNCS([process_vm_writev])

PKG_CHECK_MODULES([GLIB20], [glib-2.0])




AC_ARG_ENABLE([debug],
	AS_HELP_STRING([--enable-debug],
		[Enable debug functionalities]),
	[if test x$enableval != "xno"; then enable_debug=yes ; else enable_debug=no; fi],
	[enable_debug=no])

AC_ARG_ENABLE([fuse],
			  AS_HELP_STRING([--disable-fuse],
							 [Disable support for FUSE]),
							 [if test x$enableval = "xno"; then enable_umfuse=no; fi],)

AC_ARG_ENABLE([vde],
			  AS_HELP_STRING([--disable-vde],
							 [Disable support for VDE with libvdeplug]),
							 [if test x$enableval = "xno"; then enable_umdevtap=no; fi],)

AC_ARG_ENABLE([lwip],
			  AS_HELP_STRING([--disable-lwip],
							 [Disable support for LWIPv6 networking]),
							 [if test x$enableval = "xno"; then enable_um_lwip=no; fi],)

# umpyew disabled
#AC_ARG_ENABLE([python],
#			  AS_HELP_STRING([--disable-python],
#							 [Disable support for Python modules]),
#							 [if test x$enableval = "xno"; then enable_umpyew=no; fi],)

AM_CONDITIONAL([ENABLE_UMDEVTAP], [test x$enable_umdevtap = "xyes"])
AM_CONDITIONAL([ENABLE_UMFUSE], [test x$enable_umfuse = "xyes"])
AM_CONDITIONAL([ENABLE_UM_LWIP], [test x$enable_um_lwip = "xyes"])
# umpyew disabled
#AM_CONDITIONAL([ENABLE_UMPYEW], [test x$enable_umpyew = "xyes"])
AM_CONDITIONAL([ENABLE_DEBUG], [test x$enable_debug = "xyes"])

AC_CONFIG_FILES([Makefile
                 doc/Makefile
                 include/Makefile
                 um_lib/Makefile
                 um_cmd/Makefile
                 um_testmodule/Makefile
                 umdev/Makefile
                 umdev_testmodules/Makefile
                 umdevaudio/Makefile
                 umdevmbr/Makefile
                 umdevtap/Makefile
                 umfuse/Makefile
                 umbinfmt/Makefile
                 um_viewfs/Makefile
                 ummisc/Makefile
                 ummisc_modules/Makefile
                 umnet/Makefile
                 umnet_modules/Makefile
                 umnetlwipv6/Makefile
                 umfuse_modules/Makefile
                 xmview/Makefile])
AC_OUTPUT

if test x$enable_umdevtap = "xwarn"
then
	AC_MSG_WARN([TAP module for UMDev (umdevtap) will NOT be compiled. You
need libvdeplug development files installed.])
fi


if test x$enable_umfuse = "xwarn"
then
	AC_MSG_WARN([UMView support for FUSE modules (umfuse) will NOT be
compiled. You need FUSE development files installed.])
fi

if test x$enable_um_lwip = "xwarn"
then
	AC_MSG_WARN([UMView support for LWIPv6 networking (um_lwip) will NOT be
compiled. You need LWIPv6 development files installed.])
fi

# umpyew disabled
#if test x$enable_umpyew = "xwarn"
#then
#	AC_MSG_WARN([UMView support for Python modules (umpyew) will NOT be
#compiled. You nedd Python >= 2.4 and Python >= 2.4 development files installed.])
#fi
