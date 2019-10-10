/*---------------------------------------------------------------------------

  match.c

  The match() routine recursively compares a string to a "pattern" (regular
  expression), returning TRUE if a match is found or FALSE if not.  This
  version is specifically for use with unzip.c:  as did the previous match()
  from SEA, it leaves the case (upper, lower, or mixed) of the string alone,
  but converts any uppercase characters in the pattern to lowercase if indi-
  cated by the global var pInfo->lcflag (which is to say, string is assumed
  to have been converted to lowercase already, if such was necessary).

  ---------------------------------------------------------------------------*/


#ifdef ZIPINFO
#  undef ZIPINFO   /* make certain there is only one version of match.o */
#endif /* ZIPINFO */
#include "unzip.h"

static int  matche              __((register char *p, register char *t));
static int  matche_after_star   __((register char *p, register char *t));

/* #include "filmatch.h": */
#ifndef BOOLEAN
#  define BOOLEAN short int      /* v1.2 made it short */
#endif

/* match defines */
#define MATCH_PATTERN  6    /* bad pattern */
#define MATCH_LITERAL  5    /* match failure on literal match */
#define MATCH_RANGE    4    /* match failure on [..] construct */
#define MATCH_ABORT    3    /* premature end of text string */
#define MATCH_END      2    /* premature end of pattern string */
#define MATCH_VALID    1    /* valid match */

/* pattern defines */
#define PATTERN_VALID  0    /* valid pattern */
#define PATTERN_ESC   -1    /* literal escape at end of pattern */
#define PATTERN_RANGE -2    /* malformed range in [..] construct */
#define PATTERN_CLOSE -3    /* no end bracket in [..] construct */
#define PATTERN_EMPTY -4    /* [..] contstruct is empty */

/*----------------------------------------------------------------------------
*
*  Match the pattern PATTERN against the string TEXT;
*
*       match() returns TRUE if pattern matches, FALSE otherwise.
*       matche() returns MATCH_VALID if pattern matches, or an errorcode
*           as follows otherwise:
*
*            MATCH_PATTERN  - bad pattern
*            MATCH_RANGE    - match failure on [..] construct
*            MATCH_ABORT    - premature end of text string
*            MATCH_END      - premature end of pattern string
*            MATCH_VALID    - valid match
*
*
*  A match means the entire string TEXT is used up in matching.
*
*  In the pattern string:
*       `*' matches any sequence of characters (zero or more)
*       `?' matches any character
*       [SET] matches any character in the specified set,
*       [!SET] or [^SET] matches any character not in the specified set.
*
*  A set is composed of characters or ranges; a range looks like
*  character hyphen character (as in 0-9 or A-Z).  [0-9a-zA-Z_] is the
*  minimal set of characters allowed in the [..] pattern construct.
*  Other characters are allowed (ie. 8 bit characters) if your system
*  will support them.
*
*  To suppress the special syntactic significance of any of `[]*?!^-\',
*  in a [..] construct and match the character exactly, precede it
*  with a `\'.
*
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
*
*  Match the pattern PATTERN against the string TEXT;
*
*  returns MATCH_VALID if pattern matches, or an errorcode as follows
*  otherwise:
*
*            MATCH_PATTERN  - bad pattern
*            MATCH_RANGE    - match failure on [..] construct
*            MATCH_ABORT    - premature end of text string
*            MATCH_END      - premature end of pattern string
*            MATCH_VALID    - valid match
*
*
*  A match means the entire string TEXT is used up in matching.
*
*  In the pattern string:
*       `*' matches any sequence of characters (zero or more)
*       `?' matches any character
*       [SET] matches any character in the specified set,
*       [!SET] or [^SET] matches any character not in the specified set.
*       \ is allowed within a set to escape a character like ']' or '-'
*
*  A set is composed of characters or ranges; a range looks like
*  character hyphen character (as in 0-9 or A-Z).  [0-9a-zA-Z_] is the
*  minimal set of characters allowed in the [..] pattern construct.
*  Other characters are allowed (ie. 8 bit characters) if your system
*  will support them.
*
*  To suppress the special syntactic significance of any of `[]*?!^-\',
*  within a [..] construct and match the character exactly, precede it
*  with a `\'.
*
----------------------------------------------------------------------------*/

