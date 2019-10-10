#ifndef	__COMMON__CCOMPAT_H__
#define	__COMMON__CCOMPAT_H__

/*
 * Define some handy things that allow us to work with K&R, ANSI and C++
 * compilers in a way that is at least less painful than not working. This
 * file does mandate an ANSI C pre-processing environment.
 *
 * While ANSI allows us to not define function prototypes, C++ mandates that
 * they exist, and it's a *really* good idea to use them whereever possible.
 *
 * This file also deals with some compiler-specific features that are either
 * in the C or C++ language standards but not always available, and some
 * language extensions that are very widely available, if only because they
 * are part of the C++ standard and have been incorporated into C compilers.
 *
 * This file specifically excludes specifics about target machines and
 * compiler interactions. Such definitions belong in another file.
 */

#include <common/feature.h>


/*
 * There is some complexity on the way __STDC__ is used in practice: the ANSI
 * committee merely says that if __STDC__ is defined and value 1, then the
 * implementation is ISO-conforming.
 *
 * Unfortunately, much existing code uses #ifdef as the only test, which
 * means that some non-conforming compilers which defined __STDC__ as zero
 * caused problems (the Coherent 3.x compiler is one such; the 4.x compiler
 * uses the alternate convention). The #if test is preferable in programs,
 * since in preprocessor tests undefined symbols assume value 0, but still
 * many programs use the alternate form.
 *
 * For compilers with an intermediate status, eg. with an ISO preprocessor,
 * or support for "const" but not prototypes, or prototypes but not "const"
 * we perform individual featurectomies below.
 *
 * A general rule for future extensions: use double-underscores before and
 * after for non-parameterized macros, double-underscores before for macros
 * that take parameters. If this file's definitions are to be used by user-
 * level code, create a header that exports the definitions into the user
 * namespace.
 *
 * Jun '93 - split profile in two sections in anticipation of future growth.
 * We can't assume more than 16-bit arithmetic because of some extremely
 * losing preprocessors out there.
 */

/* __STD_PROFILE__ bits */
#define	__STRINGIZE_M__	0x0001	/* supports ISO C "stringize" */
#define	__PASTE_M__	0x0002	/* support ISO C token-pasting */
#define	__PROTODECL_M__	0x0004	/* supports prototype declarations */
#define	__PROTODEFN_M__	0x0008	/* supports prototype definitions */
#define	__CONST_M__	0x0010	/* supports "const" construct */
#define	__VOLATILE_M__	0x0020	/* supports "volatile" construct */
#define	__VOIDSTAR_M__	0x0040	/* supports "void *" type */

#define	__STD_PROFILE_MASK__	0x007F

/* __MISC_PROFILE__ bits */
#define	__NOTUSED_M__	0x0001	/* allows "not used" warning suppression */
#define	__REGISTER_M__	0x0002	/* requires "register" declaration */
#define	__LINKID_M__	0x0004	/* requires linkage specifier (eg C++) */
#define	__INLINE_M__	0x0008	/* allows inline functions */
#define	__INLINEL_M__	0x0010	/* allows inline functions with loops */
#define	__DOTDOTDOT_M__	0x0020	/* requires (...) rather than () */
#define	__NOMERGESTR_M__ 0x0040	/* cannot merge identical strings */

#define	__MISC_PROFILE_MASK__	0x007F


/*
 * The Standard C language features in one definition for simplicity. This may
 * not be all the bits in __STD_PROFILE_MASK__ once C++ standards start
 * hitting the streets. Normative addenda to ISO C will complicate this, too.
 */

#define	__STDC_M__	(__STD_PROFILE_MASK__)


