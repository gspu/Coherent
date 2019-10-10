/*
 * Conventions to represent high level language types
 * with the loader type notation.
 * The loader comprehends only the syntax of this notation;
 * semantic meaning is imposed by these conventions.
 *
 * A type consists generally of a numeric item identifying the class
 * (see below), a numeric item specifying the number of bits in the
 * storage representation, and various other numeric, string and type items
 * conveying more detailed information.
 * A type may be more or less strict by carrying more or less detail
 * (i.e. a type may be generalized by truncation).
 * In a recursively defined type (e.g. a struct with an element pointing
 * to such a struct) such a recursive reference to a type is represented
 * by a negative numeric item, indicating how many nested types to back up
 * to find the intended type.
 *
 * Alternatively, a type may be simply a name string,
 * but is then language dependent.
 */

#ifndef	LTYPE_H
#define	LTYPE_H	LTYPE_H

#include <reloc.h>

typedef	enum	ltype_t	{
		Void,
		Natural,
		Integer,
		Rational,
		Pointer,
		Array,
		Record,
		Union,
		Proc,
		Set,
		File
} ltype_t;

/*
 * void
					The return type of procedures.
 * natural	bits	range	base	name
					Natural numbers are unsigned
					subranges, enumerations, etc.
 * integer	bits	range	base	name
					Integers are signed quantities.
 * rational	bits	format	name
					Rationals cover all floating point
					representations,
					distinguished by format.
 * pointer	bits	ref_t	name
 * array	bits	elem_t	index_t	name
					The element type may be another array;
					the index type should be ordinal.
 * record	bits	nfield	field_t	...	name
 * union	bits	nmemb	memb_t	...	name
 * proc		ANYt	ret_t	nargs	arg_t	...	name
					The number of bits in a function is
					not significant.
 * set		bits	elem_t	name
					A concession to Pascal.
 * file		ANYt	elem_t	name
					Another concession.
 */

/*
 * A non-trivial example:
 *	struct	ref_t	{
 *		struct	ref_t	*next;
 *		char		*reftype;
 * 	};
 *	struct	sym_t	{
 *		struct	sym_t	*next;
 *		char		*symname;
 *		union	{
 *			char		*def;
 *			struct	ref_t	*ref;
 *		}		symtype;
 *	};
 *
 *	struct	sym_t	*symtable[64];
 *
 * Indicating a type within a type by a new level of indentation,
 * the type of symtable is:
 *	Array
 *	64*16
 *	TYPEt	Pointer
 *		16
 *		TYPEt	Record
 *			16+16+16
 *			3
 *			TYPEt	Pointer
 *				16
 *				TYPEt	-2
 *			TYPEt	Pointer
 *				16
 *				TYPEt	Integer
 *					8
 *					256
 *					-128
 *			TYPEt	Union
 *				16
 *				2
 * 				TYPEt	Pointer
 *					16
 *					TYPEt	Integer
 *						8
 *						256
 *						-128
 *				TYPEt	Pointer
 *					16
 *					TYPEt	Record
 *						16+16
 *						2
 *						TYPEt	Pointer
 *							16
 *							TYPEt	Integer
 *								8
 *								256
 *								-128
 *						TYPEt	Pointer
 *							16
 *							TYPEt	-2
 *						"struct ref_t"
 *			"struct sym_t"
 *	TYPEt	Natural
 *		16
 *		64
 *		0
 *
 * Note that only the named structs carry type names (the union was
 * not given a type name), and that the array index type carries
 * the bounds of the array (and is by convention 16 bits).
 */

#endif