static int matche(p, t)
register char *p;
register char *t;
{
    register char range_start, range_end;  /* start and end in range */

    BOOLEAN invert;             /* is this [..] or [!..] */
    BOOLEAN member_match;       /* have I matched the [..] construct? */
    BOOLEAN loop;               /* should I terminate? */

    for (;  *p;  p++, t++) {

        /* if this is the end of the text then this is the end of the match */
        if (!*t)
            return ((*p == '*') && (*++p == '\0'))?  MATCH_VALID : MATCH_ABORT;

        /* determine and react to pattern type */
        switch (*p) {

            /* single any character match */
            case '?':
                break;

            /* multiple any character match */
            case '*':
                return matche_after_star (p, t);

            /* [..] construct, single member/exclusion character match */
            case '[': {

                /* move to beginning of range */
                p++;

                /* check if this is a member match or exclusion match */
                invert = FALSE;
                if ((*p == '!') || (*p == '^')) {
                    invert = TRUE;
                    p++;
                }

                /* if closing bracket here or at range start then we have a
                   malformed pattern */
                if (*p == ']')
                    return MATCH_PATTERN;

                member_match = FALSE;
                loop = TRUE;

                while (loop) {

                    /* if end of construct then loop is done */
                    if (*p == ']') {
                        loop = FALSE;
                        continue;
                    }

                    /* matching a '!', '^', '-', '\' or a ']' */
                    if (*p == '\\')
                        range_start = range_end = *++p;
                    else
                        range_start = range_end = *p;

                    /* if end of pattern then bad pattern (Missing ']') */
                    if (!*p)
                        return MATCH_PATTERN;

                    /* check for range bar */
                    if (*++p == '-') {

                        /* get the range end */
                        range_end = *++p;

                        /* if end of pattern or construct then bad pattern */
                        if ((range_end == '\0') || (range_end == ']'))
                            return MATCH_PATTERN;

                        /* special character range end */
                        if (range_end == '\\') {
                            range_end = *++p;

                            /* if end of text then we have a bad pattern */
                            if (!range_end)
                                return MATCH_PATTERN;
                        }

                        /* move just beyond this range */
                        p++;
                    }

                    /* if the text character is in range then match found.
                     * make sure the range letters have the proper
                     * relationship to one another before comparison
                     */
                    if (range_start < range_end) {
                        if ((*t >= range_start) && (*t <= range_end)) {
                            member_match = TRUE;
                            loop = FALSE;
                        }
                    } else {
                        if ((*t >= range_end) && (*t <= range_start)) {
                            member_match = TRUE;
                            loop = FALSE;
                        }
                    }
                }

                /* if there was a match in an exclusion set then no match */
                /* if there was no match in a member set then no match */
                if ((invert && member_match) ||
                   !(invert || member_match))
                    return MATCH_RANGE;

                /* if this is not an exclusion then skip the rest of the [...]
                    construct that already matched. */
                if (member_match) {
                    while (*p != ']') {

                        /* bad pattern (Missing ']') */
                        if (!*p)
                            return MATCH_PATTERN;

                        /* skip exact match */
                        if (*p == '\\') {
                            p++;

                            /* if end of text then we have a bad pattern */
                            if (!*p)
                                return MATCH_PATTERN;
                        }

                        /* move to next pattern char */
                        p++;
                    }
                }

                break;
            }  /* switch '[' */

            /* must match this character exactly */
            default:
#ifdef OLDSTUFF
                if (*p != *t)
#else /* !OLDSTUFF */
                /* do it like arcmatch() (old unzip) did it (v1.2) */
                if (*t != (char) ((pInfo->lcflag && isupper((int)(*p)))?
                    tolower((int)(*p)) : *p))
#endif /* ?OLDSTUFF */
                    return MATCH_LITERAL;

        }  /* switch */
    }  /* for */

        /* if end of text not reached then the pattern fails */
    if (*t)
        return MATCH_END;
    else
        return MATCH_VALID;
}


