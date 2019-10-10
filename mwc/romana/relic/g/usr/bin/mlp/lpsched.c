/*****************************************************************************
   MLP-Spool.  Search the print queue (/usr/spool/mlp/queue) for reports
   to
   print.  When necesarry, spawn processes to print these reports on the
   proper printers observing the needs of concurent printing processes.

   Author: B.Blatchley (c) 1992 Magnetic Data Operation
*****************************************************************************/

#include "header.h"
#include <sys/param.h>
#include <sys/stat.h>

#define SLEEPY 30

extern char curr_printer[WORKSTR];
extern char curr_device[WORKSTR];
extern char curr_backend[WORKSTR];
extern char curr_desc[WORKSTR];
extern char curr_user[20];
extern char curr_name[20];
extern char curr_longevity[2];
extern int  curr_writeme;
extern int  curr_mailme;
extern int  curr_copies;
extern int  curr_copy;
extern int  curr_formlen;
extern int  abort_write;
extern int  kill_request;

int our_child;
int pip[2];       /* backend pipe, global so cancel can close it */


/*****************************************************************************
  There are times when this program will sleep, while waiting for new things
  to do.  This process is called when the program receives a "wakeup" signal
  from another process (or from its internal alarm-clock).
*****************************************************************************/

void wakeup()
{
   signal( SIGALRM, wakeup);
}




/*****************************************************************************
   Kill the backend process without placing the request on hold
*****************************************************************************/

void request_kill()
{
   abort_write = TRUE;
   kill_request = TRUE;
   signal( R_KILL, request_kill);
}



/*****************************************************************************
  Kill the backend process but keep the request current (will reprint when
  the despooler scans the the queue).
*****************************************************************************/

void request_cancel()
{
   abort_write = TRUE;
   kill_request = FALSE;
   signal( R_CANCEL, request_cancel);
}




/*****************************************************************************
  Extract Header Values
*****************************************************************************/

int extract_header( name)
char *name;
{
   int  fd;
   char path[WORKSTR], header[HEADER_SIZE], *t;

   sprintf( path, "%s/%s", QUEUE_PATH, name);

   if ( (fd = open( path, O_RDONLY)) != BAD) {

      read( fd, header, HEADER_SIZE);  close( fd); 

      strcpy( curr_name, name);

      t = headerval( header, H_USER, "");
         strcpy( curr_user,    unpadd( t));

      t = headerval( header, H_PRINTER, ""); 
         strcpy( curr_printer, unpadd( t));

      t = headerval( header, H_DESC, ""); 
         strcpy( curr_desc,    unpadd( t));

      t = headerval( header, H_LONGEVITY, ""); 
         strncpy( curr_longevity,  unpadd( t), 1);

      curr_mailme = FALSE; 
      if ( *(headerval( header, H_MAILME, "")) != ' ')
         curr_mailme = TRUE;

      curr_writeme = FALSE; 
      if ( *(headerval( header, H_WRITEME, "")) != ' ')
         curr_writeme = TRUE;

      sscanf( headerval( header, H_COPIES,  ""), "%d", &curr_copies);

      sscanf( headerval( header, H_FORMLEN, ""), "%d", &curr_formlen);

      return(1);
   }
   else
      return(0);
}



/*****************************************************************************
   Return TRUE if the device specified in this request is available
*****************************************************************************/

int device_available( name)
char *name;
{
   int   id;
   char *t, *c;

   if ( extract_header()) {
      printer_device( curr_printer);

      if (!strcmp( curr_device, "")) {  /* No device? */
         t = calloc(1000,1);

         scatf( t, "Printer \"%s\" has no device associated ", curr_printer);
         scatf( t, "with it.\n\nRequest #%s has been suspended.  ", &name[2]);
         scatf( t, "One way to resolve the problem\nwould be to define the ");
         scatf( t, "printer in %s.  Another\nwould be to move ", CONTROL_PATH);
         scatf( t, "the request to a different printer.\n\n");
         scatf( t, "You may need to ask MLP to reprint the request.");

         warning( curr_user, t);
         free( t);

         /* Put the request into the inactive queue */

         make_inactive( name);

         return(0);
      }

      /* find out if the device is being used */

      if (*(t = status( curr_device, ""))) {
         /* Who is using the device? */

         c = strtok( t, SEP);  sscanf( c, "%d", &id); 

         if (!kill( id, 0)) return(0);  /* process is still active */
      }

      return(1);
   }

   return(0);
}




/*****************************************************************************
   Write Request.  Setup a pipeline and spawn a backend process to handle
   the writing of the request.  We do this so that we can monitor to progress
   of the request.
*****************************************************************************/

