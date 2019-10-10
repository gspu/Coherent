

if                           Command                           if




Conditional command execution

iiff _s_e_q_u_e_n_c_e_1
tthheenn _s_e_q_u_e_n_c_e_2
[eelliiff _s_e_q_u_e_n_c_e_3
tthheenn _s_e_q_u_e_n_c_e_4] ...
[eellssee _s_e_q_u_e_n_c_e_5]
fi

The shell construct if executes commands conditionally, depending
on the exit status of the execution of other commands.

First, if executes the commands in sequence1.  If the exit status
is zero,  it executes the  commands in sequence2  and terminates.
Otherwise, it  executes the optional sequence3  if given, and ex-
ecutes sequence4  if the exit status is  zero.  It executes addi-
tional elif clauses similarly.  If the exit status of each tested
command sequence  is nonzero, it executes  the optional else part
sequence5.

Because the shell recognizes a reserved word only as the unquoted
first  word of  a command,  each  then, elif,  else, and  fi must
either occur  unquoted at the start  of a line or  be preceded by
`;'.

The shell executes if directly.

***** Example *****

For an example of this command, see the entry for trap.

***** See Also *****

commands, sh, test






















COHERENT Lexicon                                           Page 1


