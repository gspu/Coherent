
typedef	struct	NODE	{
	struct	NODE	*n_left;
	struct	NODE	*n_right;
	union {
		int	(*n_fun)();
		int	n_op;
	}	n_un;
	char	*n_s1;
	char	*n_s2;
}	NODE;

