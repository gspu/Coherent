/*
 * libc/sys/i386/sememul.c
 */

#define _SEMCTL	0
#define _SEMGET	1
#define _SEMOP	2

semctl(semid, semnum, cmd, arg)
{
	return _semsys(_SEMCTL, semid, semnum, cmd, arg);
}

semget(key, nsems, semflg)
{
	return _semsys(_SEMGET, key, nsems, semflg);
}

semop(semid, sops, nsops)
{
	return _semsys(_SEMOP, semid, sops, nsops);
}

/* end of libc/sys/i386/sememul.c */
