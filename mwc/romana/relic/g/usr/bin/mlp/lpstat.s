:
# MLP script to report the despooler's status in a Unix V manner.
#
# The following options are not supported:
#
#    -a[list]  There is no such thing as a printer's "acceptance" status.
#    -c[list]  Printers are not lumped into classes.
#

MLPDIR=/usr/spool/mlp
All=""

for Next in $*      # See if they want everything...
do
   case "$Next" in
      -t) All=" -r -d ";;
   esac
done



for Next in $* $All    # Process the various arguments...
do
   case "$Next" in
      -d)  # Print the system default destination

           dest=`awk -F= '/^[ ]*(DEFAULT|default)/ { print $2; next }
                ' $MLPDIR/controls`

           echo "system default destination: $dest";;


      -r)  # Print the despooler (scheduler) status

           sched1=`ps -xfn | awk '{ if ($3 == "despooler") { print $2 }}'`

           sched2=`awk -F= '/^[ ]*(DESPOOLER|despooler)/ { i = $2; 
                printf "%d", i; next } ' $MLPDIR/status`

           pid=`expr "$sched2" + 0`

           if [ "$sched1" = "" ]    # despooler's PID will never be -1
           then
              sched1=-1
           fi

           if test "$pid" -ne "$sched1"
           then
              echo "scheduler is not running"
           else
              echo "scheduler is running"
           fi;;


      -o)  # Print the status of each printer
           awk -F= '/^[ ]*(PRINTER|printer)/ {
                       split($2,ln,","); d = ln[2]
                       dev[d] = ln[1];

                       printf "---> ln[%s] = %s\n", d, dev[d]; next }
                    { lines[$1] = $0 }
           END {
              for (n in dev) {
                 printf "%s (%s) ", dev[n], lines[n]

                 if ( lines[n] != "") 
                    printf "%s\n", lines[n] 
                 else 
                    printf "is idle\n" 
              }
           }' $MLPDIR/controls $MLPDIR/status 
           ;;
           
   esac
done
