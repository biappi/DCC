CC = gcc -g -O -D__UNIX__

OBJS=dcc.o ast.o backend.o bundle.o chklib.o comwrite.o control.o dataflow.o \
	disassem.o error.o fixwild.o frontend.o graph.o hlicode.o icode.o \
	idioms.o locident.o parser.o perfhlib.o procs.o proplong.o \
	reducible.o scanner.o symtab.o udm.o
dcc: $(OBJS)
	$(CC) $(OBJS) -o $@

%.o: %.cpp
	$(CC) -c -o $@ $<

