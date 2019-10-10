
/****************************************************************************
   This is a filter that accepts text and some IBM/Epson print codes and
   produces output suitable for an HP laserjet printer.

   Author B.Blatchley

****************************************************************************/

#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#define  FALSE 0
#define  TRUE  (!FALSE)

static float cpi = 10.0;
static lines = 66;
static int n, m;
static int feed      = 0;
static int wide      = 0;

static int  DECI       = 7416;
static int  envelope   = 0;
static int  bottomtray = 0;

#define FF    12
#define LF    10

#define RELATIVE 0
#define ABSOLUTE 1

#define TAB       '\011'
#define LINE_FEED '\012'
#define FORM_FEED '\014'
#define CTRL_O    '\017'
#define CTRL_R    '\022'
#define ESCAPE    '\033'

#define S_START   1
#define S_START1  11
#define S_START2  12
#define S_ESCAPE  2
#define S_ESCAPE1 21
#define S_ESCAPE2 22
#define S_ESCAPE3 23
#define S_ESCAPE4 24
#define S_ESCAPE5 25

#define REMCHAR   *rl++ = c; *rl = 0

static char  rline[100];
static char *rl;

static top_margin = 0;
static page_feed  = FALSE;


/******************************************************************************
   A shell around the getc function.
******************************************************************************/

int nextc()
{
   return(getchar());
}




/******************************************************************************
  Line feed
******************************************************************************/

line_feed()
{
   if ( feed == 1) {            /* Feed only if there are more chars */
      feed = 0;

      if ( n >= lines || page_feed)    {
         putchar('\f');
         page_feed = FALSE;
      }

      n %= lines;

      m =  n * (DECI / (lines + 1));

      printf("\033&a0h%dV", (m + top_margin)); 
   }
}


/******************************************************************************
   Print Unrecognized text...
******************************************************************************/


int dot()
{
   if ( feof(stdin))
      return;

   line_feed();

   rl = rline;

   while ( *rl != 0) {
      putchar(*rl++);
 
      if ( wide == 1)
         putchar(' ');
   }

    rl = rline;
   *rl = 0; 
}





/******************************************************************************
   An IBM Print Code Recognizer, with Brett's imaging extentions.  At EOF
   when everything has been recognized.
******************************************************************************/


int translate_and_print()
{
   static int state = S_START;
          int done  = FALSE;
          int c     = nextc();

  
   while ( !done) {
      switch (state) {

         case S_START: 
            rl = rline;
         
            switch (c) {

               case LINE_FEED:
                  feed = 1; n++; 
               break;

               case FORM_FEED:
                  n = 0;
                  putchar(FF);
               break;

               case CTRL_R:
                  printf("\033(s10H");     /* cpi = 10 normal */
                  state = S_START;
               break;

               case CTRL_O:
                  printf("\033(s16H");     /* cpi = 16 condensed */
                  state = S_START;
               break;

               case ESCAPE:
                  REMCHAR;
                  state = S_ESCAPE;
               break;

               default:
                  REMCHAR;
                  dot();
               break;
            }
         break;



         case S_ESCAPE:  
            switch (c) {
               case '0':
                  lines = 88;  state = S_START; break;

               case '2':
                  lines = 66;  state = S_START; break;

               case 'E':
                  printf("\033(s3B");  /* Boldface on */
                  state = S_START;
               break;
 
               case 'F':
                  printf("\033(s0B");  /* Boldface off */
                  state = S_START;
               break;  


               case 'P': 
                  REMCHAR;
                  state = S_ESCAPE1;
               break;


               case 'M':
                  printf("\033(s12H");   /* cpi = 12.0 */ 
                  state = S_START;
               break;


               case 'W':
                  REMCHAR;
                  state = S_ESCAPE2;
               break;


               case '-':
                  REMCHAR;
                  state = S_ESCAPE3;
               break;


               case '\176':
                  REMCHAR;
                  state = S_ESCAPE4;
               break;



               default:
                  REMCHAR;
                  dot();
                  state = S_START;
               break;
            }
         break;


         case S_ESCAPE1:
            switch (c) {
               case CTRL_O:
                  printf("\033(s16H");   /* cpi = 16  condensed */
                  state = S_START;
               break;

               case CTRL_R:
                  printf("\033(s10H");     /* cpi = 10 normal */
                  state = S_START;
               break;

               default:
                  REMCHAR;
                  dot();
                  state = S_START;
               break;
            }
         break;



         case S_ESCAPE2:
            switch (c) {
               case 0:
                  wide = 0;
                  printf("\033(s%2.2fH", cpi);  /* Double wide off */
                  state = S_START;
               break;

               case 1:
                  wide = 1;
                  printf("\033(s%2.2fH", (cpi/2.0));  /* Double wide off */
                  state = S_START;
               break;

               default:
                  REMCHAR;
                  dot();
                  state = S_START;
               break;
            }
         break;



         case S_ESCAPE3:
            switch (c) {
               case '0':
                  printf("\033&d@D");   /* Underline Off */
                  state = S_START;
               break;

               case '1':
                  printf("\033&d0D");   /* Underline On */
                  state = S_START;
               break;

               default:
                  REMCHAR;
                  dot();
                  state = S_START;
               break;
            }
         break;



         case S_ESCAPE4:
            REMCHAR;

            if ( c == '2')
               state = S_ESCAPE5;
            else {
               dot();
               state = S_START;
            }
         break;



         case S_ESCAPE5:
            switch (c) {
               case 0:
                  printf("\033(s0S");  /* Reverse off (italics) */
                  state = S_START;
               break;

               case 1:
                  printf("\033(s1S");  /* Reverse on (italics) */
                  state = S_START;
               break;

               default:
                  REMCHAR;
                  dot();
                  state = S_START;
               break;
            }
      }

      if ( (c = nextc()) == EOF)
         done = TRUE;
   }
   dot();
}



main( argc, argv)
int    argc;
char  *argv[];
{
   int nn, n, list = 0, reset = 0;


   for ( nn = 1; nn < argc; nn++) {
      if ( !strncmp( "-b", argv[nn], 2)) 
         bottomtray = 1;
      else
      if ( !strncmp( "-B", argv[nn], 2)) 
         bottomtray = 1;
      else
      if ( !strncmp( "-e", argv[nn], 2)) 
         envelope = 1;
      else
      if ( !strncmp( "-E", argv[nn], 2)) 
         envelope = 1;
   } 
  
 
   if (bottomtray == 1)
      printf("\033&l4H");  /* Feed from BOTTOM tray */
   else
      printf("\033&l1H");  /* Feed from TOP tray */


   if (envelope == 1) {
      printf("\033&l81A");   /* Select Envelopes */
      printf("\033&l1O");    /* Landscape Mode */  
      lines = 21;
      DECI  = 2510;
   }
   else {
      printf("\033&12A");   /* Select Normal Paper */
      printf("\033&l0O");   /* Portrait Mode */
      lines = 66;
      DECI  = 7416;
   }


   n = 0;  /* line counter */

   m =  n * (DECI / (lines + 1));

   printf("\033&a0h%dV", m);   /* Home HP cursor */
   printf("\033(11U");         /* PC-8 font  (11U is Danish) */
   printf("\033(s0B");         /* Lightweight stroke */

   feed = 1;

   translate_and_print();

   printf("\033&a0v0H                                           \f\033E");

   printf("\033&l0O");   /* Portrait Mode */
   printf("\033&12A");   /* Select Normal Paper */
   printf("\033*c1F");   /* Delete ALL temporary soft fonts */
}
