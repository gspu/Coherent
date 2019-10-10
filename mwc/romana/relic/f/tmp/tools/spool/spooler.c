/******************************************************************************
   Various routines used by the MLP spooler system.
******************************************************************************/

#include "header.h"

char progname[16];

static char  caparea[WORKSTR];
static char *PS, *PN;

char curr_printer[WORKSTR];
char curr_device[WORKSTR];
char curr_backend[WORKSTR];
char curr_desc[WORKSTR];
char curr_user[20];
char curr_name[20];
char curr_longevity[2];
int  curr_mailme;
int  curr_writeme;
int  curr_copies;
int  curr_copy;
int  curr_formlen;
int  abort_write = FALSE;
int  kill_request = FALSE;


/******************************************************************************
   Scatf works like sprintf except the target string has the newly formated
   string appended to the end of it.  Be sure the target is big enough to 
   hold the new string as well as its current contents.  Returns a pointer
   to the target string.
******************************************************************************/

char *scatf( va_alist)
va_dcl
{
    va_list  args;
    char    *dest, *tmp;

    tmp = malloc(512);
    va_start( args);

    dest = va_arg( args, char *);

    sprintf( tmp, "%r", args);

    strcat( dest, tmp);

    va_end( args);
    free( tmp);

    return( dest);
}



/******************************************************************************
   fatal.  Handles fatal errors with "astonishing" grace.  It sends a formated
   messages to root's mailbox and terminates the program.  Warning sends a 
   formated message to the user's mailbox and returns to caller.
******************************************************************************/

void warning( va_alist)
va_dcl
{
   va_list  args;
   char    *i, *j, user[20];

   strcpy( user, "root");  if ( i = (char *) getlogin())  strcpy( user, i);

   i = calloc(1024,1);  j = calloc(1024,1);

   strcpy(  i, "/bin/echo \"\n`date`\n\"");
   strcat(  i, "'\n*** Message From The MLP Spooler ***\n\n");

   va_start( args);  
   sprintf( j, "%r", args);
   va_end( args);

   strcat( i, j); 

   if (getenv("MLPLMAIL"))
      strcat( i, "\n\n' | lmail ");
   else
      strcat( i, "\n\n' | mail ");

   strcat( i, user);

   log( "%s", j);  system( i);

   free( i);  free( j);
}

void warning_user( va_alist)
va_dcl
{
   va_list  args;
   char    *i, *j, *user;

   i = calloc(1024,1);  j = calloc(1024,1);

   strcpy(  i, "/bin/echo \"\n`date`\n\"");
   strcat(  i, "'\n*** Message From The MLP Spooler ***\n\n");

   va_start( args);  
   user = va_arg( args, char *);  sprintf( j, "%r", args);
   va_end( args);

   strcat( i, j); 

   if (getenv("MLPLMAIL"))
      strcat( i, "\n\n' | lmail ");
   else
      strcat( i, "\n\n' | mail ");

   strcat( i, user);

   log( "%s", j);  system( i);

   free( i);  free( j);
}



void fatal( va_alist)
va_dcl
{
   va_list  args;
   char    *i, *j;

   i = calloc(1024,1);  j = calloc(1024,1);

   strcpy(  i, "/bin/echo \"\n`date`\n\"");
   strcat(  i, "'\n*** Message From The MLP Spooler ***\n\n");

   va_start( args);  
   sprintf( j, "%r", args);
   va_end( args);

   strcat( i, j); 

   if (getenv("MLPLMAIL"))
      strcat( i, "\n\n' | lmail root");
   else
      strcat( i, "\n\n' | mail root");

   log( "%s", j);  system( i);

   free( i);  free( j);
   exit(0);
}


/******************************************************************************
   Removes the leading and trailing spaces from the given string.  First
   scan to the end of the string. Then eat the right hand spaces.  Then
   eat the left hand spaces.  Then copy the unpadded string back into its
   original place.
******************************************************************************/

char *unpadd( str)
char *str;
{
   int   len = 0;
   char *t, *u;

   for (t = str; *t; t++) len++;    t--;

   while ( t != str && *t == ' ') { *t = 0; t--; len--;} 

   for (t = str; *t && *t == ' '; t++) len--;

   u = calloc((len+1), 1); strncpy( u, t, len); strcpy( str, u); free( u);

   return( str);
}


/******************************************************************************
   Rename a file.
******************************************************************************/

int rename( old, new)
char *old;
char *new;
{
   unlink( new);

   if ( link( old, new) == BAD) return(2);

   if ( unlink( old) == BAD)    return(3);

   return(0);
}





