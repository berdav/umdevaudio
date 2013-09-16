UMDIR=$(PWD)/umview/xmview-os
reallyall: getumview all
all: setfiles extraconfig config compile
extraconfig:
	cd $(UMDIR);\
	autoreconf -i;
config:
	cd $(UMDIR);\
	./configure;
setfiles:
	@#backup
	if [ ! -e Makefile.am.real_bck ]; then\
		mv $(UMDIR)/Makefile.am Makefile.am.real_bck;\
	fi;
	cp Makefile.am $(UMDIR)/Makefile.am
	cp configure.ac $(UMDIR)/configure.ac
	rm $(UMDIR)/umdevaudio -rf
	cp umdevaudio $(UMDIR)/umdevaudio -r
getumview:
	@echo "\ndownloading experimental version of viewos\n"
	svn co https://view-os.svn.sourceforge.net/svnroot/view-os/branches/rd235 umview
install:
	cp default.conf ~/.umdevaudiorc
	cd $(UMDIR);\
	make install;
compile:
	cd $(UMDIR);\
	make all;
clean:
	cd $(UMDIR);\
	make clean;
extraclean:
	cd $(UMDIR);\
	make extraclean
senseforme:
	@echo "\nyeah, you're right!\n"
