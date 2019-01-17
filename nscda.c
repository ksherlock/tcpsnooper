/*=================================================
 *
 * nscda.c - Code to dump network statistics for CDA
 *
 * Copyright (C) 2004-2006 Kelvin Sherlock
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *=================================================
 *
 * 2006.12.02 KS - Initial release as open source
 *
 *=================================================*/

#pragma lint - 1
#pragma optimize - 1

#pragma cda "TCP Snooper" StartUp ShutDown

#include <intmath.h>
#include <tcpip.h>
#include <tcpipx.h>

#include <memory.h>
#include <misctool.h>
#include <texttool.h>

#include <ctype.h>
#include <stdio.h>

static char buffer[80];

const char Header1[] =
    " Ipid State       Address         sPort dPort RcvQueue  SendQueue\r";
const char Header2[] =
    " ---- -----       -------         ----- ----- --------- ---------\r";
//   xxxxx xxxxxxxxxxx xxx.xxx.xxx.xxx xxxxx xxxxx $xxxxxxxx $xxxxxxxx

void Display(Word ipid, srBuff *srBuffer) {
    static char buffer[80] = "xxxxx "           // 6	ipid
                             "xxxxxxxxxxx "     // 12	state
                             "xxx.xxx.xxx.xxx " // 16	address
                             "xxxxx "           // 6	source port
                             "xxxxx "           // 6	dest port
                             "$xxxxxxxx "       // 10     rcv queue
                             "$xxxxxxxx ";      // 10	send queue
    static char buffer16[16];

    static char *stateStr = "CLOSED      "
                            "LISTEN      "
                            "SYNSENT     "
                            "SYNRCVD     "
                            "ESTABLISHED "
                            "FINWAIT1    "
                            "FINWAIT2    "
                            "CLOSEWAIT   "
                            "LASTACK     "
                            "CLOSING     "
                            "TIMEWAIT    "
                            "UNKOWN      ";

    static destRec dest;

    char *cp;
    Word i, j;

    TCPIPGetDestination(ipid, &dest);

    for (i = 0; i < 80; i++)
        buffer[i] = ' ';

    Int2Dec(ipid, buffer, 5, 0);

    cp = buffer + 6;

    i = srBuffer->srState;
    if (i > 10)
        i = 11;

    i *= 12; // sizeof the string...
    for (j = 0; j < 12; j++)
        *cp++ = stateStr[i + j];

    cp = buffer + 18;

    // includes the last space, so we're ok.

    TCPIPConvertIPToCASCII(srBuffer->srDestIP, buffer16, 0);
    for (j = 0; buffer16[j]; j++)
        *cp++ = buffer16[j];

    i = TCPIPGetSourcePort(ipid);
    Int2Dec(i, buffer + 34, 5, 0);

    Int2Dec(srBuffer->srDestPort, buffer + 40, 5, 0);

    buffer[46] = '$';
    Long2Hex(srBuffer->srRcvQueued, buffer + 47, 8);

    buffer[56] = '$';
    Long2Hex(srBuffer->srSndQueued, buffer + 57, 8);

    buffer[66] = '\r';
    buffer[67] = 0;

    fputs(buffer, stdout);
}

asm int ReadKey(void) {
    sep #0x20 loop : lda > 0xe0c000 bpl loop sta > 0xe0c010 rep #0x20 and
        #0x7f rtl
}

/* ORCA Console control codes */
#define CURSOR_ON 0x05
#define CURSOR_OFF 0x06
#define CLEAR_EOL 29
#define CLEAR_EOS 11

/* cursor keys */
#define LEFT 0x08
#define RIGHT 0x15
#define UP 0x0b
#define DOWN 0x0a
#define ESC 0x1b

int ReadInt(void) {
    unsigned i = 0;
    unsigned c;
    unsigned rv;

    putchar(CURSOR_ON);
    while (1) {
        c = ReadKey();
        if (c == 0x1b) {
            rv = -1;
            break;
        }
        if (c == 13) {
            if (i == 0)
                rv = -1;
            break;
        } else if ((c == 8) || (c == 0x7f)) {
            if (i) {
                putchar(8);
                putchar(' ');
                putchar(8);
                i--;
                rv /= 10;
            }
        } else if ((c >= '0') && (c <= '9')) {
            if (i < 5) {
                putchar(c);
                i++;
                rv *= 10;
                rv += c - '0';
                continue;
            }
        }
        SysBeep();
    }
    putchar(CURSOR_OFF);
    return rv;
}

