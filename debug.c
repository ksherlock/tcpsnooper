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
#include <stdio.h>


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


// creates a unique file and opens it.
FILE *createFile(void)
{
static GSString32 template = {28, "*:system:tcpip:debugxxxx.txt"};
static CreateRecGS createDCB = {4, (GSString255Ptr)&template, 0xe3, 4, 0};

FILE *fp;
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

  if (_toolErr) return NULL;

  fp = fopen(template.text, "w");
  return fp;
}

void dump(FILE *fp, void *data, Word length)
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

      fprintf(fp, "%04x:  %s\r", i - 15, buffer);
      j = 0;
    }
  }

  if (i & 0x0f)
  {
    buffer[j++] = 0;
    fprintf(fp, "%04x:  %s\r", i - 15, buffer);
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
FILE *fp;


  fp = createFile();
  if (!fp) return;

  // version
  VersionString(0, TCPIPLongVersion(), buffer);
  fprintf(fp, "Version: %b\r", buffer);

  // link layer
  TCPIPGetLinkLayer(&link);
  fprintf(fp, "Link Layer:\r");
  fprintf(fp, "  MethodID: $%04x\r", link.liMethodID);
  fprintf(fp, "  Name:     %b\r", link.liName);
  VersionString(0, link.liVersion, buffer);
  fprintf(fp, "  Version:  %b\r", buffer);
  fprintf(fp, "  Flags:    $%04x\r", link.liFlags);

  lv = TCPIPGetLinkVariables();
  fprintf(fp, "Link Variables\r");
  fprintf(fp, "  Version: %d\r", lv->lvVersion);
  fprintf(fp, "  Connected: $%04x\r", lv->lvConnected);
  TCPIPConvertIPToASCII(lv->lvIPaddress, buffer, 0);
  fprintf(fp, "  IP Address: %b\r", buffer);
  fprintf(fp, "  RefCon: $%08lx\r", lv->lvRefCon);
  fprintf(fp, "  Errors: $%08lx\r", lv->lvErrors);
  fprintf(fp, "  MTU: %d\r", lv->lvMTU);


  // connect status
  fprintf(fp, "Connect Status: $%04x\r", TCPIPGetConnectStatus());

  // ip address
  l = TCPIPGetMyIPAddress();
  TCPIPConvertIPToASCII(l, buffer, 0);
  fprintf(fp, "IP Address: %b\r", buffer);

  // dns
  TCPIPGetDNS(&dns);
  TCPIPConvertIPToASCII(dns.DNSMain, buffer, 0);
  fprintf(fp, "DNS 1: %b\r", buffer);
  TCPIPConvertIPToASCII(dns.DNSAux, buffer, 0);
  fprintf(fp, "DNS 2: %b\r", buffer);

  // hostname
  TCPIPGetHostName((hnBuffPtr)buffer);
  fprintf(fp, "Hostname: %b\r", buffer);

  //mtu
  fprintf(fp, "MTU: %d\r", TCPIPGetMTU());
  fprintf(fp, "Alive Flag: $%04x\r", TCPIPGetAliveFlag());
  fprintf(fp, "Alive Minutes: %d\r", TCPIPGetAliveMinutes());
  fprintf(fp, "Login Count: %d\r", TCPIPGetLoginCount());
         


  // error table
  fprintf(fp, "\rError Table\r");
  lw = (LongWord *)TCPIPGetErrorTable();
  count = lw[0] >> 2;
  if (count > sizeof(errStrings) / 4) count = sizeof(errStrings) / 4;

  for (i = 0; i < count; i++);
  {
    fprintf(fp, "  %s -- $%08lx\r", errStrings[i], lw[i]);
  }

  // tuning table
  tune[0] = 10;

  fprintf(fp, "\rTuning Table\r");
  TCPIPGetTuningTable((tunePtr)tune);
  count = tune[0] >> 1;
  if (count > sizeof(tuneStrings) / 4) count = sizeof(tuneStrings) / 4;
  for (i = 0; i < count; i++)
  {
    fprintf(fp, "  %s -- $%04x\r", tuneStrings[i], tune[i]);
  }

  fprintf(fp, "\rDirect Page\r");
  dump(fp, (void *)TCPIPGetDP(), 0x0100);

  // dump all user records.  ipid will be even numbers in the range 0..98
  for (i = 0; i < 100; i += 2)
  {
    h = (Handle)TCPIPGetUserRecord(i);
    if (h == NULL) continue;
    size = (Word)GetHandleSize(h);
    if (_toolErr) continue;


    fprintf(fp, "\rIpid %d\r", i);

    fprintf(fp, "Datagram count (all): %d\r",
      TCPIPGetDatagramCount(i, protocolAll));

    fprintf(fp, "Datagram count (icmp): %d\r",
      TCPIPGetDatagramCount(i, protocolICMP));

    fprintf(fp, "Datagram count (tcp): %d\r",
      TCPIPGetDatagramCount(i, protocolTCP));

    fprintf(fp, "Datagram count (udp): %d\r",
      TCPIPGetDatagramCount(i, protocolUDP));


    fprintf(fp, "User statistic 1: $%08lx\r",
      TCPIPGetUserStatistic(i, 1));

    fprintf(fp, "User statistic 2: $%08lx\r",
      TCPIPGetUserStatistic(i, 2));

#if 0  // tcpipStatusUDP has a stack imbalance bug.

    TCPIPStatusUDP(i, &udp);
    if (_toolErr == 0)
    {
      fprintf(fp, "\rStatus UDP\r");
      fprintf(fp, "  Queue Size: %d\r", udp.uvQueueSize);
      fprintf(fp, "  Error: %d\r", udp.uvError);
      fprintf(fp, "  Error Tick: %ld\r", udp.uvErrorTick);
      fprintf(fp, "  Count: %ld\r", udp.uvCount);
      fprintf(fp, "  Total Count: %ld\r", udp.uvTotalCount);
      fprintf(fp, "  Dispatch Flag: %d\r", udp.uvDispatchFlag);
                                               
    }
#endif

    TCPIPStatusTCP(i, &tcp);
    if (_toolErr == 0)
    {
      fprintf(fp, "\rStatus TCP\r");
      fprintf(fp, "  State: %d\r", tcp.srState);
      fprintf(fp, "  Network Error: %d\r", tcp.srNetworkError);
      fprintf(fp, "  Send Queued: %ld\r", tcp.srSndQueued);
      fprintf(fp, "  Recv Queued: %ld\r", tcp.srRcvQueued);
      TCPIPConvertIPToASCII(tcp.srDestIP, buffer, 0);
      fprintf(fp, "  Dest IP: %b\r", buffer);
      fprintf(fp, "  Dest Port: %d\r", tcp.srDestPort);
      fprintf(fp, "  Connect Type: %d\r", tcp.srConnectType);
      fprintf(fp, "  Accept Count: %d\r", tcp.srAcceptCount);
    }                                                         


    HLock(h);
    if (1) // (size == sizeof(userRecord))
    {
      GSString255Ptr appName;
      userRecord *rec = (userRecord *)*h;
      fprintf(fp, "\rUser Record\r");
      appName = (GSString255Ptr)LGetPathname2(rec->uwUserID, 1);
      if (_toolErr == 0)
      {
        fprintf(fp, "\rApplication %.*s\r", appName->length, appName->text);
      }
      #include "ur.c"
    }
    else dump(fp, *h, size);
    HUnlock(h);
  }

  fclose(fp);
}



#if 0

void		_beginStackCheck (void);
int		_endStackCheck (void);

void main (void)
{
  _beginStackCheck();
  debug();
  fprintf(fp "%d bytes of stack used\r", _endStackCheck());
}
#endif
