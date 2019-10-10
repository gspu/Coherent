#ifndef	__KERNEL_CONFINFO_H__
#define	__KERNEL_CONFINFO_H__

/*
 * This file contains definitions used by the automatically-generated
 * configuration file "conf.c". The generated code makes heavy use of macros
 * to allow the final result to be precisely tailored to the target system
 * without modification to the code generator.
 */
/*
 *-IMPORTS:
 *	<common/ccompat.h>
 *		__EXTERN_C_BEGIN__
 *		__EXTERN_C_END__
 *		__USE_PROTO__
 *		__PROTO ()
 *	<common/xdebug.h>
 *		__LOCAL__
 *	<kernel/ddi_cpu.h>
 *		intmask_t
 *	<kernel/v_types.h>
 *		cred_t
 *		n_dev_t
 *		n_off_t
 *		major_t
 *		minor_t
 *	<sys/inline.h>
 *		_masktab []
 *		__GET_BASE_MASK ()
 *		__SET_BASE_MASK ()
 *	<sys/stream.h>
 *		struct streamtab
 *	<sys/uio.h>
 *		uio_t
 */

#include <common/ccompat.h>
#include <common/xdebug.h>
#include <kernel/ddi_cpu.h>
#include <kernel/v_types.h>
#include <sys/inline.h>
#include <sys/stream.h>
#include <sys/uio.h>


#ifdef	__USE_PROTO__
#define	DEF(ansi,k_r)		ansi
#else
#define	DEF(ansi,k_r)		k_r
#endif

/*
 * Since we don't import the definition of "buf", let's create an incomplete
 * definition at file scope. Same for "pollhead".
 */

struct buf;
struct pollhead;

#define	__OPEN(name)	__CONCAT (name, open)
#define	__CLOSE(name)	__CONCAT (name, close)
#define	__READ(name)	__CONCAT (name, read)
#define	__WRITE(name)	__CONCAT (name, write)
#define	__IOCTL(name)	__CONCAT (name, ioctl)
#define	__STRATEGY(name) __CONCAT (name, strategy)
#define	__PRINT(name)	__CONCAT (name, print)
#define	__SIZE(name)	__CONCAT (name, size)
#define	__CHPOLL(name)	__CONCAT (name, chpoll)
#define	__MMAP(name)	__CONCAT (name, mmap)
#define	__INTR(name)	__CONCAT (name, intr)
#define	__INIT(name)	__CONCAT (name, init)
#define	__HALT(name)	__CONCAT (name, halt)
#define	__START(name)	__CONCAT (name, start)
#define	__EXIT(name)	__CONCAT (name, exit)

#define	__INFO(name)	__CONCAT (name, info)
#define	__DEVFLAG(name)	__CONCAT (name, devflag)


typedef	int  (*	ddi_open_t)	__PROTO ((n_dev_t * _devp, int _oflag,
					  int _otyp, __cred_t * _credp));
#define	EXTERN_OPEN(name) __EXTERN_C__ \
  int	__OPEN (name)	__PROTO ((n_dev_t * _devp, int _oflag,\
					 int _otyp, __cred_t * _credp))

typedef	int  (* ddi_close_t)	__PROTO ((n_dev_t _dev, int _oflag, int _otyp,
					  cred_t * _credp));
#define	EXTERN_CLOSE(name) __EXTERN_C__ \
  int	__CLOSE (name)	__PROTO ((n_dev_t _dev, int _oflag, int _otyp,\
					  cred_t * _credp))

typedef	int  (*	ddi_read_t)	__PROTO ((n_dev_t _dev, uio_t * _uiop,
					  cred_t * _credp));
#define	EXTERN_READ(name) __EXTERN_C__ \
  int	__READ (name)	__PROTO ((n_dev_t _dev, uio_t * _uiop,\
					  cred_t * _credp))

typedef int  (*	ddi_write_t)	__PROTO ((n_dev_t _dev, uio_t * _uiop,
					  cred_t * _credp));
#define	EXTERN_WRITE(name) __EXTERN_C__ \
  int	__WRITE (name)	__PROTO ((n_dev_t _dev, uio_t * _uiop,\
					  cred_t * _credp))

