

.PHONY: all

all: tcpsnooper.cda netstat ifconfig

.PHONY: clean

clean:
	rm -rf o

tcpsnooper.cda : o/snooper.a 
	iix -DKeepType=cda link $(^:.a=) keep=$@

netstat : o/netstat.a
	iix link $(^:.a=) keep=$@

ifconfig : o/ifconfig.a
	iix link $(^:.a=) keep=$@

o:
	mkdir o

o/%.a : %.c | o
	iix compile $< keep=o/$*

o/%.a : %.asm | o
	iix compile $< keep=o/$*


# o/snooper.a : snooper.c | o
# 	iix compile snooper.c keep=o/snooper

# o/netstat.a : netstat.c | o
# 	iix compile netstat.c keep=o/netstat


# no longer used.

cda.mac: cda.asm
	iix macgen cda.asm cda.mac 13:ainclude:m16.= 13:orcainclude:m16.=

# o/cda.a : cda.asm | cda.mac o
# 	iix compile cda.asm keep=o/cda

# o/debug.a : debug.c ur.c | o
# 	iix compile debug.c keep=o/debug
