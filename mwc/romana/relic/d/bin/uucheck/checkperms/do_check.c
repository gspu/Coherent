/* do_check.c - Execute a file check for the current file context.
 * part of uucheck
 */
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include "y.tab.h"
#include "checkperms.h"

void
do_check(message_type, check_type, optional_message)
	int message_type;
	int check_type;
	char *optional_message;
{
	switch(check_type) {
	case CHMOD_PERMISSION:
		if(!it_exists) break; /* Skip this if the file does not exist.  */
		/* NB: chmod_chk has LOTS of output side effects.  */
		if(!chmod_chk(current_path, chmod_string, &current_stat, message_type)) {
			notice(message_type, optional_message, basename(current_path));
		}
		break; /* CHMOD_PERMISSION */
	case EXIST:
		if(!it_exists){
			notice(message_type, "%s does not exist.", current_path);
			notice(message_type, optional_message, basename(current_path));
		} /* if it does not exist.  */
		break; /* EXIST */
	case FILE_TOKEN:
		if(!it_exists) { /* Try to fix non-existence. */
			sprintf(command, "touch %s", current_path);
			sprintf(bigbuf, "You can fix it with \"%s\".\n", command);
			VERBOSE(bigbuf);
			FIX(system(command); it_exists = TRUE;);
		} /* if !it_exists */
		if (FMODE(current_stat)&S_IFREG == 0){
			notice(message_type, "%s is not a regular file.", current_path);
			notice(message_type, optional_message, basename(current_path));	
			VERBOSE("You might consider reinstalling everything.\n");
		}	
		break; /* FILE_TOKEN */
	case DIRECTORY:
		if(!it_exists) { /* Try to fix non-existence. */
			sprintf(command, "mkdir %s", current_path);
			sprintf(bigbuf, "You can fix it with \"%s\".\n", command);
			VERBOSE(bigbuf);
			FIX(system(command); it_exists = TRUE; );
		} /* if !it_exists */
		if (FMODE(current_stat)&S_IFDIR == 0){
			notice(message_type, "%s is not a directory.", current_path);
			notice(message_type, optional_message, basename(current_path));	
			VERBOSE("You might consider reinstalling everything.\n");
		}	
		break; /* DIRECTORY */
	case PIPE:
		if(!it_exists) { /* Try to fix non-existence. */
			sprintf(command, "mknod %s p", current_path);
			sprintf(bigbuf, "You can fix it with \"%s\".\n", command);
			VERBOSE(bigbuf);
			FIX(system(command); it_exists = TRUE; );
		} /* if !it_exists */
		if (FMODE(current_stat)&S_IFPIP == 0){
			notice(message_type, "%s is not a pipe.", current_path);
			notice(message_type, optional_message, basename(current_path));	
			VERBOSE("You might consider reinstalling everything.\n");
		}	
		break; /* PIPE */
	case CHARACTER_SPECIAL:
		if(!it_exists) { /* Try to fix non-existence. */
			sprintf(command, "mknod %s c %d %d", current_path,
					  major, minor);
			sprintf(bigbuf, "You can fix it with \"%s\".\n", command);
			VERBOSE(bigbuf);
			FIX(system(command); it_exists = TRUE; );
		} /* if !it_exists */
		if(!it_exists) break; /* Skip this if the file does not exist.  */
		if (FMODE(current_stat)&S_IFCHR == 0){
			notice(message_type, "%s is not a character special device.", current_path);
			notice(message_type, optional_message, basename(current_path));	
			VERBOSE("You might consider reinstalling everything.\n");
		}	
		break; /* CHARACTER_SPECIAL */
	case BLOCK_SPECIAL:
		if(!it_exists) { /* Try to fix non-existence. */
			sprintf(command, "mknod %s b %d %d", current_path,
					  major, minor);
			sprintf(bigbuf, "You can fix it with \"%s\".\n", command);
			VERBOSE(bigbuf);
			FIX(system(command); it_exists = TRUE; );
		} /* if !it_exists */
		if(!it_exists) break; /* Skip this if the file does not exist.  */
		if (FMODE(current_stat)&S_IFBLK == 0){
			notice(message_type, "%s is not a block special device.", current_path);
			notice(message_type, optional_message, basename(current_path));	
			VERBOSE("You might consider reinstalling everything.\n");
		}	
		break; /* BLOCK_SPECIAL */
	case OWNER:
		if(!it_exists) break; /* Skip this if the file does not exist.  */
		if(!owner_chk(current_path,
			      id_string,
			      &current_stat,
			      message_type,
			      optional_message)) {
			notice(message_type, optional_message, basename(current_path));
		} /* if owner does not check out.  */	
		break; /* BLOCK_SPECIAL */		break; /* OWNER */
	case GROUP:
		if(!it_exists) break; /* Skip this if the file does not exist.  */
		if(!group_chk(current_path,
			      id_string,
			      &current_stat,
			      message_type,
			      optional_message)) {
			notice(message_type, optional_message, basename(current_path));
		} /* if owner does not check out.  */	
		break; /* GROUP */
	default:
		FATAL("Impossible check type: %d\n", check_type);
	} /* switch check_type */
} /* do_check() */


