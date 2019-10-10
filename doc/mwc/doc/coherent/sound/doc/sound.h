.TH sound.h "" "" "Header File"
.PC "Define types and constants used by sound-card drivers"
.B "#include <sys/sound.h>"
.PP
The header file
.B <sys/sound.h>
defines the types and constants used by \*(CO's suite of drivers for
sound cards.
.PP
.II SoundInit
Structure
.B SoundInit
holds information used to initialize the sound card:
.DM
.ta 0.5i 3.0i
typedef struct _SoundInit
{
	int sampleRate;	/* 5-44.1 KHz */
	int bStereo;	/* stereo flag (0 = mono, 1 = stereo) */
	int bRecord;	/* record flag (0 = playback, 1 = record) */
	int bits;	/* bits per sample (8/16) */
	unsigned length;	/* data length in bytes (playback only) */
	unsigned filterFreq;	/* filter frequency */
} SoundInit;
.DE
.PP
Structure
.B MixerControl
controls the sound mixing on the card:
.DM
.ta 0.5i 3.0i
typedef struct _MixerControl
{
	int left;	/* left/mono channel (0-255) */
	int right;	/* right channel (0-255) */
	int levels;	/* actual number of distinct levels (get only) */
	int bRoot;	/* requires root permissions to change (get only) */
} MixerControl;
.DE
.PP
When a driver queries a card, it writes the information that it retrieves into
structure
.BR SoundInfo :
.DM
.ta 0.5i 3.0i
typedef struct _SoundInfo
{
	int deviceCode;	/* sound card identification code */
	char deviceName[30];	/* device name */
	int recordFlags;	/* recording capabilities */
	int playFlags;	/* playback capabilities */
	int mixerFlags;	/* mixer capabilities */
	int miscFlags;	/* miscellaneous capabilities */
} SoundInfo;
.DE
.PP
Structure
.B SoundLevel
controls the card's sound level.
Also, when the driver reads levels from the card, it writes it into this
structure.
.DM
.ta 0.5i 3.0i
typedef struct _SoundLevel
{
	int left;	/* left channel level (0-65535) */
	int right;	/* right channel level (0-65535) */
	int bClipLeft;	/* left clipping flag */
	int bClipRight;	/* right clipping flag */
} SoundLevel;
.DE
.PP
When the driver reads the driver's position (that is, the number of bytes
it has read from the card or written to it), it writes the information into a
structure of type
.BR SoundPosition :
.DM
.ta 0.5i 3.0i
typedef struct _SoundPosition
{
	unsigned bytes; /* number of bytes played/recorded */
} SoundPosition;
.DE
.PP
For details on the ioctls defined in this header file, see the Lexicon entry for
.BR ioctl() .
.SH "See Also"
.B
header files,
ioctl(),
sound
.R