typedef	int  (*	ddi_ioctl_t)	__PROTO ((n_dev_t _dev, int _cmd, _VOID * _arg,
					  int _mode, cred_t * _credp,
					  int * _rvalp));
#define	EXTERN_IOCTL(name) __EXTERN_C__ \
  int	__IOCTL (name)	__PROTO ((n_dev_t _dev, int _cmd, _VOID * _arg,\
					  int _mode, cred_t * _credp,\
					  int * _rvalp))

typedef	int  (*	ddi_strategy_t)	__PROTO ((struct buf * _bufp));
#define	EXTERN_STRATEGY(name) __EXTERN_C__ \
  int	__STRATEGY (name) __PROTO ((struct buf * _bufp))

typedef int  (*	ddi_print_t)	__PROTO ((n_dev_t _dev, char * _str));
#define	EXTERN_PRINT(name) __EXTERN_C__ \
  int	__PRINT (name)	__PROTO ((n_dev_t _dev, char * _str))

typedef int  (*	ddi_size_t)	__PROTO ((n_dev_t _dev));
#define	EXTERN_SIZE(name) __EXTERN_C__ \
  int	__SIZE (name)	__PROTO ((n_dev_t _dev))

typedef	int  (*	ddi_chpoll_t)	__PROTO ((n_dev_t _dev, short _events,
					  int _anyyet, short * _reventsp,
					  struct pollhead ** _phpp));
#define	EXTERN_CHPOLL(name) __EXTERN_C__ \
  int	__CHPOLL (name)	__PROTO ((n_dev_t _dev, short _events,\
					  int _anyyet, short * _reventsp,\
					  struct pollhead ** _phpp))

typedef	int  (*	ddi_mmap_t)	__PROTO ((n_dev_t _dev, off_t _off,
					  int _prot));
#define	EXTERN_MMAP(name) __EXTERN_C__ \
  int	__MMAP (name)	__PROTO ((n_dev_t _dev, off_t _off,\
					  int _prot))

typedef	int  (*	ddi_intr_t)	__PROTO ((int _ivec));
#define	EXTERN_INTR(name) __EXTERN_C__ \
  int	__INTR (name)	__PROTO ((int _ivec))

typedef void (*	ddi_init_t)	__PROTO ((void));
#define	EXTERN_INIT(name) __EXTERN_C__ \
  void	__INIT (name)	__PROTO ((void))

typedef	void (*	ddi_start_t)	__PROTO ((void));
#define	EXTERN_START(name) __EXTERN_C__ \
  void	__START (name)	__PROTO ((void))

typedef	void (*	ddi_exit_t)	__PROTO ((void));
#define	EXTERN_EXIT(name) __EXTERN_C__ \
  void	__EXIT (name)	__PROTO ((void))

typedef	void (*	ddi_halt_t)	__PROTO ((void));
#define	EXTERN_HALT(name) __EXTERN_C__ \
  void	__HALT (name)	__PROTO ((void))

#define	NULL_OPEN		((ddi_open_t) 0)
#define	NULL_CLOSE		((ddi_close_t) 0)
#define	NULL_READ		((ddi_read_t) 0)
#define	NULL_WRITE		((ddi_write_t) 0)
#define	NULL_IOCTL		((ddi_ioctl_t) 0)
#define	NULL_STRATEGY		((ddi_strategy_t) 0)
#define	NULL_PRINT		((ddi_print_t) 0)
#define	NULL_SIZE		((ddi_size_t) 0)
#define	NULL_CHPOLL		((ddi_chpoll_t) 0)
#define	NULL_MMAP		((ddi_mmap_t) 0)
#define	NULL_INTR		((ddi_intr_t) 0)
#define	NULL_INIT		((ddi_init_t) 0)
#define	NULL_START		((ddi_start_t) 0)
#define	NULL_EXIT		((ddi_exit_t) 0)
#define	NULL_HALT		((ddi_halt_t) 0)


#if	__COHERENT__ || defined (__MSDOS__)

#define	__KERNEL__	2

#if	defined (__MSDOS__)
#include <sys/_con.h>
#else
#include <sys/buf.h>
#include <sys/con.h>
#include <sys/reg.h>
#endif

#include <sys/io.h>

#undef	__KERNEL__

__EXTERN_C_BEGIN__

int		_forward_open	__PROTO ((o_dev_t _dev, int _mode, int _flags,
					  ddi_open_t _funcp));
