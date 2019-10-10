/******************************************************************************
   MLP Spooler header
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <signal.h>
#include <dirent.h>
#include <varargs.h>
#include <errno.h>
#include <pwd.h>

#define FPERMS 012

#define BACKEND_PATH  "/usr/spool/mlp/backend"
#define ROUTE_PATH    "/usr/spool/mlp/route"
#define QUEUE_PATH    "/usr/spool/mlp/queue"
#define CONTROL_PATH  "/usr/spool/mlp/controls"
#define STATUS_PATH   "/usr/spool/mlp/status"
#define LOG_PATH      "/usr/spool/mlp/log"
#define SPOOL_ENV     "MLP_SPOOL"
#define PRIORITY_ENV  "MLP_PRIORITY"
#define FORMLEN_ENV   "MLP_FORMLEN"
#define COPIES_ENV    "MLP_COPIES"
#define LONGEVITY_ENV "MLP_LIFE"
#define SEQNUM        "seqnum"
#define SYSTEM        "system"
#define DISPLAY       "display"
#define LOCAL         "local"
#define DESPOOLER     "despooler"
#define LOCALFEED     "localfeed"
#define FEED          "feed"
#define DOCOPIES      "docopies"
#define DEFAULT       "default"
#define TLIFE         "templife"
#define SLIFE         "shortlife"
#define LLIFE         "longlife"
#define DBACKEND      "/bin/cat"

#define HEADER_SIZE   192
#define DESC_LEN      45
#define WORKSTR       128

#define TRUE          1
#define FALSE         0
#define BAD          -1

#define BUG(x,y) {fprintf(stderr,"%s : %s\n", x, y); fflush(stderr);}

/* Header Defines */

#define HSET        TRUE
#define HGET        FALSE
#define H_USER      0
#define H_PRINTER   1
#define H_TYPE      2
#define H_FORMLEN   3
#define H_PAGES     4
#define H_DBASE     5
#define H_FORMAT    6
#define H_DSTAMP    7
#define H_DESC      8
#define H_LONGEVITY 9
#define H_COPIES    10
#define H_MAILME    11
#define H_WRITEME   12
#define H_ENTIRE    99


#define PRINTER_DISPLAY 0
#define REQUEST_DISPLAY 1
#define DEVICE_DISPLAY  2
#define PID_DISPLAY     3


#define R_KILL    SIGUSR1
#define R_CANCEL  SIGUSR2

#define A_LIFETIME 0
#define A_PRINTER  1

#define CLEAR_ALL TRUE
#define CLEAR_ONE FALSE

#define SEP  "=,\t \n"

#define E_START   0
#define E_END     1
#define E_NEXT    2
#define E_REWIND  3
#define E_FILE    4
#define E_UNLOCK  5

#define  IPC_NAME   "/usr/spool/mlp"
#define  IPC_PERMS  0666
#define  IPC_SEMS   4
#define  L_CANCEL   0
#define  L_CONTROL  1
#define  L_STATUS   2
#define  L_LOG      3 

extern char *getenv();
extern void  fatal();
extern void  warn();
extern int   rename();
extern char *header();
extern void  get_termcap();
extern char *datestamp();
extern char *uppercase();
extern char *controls();
extern char *status();
extern char *route_request();
extern int   get_seq_num();
extern char *request_filename();
extern void  local_print();
extern char *dirlist();
extern void  wakeup_despooler();
extern char *headerval();
extern char *unpadd();
extern char *basename();
extern int   scheduler_status();
extern int   writeOut();
extern char *request_name();
extern char *report_printer_status();
extern void  cancel_request();
extern int   kill_printing_request();
extern void  make_inactive();
extern char *each_printer();
extern void  MLP_lock();
extern void  MLP_unlock();
