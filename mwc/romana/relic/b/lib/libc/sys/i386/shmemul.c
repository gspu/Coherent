/* Copyright (c) Bureau d'Etudes Ciaran O'Donnell,1987,1990,1991 */
#define	_SHMAT	0
#define	_SHMCTL	1
#define	_SHMDT	2
#define	_SHMGET	3

shmat(shmid, shmaddr, shmflg)
{
	return _shmsys(_SHMAT, shmid, shmaddr, shmflg);
}

shmctl(shmid, cmd, buf)
{
	return _shmsys(_SHMCTL, shmid, cmd, buf);
}

shmdt(shmaddr)
{
	return _shmsys(_SHMDT, shmaddr);
}

shmget(key, size, shmflg)
{
	return _shmsys(_SHMGET, key, size, shmflg);
}
