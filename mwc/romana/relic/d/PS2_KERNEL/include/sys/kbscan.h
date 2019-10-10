/* (-lgl
 * 	COHERENT Version 3.2
 * 	Copyright (c) 1982, 1991 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * kbscan.h:	AT physical key number to "scan code set 3" mapping constants
 *
 *		All key values are "make" codes. Equivalent "break" codes
 *		are prefixed by K_BREAK.
 *
 *		See IBM AT keyboard technical reference for further details.
 *
 *		Note: 83-key XT layout keyboards use scan codes
 *		K_14, K_65 through K_74, and K_107. These do not exist on
 *		101- and 102-key AT keyboards.
 *
 *		Version 1.2,  06/26/91
 */
#ifndef	KBSCAN_H
#define	KBSCAN_H

/*    AT kbd   scan code  */
/*   physical    set 3    */
/*     key #    mapping   */
/*     -----    -------   */
#define	K_1	0x0E
#define	K_2	0x16
#define	K_3	0x1E
#define	K_4	0x26
#define	K_5	0x25
#define	K_6	0x2E
#define	K_7	0x36
#define	K_8	0x3D
#define	K_9	0x3E
#define	K_10	0x46
#define	K_11	0x45
#define	K_12	0x4E
#define	K_13	0x55
/* key 14 undefined for 101- and 102-key AT layouts */
#define	K_14	0x5D
#define	K_15	0x66
#define	K_16	0x0D
#define	K_17	0x15
#define	K_18	0x1D
#define	K_19	0x24
#define	K_20	0x2D
#define	K_21	0x2C
#define	K_22	0x35
#define	K_23	0x3C
#define	K_24	0x43
#define	K_25	0x44
#define	K_26	0x4D
#define	K_27	0x54
#define	K_28	0x5B
#define	K_29	0x5C		/* 101-key keyboard only */
#define	K_30	0x14
#define	K_31	0x1C
#define	K_32	0x1B
#define	K_33	0x23
#define	K_34	0x2B
#define	K_35	0x34
#define	K_36	0x33
#define	K_37	0x3B
#define	K_38	0x42
#define	K_39	0x4B
#define	K_40	0x4C
#define	K_41	0x52
#define	K_42	0x53		/* 102-key keyboard only */
#define	K_43	0x5A
#define	K_44	0x12
#define	K_45	0x13		/* 102-key keyboard only */
#define	K_46	0x1A
#define	K_47	0x22
#define	K_48	0x21
#define	K_49	0x2A
#define	K_50	0x32
#define	K_51	0x31
#define	K_52	0x3A
#define	K_53	0x41
#define	K_54	0x49
#define	K_55	0x4A
/* key 56 undefined */
#define	K_57	0x59
#define	K_58	0x11
/* key 59 undefined */
#define	K_60	0x19
#define	K_61	0x29
#define	K_62	0x39
/* key 63 undefined */
#define	K_64	0x58
/* keys 65 through 74 undefined for 101- and 102-key AT layouts */
#define	K_65	0x06
#define	K_66	0x0C
#define	K_67	0x0B
#define	K_68	0x0A
#define	K_69	0x09
#define	K_70	0x05
#define	K_71	0x04
#define	K_72	0x03
#define	K_73	0x83
#define	K_74	0x01
#define	K_75	0x67
#define	K_76	0x64
/* keys 77 and 78 undefined */
#define	K_79	0x61
#define	K_80	0x6E
#define	K_81	0x65
/* key 82 undefined */
#define	K_83	0x63
#define	K_84	0x60
#define	K_85	0x6F
#define	K_86	0x6D
/* keys 87 and 88 undefined */
#define	K_89	0x6A
#define	K_90	0x76
#define	K_91	0x6C
#define	K_92	0x6B
#define	K_93	0x69
/* key 94 undefined */
#define	K_95	0x77
#define	K_96	0x75
#define	K_97	0x73
#define	K_98	0x72
#define	K_99	0x70
#define	K_100	0x7E
#define	K_101	0x7D
#define	K_102	0x74
#define	K_103	0x7A
#define	K_104	0x71
#define	K_105	0x84
#define	K_106	0x7C
/* key 107 undefined for 101- and 102-key AT layouts */
#define	K_107	0x7B			/* on right hand side of num. keypad */
#define	K_108	0x79
/* key 109 undefined */
#define	K_110	0x08
/* key 111 undefined */
#define	K_112	0x07
#define	K_113	0x0F
#define	K_114	0x17
#define	K_115	0x1F
#define	K_116	0x27
#define	K_117	0x2F
#define	K_118	0x37
#define	K_119	0x3F
#define	K_120	0x47
#define	K_121	0x4F
#define	K_122	0x56
#define	K_123	0x5E
#define	K_124	0x57
#define	K_125	0x5F
#define	K_126	0x62

#endif
/* end of kbscan.h */
