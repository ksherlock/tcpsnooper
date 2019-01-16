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

#include <misctool.h>
#include <texttool.h>

#include <stdio.h>

extern void debug(void);

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

  WriteCString(buffer);
}

asm int ReadKey(void) {
  sep #0x20
  loop:
  lda >0xe0c0000
  bpl loop
  sta >0xe0c010
  rep #0x20
  and #0x7f
}

int ReadInt(void) {
  unsigned i = 0;
  unsigned c;
  unsigned rv;

  while (1) {
    c = ReadChar(0);
    if (c == 13) {
      WriteChar(13);
      WriteChar(10);
      if (i == 0)
        return -1;
      return rv;
    } else if ((c == 8) || (c == 0x7f)) {
      if (i) {
        WriteChar(8);
        WriteChar(' ');
        WriteChar(8);
        i--;
        rv /= 10;
      }
    } else if ((c >= '0') && (c <= '9')) {
      if (i < 5) {
        WriteChar(c);
        i++;
        rv *= 10;
        rv += c - '0';
      }
    }
  }
}
static char buffer[80];

void DisplayLinkLayer(void) {
  static linkInfoBlk link;
  variablesPtr lv;

  unsigned c;

  TCPIPGetLinkLayer(&link);

  WriteChar(0x0c);
  putchar(0x0c);

  printf("Link Layer:\r");
  printf("  MethodID:   $%04x\r", link.liMethodID);
  printf("  Name:       %b\r", link.liName);
  VersionString(0, link.liVersion, buffer);
  printf("  Version:    %b\r", buffer);
  printf("  Flags:      $%04x\r", link.liFlags);

  lv = TCPIPGetLinkVariables();
  printf("Link Variables\r");
  printf("  Version:    %d\r", lv->lvVersion);
  printf("  Connected:  $%04x\r", lv->lvConnected);
  TCPIPConvertIPToASCII(lv->lvIPaddress, buffer, 0);
  printf("  IP Address: %b\r", buffer);
  printf("  RefCon:     $%08lx\r", lv->lvRefCon);
  printf("  Errors:     $%08lx\r", lv->lvErrors);
  printf("  MTU:        %d\r", lv->lvMTU);

  WriteChar('\r');
  c = ReadChar(0);
}

void DisplayTCP(void) {

  static DNSRec dns;
  Long l;
  unsigned c;
  WriteChar(0x0c);
  putchar(0x0c);

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

  WriteChar('\r');
  c = ReadChar(0);
}

unsigned DisplayMain(void) {
  Word i;
  Word count;

  static srBuff srBuffer;

  Word line = 0;
  Word ipid;
  char c;

  ipid = 0;
  count = TCPIPGetLoginCount();

redraw:

  WriteChar(0x0c);
  WriteCString(Header1);
  WriteCString(Header2);

  line = 2;
  for (; count && ipid < 100; ipid += 2) {

    TCPIPStatusTCP(i, &srBuffer);
    if (_toolErr)
      continue;

    Display(i, &srBuffer);

    --count;
    ++line;
    if (line == 24) {
      WriteCString("-- more --");
      c = ReadChar(0) & 0x7f;
      if (c == 'Q' || c == 'q' || c == 0x1b)
        return 0;
      goto redraw;
    }
  }

  while (line < 23) {
    WriteChar('\r');
    ++line;
  }

  WriteCString("Q: Quit.  D: Save debug file");
  for (;;) {
    c = ReadChar(0) & 0x7f;
    switch (c) {
    case 'D':
    case 'd':
      // debug();
      return 1;
    case 'T':
    case 't':
      DisplayTCP();
      return 1;
    case 'L':
    case 'l':
      DisplayLinkLayer();
      return 1;
    case ' ':
      goto redraw;
      break;
    case 'q':
    case 'Q':
    case 0x1b:
      return 0;
    }
  }
}

DeviceRec device_in;
DeviceRec device_out;
Long globals_in;
Long globals_out;

void StartTT(void) {
  device_in = GetInputDevice();
  globals_in = GetInGlobals();
  device_out = GetOutputDevice();
  globals_out = GetOutGlobals();

  SetInputDevice(0, 3);
  SetOutputDevice(0, 3);
  SetInGlobals(0x7f, 0x00);
  SetOutGlobals(0xff, 0x80);

  InitTextDev(0);
  InitTextDev(1);
}
void StopTT(void) {
  SetInputDevice(device_in.deviceType, device_in.ptrOrSlot);
  SetInGlobals(globals_in >> 16, globals_in);

  SetOutputDevice(device_out.deviceType, device_out.ptrOrSlot);
  SetOutGlobals(globals_out >> 16, globals_out);

  InitTextDev(0);
  InitTextDev(1);
}

void StartUp(void) {
  StartTT();
  putchar(0x06); /* turn off cursor */

  if (TCPIPStatus() == 0 || _toolErr) {
    WriteCString("Marinetti is not active\r");
    ReadChar(0);
  } else {
    while (DisplayMain()) /* */
      ;
  }

  StopTT();
}

void ShutDown(void) {}