write_request( name, backend, start, end)
char *name;
char *backend;
int   start;
int   end;
{
   int  source, sink, feed = FALSE, copy, docopies, tmpv;
   char path[WORKSTR], tmp[WORKSTR], copies[10], *t, *u;

   signal( R_KILL,   request_kill);
   signal( R_CANCEL, request_cancel);

   /* Open the print request and the device (data source & sink) */ 

   sprintf( path, "%s/%s", QUEUE_PATH, name);

   if ( ( source = open( path, O_RDONLY)) == BAD) {
      warning_user(curr_user, "Cannot open request file: %s for request %s", path, &name[2]);
      make_inactive( name);
      return;
   }

   if ( ( sink = open( curr_device, O_WRONLY|O_CREAT|O_TRUNC|O_SYNC, 0644)) == BAD) {
      warning_user(curr_user, "Cannot open device %s for request %s", curr_device, &name[2]);
      make_inactive( name);
      return;
   }

   if( pipe( pip) == BAD) {
      warning_user(curr_user, "Cannot create pipe to despool #%s", &name[2]);
      make_inactive( name);
      return;
   }

   switch (fork()) {
   case BAD: warning_user(curr_user, "Cannot fork to despool #%s", &name[2]);
             close( source); close( sink); close( pip[0]); close( pip[1]);
             make_inactive( name);
             return;

   case   0: exit(0);

   default: setpgrp();  /* cut us loose from any control terminals! */

      /* The parent feeds the child, will noting the progress of the feeding
         in the status file -- Inquiring Minds want to know! */

      switch ( our_child = fork()) {
      case BAD: warning_user(curr_user, "Cannot fork to despool #%s", &name[2]); 
                close( source); close( sink); close( pip[0]); close( pip[1]);
                return;

      case   0: close( source); close( pip[1]); dup2( pip[0], 0); dup2( sink, 1);

                signal( SIGTERM, SIG_IGN);
                signal( SIGINT,  SIG_IGN);


                sprintf( copies, "%d", curr_copies);

                if (!strcmp( backend, DBACKEND))
                   execl( backend, backend, NULL);
                else
                execl( "/bin/sh", "/bin/sh", backend, &name[2], curr_user,
                   copies, NULL);
      }


      /* Parent feeds the child... */

      close( pip[0]); signal( SIGCLD, SIG_IGN);

   
      t = controls( FEED, ""); 
      if (!strcmp( t, "ON") || !strcmp( t, "on")) feed = TRUE;

      docopies = 1;
      t = controls( DOCOPIES, "");
      if (!strcmp( t, "ON") || !strcmp( t, "on")) docopies = curr_copies;


      for ( copy = 0; copy < docopies; copy++) {
         curr_copy = copy + 1;

         lseek( source, (long) HEADER_SIZE, 0);

         if (writeOut( source, pip[1], start, end, curr_formlen, TRUE) == BAD) {
            
            sscanf( &name[2], "%d", &tmpv);

            if ( kill_request) {
               sprintf( tmp, "\n\n*** Request #%d Terminated ***\n\f", tmpv);
               warning_user(curr_user, "The despooling of request #%d has been aborted.", tmpv);
            }
            else {
               sprintf( tmp, "\n\n*** Request #%d Canceled ***\n\f", tmpv);
               warning_user(curr_user, "Request #%d has been canceled.", tmpv);
            }
               
            write( pip[1], tmp, strlen(tmp));
            break;
         }

         if ( feed) { *tmp = '\f'; write( pip[1], tmp, 1); }
      }


      close( source); close( pip[1]); close( sink);
 

      /* If the request was aborted, keep it in the active
         part of the queue.  Otherwise, retire it to the
         inactive queue.  If the request is a reprint
         then remove it from the queue. */

      if (!kill_request)
           make_inactive( name);

      wait();


      /* Send Messages if necessary */

      if (curr_writeme || curr_mailme) {
         t = calloc(512,1);
         u = calloc(512,1);

         strcpy( t, "echo \"\n`date`\n\"'\n*** MLP Spooler Reports");
         strcat( t, " ***\n\nYour request, \"");
         strcat( t, curr_desc);
         strcat( t, ",\" ");

         if (strlen( curr_desc) > 15)
            strcat( t, "\n");

         sprintf( tmp, "has been despooled on printer: %s", curr_printer);
         strcat( t, tmp);

         if (curr_writeme) {
            sprintf( u, "%s\n\n'| write %s >/dev/null", t, curr_user);
            system( u);
         }

         if ( curr_mailme) {
            if (getenv("MLPLMAIL"))
               sprintf( u, "%s\n\n'| lmail %s 2>/dev/null", t, curr_user);
            else
               sprintf( u, "%s\n\n'| mail %s 2>/dev/null", t, curr_user);

            system( u);
         }

         free( t); free( u);
      }
   } 

   close( pip[0]);  close( pip[1]);          
}




/*****************************************************************************
   De-spool a given file.
*****************************************************************************/

