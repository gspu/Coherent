.ds TL Documentation
.NH Documentation
.PP
Full documentation for
.B gcc
is supplied by the Free Software Foundation.
This release includes it in compressed form.
When you install GNU C onto your COHERENT system, the installation script
will check how much space is free on your disk, and asks you if you want
the documentation to be uncompressed.
Documentation is installed into directory
.BR /usr/local/docs .
The following describes the contents of each file, and gives hints as
to where to find information on a given topic.
.PP
The documentation for
.B gcc
consists of approximately 24 files that are supplied in the subdirectory
.B /usr/local/docs
of this release.
The following introduces the contents of each file, to help you
find the information you seek about this product.
.IP \fBcpp.info\fR
This is the first of four files that together make up the manual on
.BR cpp ,
the preprocessor for the GNU C compiler.
It gives a copyright statement and a ``tag table,'' which gives each
subheading in the manual plus its offset (in characters) within the entire
concatenated manual.
.IP \fBcpp.info-1\fR
The second file of the manual for
.BR cpp .
It contains the following sections (note that the degree of
indentation indicates the level of subheading):
.sp \n(pDu
.nf
The C Preprocessor
	Transformations Made Globally
	Preprocessor Commands
	Header Files
		Uses of Header Files
		The `#include' Command
		How `#include' Works
		Once-Only Include Files
	Inheritance and Header Files
	Macros
		Simple Macros
		Macros with Arguments
		Predefined Macros
			Standard Predefined Macros
			Nonstandard Predefined Macros
		Stringification
		Concatenation
		Undefining Macros
		Redefining Macros
		Pitfalls and Subtleties of Macros
			Improperly Nested Constructs
			Unintended Grouping of Arithmetic
.fi
.IP \fBcpp.info-2\fR
The third file of the
.B cpp
manual.
It contains the following sections:
.sp \n(pDu
.nf
			Swallowing the Semicolon
			Duplication of Side Effects
			Self-Referential Macros
			Separate Expansion of Macro Arguments
			Cascaded Use of Macros
		Newlines in Macro Arguments
		Why Conditionals are Used
		Syntax of Conditionals
			The `#if' Command
			The `#else' Command
			The `#elif' Command
		Keeping Deleted Code for Future Reference
		Conditionals and Macros
		Assertions
		The `#error' and `#warning' Commands
	Combining Source Files
	Miscellaneous Preprocessor Commands
	C Preprocessor Output
	Invoking the C Preprocessor
Concept Index
.fi
.IP \fBcpp.info-3\fR
The last file of the manual for
.BR cpp .
It contains an index to the entire manual.
.IP \fBgcc.info\fR
This is the first of 20 files that together make up the manual for
.BR gcc .
It gives the the ``tag table'' for the \fBgcc\fR manual.
.IP \fBgcc.info-1\fR
The second file of the
.B gcc
manual.
It contains the opening sections of the manual.
.sp \n(pDu
Note that the section
.B "GNU GENERAL PUBLIC LICENSE"
describes the license by which this product is distributed to you by
Mark Williams Company, and by which you may distribute it to other people.
.sp \n(pDu
Section \fBProtect Your Freedom \(em Fight ``Look And Feel''\fR
is a political statement.
.\" In the words of Abraham Lincoln, ``Those who like this sort of thing
.\" will find this to be the sort of thing they like.''
.sp \n(pDu
Section
.B "Option Summary"
summarizes the hundreds of switches and options that are available with
.BR gcc ;
you should scan it, at least, to become aware of what you can do with
this compiler.
.sp \n(pDu
.nf
Introduction
GNU GENERAL PUBLIC LICENSE
	Preamble
	How to Apply These Terms to Your New Programs
Contributors to GNU CC
Protect Your Freedom--Fight "Look And Feel"
GNU CC Command Options
	Option Summary
	Options Controlling the Kind of Output
.fi
.IP \fBgcc.info-2\fR
The third file of the manual for
.BR gcc .
It contains the following sections:
.sp \n(pDu
.nf
	Options Controlling Dialect
	Options to Request or Suppress Warnings
	Options for Debugging Your Program or GNU CC
	Options That Control Optimization
	Options Controlling the Preprocessor
	Passing Options to the Assembler
.fi
.IP \fBgcc.info-3\fR
The fourth file of the manual for
.BR gcc .
It contains the following sections:
.sp \n(pDu
.nf
	Options for Linking
	Options for Directory Search
	Specifying Target Machine and Compiler Version
	Specifying Hardware Models and Configurations
		M680x0 Options
		VAX Options
		SPARC Options
		Convex Options
		AMD29K Options
		M88K Options
		IBM RS/6000 Options
		IBM RT Options
		MIPS Options
		Intel 386 Options
		HPPA Options
		Intel 960 Options
		DEC Alpha Options
		Options for System V
	Options for Code Generation Conventions
.fi
.IP \fBgcc.info-4\fR
The fifth file of the manual for
.BR gcc .
It contains the following sections.
Note that the section
.B "Installing GNU CC"
and its subsections
are superceded by the directions given earlier in these notes, at least
for installation onto the \*(CO system.
.sp \n(pDu
.nf
	Environment Variables Affecting GNU CC
	Running Protoize
Installing GNU CC
	Compilation in a Separate Directory
	Building and Installing a Cross-Compiler
	Installing GNU CC on the HP Precision Architecture
	Installing GNU CC on the Sun
	Installing GNU CC on the 3b1
.fi
.IP \fBgcc.info-5\fR
The sixth file of the manual for
.BR gcc .
It contains the following sections.
Note that all sections on installation are superceded by the
directions given earlier in these notes, at least for installation
under the \*(CO system:
.sp \n(pDu
.nf
	Installing GNU CC on Unos
	Installing GNU CC on VMS
	Installing GNU CC on the WE32K
	Installing GNU CC on the MIPS
GNU Extensions to the C Language
	Statements and Declarations within Expressions
	Locally Declared Labels
	Labels as Values
	Nested Functions
	Naming an Expression's Type
	Referring to a Type with `typeof'
	Generalized Lvalues
	Conditional Expressions with Omitted Operands
	Double-Word Integers
	Arrays of Length Zero
	Arrays of Variable Length
	Macros with Variable Numbers of Arguments
	Non-Lvalue Arrays May Have Subscripts
	Arithmetic on `void'- and Function-Pointers
	Non-Constant Initializers
	Constructor Expressions
	Labeled Elements in Initializers
	Case Ranges
	Cast to a Union Type
.fi
.IP \fBgcc.info-6\fR
The seventh file of the manual for
.BR gcc .
It contains the following sections:
.sp \n(pDu
.nf
	Declaring Attributes of Functions
	Prototypes and Old-Style Function Definitions
	Dollar Signs in Identifier Names
	The Character ESC in Constants
	Inquiring on Alignment of Types or Variables
	Specifying Attributes of Variables
	An Inline Function is As Fast As a Macro
	Assembler Instructions with C Expression Operands
	Controlling Names Used in Assembler Code
	Variables in Specified Registers
		Defining Global Register Variables
		Specifying Registers for Local Variables
	Alternate Keywords
	Incomplete `enum' Types
Known Causes of Trouble with GNU CC
	Actual Bugs We Haven't Fixed Yet
	Installation Problems
	Cross-Compiler Problems
.fi
.IP \fBgcc.info-7\fR
The eighth file of the manual for
.BR gcc .
It contains the following sections:
.sp \n(pDu
.nf
	Interoperation
	Incompatibilities of GNU CC
	Disappointments and Misunderstandings
	Caveats of using `protoize'
	Certain Changes We Don't Want to Make
Reporting Bugs
	Have You Found a Bug?
	Where to Report Bugs
	How to Report Bugs
.fi
.IP \fBgcc.info-8\fR
The ninth file of the manual for
.BR gcc .
It contains the following sections:
.sp \n(pDu
.nf
	Sending Patches for GNU CC
How To Get Help with GNU CC
Using GNU CC on VMS
	Include Files and VMS
	Global Declarations and VMS
	Other VMS Issues
GNU CC and Portability
Interfacing to GNU CC Output
Passes and Files of the Compiler
RTL Representation
	RTL Object Types
.fi
.IP \fBgcc.info-9\fR
The tenth file of the manual for
.BR gcc .
It contains the following sections:
.sp \n(pDu
.nf
	Access to Operands
	Flags in an RTL Expression
	Machine Modes
	Constant Expression Types
	Registers and Memory
	RTL Expressions for Arithmetic
	Comparison Operations
	Bit Fields
.fi
.IP \fBgcc.info-10\fR
The 11th file of the manual for
.BR gcc .
It contains the following sections:
.sp \n(pDu
.nf
	Conversions
	Declarations
	Side Effect Expressions
	Embedded Side-Effects on Addresses
	Assembler Instructions as Expressions
	Insns
	RTL Representation of Function-Call Insns
	Structure Sharing Assumptions
Machine Descriptions
.fi
.IP \fBgcc.info-11\fR
The 12th file of the manual for
.BR gcc .
It contains the following sections:
.sp \n(pDu
.nf
	Everything about Instruction Patterns
	Example of `define_insn'
	RTL Template for Generating and Recognizing Insns
	Output Templates and Operand Substitution
	C Statements for Generating Assembler Output
	Operand Constraints
		Simple Constraints
		Multiple Alternative Constraints
		Register Class Preferences
		Constraint Modifier Characters
		Not Using Constraints
.fi
.IP \fBgcc.info-12\fR
The 13th file of the manual for
.BR gcc .
It contains the following sections:
.sp \n(pDu
.nf
	Standard Names for Patterns Used in Generation
	When the Order of Patterns Matters
	Interdependence of Patterns
	Defining Jump Instruction Patterns
	Canonicalization of Instructions
	Defining Machine-Specific Peephole Optimizers
.fi
.IP \fBgcc.info-13\fR
The 14th file of the manual for
.BR gcc .
It contains the following sections:
.sp \n(pDu
.nf
	Defining RTL Sequences for Code Generation
	Splitting Instructions into Multiple Instructions
	Instruction Attributes
		Defining Attributes and their Values
		Attribute Expressions
		Assigning Attribute Values to Insns
		Example of Attribute Specifications
		Computing the Length of an Insn
		Constant Attributes
		Delay Slot Scheduling
		Specifying Function Units
Target Description Macros
.fi
.IP \fBgcc.info-14\fR
The 15th file of the manual for
.BR gcc .
It contains the following sections:
.sp \n(pDu
.nf
	Controlling the Compilation Driver, `gcc'
	Run-time Target Specification
	Storage Layout
	Layout of Source Language Data Types
	Register Usage
		Basic Characteristics of Registers
		Order of Allocation of Registers
		How Values Fit in Registers
		Handling Leaf Functions
		Registers That Form a Stack
		Obsolete Macros for Controlling Register Usage
.fi
.IP \fBgcc.info-15\fR
The 16th file of the manual for
.BR gcc .
It contains the following sections:
.sp \n(pDu
.nf
	Register Classes
	Describing Stack Layout and Calling Conventions
		Basic Stack Layout
		Registers That Address the Stack Frame
		Eliminating Frame Pointer and Arg Pointer
		Passing Function Arguments on the Stack
		Passing Arguments in Registers
		How Scalar Function Values Are Returned
		How Large Values Are Returned
		Caller-Saves Register Allocation
.fi
.IP \fBgcc.info-16\fR
The 17th file of the manual for
.BR gcc .
It contains the following sections:
.sp \n(pDu
.nf
		Function Entry and Exit
		Generating Code for Profiling
	Implementing the Varargs Macros
	Trampolines for Nested Functions
	Implicit Calls to Library Routines
	Addressing Modes
	Condition Code Status
.fi
.IP \fBgcc.info-17\fR
The 18th file of the manual for
.BR gcc .
It contains the following sections:
.sp \n(pDu
.nf
	Describing Relative Costs of Operations
	Dividing the Output into Sections (Texts, Data, ...)
	Position Independent Code
	Defining the Output Assembler Language
		The Overall Framework of an Assembler File
		Output of Data
		Output of Uninitialized Variables
		Output and Generation of Labels
		How Initialization Functions Are Handled
		Macros Controlling Initialization Routines
.fi
.IP \fBgcc.info-18\fR
The 19th file of the manual for
.BR gcc .
It contains the following sections:
.sp \n(pDu
.nf
		Output of Assembler Instructions
		Output of Dispatch Tables
		Assembler Commands for Alignment
	Controlling Debugging Information Format
		Macros Affecting All Debugging Formats
		Specific Options for DBX Output
		Open-Ended Hooks for DBX Format
		File Names in DBX Format
		Macros for SDB and DWARF Output
	Cross Compilation and Floating Point Format
	Miscellaneous Parameters
The Configuration File
.fi
.IP \fBgcc.info-19\fR
The 20th, and last, file of the manual for
.BR gcc .
It contains the index to the entire
.B gcc
manual.
