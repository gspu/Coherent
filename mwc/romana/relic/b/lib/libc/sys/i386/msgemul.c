/* Copyright (c) Bureau d'Etudes Ciaran O'Donnell,1987,1990,1991 */
#define	_MSGGET	0
#define	_MSGCTL	1
#define	_MSGRCV	2
#define	_MSGSND	3

msgget(key, msgflg)
{
	return _msgsys(_MSGGET, key, msgflg);
}

msgctl(msgqid, cmd, buf)
{
	return _msgsys(_MSGCTL, msgqid, cmd, buf);
}

msgrcv(msgqid, msgp, msgsz, msgtyp, msgflg)
{
	return _msgsys(_MSGRCV, msgqid, msgp, msgsz, msgtyp, msgflg);
}

msgsnd(msgqid, msgp, msgsz, msgflg)
{
	return _msgsys(_MSGSND, msgqid, msgp, msgsz, msgflg);
}
