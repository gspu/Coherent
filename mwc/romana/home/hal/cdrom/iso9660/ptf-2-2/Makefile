# makefile for CD magic
#
# Written by Rich Morin, CFCL, July, 1992

cmds:		catsub cd_dirs cd_files get_file

clean:
	rm -f *.o catsub cd_dirs cd_files get_file

catsub:	catsub.c
	cc catsub.c -o catsub

cd_dirs:	cd_dirs.c
	cc cd_dirs.c -o cd_dirs

cd_files:	cd_files.c dir_rec.c
	cc cd_files.c  dir_rec.c -o cd_files

get_file:	get_file.c
	cc get_file.c -o get_file

release:	HISTORY NOTES README *.c cdcat full makefile
	rm -rf release*
	mkdir release
	cp HISTORY NOTES README *.c cdcat full makefile release
	tar cvf release.tar release
	compress release.tar
	uuencode release.tar.Z release.tar.Z > release.tzu
	wc release.tzu