/*****************************************************************************
   Put a request in the inactive queue
*****************************************************************************/

void make_inactive( name)
char *name;
{
   char newname[20], path[WORKSTR], newpath[WORKSTR];

   if (*name == 'r') return;  /* already inactive */

   strcpy( newname, name); newname[0] = 'r';

   sprintf( path,    "%s/%s", QUEUE_PATH, name);
   sprintf( newpath, "%s/%s", QUEUE_PATH, newname);     

   rename( path, newpath);
} 





/******************************************************************************
   Get/Set header information
******************************************************************************/

char *headerval( header, opt, value)
char *header;
int   opt;
char *value;
{
   static char s[HEADER_SIZE];
   int         offset, length, n;

   switch (opt) {
      case H_USER:      offset =  0; length =  14; break;
      case H_PRINTER:   offset = 14; length =  14; break;
      case H_TYPE:      offset = 28; length =  10; break;
      case H_FORMLEN:   offset = 38; length =   3; break;
      case H_PAGES:     offset = 41; length =   4; break;
      case H_COPIES:    offset = 45; length =   2; break;
      case H_LONGEVITY: offset = 47; length =   1; break;
      case H_MAILME:    offset = 48; length =   1; break;
      case H_WRITEME:   offset = 49; length =   1; break;
      case H_DBASE:     offset = 50; length =  14; break;
      case H_FORMAT:    offset = 64; length =  14; break;
      case H_DSTAMP:    offset = 78; length =  10; break;
      case H_DESC:      offset = 88; length =  60; break;

      case H_ENTIRE:    offset =  0; length = HEADER_SIZE-2; 
                        header[HEADER_SIZE-1] = '\n';
                        header[HEADER_SIZE] = 0;
                        break;
   }

   if (value[0]) {
      for (n = 0; n < length; n++) header[offset+n] = ' ';
      for (n = 0; value[n] && n < length; n++) header[offset+n] = value[n]; 
   }

   strncpy( s, &header[offset], length); s[length] = 0;

   return( s);
}




/******************************************************************************
   Log text messages into the log file.
******************************************************************************/

log( va_alist)
va_dcl
{
   va_list    args;
   FILE      *fp;
   long       clock;
   struct tm *tt;
   char       message[1200];


   va_start( args);  sprintf( message, "%r", args);  va_end( args); 

   MLP_lock( L_LOG);

   time( &clock); tt = localtime( &clock);

   if ( fp = fopen( LOG_PATH, "a+")) {
      fprintf( fp, "%02d/%02d/%02d ", (tt->tm_mon+1), tt->tm_mday, tt->tm_year);
      fprintf( fp, "%02d:%02d - ", (tt->tm_hour), tt->tm_min);
      fprintf( fp, "%s\n", message);
      fflush( fp);
      close( fp);
   }

   MLP_unlock( L_LOG);
}



/******************************************************************************
   Get Termcap Information.  If we've already filled PS then don't bother again.
******************************************************************************/

void get_termcap()
{
   extern char *tgetstr();
   extern int   tgetent();
   char *buff, *term, *ptr;

   if ( PS) return;

   ptr = buff = calloc( 1024, 1);

   if ( buff == NULL) fatal("Not enough memory to local print");

   if ( (term = getenv("TERM")) == NULL)
      fatal("Trying to local print.  TERM must be defined.");

   if ( tgetent( buff, term) == BAD)
      fatal("Trying to local print.  Cannot find %s in /etc/termcap", term);

   ptr = caparea;

   PS = tgetstr( "PS", &ptr);
   PN = tgetstr( "PN", &ptr);

   if ( PS == NULL || PN == NULL)
      fatal("Trying to local print.\n\nBoth PS and PN need to be defined for terminal %s in /etc/termcap", term);
}







/******************************************************************************
   Return a datestamp string (actually it is the long-int in text form)
******************************************************************************/

char *datestamp()
{
   static char stamp[11];
   time_t      t;

   time( &t);  sprintf( stamp, "%010lu", t);

   return( stamp);
}



/******************************************************************************
   Given a string, convert it to uppercase.
******************************************************************************/

char *uppercase( str)
char *str;
{
   char *c = str;

   while (*c) {
      if (islower(*c)) *c = toupper(*c);
      c++;
   }

   return( str);
}



/******************************************************************************
  Given the name of a control-file parameter, return its value.  Lock the
  file so we can read/write it without it changing on us.  If the value of the
  string "newval", is non-null then assign the parameter this new new value.
******************************************************************************/

