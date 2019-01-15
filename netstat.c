/*=================================================
*
* netstat.c - Code to dump network statistics for command line
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

#pragma lint -1
#pragma optimize -1

#ifdef CDA
#pragma noroot
#endif

#include <intmath.h>
//#include <texttool.h>
#include <tcpip.h>
#include <gsos.h>

//#include <unistd.h>

extern pascal void __gsos(void *, word) inline(0, 0xe100b0);

#undef ReadGS
#undef WriteGS

#define ReadGS(parm) __gsos(parm, 0x2012)
#define WriteGS(parm) __gsos(parm, 0x2013)


extern Word GetKey(void);


static IORecGS ioDCB;

const char Header1[] =
" Ipid State       Address         sPort dPort RcvQueue  SendQueue\r";
const char Header2[] =
" ---- -----       -------         ----- ----- --------- ---------\r";
///xxxxx xxxxxxxxxxx xxx.xxx.xxx.xxx xxxxx xxxxx $xxxxxxxx $xxxxxxxx

void Display(Word ipid, srBuff *srBuffer)
{
static char buffer[80] =
"xxxxx "		// 6	ipid
"xxxxxxxxxxx "		// 12	state
"xxx.xxx.xxx.xxx "	// 16	address
"xxxxx "		// 6	source port
"xxxxx "		// 6	dest port
"$xxxxxxxx "		// 10   rcv queue
"$xxxxxxxx ";		// 10	send queue
static char buffer16[16];

static char *stateStr =
"CLOSED      "
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
Word i,j;


  TCPIPGetDestination(ipid, &dest);

  for (i = 0; i < 80; i++)  buffer[i] = ' ';

  Int2Dec(ipid, buffer, 5, 0);

  cp = buffer + 6;

  i = srBuffer->srState;
  if (i > 10) i = 11;

  i *= 12;  // sizeof the string...
  for (j = 0; j < 12; j++)
    *cp++ = stateStr[i + j];

 cp = buffer + 18;

 // includes the last space, so we're ok.

  TCPIPConvertIPToCASCII(srBuffer->srDestIP, buffer16, 0);
  for (j = 0;  buffer16[j]; j++)
    *cp++ = buffer16[j];


  i = TCPIPGetSourcePort(ipid);
  Int2Dec(i, buffer + 34, 5, 0);

  Int2Dec(srBuffer->srDestPort, buffer + 40, 5, 0);

  buffer[46] = '$';
  Long2Hex(srBuffer->srRcvQueued, buffer + 47, 8);

  buffer[56] = '$';
  Long2Hex(srBuffer->srSndQueued, buffer + 57, 8);

  buffer[66] = '\r';

  ioDCB.dataBuffer = buffer;
  ioDCB.requestCount = 67;
  WriteGS(&ioDCB);
}

void netstat(Word fd)
{
Word i;
Word count;

static srBuff srBuffer;

#ifdef CDA
Word line = 0;
char c;
#endif


  ioDCB.pCount = 5;
  ioDCB.refNum = fd;
  ioDCB.cachePriority = 0;

  #ifdef CDA

  #define xstr "\x0c" // 80 column on, clear screen
  ioDCB.dataBuffer = xstr;
  ioDCB.requestCount = sizeof(xstr) - 1;
  WriteGS(&ioDCB);
  #undef xstr

  #endif            

  if (TCPIPStatus() == 0 || _toolErr)
  {
    #define xstr "Marinetti is not active\r"
    ioDCB.dataBuffer = xstr;
    ioDCB.requestCount = sizeof(xstr) - 1;
    WriteGS(&ioDCB);
    #undef xstr

    #ifdef CDA
    //ioDCB.requestCount = 1;
    //ioDCB.dataBuffer = &c;
    //ReadGS(&ioDCB);
    c = GetKey();
    #endif

    return;
  }

  ioDCB.dataBuffer = Header1;
  ioDCB.requestCount = sizeof(Header1) - 1;
  WriteGS(&ioDCB);

  ioDCB.dataBuffer = Header2;
  ioDCB.requestCount = sizeof(Header2) - 1;
  WriteGS(&ioDCB);

  #ifdef CDA
  line = 2;
  #endif

  count = TCPIPGetLoginCount();
  if (count)
  {
    Word found;

    // Marinetti v3 has a maximum table size of 100.
    for (found = 0, i = 0; i < 100; i++)
    {
      TCPIPStatusTCP(i, &srBuffer);
      if (_toolErr) continue;

      #ifdef CDA
      line++;
      if (line == 24)
      {
        #define xstr "-- more --"
        ioDCB.dataBuffer = xstr;
        ioDCB.requestCount = sizeof(xstr) - 1;
        WriteGS(&ioDCB);
        #undef xstr

        //ioDCB.requestCount = 1;
        //ioDCB.dataBuffer = &c;
        //ReadGS(&ioDCB);
        c = GetKey();

        //if (c == 0x0d)  // return == scroll 1 line.
        //{
        //  // ^Z == clear line
        //  #define xstr "\Z"
        //  ioDCB.dataBuffer = xstr;
        //  ioDCB.requestCount = sizeof(xstr) - 1;
        //  WriteGS(&ioDCB);
        //  #undef xstr
        //
        //  line--;          
        //}
        if (c == ' ' || c == 0x0d)  // space == scroll new page.
        {
          // ^L == clear screen.
          #define xstr "\x0c"
          ioDCB.dataBuffer = xstr;
          ioDCB.requestCount = sizeof(xstr) - 1;
          WriteGS(&ioDCB);
          #undef xstr

          line = 2;

          ioDCB.dataBuffer = Header1;
          ioDCB.requestCount = sizeof(Header1) - 1;
          WriteGS(&ioDCB);

          ioDCB.dataBuffer = Header2;
          ioDCB.requestCount = sizeof(Header2) - 1;
          WriteGS(&ioDCB);
        }
        else return;

      }
      #endif

      Display(i, &srBuffer);
      found++;
      if (found == count) break;
    }
  }
  #ifdef CDA
  //ioDCB.requestCount = 1;
  //ioDCB.dataBuffer = &c;
  //ReadGS(&ioDCB);
  c = GetKey();
  #endif
}


#ifndef CDA
int main(int argc, char **argv)
{
  netstat(2); // 2 = stdout
  return 0;
}
#endif