int		_forward_close	__PROTO ((o_dev_t _dev, int _mode, int _flags,
					  ddi_close_t _funcp));
int		_forward_read	__PROTO ((o_dev_t _dev, IO * _iop,
					  ddi_read_t _funcp));
int		_forward_write	__PROTO ((o_dev_t _dev, IO * _iop,
					  ddi_write_t _funcp));
int		_forward_ioctl	__PROTO ((o_dev_t _dev, int _cmd,
					  _VOID * _arg, int mode,
					  ddi_ioctl_t _funcp));
#ifndef	__MSDOS__
int		_forward_strategy __PROTO ((BUF * _buf,
					    ddi_strategy_t _funcp));
#endif
int		_forward_chpoll	__PROTO ((o_dev_t _dev, int _events,
					  int _msec, ddi_chpoll_t _funcp));

int		_streams_open	__PROTO ((o_dev_t _dev, int _mode, int _flags,
					  struct streamtab * _infop));
int		_streams_close	__PROTO ((o_dev_t _dev, int _mode, int _flags,
					  struct streamtab * _infop));
int		_streams_read	__PROTO ((o_dev_t _dev, IO * _iop,
					  struct streamtab * _infop));
int		_streams_write	__PROTO ((o_dev_t _dev, IO * _iop,
					  struct streamtab * _infop));
int		_streams_ioctl	__PROTO ((o_dev_t _dev, int _cmd,
					  _VOID * _arg, int mode,
					  struct streamtab * _infop));
int		_streams_chpoll	__PROTO ((o_dev_t _dev, int _events,
					  int _msec,
					  struct streamtab * _infop));

__EXTERN_C_END__

typedef struct {
	int	      *	cdev_flag;
	CON		cdev_con;
} cdevsw_t;

/*
 * Note that we do not have the DFPOL flag set by default. While most compilers
 * can evaluate a "(ch) == 0 ? :" type of expression which is a constant in an
 * initializer, the MWC compiler cannot do so reliably. We fix up the flags at
 * startup time as a result.
 */

#define	CDEVSW_ENTRY(fl,op,cl,re,wr,io,ch,mm) \
	{ (fl), { DFCHR, 0, \
		  (op), (cl), 0, (re), (wr), (io), 0, 0, 0, 0, (ch) } }

#define	NULL_CDEVSW() \
	{ 0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }

#define	STREAMS_ENTRY(name) \
	{ & __DEVFLAG (name), { DFCHR | DFPOL, 0, \
		__CONCAT (name, openS), __CONCAT (name, closeS), \
		0, __CONCAT (name, readS), __CONCAT (name, writeS), \
		__CONCAT (name, ioctlS), 0, 0, 0, 0, \
		__CONCAT (name, chpollS) } }

#define	COH_OPEN(name,s) \
  __LOCAL__ int __CONCAT3 (name, open, s) \
	DEF ((o_dev_t dev, int mode, int flag),\
	     (dev, mode, flag) o_dev_t dev; int mode; int flag;)

#define	COH_CLOSE(name,s) \
  __LOCAL__ int __CONCAT3 (name, close, s) \
	DEF ((o_dev_t dev, int mode, int flag), \
	     (dev, mode, flag) o_dev_t dev; int mode; int flag;)

#define	COH_READ(name,s) \
  __LOCAL__ int __CONCAT3 (name, read, s) \
	DEF ((o_dev_t dev, IO * iop), \
	     (dev, iop) o_dev_t dev; IO * iop;)

#define	COH_WRITE(name,s) \
  __LOCAL__ int __CONCAT3 (name, write, s) \
	DEF ((o_dev_t dev, IO * iop), \
	     (dev, iop) o_dev_t dev; IO * iop;)

#define	COH_IOCTL(name,s) \
  __LOCAL__ int __CONCAT3 (name, ioctl, s) \
	DEF ((o_dev_t dev, int cmd, _VOID * arg, int mode), \
	     (dev, cmd, arg, mode) \
		o_dev_t dev; int cmd; _VOID * arg; int mode;)

#define	COH_STRATEGY(name,s) \
  __LOCAL__ int __CONCAT3 (name, strategy, s) \
	DEF ((BUF * buf), \
	     (buf) BUF * buf;)