char *parameters( param, newval, pathname, locktype)
char *param;
char *newval;
char *pathname;
int  locktype;
{
   static  char value[ WORKSTR];
   FILE   *fp, *ofp;
   char    tmp[WORKSTR], tmp1[WORKSTR], parameter[30], *c, tmppath[WORKSTR];

   sprintf( tmppath, "%s.tmp", pathname);

   strncpy( parameter, param, sizeof( parameter));  uppercase( parameter);

   strcpy( value, "");

   MLP_lock( locktype); 
 
   if ( (fp = fopen( pathname, "r"))) {

      if (!strcmp( newval, "")) {

         while (!feof( fp)) {
            fgets( tmp, sizeof( tmp), fp);

            for (c = tmp; *c && *c != '#'; c++); /* look for comment */

            if ( *tmp && !*c) {
               c = strtok( tmp, SEP);  uppercase( c);

             
               if ( !strcmp( c, parameter)) {
                  c = strtok( NULL, "\n");
                  while (*c && (*c == ' ' || *c == '\t' || *c == '=')) c++;
                  strcpy( value, c);
                  break;
               }
            }
         }
         fclose( fp);
      }
      else { 
         if ( (ofp = fopen( tmppath, "w"))) {

            while (!feof( fp)) {
               fgets( tmp, sizeof( tmp), fp);  strcpy( tmp1, tmp);

               if ( *tmp && *tmp != '#') {
                  c = strtok( tmp, SEP);  uppercase( c);

                  if ( strcmp( c, parameter)) 
                     fprintf( ofp, "%s", tmp1);
               }
               else
                  fprintf( ofp, "%s", tmp1);
            }

            fprintf( ofp, "%s = %s\n", param, newval);

            fclose( ofp);  fclose( fp);

            rename( tmppath, pathname);
         }
         else {
            fclose( fp);
            fatal("Cannot create %s", tmppath);
         }
      }
   }
   else
   if ( errno == EACCES) 
         fatal("Permission Problem with \"%s\".  It should have the suid bit set.", progname);
   else
   if ( rename( tmppath, pathname) == 0) {
      MLP_unlock( locktype);
      return( parameters( param, newval, pathname, locktype));
   }
   else {
      if ( (fp = fopen( pathname, "w"))) {
         fclose( fp);
         MLP_unlock( locktype);
         return( parameters( param, newval, pathname, locktype));
      }
      else
         fatal("Cannot create control file: %s", pathname);
   }
   

   MLP_unlock( locktype);

   return( value);
}




/******************************************************************************
  Given the name of a printer-setup parameter, return its value. 
  If the value of the string "newval", is non-null then assign the parameter
  this new new value.
******************************************************************************/

char *controls( param, newval)
char *param;
char *newval;
{
   return( parameters( param, newval, CONTROL_PATH, L_CONTROL));
}


/******************************************************************************
  Given the name of a printer-status parameter, return its value.  
  If the value of the string "newval", is non-null then assign the parameter
  this new new value.
******************************************************************************/

char *status( param, newval)
char *param;
char *newval;
{
   return( parameters( param, newval, STATUS_PATH, L_STATUS));
}




/*****************************************************************************
   Given the name of a printer, return the name of its device.  The controls
   line for a printer looks like this:

   printer = name, device, backend
*****************************************************************************/

char *printer_device( newprinter)
char *newprinter;
{
   char  line[WORKSTR], tmp[40], *c, tmppath[WORKSTR], printer[WORKSTR];
   FILE *fp;

   strcpy( printer, newprinter);

   sprintf( tmppath, "%s.tmp", CONTROL_PATH);

   MLP_lock( L_CONTROL);

   if ( (fp = fopen( CONTROL_PATH, "r"))) {

      while ( fgets( line, sizeof( line), fp)) {

         *tmp = *curr_device = *curr_backend = 0;   

         c = strtok( line, SEP);  if (c) strncpy( tmp, c, sizeof(tmp));

         if (!strcmp( "PRINTER", uppercase( tmp))) {
            c = strtok( NULL, SEP);

            if (!strcmp( printer, c)) {
               c = strtok( NULL, SEP); if (c) strncpy( curr_device,  c, WORKSTR);
               c = strtok( NULL, SEP); if (c) strncpy( curr_backend, c, WORKSTR);
               break;
            }
         }
      }

      fclose( fp);
   }
   else
   if ( errno == EACCES) {
         /* fprintf(stderr,"permission problem (%s)\n", progname); fflush(stderr); */
         fatal("Permission Problem with \"%s\".  It should have the suid bit set.", progname);
   }
   else
   if ( rename( tmppath, CONTROL_PATH) == 0) {
      MLP_unlock( L_CONTROL);
      return( printer_device( newprinter));
   }
   else
      fatal( "Missing control file: %s", CONTROL_PATH);

   MLP_unlock( L_CONTROL);

   return( curr_device);   
}