despool_file( name, start, end)
char *name;
int   start;
int   end;
{
   char path[WORKSTR], tmp[512], newname[20];
   int  backend, r_copies;

   if ( *curr_longevity == 'R') { /* Is this a reprint? */

      /* sample description: "Reprint #00000 from page 0000 to page 0000"
                                    10-|            26-|         34-|     */

      sscanf( &curr_desc[9], "%5s", tmp);
      strcpy( newname, request_name( tmp));

      sscanf( &curr_desc[26], "%d", &start);  
      sscanf( &curr_desc[34], "%d", &end);  

      sprintf( tmp, "%s/%s", QUEUE_PATH, name);
      unlink( tmp);

      r_copies = curr_copies;  strcpy( tmp, curr_printer);
      extract_header( newname);
      curr_copies = r_copies;  strcpy( curr_printer, tmp);
   }
   else
      strcpy( newname, name);


   if (*curr_backend == 0) strcpy( curr_backend, DEFAULT);

   sprintf( path, "%s/%s", BACKEND_PATH, curr_backend);

   if ( (backend = open( path, O_RDONLY)) == BAD) {
      sprintf( tmp, "Cannot find backend script (%s) for ", curr_backend);
      scatf(   tmp, "the %s printer\nwhile trying to ", curr_printer);
      scatf(   tmp, "despool request #%s\n\n", &newname[2]);
      scatf(   tmp, "Despooling done with %s.\n", DBACKEND);
      warning( tmp);
      write_request( newname, DBACKEND, start, end);
   }
   else
      write_request( newname, path, start, end);


   log( "Despooled Request #%s", &name[2]);
}



/*****************************************************************************
   De-spool.   Check the queue for work, then process it.
*****************************************************************************/

void despool()
{
   char *name, *t, *plist, tmp[WORKSTR];
   int      n = 0, parent;


   parent = getpid();

   plist = calloc(1024,1);    /* contains a list of printer in use during
                                 this pass over the spool queue */

   t = dirlist( QUEUE_PATH, FALSE);  /* return sorted spool queue list */
                                     /* This list is a two dimensional array */
                                     /* (entries x MAXNAMLEN) */
                                     /* Ascending order */

   while ( *(name = (t+(n * MAXNAMLEN)))) {

      if ( name[0] == 'R') {               /* despool the "ready" entries */
         if ( device_available( name)) {
            sprintf( tmp, "%s|", curr_device);

            if (!strstr( plist, tmp)) {
               strcat( plist, tmp);

               switch (fork()) {

               case -1: warning("Cannot fork backend process for reqest #%s", &name[2]);
                        break;

               case  0: /* claim the device */

                        sprintf( tmp, "%d, %s", getpid(), name);
                        status( curr_device, tmp);

                        despool_file( name, 0, 0);

                        /* wakeup parent to rescan for work for this device */
                        kill( parent, SIGALRM);

                        exit(0);
               }

               signal( SIGCLD, SIG_IGN);
            }
         }
      }

      n++;
   }

   free( plist); free( t);
}





/******************************************************************************
   Roll the log over after a given time so that it does not grow
   without bounds.
******************************************************************************/

#define RHOURS (7.0 * 24.0)

roll_log()
{
   long   rollover, current;
   struct stat *s;
   float  rhours;
   char   tmp[WORKSTR];

   rhours = RHOURS; sscanf( controls( TLIFE, ""), "%f", &rhours);

   rollover = (long) 3600.0 * rhours;   time( &current);

   stat( LOG_PATH, s);

   if ((s->st_ctime + rollover) < current) {
      sprintf( tmp, "%s.o", LOG_PATH);

      unlink( tmp); rename( LOG_PATH, tmp);

      warning("Logfile moved to log.o, starting new log.", "root");
   }
}




/*****************************************************************************
   Main Driver.   Tell the spooler how to find us.  Then process the queue
   and sleep until something exciting happens ;)  First check to see if
   the despooler is already running.
*****************************************************************************/

void main()
{
   char tmp[10];
   int  child, cycles;

   umask(FPERMS);

   if ( scheduler_status()) {
      printf("MLP Scheduler already started.\n");
      exit(0); 
   }

   switch (child = fork()) {
   case -1: fatal("Scheduler becoming a daemon.  Cannot fork()");

   case 0: /* I am in the background now, and will despool requests */

           chdir("/"); /* so we can dismount any mounted filesystems */

           /* for (fd = 0; fd < NOFILE; fd++) close(fd); */

           umask(0);

           setpgrp();  /* detatch me from my parent process */

           signal( SIGCLD,  SIG_IGN);
           signal( SIGALRM, wakeup);

           sprintf( tmp, "%d", getpid());  status( DESPOOLER, tmp); 

           cycles = 0;

           while ( *(status( DESPOOLER, ""))) {
              MLP_lock( L_CANCEL);

              despool();
              
              if (cycles++ > 10) {
                 grim_reaper(); /* purge the inactive queue */ 
                 cycles = 0;
              }

              MLP_unlock( L_CANCEL);

              sleep( SLEEPY);
           }

           fatal("Scheduler daemon expired gracefully"); 
           break;

   default: printf("MLP Scheduler Started, PID: %d\n", child);
            log("MLP Scheduler Started, PID: %d", child);
            break;
   }
}
