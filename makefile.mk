#=================================================
#
# makefile.mk - build file for TCP Snooper CDA and netstat command line tool
#
# Copyright (C) 2004-2006 Kelvin Sherlock
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#=================================================
#
# 2006.12.02 KS - Initial release as open source
#
#=================================================

#for use with dmake(1)
# for compiling netstat and tcpsnooper.cda

CFLAGS	+=  $(DEFINES) -v #-O

netstat: netstat.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

tcpsnooper.cda: snooper.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@
	chtyp -t cda $@

clean:
	$(RM) *.o *.root *.r

clobber: clean
	$(RM) -f netstat tcpsnooper