#define	COH_CHPOLL(name,s) \
   __LOCAL__ int __CONCAT3 (name, chpoll, s) \
	DEF ((o_dev_t dev, int events, int msec), \
	     (dev, events, msec) o_dev_t dev; int events; int msec;)

#define	DECLARE_STREAMS(name) \
 extern struct streamtab __INFO (name); \
 COH_OPEN (name, S) { \
	return _streams_open (dev, mode, flag, & __INFO (name)); \
 } \
 COH_CLOSE (name, S) { \
	return _streams_close (dev, mode, flag, & __INFO (name)); \
 } \
 COH_READ (name, S) { \
	return _streams_read (dev, iop, & __INFO (name)); \
 } \
 COH_WRITE (name, S) { \
	return _streams_write (dev, iop, & __INFO (name)); \
 } \
 COH_IOCTL (name, S) { \
	return _streams_ioctl (dev, cmd, arg, mode, & __INFO (name));\
 } \
 COH_CHPOLL (name, S) { \
	return _streams_chpoll (dev, events, msec, & __INFO (name)); \
 }

#define	DECLARE_OPEN(name) \
  COH_OPEN (name,D) { \
	EXTERN_OPEN (name); \
	return _forward_open (dev, mode, flag, __OPEN (name)); \
  }

#define	DECLARE_CLOSE(name) \
  COH_CLOSE (name,D) { \
	EXTERN_CLOSE (name); \
	return _forward_close (dev, mode, flag, __CLOSE (name)); \
  }

#define	DECLARE_READ(name) \
  COH_READ (name,D) { \
	EXTERN_READ (name); \
	return _forward_read (dev, iop, __READ (name)); \
  }

#define	DECLARE_WRITE(name) \
  COH_WRITE (name,D) { \
	EXTERN_WRITE (name); \
	return _forward_write (dev, iop, __WRITE (name)); \
  }

#define	DECLARE_IOCTL(name) \
  COH_IOCTL (name,D) { \
	EXTERN_IOCTL (name); \
	return _forward_ioctl (dev, cmd, arg, mode, __IOCTL (name)); \
  }

#define	DECLARE_STRATEGY(name) \
  COH_STRATEGY (name,D) { \
	EXTERN_STRATEGY (name); \
	return _forward_strategy (buf, __STRATEGY (name)); \
  }

#define	DECLARE_CHPOLL(name) \
  COH_CHPOLL (name,D) { \
	EXTERN_CHPOLL (name); \
	return _forward_chpoll (dev, events, msec, __CHPOLL (name)); \
  }

#define	OPEN(name)		__CONCAT (name, openD)
#define CLOSE(name)		__CONCAT (name, closeD)
#define	READ(name)		__CONCAT (name, readD)
#define	WRITE(name)		__CONCAT (name, writeD)
#define	IOCTL(name)		__CONCAT (name, ioctlD)
#define	STRATEGY(name)		__CONCAT (name, strategyD)
#define	CHPOLL(name)		__CONCAT (name, chpollD)

#endif	/* __COHERENT__ || defined (__MSDOS__) */


/*
 * Now default definitions for those things that are not overridden above.
 */

#if	! defined (DECLARE_STREAMS)
# define	DECLARE_STREAMS(name)	extern struct streamtab __INFO (name);
#endif

#if	! defined (DECLARE_MODULE)
# define	DECLARE_MODULE(name)	extern struct streamtab __INFO (name);
#endif

#if	! defined (DECLARE_OPEN) || ! defined (OPEN)
# define	DECLARE_OPEN(name)	EXTERN_OPEN (name);
# define	OPEN(name)		__OPEN (name)
#endif

#if	! defined (DECLARE_CLOSE) || ! defined (CLOSE)
# define	DECLARE_CLOSE(name)	EXTERN_CLOSE (name);
# define	CLOSE(name)		__CLOSE (name)
#endif

#if	! defined (DECLARE_READ) || ! defined (READ)
# define	DECLARE_READ(name)	EXTERN_READ (name);
# define	READ(name)		__READ (name)
#endif

#if	! defined (DECLARE_WRITE) || ! defined (WRITE)
# define	DECLARE_WRITE(name)	EXTERN_WRITE (name);
# define	WRITE(name)		__WRITE (name)
#endif

