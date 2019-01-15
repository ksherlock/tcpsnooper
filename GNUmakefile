

.PHONY: all

all: tcpsnooper netstat

tcpsnooper : o/cda.a o/nscda.a o/debug.a
	iix -DKeepType=cda link o/cda o/nscda o/debug keep=tcpsnooper

netstat : o/netstat.a
	iix link o/netstat keep=netstat

o:
	mkdir o

cda.mac: cda.asm
	iix macgen cda.asm cda.mac 13:ainclude:m16.= 13:orcainclude:m16.=


o/cda.a : cda.asm | cda.mac o
	iix compile cda.asm keep=o/cda

o/nscda.a : nscda.c | o
	iix compile nscda.c keep=o/nscda

o/netstat.a : netstat.c | o
	iix compile netstat.c keep=o/netstat

o/debug.a : debug.c ur.c | o
	iix compile debug.c keep=o/debug