boolean
owner_chk(path, id_string, statptr, msg_type, optional_message)
	unsigned char *path;
	unsigned char *id_string;
	struct stat *statptr;
	int msg_type;	/* Error messages or just warning messages?  */
	char *optional_message;
{
	struct passwd *tmp_pwd;

	/* fill in statptr if it isn't already */
	if (statptr == (struct stat *) NULL) {
		statptr = (struct stat *) malloc(sizeof(struct stat));
		stat(path, statptr);
	}

	/* Translate the expected user name to a uid.  We want to
	 * do this instead of converting the uid in the stat struct to a
	 * string, because more than one user could have the same uid.
	 * With "uucp" this even happens frequently.
	 */
	if ((tmp_pwd = getpwnam(id_string)) == NULL) {
		sprintf(bigbuf, "User %s does not exist!\n", id_string);
		ERROR(bigbuf);
		VERBOSE("Use the newusr command to add this user.\n");
		FATAL(NULL, NULL);
	}

	if (tmp_pwd->pw_uid != statptr->st_uid) {
		sprintf(bigbuf, "%s is not owned by %s.", path, id_string);
		notice(msg_type, bigbuf, NULL);
		notice(msg_type, optional_message, basename(path));
		sprintf(command, "chown %s %s", id_string, path);
		sprintf(bigbuf, "Use \"%s\" to fix this.\n", command);
		VERBOSE(bigbuf);
		FIX(system(command););
	} /* if (id != statptr->st_uid) */

} /* owner_chk() */

boolean
group_chk(path, id_string, statptr, msg_type, optional_message)
	unsigned char *path;
	unsigned char *id_string;
	struct stat *statptr;
	int msg_type;	/* Error messages or just warning messages?  */
	char *optional_message;
{
	struct group *tmp_grp;


	/* fill in statptr if it isn't already */
	if (statptr == (struct stat *) NULL) {
		statptr = (struct stat *) malloc(sizeof(struct stat));
		stat(path, statptr);
	}

	/* Translate the expected group name to a gid.  We want to
	 * do this instead of converting the gid in the stat struct to a
	 * string, because more than one group could have the same gid.
	 */
	if ((tmp_grp = getgrnam(id_string)) == NULL) {
		sprintf(bigbuf, "Group %s does not exist!\n", id_string);
		ERROR(bigbuf);
		VERBOSE("Edit /etc/group to add this group.\n");
		FATAL(NULL, NULL);
	}

	if (tmp_grp->gr_gid != statptr->st_gid) {
		sprintf(bigbuf, "%s is not group %s.", path, id_string);
		notice(msg_type, bigbuf, NULL);
		notice(msg_type, optional_message, basename(path));
		sprintf(command, "chgrp %s %s", id_string, path);
		sprintf(bigbuf, "Use \"%s\" to fix this.\n", command);
		VERBOSE(bigbuf);
		FIX(system(command););
	} /* if (id != statptr->st_uid) */

} /* group_chk() */
