Berardi Davide 2013

UmDevAudio is a umview module for processing of ALSA - compatible sound cards

to compile place umview inside a directory called umview inside the root directory of the project

or simply launch

make reallyall

make install

(the build process will be modified to not recompile all umview)

note:
	tinyalsa has been modified to not use mmap at any cost, in case we can make it fail in umview with returning MMAP_WRONG when we encounter the specified offset