/*
 * Below, we attempt to determine a configuration suitable for the translator
 * that is working on the current program. Each group of macros attempts to
 * set a preprocessor macro __PROFILE__ with a bit-mask of the features
 * supported by the current translator.
 *
 * This approach has been taken since it considerably simplifies both the
 * task of adding new features to test for and adding new translators. Many
 * other programs intermingle the tasks of determining the translator and
 * defining the responses to that determination; in general, such programs
 * fail to be maintainable when the matrix of features and translators grows
 * larger than about 3x3.
 *
 * Jun '93 - With the split into two parts, we still let people use the single
 * __PROFILE__ definition if their translator's preprocessor can deal with
 * 32-bit arithmetic.
 */

#if  	defined (__PROFILE__)				/* user-overridden */

# if	(1 << 31) == 0
#  error Your preprocessor cannot handle 32-bit arithmetic!
# endif
# if	__PROFILE__ & ~ (__STD_PROFILE_MASK__ | (__MISC_PROFILE_MASK << 16))
#  error __PROFILE__ contains unknown flag bits.
# endif

# define  __STD_PROFILE__	(__PROFILE__ & __STD_PROFILE_MASK__)
# define  __MISC_PROFILE__	((__PROFILE__ >> 16) & __MISC_PROFILE_MASK__)

#elif	defined (__STD_PROFILE__) || defined (__MISC_PROFILE__)

# ifndef __STD_PROFILE__
#  error If you defined __MISC_PROFILE__, you must also define __STD_PROFILE__
# elif   __STD_PROFILE__ & ~ __STD_PROFILE_MASK__
#  error Unknown bits set in __STD_PROFILE__
# endif

# ifndef __MISC_PROFILE__
#  error If you defined __STD_PROFILE__, you must also define __MISC_PROFILE__
# elif   __MISC_PROFILE__ & ~ __MISC_PROFILE_MASK__
#  error Unknown bits set in __MISC_PROFILE__
# endif

#elif	defined (__cplusplus)				/* C++ */

# ifdef	__GNUC__
#  define __STD_PROFILE__	__STDC_M__
#  define __MISC_PROFILE__	(__NOTUSED_M__ | __LINKID_M__ | \
				 __INLINE_M__ | __INLINEL_M__ | \
				 __DOTDOTDOT_2M__)
# else
#  define __STD_PROFILE__	__STDC_M__
#  define __MISC_PROFILE__	(__NOTUSED_M__ | __LINKID_M__ | \
				 __INLINE_M__ | __DOTDOTDOT_M__)
# endif

#elif	__BORLANDC__					/* Borland C */

# if	__BORLANDC__ >= 0x410
#  define __STD_PROFILE__	__STDC_M__
#  define __MISC_PROFILE__	0	/* features restricted to C++ */
# else
#  define __STD_PROFILE__	__STDC_M__
#  define __MISC_PROFILE__	(__NOTUSED_M__ | __INLINE_M__)
# endif

#elif	defined (__GNUC__)				/* GCC w/o C++ */

# define  __STD_PROFILE__	__STDC_M__
# define  __MISC_PROFILE__	(__INLINE_M__ | __INLINEL_M__)

#elif	__STDC__ + 0					/* minimal ANSI C */

# define  __STD_PROFILE__	__STDC_M__
# define  __MISC_PROFILE__	0

#elif	__COHERENT__					/* MWC Coherent */

# define  __STD_PROFILE__	(__STRINGIZE_M__ | __PASTE_M__)
# define  __MISC_PROFILE__	(__REGISTER_M__ | __NOMERGESTR_M__)

#else							/* VANILLA */

# define  __STD_PROFILE__	0
# define  __MISC_PROFILE__	__REGISTER_M__

#endif


/*
 * In the following sections we determine the responses to take on the basis
 * of whether or not each feature/misfeature is supported by the current
 * translator.
 *
 * In cases where the feature requires considerable change to source code,
 * such as prototyping and inline functions, we define both an existence
 * feature-test and a value macro.
 *
 * For the case of inline functions, this is because the function should not
 * appear at all in the souce code unless unlining is supported (and because
 * often a macro may suffice in place, although with less safety).
 *
 * In addition, the tests below always check to see whether a particular
 * symbol is defined already, allowing almost any feature to be turned on or
 * off at will from the command-line. This is useful when testing the
 * characteristics of a new translator, and may often be useful to suppress
 * certain features to aid in debugging.
 */

