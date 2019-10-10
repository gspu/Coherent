Note to Fred:

	You can put a value for the color display mode in the Xconfig
        file.  The default is PsuedoColor.  The value is put in on a
	line by itself.  The value can be one of:

		DirectColor
		TrueColor
		PsuedoColor
		StaticColor
		GrayScale
		StaticGray

	Color tables are essentially limited by what Xfree86 1.2 will
	display.  When is the port of Xfree86 2.0 going to be ready? :-)

	Xv and Xloadimage use the color table that they think will produce
	results.  Xloadimage can be forced to us a particular color table
	but the results can be really strange.
