
/******************************************************************************
   lp.   This is the Multiple-Line-Printer spooler interface to applications
         and the Coherent user.  Its job is to accept a print job and place it
   into the spool queue.  As part of this, a special filename is formed to 
   ensure that requests are printed in a prioritized-first-come-first-served
   manner.  The actual file is given a 192 byte header.  Then the text of the
   report is appended on the header.  Much of the information stored in the
   header is extracted from an environment variable called MLP_SPOOL.  If a 
   printer is not specified, then the user name is used to derive the printer
   name.  This allows users to "route" their requests to various printers
   on demmand.

   This program's command line interface is very similar to lp in Unix SV.

   One noteable difference:  Output is ALWAYS copied into the spool directory.
   This is to facilitate deferred de-spooling.

   Author: B.Blatchley (c) Magnetic Data Operation 1992
******************************************************************************/

#include "header.h"

#define PRIORITY  '2'
#define LONGEVITY 'S'

static int  input_from_stdin = FALSE;
static char priority = PRIORITY;
static char qstatus = 'R';
static char keep_status = LONGEVITY;
static int  copies = 1;
static int  mailme = FALSE;
static int  writeme = FALSE;
static int  silent = FALSE;
static char printer[15];
static char user[15];
static char title[WORKSTR];
static char path[WORKSTR];
static char message[WORKSTR];

extern int  curr_formlen;


/******************************************************************************
   Return a new filename for the spool request.
******************************************************************************/

char *request_filename( when, priority)
char when;
char priority;
{
   static char name[WORKSTR];
   int         seqnum;
   char        num[10];

   seqnum = get_seq_num();

   sprintf( name, "%c%c%05d", when, priority, seqnum);

   sprintf( num, "%d ", seqnum);  strcat( message, num);   

   return( name); 
}




/******************************************************************************
   Given the contents of the SPOOLER environment variable other things and 
   return a header string.  This header will precede the actual printed-output.
******************************************************************************/

char *make_header()
{
   static char header[ HEADER_SIZE+1];
   char        tmp[ WORKSTR], s[30];

   headerval( header, H_ENTIRE, " ");

   if ( input_from_stdin) headerval( header, H_DESC, "Standard Input");
   if (path[0])           headerval( header, H_DESC,  path);
   if (title[0])          headerval( header, H_DESC,  title);

   sprintf( s, "%03d", curr_formlen);  headerval( header, H_FORMLEN, s);

   headerval( header, H_USER,    user);
   headerval( header, H_PRINTER, printer); 

   if (printer[0]) headerval( header, H_PRINTER, printer);

   headerval( header, H_DSTAMP,  datestamp());

   sprintf( tmp, "%02d", copies);     headerval( header, H_COPIES,    tmp);
   sprintf( tmp, "%c", keep_status);  headerval( header, H_LONGEVITY, tmp);

   if (mailme)  headerval( header, H_MAILME,  "M");
   if (writeme) headerval( header, H_WRITEME, "W");
   

   /* Fill from SPOOL Environment variable, if available */

   if (getenv(SPOOL_ENV)) { 
      strcpy( tmp, getenv( SPOOL_ENV));

      strncpy( s, &tmp[0],  10); headerval( header, H_TYPE,    s);
      strncpy( s, &tmp[10],  3); headerval( header, H_FORMLEN, s);
      strncpy( s, &tmp[13], 14); headerval( header, H_DBASE,   s);
      strncpy( s, &tmp[27], 14); headerval( header, H_FORMAT,  s);
      strncpy( s, &tmp[41], 43); headerval( header, H_DESC,    s);
   }

   return( header);
}




/******************************************************************************
   Create a spooled printout.  Count the pages as they go by and
   put this in the header for later use.
******************************************************************************/

spool_output()
{
   char *qname, *header, buffer[256], tmp[WORKSTR];
   int   out, pages, n;


   qname  = request_filename( qstatus, priority);
   header = make_header();

   if (!strcmp(LOCAL, printer) || !strcmp(DISPLAY, printer))
      qname[0] = 'r';

   sprintf( buffer, "%s/%s", QUEUE_PATH, qname);

   if (( out = open( buffer, O_RDWR|O_CREAT|O_TRUNC, 0644))) {
      write( out, header, HEADER_SIZE);

      pages = writeOut( 0, out, 0, 9999, curr_formlen, FALSE);

      sprintf( buffer, "%04d", pages);  headerval( header, H_PAGES, buffer);

      lseek( out, 0L, 0);               write( out, header, HEADER_SIZE);

      if (!strcmp(LOCAL, printer))  {
         silent = TRUE;

         for ( n = 0; n < copies; n++)
            local_print( out, 0, 0);
      }

      close( out);

      if (!strcmp(DISPLAY, printer)) {
         silent = TRUE;

         sprintf( tmp, "review %s/%s %d -mlp; clear", QUEUE_PATH, qname, curr_formlen);
         system(tmp);
      }
   } 
   else
      fatal("Cannot write request \"%s\" into the spool queue: %s", qname,
         QUEUE_PATH);
}