/*
 * Some old Reiser preprocessors looked for macro-expansions unside quoted
 * strings; this was useful, but dangerous, so ISO C defined a new operator
 * for doing just this job. A tip of the hat to P.J. Plauger for the form of
 * these macros.
 */

#ifndef	__STRING
# if	__STD_PROFILE__ & __STRINGIZE_M__

#  define __STRING(x)	__VALUE(x)
#  define __VALUE(x)	#x

# else

#  define __STRING(x)	"x"

# endif
#endif


/*
 * As above, this is a feature supported in old systems by ill-advised hacks,
 * so ISO C has a special operator just for the job. We define __CONCAT3 ()
 * and __CONCAT4 () as primitives just for fun; it's handy, and nested calls
 * to __CONCAT () are just too messy to be practical.
 */

#ifndef	__CONCAT
# if	__STD_PROFILE__ & __PASTE_M__

#  define __CONCAT(x,y)		x##y
#  define __CONCAT3(x,y,z)	x##y##z
#  define __CONCAT4(a,b,c,d)	a##b##c##d

# else

#  define __CONCAT(x,y)		x/**/y
#  define __CONCAT3(x,y,z)	x/**/y/**/z
#  define __CONCAT4(a,b,c,d)	a/**/b/**/c/**/d

# endif
#endif


/*
 * __USE_PROTO__ is a general test which can be performed in .c files to see
 * whether to use a prototype form or a K&R form, since the two are so
 * different. This has the advantage that some tools which are hard-wired for
 * K&R source code can get confused by macros in the function header, so
 * keeping a real K&R header around can help.
 *
 * __PROTO () is a macro that can be used in header files, since all that
 * differs between K&R and ANSI external definitions is whether the types
 * are present.
 *
 * The difference between the two is important, especially when "lint"-like
 * tools are used. In order to check for consistency between the prototype
 * and K&R-style definitions, it may be necessary to enable prototypes in
 * the header files while suppressing them in the C files.
 */

#ifndef	__PROTO
# if 	__STD_PROFILE__ & __PROTODECL_M__

#  define  __PROTO(p)	p

# else	/* prototypes are not supported */

#  define  __PROTO(p)	()

# endif
# if	__STD_PROFILE__ & __PROTODEFN_M__

#  define	__USE_PROTO__	1

# endif
#endif	/* ! defined (__PROTO) */


/*
 * There are several existing compilers still in use which either do not
 * support the notion of a "const" language element or implement the feature
 * incorrectly with respect to the C standard.
 *
 * For these compilers, we allow the "const" specifier in prototypes, local
 * variables and structure declarations to be suppressed. Note that "const"
 * will never appear in a K&R function header.
 */

#ifndef	__CONST__
# if	__STD_PROFILE__ & __CONST_M__

#  define  __CONST__	const

# else	/* const is not supported */

#  define  __CONST__

# endif
#endif	/* ! defined (__CONST__) */


/*
 * For symmetry with the "const" definition, we provide a wrapper for the
 * "volatile" feature. Note that for some reason "volatile" is available in
 * some compilers that do not implement "const", probably because the feature
 * was defined in simpler terms.
 *
 * For these compilers, we allow the "volatile" specifier in prototypes,
 * local variables, and structure declarations to be suppressed.
 */

#ifndef	__VOLATILE__
# if	__STD_PROFILE__ & __VOLATILE_M__

#  define  __VOLATILE__	volatile

# else	/* const is not supported */

#  define  __VOLATILE__

# endif
#endif	/* ! defined (__VOLATILE__) */