/*----------------------------------------------------------------------------
*
* recursively call matche() with final segment of PATTERN and of TEXT.
*
----------------------------------------------------------------------------*/

static int matche_after_star (p,t)
register char *p;
register char *t;
{
    register int match = 0;
    register int nextp;

    /* pass over existing ? and * in pattern */
    while ((*p == '?') || (*p == '*')) {

        /* take one char for each ? and +; if end of text then no match */
        if ((*p == '?') && (!*t++))
                return MATCH_ABORT;

        /* move to next char in pattern */
        p++;
    }

    /* if end of pattern we have matched regardless of text left */
    if (!*p)
        return MATCH_VALID;

    /* get the next character to match which must be a literal or '[' */
    nextp = *p;

    /* Continue until we run out of text or definite result seen */
    do {
        /* a precondition for matching is that the next character
         * in the pattern match the next character in the text or that
         * the next pattern char is the beginning of a range.  Increment
         * text pointer as we go here.
         */
        if ((nextp == *t) || (nextp == '['))
            match = matche(p, t);

        /* if the end of text is reached then no match */
        if (!*t++)
            match = MATCH_ABORT;

    } while ((match != MATCH_VALID) &&
             (match != MATCH_ABORT) &&
             (match != MATCH_PATTERN));

    /* return result */
    return match;
}


/*----------------------------------------------------------------------------
*
* match() is a shell to matche() to return only BOOLEAN values.
*
----------------------------------------------------------------------------*/

int match(string,pattern)
char *string;
char *pattern;
{
    int error_type;
    error_type = matche(pattern,string);
    return (error_type == MATCH_VALID ) ? TRUE : FALSE;
}


#ifdef TEST_MATCH

/*----------------------------------------------------------------------------
*
* Return TRUE if PATTERN has any special wildcard characters
*
----------------------------------------------------------------------------*/

BOOLEAN is_pattern (char *pattern);

/*----------------------------------------------------------------------------
*
* Return TRUE if PATTERN has is a well formed regular expression according
* to the above syntax
*
* error_type is a return code based on the type of pattern error.  Zero is
* returned in error_type if the pattern is a valid one.  error_type return
* values are as follows:
*
*   PATTERN_VALID - pattern is well formed
*   PATTERN_RANGE - [..] construct has a no end range in a '-' pair (ie [a-])
*   PATTERN_CLOSE - [..] construct has no end bracket (ie [abc-g )
*   PATTERN_EMPTY - [..] construct is empty (ie [])
*
----------------------------------------------------------------------------*/

BOOLEAN is_valid_pattern (char *pattern, int *error_type);
int fast_match_after_star (register char *pattern, register char *text);

/*----------------------------------------------------------------------------
*
* Return TRUE if PATTERN has any special wildcard characters
*
----------------------------------------------------------------------------*/

BOOLEAN is_pattern (char *p)
{
    while (*p)
        switch (*p++) {
            case '?':
            case '*':
            case '[':
                return TRUE;
        }
    return FALSE;
}


/*----------------------------------------------------------------------------
*
* Return TRUE if PATTERN has is a well formed regular expression according
* to the above syntax
*
* error_type is a return code based on the type of pattern error.  Zero is
* returned in error_type if the pattern is a valid one.  error_type return
* values are as follows:
*
*   PATTERN_VALID - pattern is well formed
*   PATTERN_RANGE - [..] construct has a no end range in a '-' pair (ie [a-])
*   PATTERN_CLOSE - [..] construct has no end bracket (ie [abc-g )
*   PATTERN_EMPTY - [..] construct is empty (ie [])
*
----------------------------------------------------------------------------*/