/******************************************************************************
   Reprint a request that has already been spooled.
******************************************************************************/

reprint_request( seqnum, start, end)
int seqnum;
int start;
int end;
{
   char *qname, *header, path[WORKSTR], tmp[WORKSTR], request[20];
   int   out, n;

   if ( start > end) {
      n = end; end = start; start = n;
   }

   if ( start > 0 && end == 0) end = 9999;

   sprintf( tmp, "%05d", seqnum);
   strcpy( request, request_name( tmp));

   if (*request == 0) {
      warning("Cannot open request #%05d to despool.", seqnum);
      silent = TRUE;
      return;
   }

   if (!strcmp(LOCAL, printer)) {
      silent = TRUE;
      sprintf( path, "%s/%s", QUEUE_PATH, request);

      if ( (out = open( path, O_RDWR)) != BAD) {
         lseek( out, (long) HEADER_SIZE, 0);

         for ( n = 0; n < copies; n++)
            local_print( out, start, end);

         close( out);
      }
   }
   if (!strcmp(DISPLAY, printer)) {
      silent = TRUE;
      sprintf( path, "review %s/%s %d -mlp; clear", QUEUE_PATH, request, curr_formlen);
      system( path);
   }
   else {
      qname  = request_filename( qstatus, priority);
      header = make_header();

      sprintf( path, "%s/%s", QUEUE_PATH, qname);

      if (( out = open( path, O_RDWR|O_CREAT|O_TRUNC, 0644))) {

         sprintf( tmp, "Reprint #%05u from page %04u to %04u", 
            seqnum, start, end);

         headerval( header, H_DESC, tmp);

         headerval( header, H_LONGEVITY, "R");

         write( out, header, HEADER_SIZE);

         close( out);
      } 
      else
         fatal("Cannot write request \"%s\" into the spool queue: %s", qname,
            QUEUE_PATH);
   }
}
   


/****************************************************************************
   Echo user's command
****************************************************************************/

void echo_command()
{
   char *t, *u;

   if (silent) return;

   if (message[0]) {
      t = strtok( message, " "); u = strtok( NULL, " ");

      if (u != NULL)
         fprintf( stderr, "(MLP Spooled Requests: %s, %s", t, u);
      else
         fprintf( stderr, "(MLP Spooled Request: %s", t);
   
      while (t = strtok( NULL, " "))
         fprintf( stderr, ", %s", t);
   }

   fprintf( stderr, ")\n");
}




/******************************************************************************
   Ask the despooler to shutdown.  Remove the despooler's PID from the status  
   file.  Then wakeup the despooler (which will read the newly changed status
   file).  When the despooler sees its PID removed, it will terminate.
******************************************************************************/

shutdown_despooler()
{
   int   pid;
   char *value;

   if (getuid() == 0) {
      value = status( DESPOOLER, "");  sscanf( value, "%d", &pid);

      status( DESPOOLER, " ");

      if ( pid > 0) kill( pid, SIGALRM);

      printf("MLP Despooler Shutdown.\n");
      wakeup_despooler();      
   }
   else
      printf("\007\nSorry, you must be superuser to do this.\n\n");
}




/******************************************************************************
   Find printer name "unknown" is assumed to be undefined in the route
   directory--therefore the printer for unknown users will be the system
   default.  HOWEVER, if the route file "unknown" is filled with the name of
   a printer, then these request will go to the this printer instead.  Kind
   of a freebe feature!   Get the user's login name too.
******************************************************************************/

char *get_printer_and_user_name()
{
   strcpy( user,    "unknown"); 
   strcpy( printer, "unknown"); 

   if ( getlogin())
      strcpy( user, getlogin());

   strcpy( printer, route_request( user));

   if (!strcmp( SYSTEM, printer))
      strcpy( printer, controls(DEFAULT,""));
}




