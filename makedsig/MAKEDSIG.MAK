CFLAGS = -Zi -c -AL -D__MSDOS__

makedsig.exe:	makedsig.obj perfhlib.obj fixwild.obj
	link /CO makedsig perfhlib fixwild;

makedsig.obj:	makedsig.c dcc.h perfhlib.h
	cl $(CFLAGS) $*.c

perfhlib.obj:	perfhlib.c dcc.h perfhlib.h
	cl $(CFLAGS) $*.c

fixwild.obj:	fixwild.c dcc.h perfhlib.h
	cl $(CFLAGS) $*.c