#if	! defined (DECLARE_IOCTL) || ! defined (IOCTL)
# define	DECLARE_IOCTL(name)	EXTERN_IOCTL (name);
# define	IOCTL(name)		__IOCTL (name)
#endif

#if	! defined (DECLARE_CHPOLL) || ! defined (CHPOLL)
# define	DECLARE_CHPOLL(name)	EXTERN_CHPOLL (name);
# define	CHPOLL(name)		__CHPOLL (name)
#endif

#if	! defined (DECLARE_MMAP) || ! defined (MMAP)
# define	DECLARE_MMAP(name)	EXTERN_MMAP (name);
# define	MMAP(name)		__MMAP (name)
#endif

#if	! defined (DECLARE_STRATEGY) || ! defined (STRATEGY)
# define	DECLARE_STRATEGY(name)	EXTERN_STRATEGY (name);
# define	STRATEGY(name)		__STRATEGY (name)
#endif

#if	! defined (DECLARE_PRINT) || ! defined (PRINT)
# define	DECLARE_PRINT(name)	EXTERN_PRINT (name);
# define	PRINT(name)		__PRINT (name)
#endif

#if	! defined (DECLARE_SIZE) || ! defined (SIZE)
# define	DECLARE_SIZE(name)	EXTERN_SIZE (name);
# define	SIZE(name)		__SIZE (name)
#endif

#if	! defined (DECLARE_INTR) || ! defined (INTR)
# define	DECLARE_INTR(name)	EXTERN_INTR (name);
# define	INTR(name)		__INTR (name)
#endif

#if	! defined (DECLARE_INIT) || ! defined (INIT)
# define	DECLARE_INIT(name)	EXTERN_INIT (name);
# define	INIT(name)		__INIT (name)
#endif

#if	! defined (DECLARE_START) || ! defined (START)
# define	DECLARE_START(name)	EXTERN_START (name);
# define	START(name)		__START (name)
#endif

#if	! defined (DECLARE_EXIT) || ! defined (EXIT)
# define	DECLARE_EXIT(name)	EXTERN_EXIT (name);
# define	EXIT(name)		__EXIT (name)
#endif

#if	! defined (DECLARE_HALT) || ! defined (HALT)
# define	DECLARE_HALT(name)	EXTERN_HALT (name);
# define	HALT(name)		__EXIT (halt)
#endif

#ifndef	CDEVSW_ENTRY

typedef struct {
	int	      *	cdev_flag;
	struct streamtab
		      *	cdev_stream;
	ddi_open_t	cdev_open;
	ddi_close_t	cdev_close;
	ddi_read_t	cdev_read;
	ddi_write_t	cdev_write;
	ddi_ioctl_t	cdev_ioctl;
	ddi_chpoll_t	cdev_chpoll;
	ddi_mmap_t	cdev_mmap;
} cdevsw_t;

#define	CDEVSW_ENTRY(fl,op,cl,re,wr,io,ch,mm) \
	{ (fl), 0, (op), (cl), (re), (wr), (io), (ch), (mm) }

#define	STREAMS_ENTRY(name) \
	{ & __DEVFLAG (name), & __INFO (name), 0, 0, 0, 0, 0, 0 }

#define	NULL_CDEVSW() \
	{ 0, 0, 0, 0, 0, 0, 0, 0 }

#endif	/* ! defined (CDEVSW_ENTRY) */

#ifndef	BDEVSW_ENTRY

typedef struct {
	int	      *	bdev_flag;
	ddi_open_t	bdev_open;
	ddi_close_t	bdev_close;
	ddi_strategy_t	bdev_strategy;
	ddi_print_t	bdev_print;
	ddi_size_t	bdev_size;
} bdevsw_t;

#define	BDEVSW_ENTRY(fl,op,cl,st,pr,si) \
	{ (fl), (op), (cl), (st), (pr), (si) }

#define	NULL_BDEVSW() \
	{ 0, 0, 0, 0, 0, 0 }

#endif	/* ! defined (BDEVSW_ENTRY) */


#ifndef	MODSW_ENTRY

typedef	struct {
	int	      *	mod_flag;
	struct streamtab
		      *	mod_stream;
} modsw_t;

