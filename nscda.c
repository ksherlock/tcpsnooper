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

#pragma lint -1
#pragma optimize -1

#pragma noroot

#include <intmath.h>
#include <texttool.h>
#include <tcpip.h>

#include <stdio.h>


extern Word GetKey(void);
extern void debug(void);

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
  buffer[67] = 0;

  WriteCString(buffer);

}

void netstat(void)
{
Word i;
Word count;

static srBuff srBuffer;

Word line = 0;
char c;


  if (TCPIPStatus() == 0 || _toolErr)
  {
    WriteCString("Marinetti is not active\r");
    c = GetKey();
    return;
  }

  WriteCString(Header1);
  WriteCString(Header2);

  line = 2;

  count = TCPIPGetLoginCount();
  if (count)
  {
    Word found;

    // Marinetti v3 has a maximum table size of 100.
    for (found = 0, i = 0; i < 100; i += 2)
    {
      TCPIPStatusTCP(i, &srBuffer);
      if (_toolErr) continue;

      line++;
      if (line == 24)
      {
        WriteCString("-- more --");

        c = GetKey();

        if (c == ' ' || c == 0x0d)  // space == scroll new page.
        {
          // ^L == clear screen.
          WriteChar(0x0c);

          WriteCString(Header1);
          WriteCString(Header2);
          line = 2;
        }
        else return;

      }

      Display(i, &srBuffer);
      found++;
      if (found == count) break;
    }
  }
  WriteCString("\rQ: Quit.  D: Save debug file");
  c = GetKey();
  if (c == 'D' || c == 'd') debug();
}