/******************************************************************************
   Given a user name, return the name of the printer to which they are cur-
   rently routed.
******************************************************************************/

char *route_request( user)
char *user;
{
   static  char printer[15];
   FILE   *fp;
   char         path[WORKSTR], *c;

   sprintf( path, "%s/%s", ROUTE_PATH, user);


   strcpy( printer, "");

   if ( (fp = fopen( path, "r"))) {
      fgets( printer, sizeof( printer), fp);
      fclose( fp);
   }
   else {
      strcpy( printer, controls( DEFAULT,""));

      if (!strcmp( printer, ""))
         fatal( "Controls file (%s) needs a default printer entry", CONTROL_PATH);
   }

   if ( c = strchr( printer, '\n')) *c = 0;

   return( printer);
}





/******************************************************************************
   Return a new report sequence number.
******************************************************************************/

int get_seq_num()
{
   char *v, newnum[10];
   int   n;

   v = status("seqnum","");          if ( v[0] == 0)  v = "00001";
   
   sscanf( v, "%d", &n);               if ( n >= 32001) n = 1;

   sprintf( newnum, "%05u", (n+1));    status( SEQNUM, newnum);

   return(n);
}




/******************************************************************************
   Local print.  Send the contents of the spool file (less the header)
   to the printer connected to the user's terminal.  We assume that we are
   positioned past the header.
******************************************************************************/

void local_print( fd, start, end)
int fd;
int start;
int end;
{
   char *c, k;
   long here;


   here = lseek( fd, 0L, 1);

   get_termcap();

   write( 1, PN, strlen( PN));

   writeOut( fd, 1, start, end, curr_formlen, FALSE);

   c = controls( LOCALFEED, "");

   if (!strcmp( c, "ON") || !strcmp( c, "on")) { k = '\f'; write( 1, &k, 1); }

   write( 1, PS, strlen( PS));

   lseek( fd, here, 0);
}



/*****************************************************************************
   Return a sorted list of the spool queue.  The data structure returned is
   a two dimensional array ( entries x MAXNAMLEN).
*****************************************************************************/

char *dirlist( dirname, descend)
char *dirname;
int   descend;
{
   extern char *realloc();
   DIR *dr;
   struct dirent *dp;
   char *t, tmp[MAXNAMLEN];
   int   size, top, swapit, i, j, gap, comp;

   dr = opendir( dirname);  t = calloc( MAXNAMLEN, 1);  size = 0;

   while ( ( dp = readdir( dr)) != NULL)
      if (!strcmp( ".", dp->d_name) || !strcmp( "..", dp->d_name))
         continue;
      else {
         strncpy( (t+(size * MAXNAMLEN)), dp->d_name, MAXNAMLEN);
         size++;
         t = realloc( t, ((size+1) * MAXNAMLEN));
      }

   closedir( dr);

   strcpy( (t+(size * MAXNAMLEN)), "");


   /* Use a COMBsort to sort the items in this directory */
   /* This sort is like a bubble sort only MUCH faster */

   gap = size;

   do {
      gap = gap * 10 / 13;    if (gap == 0) gap = 1;

      top = size - gap;       swapit = 0;

      for ( i = 0; i < top; i++) {
         j = i + gap;
        
         comp = strcmp( (t+(i * MAXNAMLEN)), (t+(j * MAXNAMLEN)));

         if (descend) {      /* Reverse the sort order */
            if ( comp > 0)
               comp = 0;
            else
               comp = 1;
         }
       
         if ( comp > 0) {
            strcpy( tmp, (t+(i * MAXNAMLEN)));
            strcpy( (t+(i * MAXNAMLEN)), (t+(j * MAXNAMLEN)));
            strcpy( (t+(j * MAXNAMLEN)), tmp);

            swapit++;
         } 
      }
   } while ( swapit || gap > 1);
   return( t);
}



/******************************************************************************
   Wakeup The Despooler by sending it an alarm signal.  The despooler's PID
   will be found in the controls database.
******************************************************************************/

void wakeup_despooler()
{
   char  *v;
   int    pid = 0;

   v = status( DESPOOLER,"");

   sscanf( v, "%d", &pid);

   if ( pid > 0) kill( pid, SIGALRM);
}





