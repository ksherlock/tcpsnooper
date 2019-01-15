/*=================================================
*
* debug.c - Code to dump Marinetti internals for TCP Snooper CDA
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

#pragma noroot
#pragma lint -1
#pragma optimize -1
#pragma debug 0x8000

#include <gsos.h>
#include <tcpip.h>
#include <tcpipx.h>
#include <memory.h>
#include <intmath.h>
#include <loader.h>
#include <misctool.h>


extern int fdprintf(int, const char *, ...);
extern int orca_sprintf(char *, const char *, ...);

char *errStrings[] =
{
"tcpDGMSTBLEN",
"tcpDGMSTOTAL",
"tcpDGMSFRAGSIN",
"tcpDGMSFRAGSLOST",
"tcpDGMSBUILT",
"tcpDGMSOK",
"tcpDGMSBADCHK",
"tcpDGMSBADHEADLEN",
"tcpDGMSBADPROTO",
"tcpDGMSBADIP",
"tcpDGMSICMP",
"tcpDGMSICMPUSER",
"tcpDGMSICMPKERNEL",
"tcpDGMSICMPBAD",
"tcpDGMSICMPBADTYPE",
"tcpDGMSICMPBADCODE",
"tcpDGMSICMPECHORQ",
"tcpDGMSICMPECHORQOUT",
"tcpDGMSICMPECHORP",
"tcpDGMSICMPECHORPBADID",

"tcpDGMSUDP",
"tcpDGMSUDPBAD",
"tcpDGMSUDPNOPORT",

"tcpDGMSTCP",
"tcpDGMSTCPBAD",
"tcpDGMSTCPNOPORT",
"tcpDGMSTCPQUEUED",
"tcpDGMSTCPOLD",	
"tcpDGMSOFRAGMENTS"			
"tcpDGMSFRAGMENTED"
};

char *tuneStrings[] =
{
"tcpTUNECOUNT",
"tcpTUNEIPUSERPOLLCT",
"tcpTUNEIPRUNQFREQ",
"tcpTUNEIPRUNQCT",		
"tcpTUNETCPUSERPOLL"	
};



long write(Word fd, const void *data, LongWord size)
{
static IORecGS ioDCB = { 4, 0, 0, 0};

  ioDCB.refNum = fd;
  ioDCB.dataBuffer = (Pointer)data;
  ioDCB.requestCount = size;

  WriteGS(&ioDCB);
  return _toolErr ? -1 : ioDCB.transferCount;
}


// creates a unique file and opens it.
int createFile(void)
{
static GSString32 template = {28, "*:system:tcpip:debugxxxx.txt"};
static CreateRecGS createDCB = {4, (GSString255Ptr)&template, 0xe3, 4, 0};
static OpenRecGS openDCB = {4, 0, (GSString255Ptr)&template, writeEnable, 0};

word i;

  for (i = 0; i < 9999; i++)
  {

    Int2Dec(i, &template.text[20], 4, 0);
    template.text[20] |= 0x10;  // convert ' ' (0x20) to '0' (0x30)
    template.text[21] |= 0x10;
    template.text[22] |= 0x10;
    template.text[23] |= 0x10;
                              

    CreateGS(&createDCB);
    if (_toolErr == dupPathname) continue;
    break;
  }

  if (_toolErr) return 0;

  OpenGS(&openDCB);
  if (_toolErr) return 0;

  return openDCB.refNum;
}

void dump(Word fd, void *data, Word length)
{
Word i;
Word j;
Word k;
static char buffer[80];

  
  for (i = 0, j = 0; i < length; i++)
  {

    k = ((char *)data)[i];
    buffer[j++] = "0123456789abcdef"[k >> 4];
    buffer[j++] = "0123456789abcdef"[k & 0x0f];
    buffer[j++] = ' ';

    if ((i & 0x0f) == 0x07)  buffer[j++] = ' ';

    if ((i & 0x0f) == 0x0f)
    {
      buffer[j++] = 0;

      fdprintf(fd, "%04x:  %s\r", i - 15, buffer);
      //write(fd, buffer, j);
      j = 0;
    }
  }

  if (i & 0x0f)
  {
    buffer[j++] = 0;
    fdprintf(fd, "%04x:  %s\r", i - 15, buffer);
    //write(fd, buffer, j);                         
  }

}



void debug(void)
{

Word i;
Long l;

LongWord *lw;
variablesPtr lv;

static Word tune[5];
static linkInfoBlk link;
static char buffer[80];
static DNSRec dns;
static udpVars udp;
static srBuff tcp;


Word count;
Handle h;
Word size;
Word fd;
Word closeDCB[2];


  fd = createFile();
  if (fd == 0) return;

  // version
  VersionString(0, TCPIPLongVersion(), buffer);
  fdprintf(fd, "Version: %b\r", buffer);

  // link layer
  TCPIPGetLinkLayer(&link);
  fdprintf(fd, "Link Layer:\r");
  fdprintf(fd, "  MethodID: $%04x\r", link.liMethodID);
  fdprintf(fd, "  Name:     %b\r", link.liName);
  VersionString(0, link.liVersion, buffer);
  fdprintf(fd, "  Version:  %b\r", buffer);
  fdprintf(fd, "  Flags:    $%04x\r", link.liFlags);

  lv = TCPIPGetLinkVariables();
  fdprintf(fd, "Link Variables\r");
  fdprintf(fd, "  Version: %d\r", lv->lvVersion);
  fdprintf(fd, "  Connected: $%04x\r", lv->lvConnected);
  TCPIPConvertIPToASCII(lv->lvIPaddress, buffer, 0);
  fdprintf(fd, "  IP Address: %b\r", buffer);
  fdprintf(fd, "  RefCon: $%08lx\r", lv->lvRefCon);
  fdprintf(fd, "  Errors: $%08lx\r", lv->lvErrors);
  fdprintf(fd, "  MTU: %d\r", lv->lvMTU);


  // connect status
  fdprintf(fd, "Connect Status: $%04x\r", TCPIPGetConnectStatus());

  // ip address
  l = TCPIPGetMyIPAddress();
  TCPIPConvertIPToASCII(l, buffer, 0);
  fdprintf(fd, "IP Address: %b\r", buffer);

  // dns
  TCPIPGetDNS(&dns);
  TCPIPConvertIPToASCII(dns.DNSMain, buffer, 0);
  fdprintf(fd, "DNS 1: %b\r", buffer);
  TCPIPConvertIPToASCII(dns.DNSAux, buffer, 0);
  fdprintf(fd, "DNS 2: %b\r", buffer);

  // hostname
  TCPIPGetHostName((hnBuffPtr)buffer);
  fdprintf(fd, "Hostname: %b\r", buffer);

  //mtu
  fdprintf(fd, "MTU: %d\r", TCPIPGetMTU());
  fdprintf(fd, "Alive Flag: $%04x\r", TCPIPGetAliveFlag());
  fdprintf(fd, "Alive Minutes: %d\r", TCPIPGetAliveMinutes());
  fdprintf(fd, "Login Count: %d\r", TCPIPGetLoginCount());
         


  // error table
  fdprintf(fd, "\rError Table\r");
  lw = (LongWord *)TCPIPGetErrorTable();
  count = lw[0] >> 2;
  if (count > sizeof(errStrings) / 4) count = sizeof(errStrings) / 4;

  for (i = 0; i < count; i++);
  {
    fdprintf(fd, "  %s -- $%08lx\r", errStrings[i], lw[i]);
  }

  // tuning table
  tune[0] = 10;

  fdprintf(fd, "\rTuning Table\r");
  TCPIPGetTuningTable((tunePtr)tune);
  count = tune[0] >> 1;
  if (count > sizeof(tuneStrings) / 4) count = sizeof(tuneStrings) / 4;
  for (i = 0; i < count; i++)
  {
    fdprintf(fd, "  %s -- $%04x\r", tuneStrings[i], tune[i]);
  }

  fdprintf(fd, "\rDirect Page\r");
  dump(fd, (void *)TCPIPGetDP(), 0x0100);

  // dump all user records.  ipid will be even numbers in the range 0..98
  for (i = 0; i < 100; i += 2)
  {
    h = (Handle)TCPIPGetUserRecord(i);
    if (h == NULL) continue;
    size = (Word)GetHandleSize(h);
    if (_toolErr) continue;


    fdprintf(fd, "\rIpid %d\r", i);

    fdprintf(fd, "Datagram count (all): %d\r",
      TCPIPGetDatagramCount(i, protocolAll));

    fdprintf(fd, "Datagram count (icmp): %d\r",
      TCPIPGetDatagramCount(i, protocolICMP));

    fdprintf(fd, "Datagram count (tcp): %d\r",
      TCPIPGetDatagramCount(i, protocolTCP));

    fdprintf(fd, "Datagram count (udp): %d\r",
      TCPIPGetDatagramCount(i, protocolUDP));


    fdprintf(fd, "User statistic 1: $%08lx\r",
      TCPIPGetUserStatistic(i, 1));

    fdprintf(fd, "User statistic 2: $%08lx\r",
      TCPIPGetUserStatistic(i, 2));

#if 0  // tcpipStatusUDP has a stack imbalance bug.

    TCPIPStatusUDP(i, &udp);
    if (_toolErr == 0)
    {
      fdprintf(fd, "\rStatus UDP\r");
      fdprintf(fd, "  Queue Size: %d\r", udp.uvQueueSize);
      fdprintf(fd, "  Error: %d\r", udp.uvError);
      fdprintf(fd, "  Error Tick: %ld\r", udp.uvErrorTick);
      fdprintf(fd, "  Count: %ld\r", udp.uvCount);
      fdprintf(fd, "  Total Count: %ld\r", udp.uvTotalCount);
      fdprintf(fd, "  Dispatch Flag: %d\r", udp.uvDispatchFlag);
                                               
    }
#endif

    TCPIPStatusTCP(i, &tcp);
    if (_toolErr == 0)
    {
      fdprintf(fd, "\rStatus TCP\r");
      fdprintf(fd, "  State: %d\r", tcp.srState);
      fdprintf(fd, "  Network Error: %d\r", tcp.srNetworkError);
      fdprintf(fd, "  Send Queued: %ld\r", tcp.srSndQueued);
      fdprintf(fd, "  Recv Queued: %ld\r", tcp.srRcvQueued);
      TCPIPConvertIPToASCII(tcp.srDestIP, buffer, 0);
      fdprintf(fd, "  Dest IP: %b\r", buffer);
      fdprintf(fd, "  Dest Port: %d\r", tcp.srDestPort);
      fdprintf(fd, "  Connect Type: %d\r", tcp.srConnectType);
      fdprintf(fd, "  Accept Count: %d\r", tcp.srAcceptCount);
    }                                                         


    HLock(h);
    if (1) // (size == sizeof(userRecord))
    {
      GSString255Ptr appName;
      userRecord *rec = (userRecord *)*h;
      fdprintf(fd, "\rUser Record\r");
      appName = (GSString255Ptr)LGetPathname2(rec->uwUserID, 1);
      if (_toolErr == 0)
      {
        fdprintf(fd, "\rApplication %B\r", appName);
      }
      #include "ur.c"
    }
    else dump(fd, *h, size);
    HUnlock(h);
  }

  closeDCB[0] = 1;
  closeDCB[1] = fd;
  CloseGS(closeDCB);


}



#if 0

void		_beginStackCheck (void);
int		_endStackCheck (void);

void main (void)
{
  _beginStackCheck();
  debug();
  fdprintf(2, "%d bytes of stack used\r", _endStackCheck());
}
#endif