/******************************************************************************
   Check for environmental overides for the priority, form length and copies.
******************************************************************************/

char *get_environment()
{
   char tmp[WORKSTR];
   int  tmpv;

   if (getenv(PRIORITY_ENV)) { 
      strcpy( tmp, getenv( PRIORITY_ENV));  tmp[1] = 0;

      if (*tmp < '0') *tmp = '0';
      if (*tmp > '9') *tmp = '9';  

      priority = *tmp;
   }

   if (getenv(LONGEVITY_ENV)) { 
      strcpy( tmp, getenv( LONGEVITY_ENV));  tmp[1] = 0;

      switch (*tmp) {
         case 'l':
         case 'L': keep_status = 'L'; break;
         case 't':
         case 'T': keep_status = 'T'; break;
         case 's':
         case 'S':
          default: keep_status = 'S';
      }
   }


   curr_formlen = 66;

   if (getenv(FORMLEN_ENV))
      if ( sscanf( getenv( FORMLEN_ENV), "%d", &tmpv))
         curr_formlen = tmpv;


   if (getenv(COPIES_ENV)) {
      copies = 66;

      if ( sscanf( getenv( COPIES_ENV), "%d", &tmpv))
         copies = tmpv;
   }
}





/******************************************************************************
   Main Driver.   Parse the arguments and spool each report named (otherwise
   spool the contents of standard input).   Then send the despooler a wakup
   call.

   NOTE:  This program does DIFFERENT things depending on what it is called.
             1) as "lp" it does lp kinds of things.
             2) as "lpshut" it shuts down the despooler.
             3) as "reprint" it reprints spooled requests
******************************************************************************/

void main( argc, argv)
int   argc;
char *argv[];
{
   extern char *optarg;
   extern int   optind;
   extern char *progname;
   char         o;
   int          file, reprint, seqnum, start, end;

   reprint = FALSE;

   umask(FPERMS);

   progname = basename( argv[0]);

   get_printer_and_user_name();



   /******** As the lpshut command *************/

   if (!strcmp( progname, "lpshut")) {
      shutdown_despooler();

      log("Shutdown Scheduler Daemon");

      /* cancel the currently running backend processes */

      if (argv[1] == NULL) 
         kill_printing_request( 0, R_KILL, CLEAR_ALL);
      else
      if (strcmp( argv[1], "-d"))
         kill_printing_request( 0, R_KILL, CLEAR_ALL);
      
      exit(0);
   }


   /******** As the reprint command *************/

   if (!strcmp( progname, "reprint")) {
      if ( argc == 1)
         printf( "\nusage:  reprint request_num [start_page] [end_page]\n\n");
      else {
         get_environment();

         seqnum = 0; sscanf( argv[1], "%d", &seqnum);
         start  = 0; sscanf( argv[2], "%d", &start);
         end    = 0; sscanf( argv[3], "%d", &end);

         reprint_request( seqnum, start, end);
         wakeup_despooler();
      }
      exit(0);
   }


   /******** As the lp command *************/

   while ( (o = getopt( argc, argv, "bd:mn:t:wsSR")) != EOF) {
      switch (o) {
         case 'd': strncpy( printer, optarg, sizeof( printer));  break;
         case 't': strncpy( title,   optarg, sizeof( title));    break;
         case 'n': sscanf( optarg, "%d", &copies);               break;
         case 'm': mailme  = TRUE;                               break;
         case 's': silent  = TRUE;                               break;
         case 'w': writeme = TRUE;                               break;
         case 'R': reprint = TRUE;                               break;

         case 'S': shutdown_despooler();                         exit(0);
      }
   }

   get_environment();

   if ( reprint) {
      seqnum = 0; sscanf( argv[optind],   "%d", &seqnum);
      start  = 0; sscanf( argv[optind+1], "%d", &start);
      end    = 0; sscanf( argv[optind+2], "%d", &end);

      reprint_request( seqnum, start, end);
   }
   else {
      if ( optind < argc) {
         while (optind < argc) {

            if (strcmp( argv[optind], "-"))
               if ( ( file = open( argv[optind], O_RDONLY))) {
                  dup2( file, 0);
                  strncpy( path, argv[optind], strlen( argv[optind]));
                  spool_output();
               }

            optind++;
         }
      }
      else {
         input_from_stdin = TRUE;
         spool_output();
      }
   }

   echo_command();

   wakeup_despooler();
}
