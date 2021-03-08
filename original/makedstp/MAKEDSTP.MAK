CFLAGS = -Zi -c -AL -D__MSDOS__

makedstp.exe:	makedstp.obj perfhlib.obj fixwild.obj
	link /CO makedstp perfhlib fixwild;

makedstp.obj:	makedstp.c dcc.h perfhlib.h
	cl $(CFLAGS) $*.c

perfhlib.obj:	perfhlib.c dcc.h perfhlib.h
	cl $(CFLAGS) $*.c

fixwild.obj:	fixwild.c dcc.h perfhlib.h
	cl $(CFLAGS) $*.c

