.TH hdioctl.h "" "" "Header File"
.PC "Control hard-disk I/O"
.B "#include <sys/hdioctl.h>"
.PP
Header file
.B <sys/hdioctl.h>
declares constants and structures used to control hard-disk I/O.
.PP
.II ide_info
Structure
.B ide_info
is used by the command
.B ideinfo
to hold information about IDE drives.
It is defined as follows:
.DM
.ta 0.5i 3.0i
typedef struct ide_info {
	unsigned short ii_config;	/* Configuration */
	unsigned short ii_cyl;	/* Cylinders (default translation mode) */
	unsigned short ii_reserved;	/* reserved */
	unsigned short ii_heads;	/* heads (default translation mode */
	unsigned short ii_bpt;	/* bytes per track (unformatted) */
	unsigned short ii_bps;	/* bytes per sector (unformatted) */
	unsigned short ii_spt;	/* sectors per track (default translation mode) */
	unsigned short ii_vendor1[3];	/* vendor's unique data */
	unsigned short ii_serialnum[10];	/* serial number in ASCII */
	unsigned short ii_buffertype;	/* buffer type */
	unsigned short ii_buffersize;	/* buffer size in 512-byte sectors */
	unsigned short ii_eccbyteslong;	/* ecc bytes for r/w long */
	unsigned short ii_firmrev[4];	/* firmware revision in ASCII */
	unsigned short ii_modelnum[20];	/* model number in ASCII */
	unsigned short ii_doublewordio;	/* double word transfer flag */
	unsigned short ii_capabilities;	/* capabilities */
	unsigned short ii_reserved2;	/* reserved */
	unsigned short ii_piomode;	/* PIO data transfer timing mode */
	unsigned short ii_dmamode;	/* DMA data transfer timing mode */
	unsigned short ii_reserved3[75];	/* reserved */
	unsigned short ii_vendor2[32];	/* vendor unique data */
	unsigned short ii_reserved4[96]; /* reserved */
} ide_info_t;
.DE
Field
.B ii_config
is a set of flags that describes how the drive is configured, as follows:
.IP "\fBbit 0\fR"
Not used.
.IS "\fBbit 1\fR"
Disk is hard sectored.
.IS "\fBbit 2\fR"
Disk is soft sectored.
.IS "\fBbit 3\fR"
Disk is not MFM encoded.
.IS "\fBbit 4\fR"
Disk's head switch time is less than 15 microseconds.
.IS "\fBbit 5\fR"
Spindled motor control option is implemented.
.IS "\fBbit 6\fR"
Fixed drive.
.IS "\fBbit 7\fR"
Not used.
.IS "\fBbit 8\fR"
Disk's transfer rate is less than five megabytes per second.
.IS "\fBbit 9\fR"
Disk's transfer rate exceeds five megabytes per second
but less than or equal to 10 megabytes per second.
.IS "\fBbit 10\fR"
The disk's transfer rate exceeds ten megabytes per second.
.IS "\fBbit 11\fR"
The rotational's speed tolerance is greater than 0.5%.
.IS "\fBbit 12\fR"
The data strobe offset option is available.
.IS "\fBbit 13\fR"
The track offset option is available.
.IS "\fBbit 14\fR"
The format speed-tolerance gap required.
.IS "\fBbit 15\fR"
Not used.
.PP
Structure
.B hdparm_s
holds the drive's attributes.
It is configured for binary compatibility with ROM data.
.DM
.ta 0.5i 3.0i
typedef struct hdparm_s {
	unsigned char ncyl[2];	/* number of cylinders */
	unsigned char nhead;	/* number heads */
	unsigned char rwccp[2];	/* reduced write curr cyl */
	unsigned char wpcc[2];	/* write pre-compensation cyl */
	unsigned char eccl;	/* max ecc data length */
	unsigned char ctrl;	/* control byte */
	unsigned char fill2[3];
	unsigned char landc[2];	/* landing zone cylinder */
	unsigned char nspt;	/* number of sectors per track */
	unsigned char hdfill3;
} hdparm_t;
.DE
.SH "See Also"
.Xr "hard disk," hard_disk
.Xr "header files," header_fi
.Xr "ideinfo" ideinfo