/******************************************************************************
   Basename.  Strip the leading path information from a pathname.
******************************************************************************/

char *basename( path)
char *path;
{
   char *c;

   if (path == NULL) return("");

   for (c = path; *c; c++); c--;           /* go to the end of the string */

   while (c != path && *(c-1) != '/') c--; /* step back til we see a slash */

   return( c);
}




/******************************************************************************
   Return TRUE if the scheduler is running, FALSE if not.
******************************************************************************/

int scheduler_status()
{
   char *t;
   int   pid;

   t = status( DESPOOLER, "");  sscanf( t, "%d", &pid);
   
   if ( pid > 1 && !kill( pid, 0))
      return( TRUE);
   else
      return( FALSE);
}




/******************************************************************************
   This routine does all the actual I/O to the device.  The tricky part is
   counting the pages and (optionally printing only the desired range of pages).
   It is used in several places.  Source and Sink are the input and output
   file descriptors.  The source's file pointer is expected to be at the
   start of the user's data; that is, seek past the header, if there is one.
   Start and end are the starting and ending page numbers for the reqeuest.
   If both of these are zero then we will send all of the source data to the
   sink, otherwise we send source to sink only when we are within the page
   range.  Counting pages depends heavily on the form length (curr_formlen)
   and the nature of the source data.  If you are sending bitmaps to a laser
   printer though this routine, the page numbers will probably be wildly
   skewed!  At this point, I know of no convienent way around this.  You'd
   have to parse the input for ALL known laser printers. Then the next new
   one could possibly make your effort moot.  Maybe this could be done for
   HPCL and Postscript?  If the flag "report" is TRUE then the routine's
   progress will be reported in the status file.  This routine returns the
   number of pages that it has processed.
******************************************************************************/

#define IOBUFF 1024

int writeOut( source, sink, start, end, formlen, report)
int source;
int sink;
int start;
int end;
int formlen;
int report;
{
   int   result, lines, progress;
   int   pages;
   long  rsize, oldpos;
   char  tmp[WORKSTR];
   char *i, *ibuff, *imax;  /* input buffer pointers */
   char *o, *obuff, *omax;  /* output buffer pointers */

   /* how big is the source file? */
 
   oldpos = lseek( source, 0L, 1);
   rsize  = lseek( source, 0L, 2);  lseek( source, oldpos, 0);

   
   ibuff = malloc( IOBUFF);  obuff = malloc( IOBUFF);

   o = obuff;  omax = obuff + IOBUFF;

   lines = 0;  pages = 1;
 
   while ( result = read( source, ibuff, IOBUFF)) {
      if (abort_write) {
         o = obuff;
         pages = -1;
         break;
      }

      if ( start == 0 && end == 0)
         write( sink, ibuff, result);   /* print entire request */
      else {

         /* print from start to end page of request */

         for ( i = ibuff, imax = ibuff + result; i < imax; i++) {

            if ( *i == '\n') {
               lines++;

               if ( lines > formlen) {
                  lines = 0;
                  /* lines -= formlen; */
                  pages++;
               }
            }
            else
            if ( *i == '\f') {
               pages++;
               lines = 0;
            }

            if ( pages >= start && pages <= end) {

               *o++ = *i;

               if ( o >= omax) {

                  write( sink, obuff, IOBUFF);
                  o = obuff;
               }
            }
         }
      }


      /* Report Progress to the status file */

      if (report) {
         progress = (int) (lseek( source, 0L, 1) * 100L / rsize);

         sprintf( tmp, "%d, %s, %s, %d, %d of %d", getpid(), curr_name, 
            curr_printer, progress, curr_copy, curr_copies);

         status( curr_device, tmp);
      }
   }

   /* write anything leftover in the output buffer */

   if ( o != obuff) write( sink, obuff, (int) (o - obuff)); 


   free( ibuff);  free( obuff);

   return( pages);
}



/******************************************************************************
   Given the sequence number of a request, return the full file name of
   the request.
******************************************************************************/

char *request_name( seqnum)
char *seqnum;
{
   DIR *dr;
   struct dirent *dp;
   int val;
   char tvalue[15];
   static char value[15];


   val = 0;  sscanf( seqnum, "%d", &val);   sprintf( tvalue, "%05u", val);

   dr = opendir( QUEUE_PATH);  value[0] = 0;


   while ( ( dp = readdir( dr)) != NULL)
      if (!strcmp( ".", dp->d_name) || !strcmp( "..", dp->d_name))
         continue;
      else {
         if (!strncmp( tvalue, &dp->d_name[2], 5)) {
            sprintf( value, "%s", dp->d_name);
            break;
         }
      }

   closedir( dr);

   return( value);
}




