cwd=
shell=/bin/sh
editor=vi
print-spooler=lpr -B
view=more
me-disp-attr=reverse
pd-disp-color=normal
pd-disp-attr=both
se-disp-attr=both
idle-time=3
cmd=
reaction=ask
actions=
	[Mm]akefile:make
	*.mk:make -f %F
	*.sh:sh %F
	*.c:cc -c -O %F
	*.sc:sc %F
	*.a:ar tv %F | more
	*.[1-9]:nroff -man %F | more
	*.tar.F:fcat %F | tar xvf -
	*.F:fcat %F | more
	*.tar.Z:zcat %F | tar xvf -
	*.Z:zcat %F | more
	*.tar.z:zcat %F | tar xvf -
	*.z:zcat %F | more