/*
 * Some compilers support the "void" type, but not the semantics of "void *".
 *
 * The following definition is similar to a usage in System V documentation
 * which probably exists for the same reason, except that we use two
 * underscores in ours before and after, where theirs is called _VOID.
 *
 * A word of explanation; what we are doing here is declaring an incomplete
 * type; pointers to this incomplete type should function more-or-less as
 * a pointer to "void" would except for lacking the implicit conversions that
 * are special to "void *", because "void" is an "incomplete type that cannot
 * be completed". Read the standard if you don't know about incomplete types.
 */

#ifndef	__VOID__
# if	__STD_PROFILE__ & __VOIDSTAR_M__

#  define  __VOID__	void

# else	/* void with a pointer is not supported */

#  if	__COHERENT__ && ! __GNUC__	/* stroke a bug in Coherent 'cc' */

#   define  __VOID__		char

#else

typedef	struct __deep_magic__	__void__;
#   define  __VOID__		__void__

#  endif

# endif
#endif	/* ! defined (__VOID__) */


/*
 * In order for some of the useful compiler extensions below to be kept
 * available during a "strict" compile (assuming that the feature-tests above
 * enable their use) then the convention is to prepend compiler-specific
 * keywords with double-underscores.
 *
 * This also serves to document which usages are not ISO C. Note that this
 * may have to change a little for a potential standardized C++.
 */

#if	__STDC__ + 0

# define   __NON_ISO(k)		__CONCAT (__, k)

#else

# define   __NON_ISO(k)		k

#endif


/*
 * A feature defined as part of the C++ language that also exists in many
 * C implementations is the ability to suppress "argument not used" warnings
 * in some cases by omitting the name of the variable in the function
 * prototype and merely giving the type.
 *
 * This feature is common in type-checking compilers since the checking of
 * function pointer arguments and other extra checks mean that functions
 * must be declared with unused arguments to match the shape of some function
 * pointer.
 *
 * Of course, it is desirable to leave the original name of the variable in
 * the same place for documentation purposes, often commented out, but this
 * usage chokes some compilers. It seems preferable use the following
 * definition to explicitly state the intention, even in cases where the
 * compiler generates spurious warnings.
 */

#ifndef	__NOTUSED
# if	__MISC_PROFILE__ & __NOTUSED_M__

#  define  __NOTUSED(name)	/* name */

# else	/* does not understand name suppression */

#  define  __NOTUSED(name)	__CONCAT (unused_, name)

# endif
#endif	/* ! defined (__NOTUSED) */


/*
 * Most modern compilers perform their own register allocation and ignore
 * the "register" directive from K&R C. Such compilers usually have debugging
 * tools that know how to deal with variables that spend at least part of
 * their lifetime in a machine register (or at worst, the option to suppress
 * the auto-register allocation).
 *
 * For compilers that require a register declaration for a variable to be
 * placed in a machine register, often it is desirable to suppress the use
 * of registers when debugging.
 */

#ifndef	__REGISTER__
# if	__MISC_PROFILE__ & __REGISTER_M__

#  define  __REGISTER__		register

# else

#  define  __REGISTER__

# endif
#endif	/* ! defined (__REGISTER__) */


/*
 * Some compilers for C-like languages such as C++, Objective-C or even
 * conceivably Pascal/Modula-2/Fortran support cross-language linkage.
 *
 * The standard way of doing this within the C family is to use a special
 * form of "extern" which names the language a function is implemented in.
 * Functions which are implemented in C in a library should be declared as
 * such in the exported header.
 *
 * Currently, this is most important for C++.
 */

#ifndef	__EXTERN_C__
# if	__MISC_PROFILE__ & __LINKID_M__

#  define  __EXTERN_C__		extern "C"
#  define  __EXTERN_C_BEGIN__	__EXTERN_C__ {
#  define  __EXTERN_C_END__	}

# else	/* this is being compiled by a C compiler */