/****************************************************************************
   Given the report sequence number, return the owner's name
****************************************************************************/

char *request_owner( seqnum)
char *seqnum;
{
   static char owner[20];
   char path[WORKSTR], head[HEADER_SIZE];
   int  fd;

   strcpy( owner, request_name( seqnum));

   if (*owner) {
      sprintf( path, "%s/%s", QUEUE_PATH, owner);

      if ( (fd = open( path, O_RDONLY)) != BAD) {
         read( fd, head, HEADER_SIZE);
         close( fd);

         strcpy( owner, headerval( head, H_USER, ""));
         unpadd( owner);
      }
   }
   else
      *owner = 0;

   return( owner);
}




/****************************************************************************
  Return 1 if I (the program's user) own the request, 0 otherwise. If I'm
  superuser, then I own it all.
****************************************************************************/

int do_i_own_it( seqnum)
char *seqnum;
{
   struct passwd *p;
   
   p = getpwuid( getuid());

   if ((p && !strcmp( p->pw_name, request_owner( seqnum))) ||
      (getuid() == 0))
      return(1);
   else
      return(0);
}





/******************************************************************************
   Report the status of all the printers defined in the control file.
******************************************************************************/

char *each_printer( command)
int command;
{
   static FILE *fp;
   static char  line[WORKSTR];
   char         tmp[WORKSTR], *t;

   *line = 0;

   switch (command) {
      case E_START: MLP_lock( L_CONTROL);

                    if ( (fp = fopen( CONTROL_PATH, "r+")) == NULL) {
                       sprintf( tmp, "%s.tmp", CONTROL_PATH);

                       if ( errno == EACCES)
                          fatal("Permission Problem with \"%s\".  It should have the suid bit set.", progname);
                       else
                       if ( rename( tmp, CONTROL_PATH) == 0) {
                          MLP_unlock( L_CONTROL);
                          return( each_printer( command));
                       }
                       else
                          fatal( "Missing control file: %s", CONTROL_PATH);
                    }
                    break;

      case E_NEXT:  while (fgets( tmp, sizeof(tmp), fp)) {

                       t = strtok( tmp, SEP); uppercase( t);
                       if (!strcmp( "PRINTER", t)) {
                          for (t = tmp; *t; t++); t++;
                          while (strchr(SEP, *t)) t++;
                          strcpy( line, t);
                          break;
                       }
                    }
                    break;

      case E_REWIND: fseek( fp, 0L, SEEK_SET); break;

      case E_END:    fclose( fp);
      case E_UNLOCK: MLP_unlock( L_CONTROL); break;

      case E_FILE:   return( (char *) fp);
   }

   return( line);
}




