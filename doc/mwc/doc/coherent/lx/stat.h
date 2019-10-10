.TH stat.h "" "" "Header File"
.PC "Definitions and declarations used to obtain file status"
.B "#include <sys/stat.h>"
.PP
.B "stat.h"
is a header file that declares the structure
.B stat
plus constants used by the routines that manipulate
files, directories, and named pipes.
It holds the prototypes for the routines
.BR chmod() ,
.BR fstat() ,
.BR mkdir() ,
.BR stat() ,
and
.BR umask() .
.PP
.II stat
The following summarizes the structure
.BR stat :
.DM
.ta 0.5i 2.0i 3.5i
struct stat {
	dev_t	st_dev;	/* Device */
	ino_t	st_ino;	/* Inode number */
	mode_t	st_mode;	/* Mode */
	nlink_t	st_nlink;	/* Link count */
	uid_t	st_uid;	/* User id */
	gid_t	st_gid;	/* Group id */
	dev_t	st_rdev;	/* Real device; NB, this is non-POSIX */
	off_t	st_size;	/* Size */
	time_t	st_atime;	/* Access time */
	time_t	st_mtime;	/* Modify time */
	time_t	st_ctime;	/* Change time */
};
.DE
.PP
.B st_dev
and
.B st_ino
together form a unique description of the file.
The former is the device on which the file and its i-node reside,
whereas the latter is the index number of the file.
.B st_mode
gives the permission bits, as outlined below.
.B st_nlink
gives the number of links to the file.
.B st_uid
and
.B st_gid,
respectively given
the user id and group id of the owner.
.BR st_rdev ,
valid only for special files,
holds the major and minor numbers for the file.
.B st_size
gives the size of the file, in bytes.
For a pipe,
the size is the number of bytes waiting to be read from the pipe.
.PP
Three entries for each file give the last occurrences of various events
in the file's history.
.B st_atime
gives time the file was last read or written to.
.B st_mtime
gives the time of the
last modification, write for files, create or delete
entry for directories.
.B st_ctime
gives the last change to the attributes, not including times and size.
.PP
The following manifest constants define file types:
.DS
.ta 0.5i 1.5i
	\fBS_IFMT\fR	Type
	\fBS_IFDIR\fR	Directory
	\fBS_IFCHR\fR	Character-special file
	\fBS_IFPIP\fR	Pipe
	\fBS_IFIFO\fR	Pipe
	\fBS_IFBLK\fR	Block-special file
	\fBS_IFREG\fR	Regular file
.DE
.PP
The following manifest constants define file modes:
.DS
.ta 0.5i 1.5i
	\fBS_IREAD\fR	Read permission, owner
	\fBS_IWRITE\fR	Write permission, owner
	\fBS_IEXEC\fR	Execute/search permission, owner
	\fBS_IRWXU\fR	RWX permission, owner
	\fBS_IRUSR\fR	Read permission, owner
	\fBS_IWUSR\fR	Write permission, owner
	\fBS_IXUSR\fR	Execute/search permission, owner
	\fBS_IRWXG\fR	RWX permission, group
	\fBS_IRGRP\fR	Read permission, group
	\fBS_IWGRP\fR	Write permission, group
	\fBS_IXGRP\fR	Execute/search permission, group
	\fBS_IRWXO\fR	RWX permission, other
	\fBS_IROTH\fR	Read permission, other
	\fBS_IWOTH\fR	Write permission, other
	\fBS_IXOTH\fR	Execute/search permission, other
.DE
.SH "See Also"
.Xr "chmod()," chmod.s
.Xr "fstat()," fstat
.Xr "header file," header_fi
.Xr "stat()" stat
.br
\*(PX Standard, \(sc5.6.1
