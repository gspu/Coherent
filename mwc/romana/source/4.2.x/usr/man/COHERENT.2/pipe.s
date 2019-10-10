ppiippee() -- System Call (libc)

Open a pipe
#iinncclluuddee <uunniissttdd.hh>
iinntt ppiippee(_f_d)
iinntt _f_d[22];

A _p_i_p_e is an  interprocess communication mechanism.  ppiippee() creates a pipe,
typically to construct pipelines in the shell sshh.

ppiippee()  fills in  _f_d[00] and  _f_d[11]  with _r_e_a_d  and _w_r_i_t_e  file descriptors,
respectively.  The file descriptors allow  the transfer of data from one or
more writers  to one or more  readers.  Pipes are buffered  to 5,120 bytes.
If more than  5,120 bytes are written into the  pipe, the wwrriittee() call will
not return until the reader has  removed sufficient data for the wwrriittee() to
complete.  If a  rreeaadd() occurs on an empty pipe,  its completion awaits the
writing of data.

When all  writing processe close  their write file  descriptors, the reader
receives an  end of file indication.   A write on a  pipe with no remaining
readers generates a SSIIGGPPIIPPEE signal to the caller.

ppiippee() is  generally called just  before ffoorrkk(). Once the  parent and child
processes are created, the unused file descriptors should be closed in each
process.

_E_x_a_m_p_l_e
The  following example  prints the  word WWaaiittiinngg  until a  line of  data is
entered.  It illustrates how to use ppiippee(), ffssttaatt(), and ffoorrkk().

#include <stdio.h>
#include <sys/stat.h>       /* for stat */
#include <sgtty.h>      /* for stty/gtty functions */
#include <unistd.h>

static int fd[2];       /* pipe array */

main()
{
    printf("This prints 'Waiting' every second until a 'q' is hit.\n");

    /*
     * Pipe may also be constructed by /etc/mknod
     * If it is desired to have tasks communicate where
     * they are not parent and child. In this case make
     * sure the constructed pipe has the correct owner and
     * permissions. Such pipe may be used exactly like this
     * but open()ed on each side.
     */

    if (-1 == pipe(fd)) {
        fprintf(stderr, "Cannot open pipe\n");
        exit(EXIT_FAILURE);
    }

    if (fork())
        parentProcess();
    else
        childProcess();
    exit(EXIT_SUCCESS);
}

parentProcess()
{
    struct stat s;
    char buff;

    for (buff = ' '; 'q' != buff;) {
        fstat(fd[0], &s);   /* get status of pipe */
        if (s.st_size) {        /* char in the pipe */
            read(fd[0], &buff, sizeof(buff));
            printf("Got a '%c'\n", buff);
            continue;
        }

        /*
         * This can be any process, it can use system()
         * or exec()
         */
        printf("Waiting\n");
        sleep(1);
    }
}

childProcess()
{
    struct  sgttyb  os, ns;
    char buff;

    gtty(fileno(stdin), &os);   /* save old state */
    ns = os;            /* get base of new state */
    ns.sg_flags |= RAW;     /* process each character as entered */
    ns.sg_flags &= ~(ECHO|CRMOD);   /* no echo for now... */
    stty(fileno(stdin), &ns);   /* set mode */

    do {
        buff = getchar();   /* wait for the keyboard */
        write(fd[1], &buff, sizeof(buff));
    } while ('q' != buff);

    stty(fileno(stdin), &os);   /* reset mode */
}

_S_e_e _A_l_s_o
cclloossee(),  lliibbcc,   lliibbssoocckkeett,  mmkkffiiffoo(),  mmkknnoodd(),   rreeaadd(),  sshh,  ssiiggnnaall(),
uunniissttdd.hh, wwrriittee()
POSIX Standard, section 6.1.1

_D_i_a_g_n_o_s_t_i_c_s
ppiippee() returns zero  on successful calls, or -1 if  it could not create the
pipe.

If it is  necessary to create a pipe between  tasks that are not parent and
child, use  /eettcc/mmkknnoodd to create  a named pipe.   These named pipes  can be
opened and used by  different programs for communication.  Remember to give
them the correct owner and permissions.

If you  attempt to open a  pipe write only, OO_NNDDEELLAAYY is  set, and there are
currently  no readers  on this  pipe, ooppeenn()  returns immediately  and sets
eerrrrnnoo to EENNXXIIOO.