#  define  __EXTERN_C__
#  define  __EXTERN_C_BEGIN__
#  define  __EXTERN_C_END__

# endif
#endif	/* ! defined (__EXTERN_C__) */


/*
 * All C++ compilers and many C compilers support the notion of "inline
 * functions" as an alternative to macros that (i) can be used to wrap up
 * casts so they are only used in safe contexts, (ii) can be used as an
 * alternative to macros that allow arguments with side-effects.
 *
 * This comes in two strengths: can inline anything, and can inline anything
 * that does not contain a loop. GNU C has extra strength, can inline tail-
 * recursive inline function, but that facility is not sufficiently widespread
 * to be useful as yet.
 *
 * The question is, what should the default setting of the client tests be?
 * #if ! __NO_INLINE__ is a double-negative, so don't use that. The
 * possibility of defining __INLINE__ as "static" so that inline functions
 * appear in the module separately breakpointable from other modules is a
 * desirable facility (assuming the debug namespace is separate from the
 * linkage namespace, likely in a system sophisticated enough to support
 * inlining). Furthermore, be aware of any interactions with the __LOCAL__
 * macro defined in <common/xdebug.h>
 */

#ifndef __INLINE__
# if	__MISC_PROFILE__ & __INLINE_M__

#  define __INLINE__		__NON_ISO (inline)
#  define __USE_INLINE__	1

# else

#  define __INLINE__

# endif
#endif

#ifndef __INLINEL__
# if	__MISC_PROFILE__ & __INLINEL_M__

#  define __INLINEL__		__NON_ISO (inline)
#  define __USE_INLINEL__	1

# else

#  define __INLINEL__

# endif
#endif


/*
 * One particular incompatibility between ANSI C and C++ code exists in the
 * way in which prototypes which do not specify any types at all are handled.
 * Under C++, the () form is used to imply (void), since such declarations
 * are extremely common and because early versions of the C++ translators did
 * not allow any declarations in the argument lists of constructors or
 * destructors, not even void, so this form was used to syntactically imply
 * (void).
 *
 * The ANSI C committe declared that a prototype of the form
 *	extern char * malloc ();
 * said nothing whatsoever about the types of it's arguments, since such
 * declarations were extremely common in K&R C code, and doing anything else
 * would gratuitously require considerable rewriting.
 *
 * Unfortunately, the ANSI C committee decided that the special form "..." to
 * introduce variable arguments was not valid unless preceeded by a regular
 * argument type declaration, so that there is no way of being unambiguous
 * that will compile under both transators.
 *
 *		ANSI		C++
 *
 * ()		(...)		(void)		ambiguous
 *
 * (void)	(void)		(void)		unambiguous
 *
 * (...)	error		(...)		thanks, X3J11
 *
 * Use the preprocessor symbol __ANY_ARGS__ in this context to expand to
 * whatever the current translator needs to see for it to make no assumptions
 * about the number and type of any function arguments.
 */

#ifndef	__ANY_ARGS__
# if	__MISC_PROFILE__ & __DOTDOTDOT__

#  define  __ANY_ARGS__		...

# else

#  define  __ANY_ARGS__

# endif
#endif	/* ! defined (__ANY_ARGS__) */


/*
 * String constants have been mightily botched in C, largely because K&R C
 * always treated strings as unique, writeable data. The introduction of the
 * 'const' qualifier should have made things simpler by allowing compilers to
 * figure out whether a string is being used in a 'const' or non-'const'
 * way. However, the precedent of just making all strings which are not being
 * used as character array initializers into shareable constants has already
 * been established.
 *
 * This has an effect on complex macros, because a string in a macro may (in
 * a K&R environment or an ISO environment with certain switch settings)
 * cause a host of anonymous, identical string constants to be created, which
 * have to be suppressed by using a static declaration. On the other hand,
 * this does worse than a compiler which does share strings, because if the
 * macro is not actually used then the data space is still taken up.
 *
 * Here we give a hint to those macros that care.
 */

