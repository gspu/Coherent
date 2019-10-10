/*
 * System error messages
 */

char	*sys_errlist[] = {
	"",
	"not the super user",
	"no such file or directory",
	"no such process",
	"interrupted system call",
	"I/O error",
	"no such device or address",
	"arg list too long",
	"exec format error",
	"bad file number",
	"no children",
	"no more processes",
	"not enough memory",
	"permission denied",
	"bad address",
	"block device required",
	"mount device busy",
	"file exists",
	"cross-device link",
	"no such device",
	"not a directory",
	"is a directory",
	"invalid argument",
	"file table overflow",
	"too many open files",
	"not a typewriter",
	"file busy",
	"file too large",
	"no space left on device",
	"illegal seek",
	"read-only file system",
	"too many links",
	"broken pipe",
	"math argument",
	"result too large",
	"out of kernel space",
	"driver not loaded",
	"bad exec format",
	"device needs attention",
	"device busy"
};

int	sys_nerr = sizeof (sys_errlist)/sizeof (sys_errlist[0]);