BOOLEAN is_valid_pattern (char *p, int *error_type)
{
    /* init error_type */
    *error_type = PATTERN_VALID;

    /* loop through pattern to EOS */
    while (*p) {

        /* determine pattern type */
        switch (*p) {

            /* the [..] construct must be well formed */
            case '[':
                p++;

                /* if the next character is ']' then bad pattern */
                if (*p == ']') {
                    *error_type = PATTERN_EMPTY;
                    return FALSE;
                }

                /* if end of pattern here then bad pattern */
                if (!*p) {
                    *error_type = PATTERN_CLOSE;
                    return FALSE;
                }

                /* loop to end of [..] construct */
                while (*p != ']') {

                    /* check for literal escape */
                    if (*p == '\\') {
                        p++;

                        /* if end of pattern here then bad pattern */
                        if (!*p++) {
                            *error_type = PATTERN_ESC;
                            return FALSE;
                        }
                    } else
                        p++;

                    /* if end of pattern here then bad pattern */
                    if (!*p) {
                        *error_type = PATTERN_CLOSE;
                        return FALSE;
                    }

                    /* if this a range */
                    if (*p == '-') {

                        /* we must have an end of range */
                        if (!*++p || (*p == ']')) {
                            *error_type = PATTERN_RANGE;
                            return FALSE;
                        } else {

                            /* check for literal escape */
                            if (*p == '\\')
                                p++;

                            /* if end of pattern here then bad pattern */
                            if (!*p++) {
                                *error_type = PATTERN_ESC;
                                return FALSE;
                            }
                        }
                    }
                }
                break;

            /* all other characters are valid pattern elements */
            case '*':
            case '?':
            default:
                p++;                /* "normal" character */
                break;
        }    /* switch */
    }

    return TRUE;
}


    /*
    * This test main expects as first arg the pattern and as second arg
    * the match string.  Output is yay or nay on match.  If nay on
    * match then the error code is parsed and written.
    */

#include <stdio.h>

int main(int argc, char *argv[])
{
    int error;
    int is_valid_error;

    if (argc != 3)
        printf("Usage:  MATCH Pattern Text\n");
    else {
        printf("Pattern: %s\n", argv[1]);
        printf("Text   : %s\n", argv[2]);

        if (!is_pattern(argv[1]))
            printf("    First Argument Is Not A Pattern\n");
        else {
            match(argv[1],argv[2]) ? printf("TRUE") : printf("FALSE");
            error = matche(argv[1],argv[2]);
            is_valid_pattern(argv[1],&is_valid_error);

            switch (error) {
                case MATCH_VALID:
                    printf("    Match Successful");
                    if (is_valid_error != PATTERN_VALID)
                        printf(" -- is_valid_pattern() is complaining\n");
                    else
                        printf("\n");
                    break;
                case MATCH_RANGE:
                    printf("    Match Failed on [..]\n");
                    break;
                case MATCH_ABORT:
                    printf("    Match Failed on Early Text Termination\n");
                    break;
                case MATCH_END:
                    printf("    Match Failed on Early Pattern Termination\n");
                    break;
                case MATCH_PATTERN:
                    switch (is_valid_error) {
                        case PATTERN_VALID:
                            printf("    Internal Disagreement On Pattern\n");
                            break;
                        case PATTERN_RANGE:
                            printf("    No End of Range in [..] Construct\n");
                            break;
                        case PATTERN_CLOSE:
                            printf("    [..] Construct is Open\n");
                            break;
                        case PATTERN_EMPTY:
                            printf("    [..] Construct is Empty\n");
                            break;
                        default:
                            printf("    Internal Error in is_valid_pattern()\n");
                    }
                    break;
                default:
                    printf("    Internal Error in matche()\n");
                    break;
            } /* switch */
        }

    }
    return(0);
}

#endif  /* TEST_MATCH */
