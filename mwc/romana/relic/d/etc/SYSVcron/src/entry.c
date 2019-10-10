/*
 * entry.c
 *
 * Keep track about all children processes.
 *
 */
#include	<stdio.h>
#include	<string.h>
#include	"cron.h"

#define	HUGE	1024		/* Size of the hostname+domain, is it enough */

extern char	*mktemp();	/* Generate temporary name */
extern int	gethostname();	/* Get host name */
extern int	getdomain();	/* Get domain name */

child_id	*add_entry(),	/* Add an new entry to a link list */
		*find_entry(),	/* Find an entry in the link list */
		*del_entry();	/* Remove an entry from the link list */
void		mail_entry();	/* Send mail to user if command failed */

extern child_id	*current;	/* Pointer to the lasr allocated struct. */

/*
 * Add a new entry to the link list. Return pointer to the new entry or
 * NULL if entry cannot be allocated.
 */
child_id *add_entry(pid, u_name, c_time, cmd)
int	pid;		/* Process ID */
char	*u_name;	/* User name */
time_t	c_time;		/* Time when command started */
char	*cmd;		/* Command */
{
	register child_id	*pst;

	/* Structure child_id has one extracharacter at the end */
	if ((pst = (child_id *) malloc(sizeof(child_id) + strlen(cmd))) 
								== NULL) {
		fprintf(stderr, "crond: out of memory\n");
	} else {	/* Write information about child to a table */
		pst->prev = current;
		if (current != NULL)
			current->next = pst;
		current = pst;
		current->next = NULL;
		current->pid = pid;
		strncpy(current->name, u_name, MAX_UNAME - 1);
		current->time = c_time;
		strcpy(current->command, cmd);
		Dprint("add_entry: added ID is %d, ", current->pid);
	}
	return current;
}

/*
 * Find an entry by process id. Return pointer to the entry or
 * NULL if entry not found.
 */
child_id *find_entry(pid)
int	pid;
{
	register child_id	*pst;

	Dprint("find_entry: request ID is %d. List of incore pids:\n", pid);
	for (pst = current; pst != NULL; pst = pst->prev) {
		Dprint(" %d ", pst->pid);
		if (pst->pid == pid)
			break;
	}
	Dprint("find_entry: found ID is %d\n", pst->pid);
	return pst;
}

/*
 * Delete entry. Return pointer to the last allocated entry ("current").
 */
child_id *del_entry(corpse)
child_id	*corpse;	/* Entry that should be deleted */
{
	child_id	*pst;

	Dprint("del_entry: ID should be %d\t", corpse->pid);
	Dprint("next_id is %d\t", corpse->next->pid);
	Dprint("prev_id is %d\n", corpse->prev->pid);

	if (corpse == NULL)
		return current;

	if ((pst = corpse->prev) != NULL) {
		pst->next = corpse->next;
		Dprint("del_entry: prev_id is %d\n", pst->pid);
	}

	if ((pst = corpse->next) != NULL) {
		pst->prev = corpse->prev;
		Dprint("del_entry: next_id is %d\n", pst->pid);
	}

	if (corpse->next == NULL) {
		pst = corpse->prev;
		free(corpse);
		return pst;
	}
	free(corpse);
	return current;
}	

char	tmpFoo[] = "/tmp/cronXXXXXX";	/* Temporary file */

/*
 * Mail entry. Send mail to a user about failed command.
 */
void mail_entry(entry)
child_id	*entry;
{
	char	mailBuf[MAX_STR_LEN];	/* Message buffer */
	char	tmpName[20];		/* Name of the temporary file */
	char	tmpBuf[HUGE];		/* Huge buffer for hostname */
	FILE	*fp;			/* Pointer to a temporary file */
	int	sz;			/* Size of the Hostname + '.' */

	*tmpBuf = 0;	/* Just to have */

	strcpy(tmpName, mktemp(tmpFoo));
	if ((fp = fopen(tmpName, "w+")) == NULL) {
		fprintf(stderr, "crond: cannot open temporary file %s.\n",
							tmpName);
		return;
	}

	if (gethostname(tmpBuf, HUGE))
		strcat(tmpBuf, "");
	else {
		strcat(tmpBuf, ".");
		sz = strlen(tmpBuf);
		if (getdomain(&tmpBuf[sz], HUGE - sz));
			strcat(tmpBuf, "");
	}
	fprintf(fp, "From: CRON@%s\n", tmpBuf);
	fprintf(fp, "Subject: failed command\n\n");
	fprintf(fp, "Time was %sCommand was \"%s\".\n\n", 
			ctime(&(entry->time)), entry->command);
	fprintf(fp, "Have a good one cron!\n");
	rewind(fp);
	sprintf(mailBuf, "smail %s < %s", entry->name, tmpName);
	system(mailBuf);
	fclose(fp);
	unlink(tmpName);
}	