char *report_printer_status( display_type)
{
   static char request_list[WORKSTR];
   int    pid, req;
   char   tmp[40], *t, *c, *d, *header, rpath[WORKSTR];
   char   request[20], user[20], timestr[40];
   long   rtime;

   request_list[0] = 0;  /* will contain a list of all the request that are
                            currently printing.  This is so we can skip these
                            when we go over the list of all pending requests.*/


   each_printer( E_START);

   while ( *(t = each_printer( E_NEXT))) {

         *tmp = *curr_printer = *curr_device = *curr_backend = 0;   

         c = strtok( t   , SEP); if (c) strncpy( curr_printer, c, WORKSTR);
         c = strtok( NULL, SEP); if (c) strncpy( curr_device,  c, WORKSTR);
         c = strtok( NULL, SEP); if (c) strncpy( curr_backend, c, WORKSTR);


         /*** Now look at this printer from the status database ***/

         strcpy( tmp, status( curr_device, ""));


         c = strtok( tmp, SEP);  pid = 0; sscanf( c, "%d", &pid);

         if (pid > 0 && kill( pid, 0)) c = NULL;

         switch (display_type) {
         case PID_DISPLAY:
            if (c != NULL) {
               c = strtok( NULL, SEP);
               if (c) sprintf( timestr, "%d|%s ", pid, (c+2));

               if ((strlen( request_list)+strlen( timestr))
                     < sizeof( request_list))
                  strcat( request_list, timestr);
            }
         break;

         case DEVICE_DISPLAY:
            printf("device for %s: %s\n", curr_printer, curr_device);
         break;

         case PRINTER_DISPLAY:
            *request = 0; if ( d = strtok( NULL, SEP)) strcpy( request, d);

            d = strtok( NULL, SEP);

            if ( c == NULL || d == NULL || strcmp(d, curr_printer))
               printf("printer %s is idle\n", curr_printer);
            else {
               printf("printer %s is printing request #", curr_printer);
               printf("%s, ", &request[2]);

               if ( c = strtok( NULL, SEP))
                  printf("%s%% sent to device", c);

               if ( c = strtok( NULL, ","))
                  printf(", copy %s", c);
   
               printf("\n");
            }
         break;

         case REQUEST_DISPLAY:
            if ( c != NULL) {
               strcpy( request, strtok( NULL, SEP));
               sprintf( rpath, "%s/%s", QUEUE_PATH, request);

               if ((c = strtok( NULL, SEP)) && !strcpy( c, curr_printer)) {   
              
                  if ( (req = open( rpath, O_RDONLY)) != BAD) {
                     header = calloc( HEADER_SIZE, 1);
                     read( req, header, HEADER_SIZE);
                     close( req);

                     strcpy( user, headerval( header, H_USER, ""));
                     unpadd( user);

                     strcpy( timestr, headerval( header, H_DSTAMP, ""));
                     rtime = 0L; sscanf( timestr, "%ld", &rtime);
                     strcpy( timestr, ctime(&rtime));
                     timestr[strlen(timestr)-1] = 0;

                     free( header);

                     printf( "%s-%s\t\t%c\t%s", user,
                        (request+2), *(request+1), timestr);

                     printf( "\ton %s\n", curr_printer);


                     sprintf( timestr, "%s|", request);
                     if ((strlen( request_list)+strlen( timestr))
                          < sizeof( request_list))
                        strcat( request_list, timestr);
                  }
               }                  
            }

         break;
         }
   }

   each_printer( E_END);

   return( request_list);
}



/******************************************************************************
   Cancel currently printing requests (not pending ones!).  Kill only the ones
   I own.  If I'm superuser, then I own it all.
******************************************************************************/

int kill_printing_request( seqnum, sig, flag)
int  seqnum;
int  sig;
int  flag;
{
   int   pid, seq, sucessfull = FALSE;
   char *list, *t;

   list = report_printer_status( PID_DISPLAY);

   t = strtok( list, " ");

   do {
      if (t) {
         sscanf( t, "%d|%d", &pid, &seq);

         if ( flag || (seq == seqnum)) 
            if (do_i_own_it( seqnum)) {
               kill( pid, sig);
               sucessfull = TRUE;
            }

         t = strtok( NULL, " ");
      }
   } while (t);

   if (flag) log("Abort Currently Printing Requests");

   return( sucessfull);
}




/*****************************************************************************
   This routine scans the spool queue looking for request that need to be
   removed.  That is, they need to die of old age!  There are four classes
   of longevity: (R) reprint, (T) temporary, (S) shortterm, and (L) longterm.
   R requests are really just headers and they are deleted as soon as they
   have despooled.  This routine handles the other three classes.
*****************************************************************************/

#define THOURS 0.5
#define SHOURS 24.0
#define LHOURS (7.0 * 24.0)

void grim_reaper()
{
   long   temporary, shortterm, longterm, current, dstamp, elapst;
   float  thours, shours, lhours;
   char  *entries, *name, header[HEADER_SIZE], path[WORKSTR], life[2];
   char   desc[WORKSTR];
   int    n, fd, die;

   thours = THOURS; sscanf( controls( TLIFE, ""), "%f", &thours);
   shours = SHOURS; sscanf( controls( SLIFE, ""), "%f", &shours);
   lhours = LHOURS; sscanf( controls( LLIFE, ""), "%f", &lhours);

   temporary = (long) 3600.0 * thours;
   shortterm = (long) 3600.0 * shours;
   longterm  = (long) 3600.0 * lhours;

   time( &current);

   entries  = dirlist( QUEUE_PATH, FALSE);  /* return sorted spool queue list */
                                            /* This list is a two dimensional */
                                            /* array (entries x MAXNAMLEN) */
                                            /* Ascending order */

   for ( n = 0; *(name = (entries+(n * MAXNAMLEN))); n++) {
      if ( *name == 'R') continue;

      sprintf( path, "%s/%s", QUEUE_PATH, name);

      if ( fd = open( path, O_RDONLY)) {
         read( fd, header, HEADER_SIZE);
         dstamp = 0L; 
         sscanf( headerval( header, H_DSTAMP,    ""), "%ld", &dstamp);
         sscanf( headerval( header, H_LONGEVITY, ""), "%1s", life);
         strcpy( desc, headerval( header, H_DESC, ""));
         unpadd( desc);
         close( fd);

         elapst = current - dstamp;    die = FALSE;

         switch (*life) {
            case 'T': if (elapst > temporary) die = TRUE; break;
            case 'S': if (elapst > shortterm) die = TRUE; break;
            case 'L': if (elapst > longterm)  die = TRUE; break;
         }

         if (die) { 
            log( "Request #%s \"%s\" expired.", &name[2], desc);
            unlink( path);
            /* printf("unlink %s\n", path); */
         }
      }
   }

   free( entries);
}



