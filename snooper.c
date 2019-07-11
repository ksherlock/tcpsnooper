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
                             "$xxxxxxxx "       // 10	rcv queue
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
                            "UNKNOWN     ";

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
/* clang-format off */
asm int ReadKey(void) {
    sep #0x20
loop:
    lda >0xe0c000
    bpl loop
    sta >0xe0c010
    rep #0x20
    and #0x7f
    rtl
}
/* clang-format on */

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
    unsigned rv = 0;

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
                fputs("\x08 \x08", stdout);
                /*
                putchar(8);
                putchar(' ');
                putchar(8);
                */
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

    if (length > 16 * 16)
        length = 16 * 16;

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

            printf("%04x: %s%s\r", i & ~0x0f, buffer, text);
            j = 0;
            x = 0;
        }
    }

    if (i & 0x0f) {
        while (j < 52)
            buffer[j++] = ' ';
        buffer[j++] = 0;
        text[x++] = 0;

        printf("%04x: %s%s\r", i & ~0x0f, buffer, text);
    }
}

void print_tab(const char *name, unsigned len) {

    putchar(0x0c);
    putchar(30);
    putchar(32 + ((80 - len) >> 1));
    putchar(32 + 0);
    fputs(name, stdout);
    putchar('\r');
    putchar('\r');

#if 0
    static const char *underscore80 = 
    "________________________________________"
    "________________________________________";


    printf("  %.*s\r", len + 2, underscore80);
    printf("_/ %s \\", name);
    printf("%.*s\r\r", 80 - len - 5, underscore80);
#endif
}

void dump_handle(Handle h) {
    Word size;
    void *address;

    address = 0;
    size = 0;

    if (h) {
        size = GetHandleSize(h);
        address = *(void **)h;
    }

    printf("  Handle: $%08lx Address: $%08lx Size: $%04x\r\r", (LongWord)h,
           (LongWord)address, size);


    if (size) {
        hexdump(address, size);
    }
}

void DisplayDP(void) {

    Word dp;

    print_tab("Direct Page", 11);

    dp = TCPIPGetDP();
    printf("Address: $%04x\r\r", dp);
    hexdump((void *)dp, 0x0100);
}

void DisplayLinkLayer(void) {
    static linkInfoBlk link;
    variablesPtr lv;

    TCPIPGetLinkLayer(&link);
    lv = TCPIPGetLinkVariables();

    print_tab("Link Layer Status", 17);

    printf("  MethodID:           $%04x\r", link.liMethodID);
    printf("  Name:               %b\r", link.liName);
    VersionString(0, link.liVersion, buffer);
    printf("  Version:            %b\r", buffer);
    printf("  Flags:              $%04x\r", link.liFlags);

    printf("\r  Variables:\r");

    printf("  Version:            %d\r", lv->lvVersion);
    printf("  Connected:          $%04x\r", lv->lvConnected);
    TCPIPConvertIPToASCII(lv->lvIPaddress, buffer, 0);
    printf("  IP Address:         %b\r", buffer);
    printf("  RefCon:             $%08lx\r", lv->lvRefCon);
    printf("  Errors:             $%08lx\r", lv->lvErrors);
    printf("  MTU:                %d\r", lv->lvMTU);
}

void DisplayTCP(void) {

    static DNSRec dns;
    Long l;

    print_tab("TCP Status", 10);

    // version
    VersionString(0, TCPIPLongVersion(), buffer);
    printf("  Version:            %b\r", buffer);
    printf("  Connect Status:     $%04x\r", TCPIPGetConnectStatus());

    // ip address
    l = TCPIPGetMyIPAddress();
    TCPIPConvertIPToASCII(l, buffer, 0);
    printf("  IP Address:         %b\r", buffer);

    // dns
    TCPIPGetDNS(&dns);
    TCPIPConvertIPToASCII(dns.DNSMain, buffer, 0);
    printf("  DNS 1:              %b\r", buffer);
    TCPIPConvertIPToASCII(dns.DNSAux, buffer, 0);
    printf("  DNS 2:              %b\r", buffer);

    // hostname
    TCPIPGetHostName((hnBuffPtr)buffer);
    printf("  Hostname:           %b\r", buffer);

    // mtu
    printf("  MTU:                %d\r", TCPIPGetMTU());
    printf("  Alive Flag:         $%04x\r", TCPIPGetAliveFlag());
    printf("  Alive Minutes:      %d\r", TCPIPGetAliveMinutes());
    printf("  Login Count:        %d\r", TCPIPGetLoginCount());
}

