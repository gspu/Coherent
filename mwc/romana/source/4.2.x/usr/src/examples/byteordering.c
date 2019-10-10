main()
{
	union
	{
		char b[4];
		int i[2];
		long l;
	} u;
	u.l = 0x12345678L;

	printf("%x %x %x %x\n",
		u.b[0], u.b[1], u.b[2], u.b[3]);
	printf("%x %x\n", u.i[0], u.i[1]);
	printf("%lx\n", u.l);
}
