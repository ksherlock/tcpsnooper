/*=================================================
*
* ur.c - Code to dump Marinetti user record for TCP Snooper CDA
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

fdprintf(fd, "  uwUserID: %04x\r", rec->uwUserID);
fdprintf(fd, "  uwDestIP: %08lx\r", rec->uwDestIP);
fdprintf(fd, "  uwDestPort: %04x\r", rec->uwDestPort);
fdprintf(fd, "  uwIP_TOS: %04x\r", rec->uwIP_TOS);
fdprintf(fd, "  uwIP_TTL: %04x\r", rec->uwIP_TTL);

fdprintf(fd, "  uwSourcePort: %04x\r", rec->uwSourcePort);
fdprintf(fd, "  uwLogoutPending: %04x\r", rec->uwLogoutPending);
fdprintf(fd, "  uwICMPQueue: %08lx\r", rec->uwICMPQueue);
fdprintf(fd, "  uwTCPQueue: %08lx\r", rec->uwTCPQueue);

fdprintf(fd, "  uwTCPMaxSendSeg: %04x\r", rec->uwTCPMaxSendSeg);
fdprintf(fd, "  uwTCPMaxReceiveSeg: %04x\r", rec->uwTCPMaxReceiveSeg);
fdprintf(fd, "  uwTCPDataInQ: %08lx\r", rec->uwTCPDataInQ);
fdprintf(fd, "  uwTCPDataIn: %08lx\r", rec->uwTCPDataIn);
fdprintf(fd, "  uwTCPPushInFlag: %04x\r", rec->uwTCPPushInFlag);
fdprintf(fd, "  uwTCPPushInOffset: %08lx\r", rec->uwTCPPushInOffset);
fdprintf(fd, "  uwTCPPushOutFlag: %04x\r", rec->uwTCPPushOutFlag);
fdprintf(fd, "  uwTCPPushOutSEQ: %08lx\r", rec->uwTCPPushOutSEQ);
fdprintf(fd, "  uwTCPDataOut: %08lx\r", rec->uwTCPDataOut);
fdprintf(fd, "  uwSND_UNA: %08lx\r", rec->uwSND_UNA);
fdprintf(fd, "  uwSND_NXT: %08lx\r", rec->uwSND_NXT);
fdprintf(fd, "  uwSND_WND: %04x\r", rec->uwSND_WND);
fdprintf(fd, "  uwSND_UP: %04x\r", rec->uwSND_UP);
fdprintf(fd, "  uwSND_WL1: %08lx\r", rec->uwSND_WL1);
fdprintf(fd, "  uwSND_WL2: %08lx\r", rec->uwSND_WL2);
fdprintf(fd, "  uwISS: %08lx\r", rec->uwISS);
fdprintf(fd, "  uwRCV_NXT: %08lx\r", rec->uwRCV_NXT);
fdprintf(fd, "  uwRCV_WND: %04x\r", rec->uwRCV_WND);
fdprintf(fd, "  uwRCV_UP: %04x\r", rec->uwRCV_UP);
fdprintf(fd, "  uwIRS: %08lx\r", rec->uwIRS);
fdprintf(fd, "  uwTCP_State: %04x\r", rec->uwTCP_State);
fdprintf(fd, "  uwTCP_StateTick: %08lx\r", rec->uwTCP_StateTick);
fdprintf(fd, "  uwTCP_ErrCode: %04x\r", rec->uwTCP_ErrCode);
fdprintf(fd, "  uwTCP_ICMPError: %04x\r", rec->uwTCP_ICMPError);
fdprintf(fd, "  uwTCP_Server: %04x\r", rec->uwTCP_Server);
fdprintf(fd, "  uwTCP_ChildList: %08lx\r", rec->uwTCP_ChildList);
fdprintf(fd, "  uwTCP_ACKPending: %04x\r", rec->uwTCP_ACKPending);
fdprintf(fd, "  uwTCP_ForceFIN: %04x\r", rec->uwTCP_ForceFIN);
fdprintf(fd, "  uwTCP_FINSEQ: %08lx\r", rec->uwTCP_FINSEQ);
fdprintf(fd, "  uwTCP_MyFINACKed: %04x\r", rec->uwTCP_MyFINACKed);
fdprintf(fd, "  uwTCP_Timer: %08lx\r", rec->uwTCP_Timer);
fdprintf(fd, "  uwTCP_TimerState: %04x\r", rec->uwTCP_TimerState);
fdprintf(fd, "  uwTCP_rt_timer: %04x\r", rec->uwTCP_rt_timer);
fdprintf(fd, "  uwTCP_2MSL_timer: %04x\r", rec->uwTCP_2MSL_timer);
fdprintf(fd, "  uwTCP_SaveTTL: %04x\r", rec->uwTCP_SaveTTL);
fdprintf(fd, "  uwTCP_SaveTOS: %04x\r", rec->uwTCP_SaveTOS);
fdprintf(fd, "  uwTCP_TotalIN: %08lx\r", rec->uwTCP_TotalIN);
fdprintf(fd, "  uwTCP_TotalOUT: %08lx\r", rec->uwTCP_TotalOUT);

fdprintf(fd, "  uwUDP_Server : %04x\r", rec->uwUDP_Server);
fdprintf(fd, "  uwUDPQueue : %08lx\r", rec->uwUDPQueue);
fdprintf(fd, "  uwUDPError : %04x\r", rec->uwUDPError);
fdprintf(fd, "  uwUDPErrorTick : %08lx\r", rec->uwUDPErrorTick);
fdprintf(fd, "  uwUDPCount : %08lx\r", rec->uwUDPCount);

fdprintf(fd, "  uwTriggers[0]: %08lx\r", rec->uwTriggers[0]);
fdprintf(fd, "  uwSysTriggers[0]: %08lx\r", rec->uwSysTriggers[0]);