void hexdump(const void *data, Word length) {
    Word i;
    Word j;
    Word k;
    Word x;
    static char text[17];

    if (length > 16 * 320)
        length = 16 * 320;

    for (i = 0, j = 0, x = 0; i < length; i++) {

        k = ((unsigned char *)data)[i];
        buffer[j++] = "0123456789abcdef"[k >> 4];
        buffer[j++] = "0123456789abcdef"[k & 0x0f];
        buffer[j++] = ' ';

        text[x++] = isprint(k) ? k : '.';

        if ((i & 0x0f) == 0x07)
            buffer[j++] = ' ';

        if ((i & 0x0f) == 0x0f) {
            buffer[j++] = ' ';
            buffer[j++] = 0;
            text[x++] = 0;

            printf("%04x: %s%s\r", i - 15, buffer, text);
            j = 0;
            x = 0;
        }
    }

    if (i & 0x0f) {
        while (j < 52)
            buffer[j++] = ' ';
        buffer[j++] = 0;
        text[x++] = 0;

        printf("%04x: %s%s\r", i - 15, buffer, text);
    }
}

void DisplayQueue(Word which, const userRecord *rec) {
    Handle h;
    Word size;

    putchar(0x0c);
    if (which == 'S') {
        fputs("Send Queue\r\r");
        h = (Handle)rec->uwTCPDataOut;
    } else {
        fputs("Receive Queue\r\r");
        h = (Handle)rec->uwTCPDataIn;
    }

    if (h) {
        size = GetHandleSize(h);
        hexdump(*h, size);
    }
}

void DisplayDP(void) {

    Word dp;
    putchar(0x0c);
    fputs("Marinetti DP\r\r", stdout);

    dp = TCPIPGetDP();
    printf("Direct Page: $%04x\r", dp);
    hexdump((void *)dp, 0x0100);
}

void DisplayLinkLayer(void) {
    static linkInfoBlk link;
    variablesPtr lv;

    unsigned c;

    TCPIPGetLinkLayer(&link);
    lv = TCPIPGetLinkVariables();

    putchar(0x0c);
    fputs("Link Layer Status\r\r", stdout);

    printf("MethodID:   $%04x\r", link.liMethodID);
    printf("Name:       %b\r", link.liName);
    VersionString(0, link.liVersion, buffer);
    printf("Version:    %b\r", buffer);
    printf("Flags:      $%04x\r", link.liFlags);
    fputs("\r", stdout);

    printf("Variables:\r");
    printf("Version:    %d\r", lv->lvVersion);
    printf("Connected:  $%04x\r", lv->lvConnected);
    TCPIPConvertIPToASCII(lv->lvIPaddress, buffer, 0);
    printf("IP Address: %b\r", buffer);
    printf("RefCon:     $%08lx\r", lv->lvRefCon);
    printf("Errors:     $%08lx\r", lv->lvErrors);
    printf("MTU:        %d\r", lv->lvMTU);
}

void DisplayTCP(void) {

    static DNSRec dns;
    Long l;
    unsigned c;

    putchar(0x0c);
    fputs("TCP Status\r\r", stdout);

    // version
    VersionString(0, TCPIPLongVersion(), buffer);
    printf("Version:        %b\r", buffer);
    printf("Connect Status: $%04x\r", TCPIPGetConnectStatus());

    // ip address
    l = TCPIPGetMyIPAddress();
    TCPIPConvertIPToASCII(l, buffer, 0);
    printf("IP Address:     %b\r", buffer);

    // dns
    TCPIPGetDNS(&dns);
    TCPIPConvertIPToASCII(dns.DNSMain, buffer, 0);
    printf("DNS 1:          %b\r", buffer);
    TCPIPConvertIPToASCII(dns.DNSAux, buffer, 0);
    printf("DNS 2:          %b\r", buffer);

    // hostname
    TCPIPGetHostName((hnBuffPtr)buffer);
    printf("Hostname:       %b\r", buffer);

    // mtu
    printf("MTU:            %d\r", TCPIPGetMTU());
    printf("Alive Flag:     $%04x\r", TCPIPGetAliveFlag());
    printf("Alive Minutes:  %d\r", TCPIPGetAliveMinutes());
    printf("Login Count:    %d\r", TCPIPGetLoginCount());
}

