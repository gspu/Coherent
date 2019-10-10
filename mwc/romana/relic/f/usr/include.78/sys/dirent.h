struct dirent {
	long	d_ino;	/* i-node number */
	daddr_t	d_off;	/* offset in actual directory*/
	unsigned short	d_reclen;  /*record length*/
	char	d_name[1];
};
