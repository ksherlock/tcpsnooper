

.PHONY: all

all: tcpsnooper.cda netstat

tcpsnooper.cda : o/snooper.a 
	iix -DKeepType=cda link o/snooper keep=tcpsnooper.cda

netstat : o/netstat.a
	iix link o/netstat keep=netstat

o:
	mkdir o


o/snooper.a : snooper.c | o
	iix compile snooper.c keep=o/snooper

o/netstat.a : netstat.c | o
	iix compile netstat.c keep=o/netstat

# no longer used.

cda.mac: cda.asm
	iix macgen cda.asm cda.mac 13:ainclude:m16.= 13:orcainclude:m16.=

o/cda.a : cda.asm | cda.mac o
	iix compile cda.asm keep=o/cda

o/debug.a : debug.c ur.c | o
	iix compile debug.c keep=o/debug