unsigned DisplayIpid(unsigned ipid) {
    /* extended debug information */

    enum { MAX_PAGE = 0; };
    Handle h;
    Word size;
    unsigned page = 0;
    userRecord *rec;
    unsigned c;

    putchar(0x0c);
    printf("IPID: %d\r", ipid);

    h = (Handle)TCPIPGetUserRecord(ipid);
    if (_toolErr || !h) {
        printf("\r\rInvalid PID");
        ReadKey();
        return;
    }
    size = (Word)GetHandleSize(h);
    rec = (userRecord *)*h;

    for (;;) {

        switch (page) {
        case 0:
            break;
        }

        for (;;) {
            c = ReadKey();
            if (c == 0x1b || c == 'Q' || c == 'q')
                return;
            if (c == LEFT) {
                if (page == 0)
                    page = MAX_PAGE;
                else
                    --page;
                break;
            }
            if (c == RIGHT) {
                if (page == MAX_PAGE)
                    page = 0;
                else
                    ++page;
                break;
            }
            SysBeep();
        }
    }
}

printf("Datagram count (all): %d\r", TCPIPGetDatagramCount(ipid, protocolAll));

printf("Datagram count (icmp): %d\r",
       TCPIPGetDatagramCount(ipid, protocolICMP));

printf("Datagram count (tcp): %d\r", TCPIPGetDatagramCount(ipid, protocolTCP));

printf("Datagram count (udp): %d\r", TCPIPGetDatagramCount(ipid, protocolUDP));

printf("User statistic 1: $%08lx\r", TCPIPGetUserStatistic(ipid, 1));

printf("User statistic 2: $%08lx\r", TCPIPGetUserStatistic(ipid, 2));
}

void DisplayIpids(void) {

    static srBuff srBuffer;

    unsigned ipid;
    unsigned line;
    unsigned count;

    putchar(0x0c);
    fputs(Header1, stdout);
    fputs(Header2, stdout);

    line = 2;
    count = TCPIPGetLoginCount();

    if (!count)
        return;
    /* in theory, there could be 50 ipids.  */
    /* 20 ought to be enough */

    for (ipid = 0; ipid < 100; ipid += 2) {

        TCPIPStatusTCP(ipid, &srBuffer);
        if (_toolErr)
            continue;

        Display(ipid, &srBuffer);

        --count;
        ++line;
        if (count == 0)
            return;
        if (line == 23)
            return;
    }
}

void DisplayMain(void) {

    enum { MAX_PAGE = 3 };
    char c;
    unsigned page = 0;

    for (;;) {

        switch (page) {
        case 0:
            DisplayIpids();
            break;
        case 1:
            DisplayTCP();
            break;
        case 2:
            DisplayLinkLayer();
            break;
        case 3:
            DisplayDP();
            break;
        }

    /* DCA */
    menu:
        putchar(30);
        putchar(32 + 0);
        putchar(32 + 23);
        putchar(29); /* erase line */

        if (page == 0) {
            fputs("Q: Quit I: Info", stdout);
        } else {
            fputs("Q: Quit", stdout);
        }

        for (;;) {
            c = ReadKey();
            if (c == 'Q' || c == 'q' || c == ESC)
                return;
            if (c == LEFT) {
                if (page == 0)
                    page = MAX_PAGE;
                else
                    --page;
                break;
            }
            if (c == RIGHT) {
                if (page == MAX_PAGE)
                    page = 0;
                else
                    ++page;
                break;
            }
            if (c == 'I' || c == 'i') {

                putchar(30);
                putchar(32 + 0);
                putchar(32 + 23);
                putchar(29); /* erase line */
                fputs("ipid: ", stdout);

                int ipid = ReadInt();
                if (ipid < 0)
                    goto menu;
                DisplayIpid(ipid);
                break;
            }
            SysBeep();
        }
    }
}

void StartUp(void) {
    putchar(CURSOR_OFF); /* turn off cursor */

    if (TCPIPStatus() == 0 || _toolErr) {
        fputs("Marinetti is not active", stdout);
        ReadKey();
        return;
    }
    DisplayMain();
}

void ShutDown(void) {}
