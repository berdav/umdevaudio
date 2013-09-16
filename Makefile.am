ACLOCAL_AMFLAGS = -I m4

DEBUGLEVEL = 3

SUBDIRS = doc include um_lib um_cmd um_testmodule umdev umdev_testmodules umdevmbr umdevaudio umbinfmt xmview ummisc ummisc_modules um_viewfs umnet umnet_modules umfuse_modules

if ENABLE_UMDEVTAP
SUBDIRS += umdevtap
endif

if ENABLE_UMFUSE
SUBDIRS += umfuse
endif

if ENABLE_UM_LWIP
SUBDIRS += umnetlwipv6
endif

#if ENABLE_UMDEVAUDIO
SUBDIRS += umdevaudio
#endif

tags:
	ctags -R .

cscope:
	cscope -R -b

vim: tags cscope

CLEANFILES = ctags cscope.out

extraclean: maintainer-clean
	rm -rf aclocal.m4 autom4te.cache autoscan.log configure.scan \
	config.h.in* configure depcomp install-sh Makefile.in config.guess config.sub \
	ltmain.sh missing `find . -name Makefile.in` umview-*.tar.gz \
	umview-*.tar.bz2


if ENABLE_DEBUG

release:
	for subdir in $(SUBDIRS); \
	do \
		$(MAKE) -C $$subdir $@ || break;
	done

debug: all

else

release: all

debug:
	for subdir in $(SUBDIRS); \
	do \
		$(MAKE) -C $$subdir $@ || break; \
	done

endif

