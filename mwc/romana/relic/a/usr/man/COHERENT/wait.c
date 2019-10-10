

wait                         Command                         wait




Await completion of background process

wwaaiitt [_p_i_d]

Typing the  character `&' after  a command tells the  shell sh to
execute it  as a background (or  detached) process; otherwise, it
is executed as a foreground process.  You can perform other tasks
while a  background process is being  executed.  The shell prints
the process  id number of each background process  when it is in-
voked.  ps reports on currently active processes.

The command  wait tells the shell to  suspend execution until the
child  process with  the given  pid is completed.   If no  pid is
given, wait suspends execution until all background processes are
completed.   If the  process with  the given pid  is not  a child
process of the current shell, wait returns immediately.

The shell executes wait directly.

***** See Also *****

commands, ps, sh

***** Notes *****

If a  subshell invokes a background  process and then terminates,
wait will return  immediately rather than waiting for termination
of the grandchild process.




























COHERENT Lexicon                                           Page 1