void DisplayIpid2(unsigned page, userRecord *rec) {


    static char *error_codes[] = {
        "OK",
        "DeafDestPort",
        "HostReset",
        "ConExists",
        "ConIllegal",
        "NoResources",
        "NoSocket",
        "BadPrec",
        "BadSec",
        "BadConnection",
        "ConClosing",
        "Closing",
        "ConReset",
        "UserTimeout",
        "ConRefused",
    };

    static char *states[] = {
        "CLOSED",
        "LISTEN",
        "SYNSENT",
        "SYNRCVD",
        "ESTABLISHED",
        "FINWAIT1",
        "FINWAIT2",
        "CLOSEWAIT",
        "LASTACK",
        "CLOSING",
        "TIMEWAIT",
    };

    static char *timer_states[] = {
        "OFF",
        "SYN",
        "DATA",
        "2MSL",
        "SYNACK",
    };

    unsigned x,y;

    print_tab("User Record", 11);

    if (page == 0) {
        TCPIPConvertIPToASCII(rec->uwDestIP, buffer, 0);

        printf("  uwUserID:           $%04x\r", rec->uwUserID);
        printf("  uwDestIP:           $%08lx (%b)\r", rec->uwDestIP, buffer);
        printf("  uwDestPort:         $%04x     (%u)\r", rec->uwDestPort,
               rec->uwDestPort);
        printf("  uwIP_TOS:           $%04x\r", rec->uwIP_TOS);
        printf("  uwIP_TTL:           $%04x\r", rec->uwIP_TTL);

        printf("  uwSourcePort:       $%04x     (%u)\r", rec->uwSourcePort,
               rec->uwSourcePort);
        printf("  uwLogoutPending:    $%04x\r", rec->uwLogoutPending);
        printf("  uwICMPQueue:        $%08lx\r", rec->uwICMPQueue);
        printf("  uwTCPQueue:         $%08lx\r", rec->uwTCPQueue);

        printf("  uwTCPMaxSendSeg:    $%04x\r", rec->uwTCPMaxSendSeg);
        printf("  uwTCPMaxReceiveSeg: $%04x\r", rec->uwTCPMaxReceiveSeg);
        printf("  uwTCPDataInQ:       $%08lx\r", rec->uwTCPDataInQ);
        printf("  uwTCPDataIn:        $%08lx\r", rec->uwTCPDataIn);
        printf("  uwTCPPushInFlag:    $%04x\r", rec->uwTCPPushInFlag);
        printf("  uwTCPPushInOffset:  $%08lx\r", rec->uwTCPPushInOffset);
        printf("  uwTCPPushOutFlag:   $%04x\r", rec->uwTCPPushOutFlag);
        printf("  uwTCPPushOutSEQ:    $%08lx\r", rec->uwTCPPushOutSEQ);
        printf("  uwTCPDataOut:       $%08lx\r", rec->uwTCPDataOut);

        return;
    }
    if (page == 1) {
        printf("  uwSND_UNA:          $%08lx\r", rec->uwSND_UNA);
        printf("  uwSND_NXT:          $%08lx\r", rec->uwSND_NXT);
        printf("  uwSND_WND:          $%04x\r", rec->uwSND_WND);
        printf("  uwSND_UP:           $%04x\r", rec->uwSND_UP);
        printf("  uwSND_WL1:          $%08lx\r", rec->uwSND_WL1);
        printf("  uwSND_WL2:          $%08lx\r", rec->uwSND_WL2);
        printf("  uwISS:              $%08lx\r", rec->uwISS);
        printf("  uwRCV_NXT:          $%08lx\r", rec->uwRCV_NXT);
        printf("  uwRCV_WND:          $%04x\r", rec->uwRCV_WND);
        printf("  uwRCV_UP:           $%04x\r", rec->uwRCV_UP);
        printf("  uwIRS:              $%08lx\r", rec->uwIRS);
        x = rec->uwTCP_State;
        printf("  uwTCP_State:        $%04x", x);
        if (x < sizeof(states)/sizeof(states[0]))
            printf("     (%s)", states[x]);
        fputs("\r", stdout);

        printf("  uwTCP_StateTick:    $%08lx\r", rec->uwTCP_StateTick);

        x = rec->uwTCP_ErrCode;
        printf("  uwTCP_ErrCode:      $%04x", x);
        if (x < sizeof(error_codes)/sizeof(error_codes[0]))
            printf("     (%s)", error_codes[x]);
        fputs("\r", stdout);

        printf("  uwTCP_ICMPError:    $%04x\r", rec->uwTCP_ICMPError);
        printf("  uwTCP_Server:       $%04x\r", rec->uwTCP_Server);
        printf("  uwTCP_ChildList:    $%08lx\r", rec->uwTCP_ChildList);
        printf("  uwTCP_ACKPending:   $%04x\r", rec->uwTCP_ACKPending);

        return;
    }
    if (page == 2) {
        printf("  uwTCP_ForceFIN:     $%04x\r", rec->uwTCP_ForceFIN);
        printf("  uwTCP_FINSEQ:       $%08lx\r", rec->uwTCP_FINSEQ);
        printf("  uwTCP_MyFINACKed:   $%04x\r", rec->uwTCP_MyFINACKed);
        printf("  uwTCP_Timer:        $%08lx\r", rec->uwTCP_Timer);

        x = rec->uwTCP_TimerState;
        printf("  uwTCP_TimerState:   $%04x", x);
        if ((x & 0x01) == 0) {
            x >>= 1;
            if (x < sizeof(timer_states)/sizeof(timer_states[0]))
                printf("     (%s)", timer_states[x]);
        }
        fputs("\r", stdout);

        printf("  uwTCP_rt_timer:     $%04x\r", rec->uwTCP_rt_timer);
        printf("  uwTCP_2MSL_timer:   $%04x\r", rec->uwTCP_2MSL_timer);
        printf("  uwTCP_SaveTTL:      $%04x\r", rec->uwTCP_SaveTTL);
        printf("  uwTCP_SaveTOS:      $%04x\r", rec->uwTCP_SaveTOS);
        printf("  uwTCP_TotalIN:      $%08lx\r", rec->uwTCP_TotalIN);
        printf("  uwTCP_TotalOUT:     $%08lx\r", rec->uwTCP_TotalOUT);

        printf("  uwUDP_Server:       $%04x\r", rec->uwUDP_Server);
        printf("  uwUDPQueue:         $%08lx\r", rec->uwUDPQueue);
        printf("  uwUDPError:         $%04x\r", rec->uwUDPError);
        printf("  uwUDPErrorTick:     $%08lx\r", rec->uwUDPErrorTick);
        printf("  uwUDPCount:         $%08lx\r", rec->uwUDPCount);

        printf("  uwTriggers[0]:      $%08lx\r", rec->uwTriggers[0]);
        printf("  uwSysTriggers[0]:   $%08lx\r", rec->uwSysTriggers[0]);
    }
}

