#include <stdio.h>
#include <stdarg.h>

void numprint(char* format ...)
{
	va_list ap;
	va_start(ap, format);

	for (char* p=format; *p; p++)
	{
	  if (*p == '%')
	  {
	    switch (*++p)
	    {
	      case 'd' :
		int ival = va_arg(ap, int);
		printf("%d",ival);

	      case 'f' :
		double dval = va_arg(ap, double);
		printf("%f",dval);
	    }
	  }
	  else printf("%c", *p);
	}
	va_end(ap);
}