#define	MODSW_ENTRY(name)	{ & __DEVFLAG (name), & __INFO (name) }

#define	NULL_MODSW()		{ 0, 0 }

#endif	/* ! defined (MODSW_ENTRY) */


#define	__VECTOR(vec)	__CONCAT (vector, vec)
#define	__PREV(vec)	__CONCAT3 (vector, vec, _prev)


#ifdef	__BORLANDC__

/*
 * Under MS-DOS, we have to avoid disrupting the existing software. That means
 * we have to take care to chain to the appropriate previous handlers; given
 * that Borland C++ has support for this, interrupts under C++ are supported
 * using native facilities rather than a generic trap layer. A generic trap
 * layer should be doing similar things, of course.
 */

typedef	interrupt void (* intthunk_t)	__PROTO	((__ANY_ARGS__));

__EXTERN_C_BEGIN__

void	_chain_intr (intthunk_t _prev_int);	/* Borland C++ library */
void	CHECK_DEFER (void);

__EXTERN_C_END__


typedef	struct {
	int		int_vector;
	intthunk_t	int_thunk;
	intthunk_t    *	int_prev;
} intr_t;

#define	BEGIN_THUNK(vec,mask) \
  __LOCAL__ intthunk_t	__PREV (vec); \
  __LOCAL__ interrupt void __VECTOR (vec) \
	DEF ((__ANY_ARGS__), __ARGS (())) { \
		intmask_t	oldmask = __GET_BASE_MASK (); \
		pl_t		prev_pl; \
		if (__PREV (vec) != 0) \
			(* __PREV (vec)) (); \
		__SET_BASE_MASK (oldmask | (mask)); \
		ddi_cpu_data ()->dc_int_level ++; \
		prev_pl = spltimeout (); \
		__SEND_EOI (vec); \
		__CHEAP_ENABLE_INTS ();

#define	CALL_INTR(vec,name)	(void) __INTR (name) (vec);

#define	END_THUNK(vec) \
		__CHEAP_DISABLE_INTS (); \
		__SET_BASE_MASK (oldmask); \
		splx (prev_pl); \
		if (ddi_cpu_data ()->dc_int_level == 1 && \
		    ddi_cpu_data ()->dc_ipl == plbase) \
			CHECK_DEFER (); \
		ddi_cpu_data ()->dc_int_level --; \
	}

#define	INTR_THUNK(vec)		{ vec, __VECTOR (vec), & __PREV (vec) }

#else

typedef	void 	     (*	intthunk_t)	__PROTO ((__ANY_ARGS__));

typedef	struct {
	int		int_vector;
	intthunk_t	int_thunk;
} intr_t;

#define	BEGIN_THUNK(vec,mask) \
  __LOCAL__ void __VECTOR (vec) \
	DEF ((void), __ARGS (())) { \
		intmask_t	oldmask = __GET_BASE_MASK (); \
		__SET_BASE_MASK (oldmask | (mask)); \
		ddi_cpu_data ()->dc_int_level ++;

#define	CALL_INTR(vec,name)	(void) __INTR (name) (vec);

#define	END_THUNK(vec) \
		ddi_cpu_data ()->dc_int_level --; \
		__SET_BASE_MASK (oldmask); \
	}

#define	INTR_THUNK(vec)		{ vec, __VECTOR (vec) }

#endif


typedef	ddi_init_t	init_t;
typedef	ddi_start_t	start_t;
typedef	ddi_exit_t	exit_t;
typedef	ddi_halt_t	halt_t;

extern	init_t		inittab [];
extern	unsigned int	ninit;

extern	start_t		starttab [];
extern	unsigned int	nstart;

extern	exit_t		exittab [];
extern	unsigned int	nexit;

extern	halt_t		halttab [];
extern	unsigned int	nhalt;

extern	cdevsw_t	cdevsw [];
extern	unsigned int	ncdevsw;

extern	bdevsw_t	bdevsw [];
extern	unsigned int	nbdevsw;

extern	modsw_t		modsw [];
extern	unsigned int	nmodsw;

extern	major_t		_maxmajor;
extern	major_t		_major [];
extern	minor_t		_minor [];

extern	intr_t		inttab [];
extern	unsigned int	nintr;

#endif	/* ! defined (__KERNEL_CONFINFO_H__) */