/******************************************************************************
  ftok() implements Unix's function of the same name.  Given a valid pathname
  and a project identifier, return a "unique" handle.  This handle is used by
  the Unix IPC.  If the path is not found, a -1 is returned.  For some reason,
  Coherent does not implement this function.
******************************************************************************/

key_t ftok( path, proj)
char *path;
char  proj;
{
   struct stat s;

   if (stat( path, &s) == BAD) return( (key_t) -1L);

   return( (key_t) (s.st_ino | (proj << 16) | (s.st_dev << 24)));
}


/******************************************************************************
   MLP_Lock.  This routine can lock several critical sections involving MLP
   process.  These are: Cancel, Controls and Status.  The controls and status
   files need to be locked while they are modified.  The Cancel lock keeps
   processes that cancel requests out of the hair of the despooler and vice-
   versa.

   ***NOTE***
   Because Mark Williams has not implemented SEM_UNDO, we will fall back to
   lockfiles for safety reasons.
******************************************************************************/

int locks[IPC_SEMS];    /* contains lockfile fd's */

void MLP_lock( which)
int which;
{
   char path[WORKSTR];

   sprintf( path, "%s/LCK..%d", QUEUE_PATH, which);

   locks[which] = open( path, O_RDWR); lockf( locks[which], F_LOCK, 0L);

/* SEM_UNDO is not yet implemented...

   static struct sembuf lcancel[2]  = { 0,0,0,  0,1,SEM_UNDO };
   static struct sembuf lcontrol[2] = { 1,0,0,  1,1,SEM_UNDO };
   static struct sembuf lstatus[2]  = { 2,0,0,  2,1,SEM_UNDO };
   static struct sembuf llog[2]     = { 3,0,0,  3,1,SEM_UNDO };

   struct sembuf *s;
   key_t k;
   int   sID;

   k = ftok( IPC_NAME, 1);

   if ((sID = semget( k, IPC_SEMS, IPC_PERMS|IPC_CREAT|IPC_EXCL)) == BAD) {
      if ((sID = semget( k, IPC_SEMS, 0)) == BAD) {
         return;
      }
   }

   switch (which) {
      case L_CANCEL:  semop( sID, lcancel,  2); break;
      case L_CONTROL: semop( sID, lcontrol, 2); break;
      case L_STATUS:  semop( sID, lstatus,  2); break;
      case L_LOG:     semop( sID, llog,     2); break;
   }
*/
}



void MLP_unlock( which)
int which;
{
   char path[WORKSTR];

   sprintf( path, "%s/LCK..%d", QUEUE_PATH, which);

   lockf( locks[which], F_ULOCK, 0L); close( locks[which]); unlink( path);

/* SEM_UNDO is not yet implemented...

   static struct sembuf lcancel[1]  = { 0,-1, IPC_NOWAIT|SEM_UNDO };
   static struct sembuf lcontrol[1] = { 1,-1, IPC_NOWAIT|SEM_UNDO };
   static struct sembuf lstatus[1]  = { 2,-1, IPC_NOWAIT|SEM_UNDO };
   static struct sembuf llog[1]  =    { 3,-1, IPC_NOWAIT|SEM_UNDO };

   key_t k;
   int   sID;

   k = ftok( IPC_NAME, 1);

   if ((sID = semget( k, IPC_SEMS, IPC_PERMS|IPC_CREAT|IPC_EXCL)) == BAD) {
      if ((sID = semget( k, IPC_SEMS, 0)) == BAD) {
         return;
      }
   }

   switch (which) {
      case L_CANCEL:  semop( sID, lcancel,  1); break;
      case L_CONTROL: semop( sID, lcontrol, 1); break;
      case L_STATUS:  semop( sID, lstatus,  1); break;
      case L_LOG:     semop( sID, llog,     1); break;
   }
*/
}