int DisplayIpid(unsigned ipid) {
    /* extended debug information */

    enum { MAX_PAGE = 8 };
    Handle h;
    Word size;
    unsigned page = 0;
    userRecord *rec;
    unsigned c;

    if ((ipid > 100) || (ipid & 0x01))
        return -1;

    h = (Handle)TCPIPGetUserRecord(ipid);
    if (_toolErr || !h)
        return -1;

    size = (Word)GetHandleSize(h);
    rec = (userRecord *)*h;

    for (;;) {

        switch (page) {
        case 0:
        case 1:
        case 2:
            DisplayIpid2(page, rec);
            break;
        case 3:
            print_tab("TCP Data In", 11);
            dump_handle((Handle)rec->uwTCPDataIn);
            break;
        case 4:
            print_tab("TCP Data Out", 12);
            dump_handle((Handle)rec->uwTCPDataOut);
            break;
        case 5:
            print_tab("ICMP Queue", 10);
            dump_handle((Handle)rec->uwICMPQueue);
            break;
        case 6:
            print_tab("TCP Queue", 9);
            dump_handle((Handle)rec->uwTCPQueue);
            break;
        case 7:
            print_tab("TCP Data In Queue", 17);
            dump_handle((Handle)rec->uwTCPDataInQ);
            break;
        case 8:
            print_tab("UDP Queue", 9);
            dump_handle((Handle)rec->uwUDPQueue);
            break;
        }

        for (;;) {
            c = ReadKey();
            if (c == 0x1b || c == 'Q' || c == 'q')
                return 0;
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

void DisplayTuning(void) {

    /* clang-format off */
    static char *tuneStrings[] = {
        "COUNT:             ",
        "IP USER POLL CT:   ",
        "IP RUNQ FREQ:      ",
        "IP RUNQ CT:        ",
        "TCP USER POLL:     "
    };
    /* clang-format on */

    static Word tune[5];

    unsigned count;
    unsigned i;

    print_tab("Tuning", 6);

    TCPIPGetTuningTable((tunePtr)tune);
    count = tune[0] >> 1;
    if (count > sizeof(tuneStrings) / 4)
        count = sizeof(tuneStrings) / 4;
    for (i = 1; i < count; i++) {
        printf("  %s %u\r", tuneStrings[i], tune[i]);
    }
}

void DisplayErrors(void) {

    /* clang-format off */
    static char *errStrings[] =
    {
        "TBLEN:                     ",
        "TOTAL:                     ",
        "FRAGS IN:                  ",
        "FRAGS LOST:                ",
        "BUILT:                     ",
        "OK:                        ",
        "BAD CHK:                   ",
        "BAD HEADLEN:               ",
        "BAD PROTO:                 ",
        "BAD IP:                    ",
        "ICMP:                      ",
        "ICMP USER:                 ",
        "ICMP KERNEL:               ",
        "ICMP BAD:                  ",
        "ICMP BAD TYPE:             ",
        "ICMP BAD CODE:             ",
        "ICMP ECHO RQ:              ",
        "ICMP ECHO RQ OUT:          ",
        "ICMP ECHO RP:              ",
        "ICMP ECHO RP BAD ID:       ",
        "UDP:                       ",
        "UDP BAD:                   ",
        "UDP NOPORT:                ",
        "TCP:                       ",
        "TCP BAD:                   ",
        "TCP NOPORT:                ",
        "TCP QUEUED:                ",
        "TCP OLD:                   ",
        "OFRAGMENTS:                ",
        "FRAGMENTED:                "
    };
    /* clang-format on */
 
    LongWord *ptr;
    unsigned count;
    unsigned i;

    print_tab("Error Table", 11);

    ptr = (LongWord *)TCPIPGetErrorTable();
    if (!ptr) return;

    count = ptr[0] >> 2;
    if (count > sizeof(errStrings) / sizeof(errStrings[0]))
        count = sizeof(errStrings) / sizeof(errStrings[0]);

    if (count == 0) /* marinetti bug */
        count = sizeof(errStrings) / sizeof(errStrings[0]);

    for (i = 1; i < count; ++i) {

        printf("  %s %-9lu", errStrings[i], ptr[i]);

        if (!(i & 0x01)) putchar('\r');
        else putchar(' ');
    }
    putchar('\r');
}

void DisplayToolErrors(unsigned page) {

    print_tab("Tool Errors", 11);
    if (page == 0)
        fputs(
            "  $3601 BADIPID           Bad IPID for this request\r"
            "  $3602 NOCONNECTION      Not connected to the network\r"
            "  $3603 NORECONDATA       No reconnect data\r"
            "  $3604 LINKERROR         Problem with the link layer\r"
            "  $3605 SCRIPTFAILED      The script failed / timed out\r"
            "  $3606 CONNECTED         Not while connected to the network\r"
            "  $3607 SOCKETOPEN        Socket still open\r"
            "  $3608 INITNOTFOUND      Init not found in memory\r"
            "  $3609 VERSIONMISMATCH   Different versions of tool, init, cdev\r"
            "  $360A BADTUNETABLELEN   Bad tune table length\r"
            "  $360B IPIDTABLEFULL     IPID table full\r"
            "  $360C NOICMPQUEUED      No ICMP datagrams in the queue\r"
            "  $360D LOGINSPENDING     There are still IPIDs logged in\r"
            "  $360E TCPIPNOTACTIVE    Not active. Probably in P8 mode.\r"
            "  $360F NODNSERVERS       No servers registered with Marinetti\r",
            stdout);
    if (page == 1)
        fputs(
            "  $3610 DNRBUSY           DNR is current busy. Try again later\r"
            "  $3611 NOLINKLAYER       Unable to load link layer module\r"
            "  $3612 BADLINKLAYER      Not a link layer module\r"
            "  $3613 ENJOYCOKE         But not so close to the keyboard\r"
            "  $3614 NORECONSUPPRT     This module doesn't support reconnect\r"
            "  $3615 USERABORTED       The user aborted the connect/disconnect script\r"
            "  $3616 BADUSERPASS       Invalid username and/or password\r"
            "  $3617 BADPARAMETER      Invalid parameter for this call\r"
            "  $3618 BADENVIRONMENT    No desktop or tools not started\r"
            "  $3619 NOINCOMING        There is no pending incoming request\r"
            "  $361A LINKBUSY          Modem or interface is busy\r"
            "  $361B NOLINKINTERFACE   No dial tone or similar\r"
            "  $361C NOLINKRESPONSE    No modem answer or similar\r"
            "  $361D NODNRPENDING      No such entry in DNR list\r"
            "  $361E BADALIVEMINUTES   Minutes value is invalid\r"
            "  $361F BUFFERTOOSMALL    Buffer is too small\r"
            "  $3620 NOTSERVER         This ipid is not set up as a server\r"
            "  $3621 BADTRIGGERNUM     Invalid trigger number\r",
        stdout);

}

/*
putchar(30);
putchar(32 + 0);
putchar(32 + 23);
putchar(29);
*/
#define status_line() fputs("\x1e\x20\x37\x1d", stdout)
void DisplayMain(void) {

    enum { MAX_PAGE = 7 };
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
            DisplayErrors();
            break;
        case 4:
            DisplayTuning();
            break;
        case 5:
            DisplayDP();
            break;
        case 6:
            DisplayToolErrors(0);
            break;
        case 7:
            DisplayToolErrors(1);
            break;
        }

    menu:
        status_line();

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
                int ok, ipid;
                status_line();
                fputs("ipid: ", stdout);

                ipid = ReadInt();
                if (ipid < 0)
                    goto menu;
                ok = DisplayIpid(ipid);
                if (ok < 0) {
                    status_line();
                    SysBeep();
                    fputs("Invalid ipid", stdout);
                    ReadKey();
                    goto menu;
                }
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