#ifndef	_SHARED_STRINGS
# if	(__MISC_PROFILE__ & __NOMERGESTR_M__) == 0

#  define	_SHARED_STRINGS		1

# endif
#endif	/* ! defined (_SHARED_STRINGS) */


/*
 * It is frequently convenient to be able to map a pointer to a member of a
 * structure back to a pointer to the parent structure. Unfortunately, while
 * ISO C mandates that conversions between pointer types and will not cause
 * loss of information, there are few things that can portably be done with
 * such a converted pointer. In particular, the easy way of mapping back from
 * a structure member pointer to the containing structure performs address
 * arithmetic on character pointers using offsetof (). It appears that this
 * puts us in the realm of implementation-defined behaviour, except when the
 * offsetof () the member is 0.
 */

#ifndef	__DOWNCAST

# define	__DOWNCAST(str,mem,ptomem) \
			((str *) ((char *) (ptomem) - \
					 offsetof (str, mem)))

#endif


/*
 * This is a minor K&R compatibility issue: certain K&R compilers reject the
 * ISO C idiom of enclosing a macro name in parentheses to suppress macro
 * expansion when this idiom is used in function declarations. To get around
 * this, we can use the ISO preprocessor in a clumsy fashion by providing an
 * identity macro to provide the same overall effect of making the name we
 * wish to suppress expansion for not be immediately followed by a left
 * parenthesis (it will be followed by a parenthesis eventually, but since the
 * proprocessor won't revisit the text it has seen before the expansion of
 * the identity macro we get the behaviour we want).
 */

#define	__ARGS(x)	x


/*
 * The POSIX.1 standard discusses a special namespace issue; how can standard
 * structures be portably extended, given that the structure tags are in the
 * user namespace. For structures which have members with regular names and
 * which are likely to be extended, the POSIX.1 standard deals with this by
 * implicitly reserving all names of that form (something which further
 * underscores the restrictions on standard headers not including each other).
 *
 * However, for situations where we wish to extend a structure not covered by
 * the namespace reservation rules, or we wish to name a member according to
 * some other usage, we must take care to not define the member such that it
 * might conflict with some macro name which the user is permitted to define.
 * See POSIX.1 B.2.7.2 for discussion of this point.
 *
 * The following definition can be used to wrap the definition of structure
 * member names such that those names will not conflict with user macros if
 * _POSIX_SOURCE is defined. This form can be used in references to the member
 * name which may be encapsulated in macros so that there is no loss of
 * functionality or alteration of behaviour when _POSIX_SOURCE is used.
 */

#if	_POSIX_SOURCE
# define	__NON_POSIX(name)	__CONCAT (_, name)
#else
# define	__NON_POSIX(name)	name
#endif


/*
 * GCC has a special feature for declaring functions as not ever returning.
 */

#if	__GNUC__
# define	__NO_RETURN__	volatile
#else
# define	__NO_RETURN__
#endif


/*
 * Undefine all our internal symbols... why pollute the namespace?
 */

#undef	__STRINGIZE_M__
#undef	__PASTE_M__
#undef	__PROTODECL_M__
#undef	__PROTODEFN_M__
#undef	__CONST_M__
#undef	__VOLATILE_M__
#undef	__VOIDSTAR_M__
#undef	__STD_PROFILE_MASK__

#undef	__NOTUSED_M__
#undef	__REGISTER_M__
#undef	__LINKID_M__
#undef	__INLINE_M__
#undef	__INLINEL_M__
#undef	__DOTDOTDOT_M__
#undef	__NOMERGESTR_M__
#undef	__MISC_PROFILE_MASK__

#undef	__STDC_M__
#undef	__PROFILE__
#undef	__STD_PROFILE__
#undef	__MISC_PROFILE__

#endif	/* ! defined (__COMMON__CCOMPAT_H__) */
