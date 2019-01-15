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

fprintf(fp, "  uwUserID: %04x\r", rec->uwUserID);
fprintf(fp, "  uwDestIP: %08lx\r", rec->uwDestIP);
fprintf(fp, "  uwDestPort: %04x\r", rec->uwDestPort);
fprintf(fp, "  uwIP_TOS: %04x\r", rec->uwIP_TOS);
fprintf(fp, "  uwIP_TTL: %04x\r", rec->uwIP_TTL);

fprintf(fp, "  uwSourcePort: %04x\r", rec->uwSourcePort);
fprintf(fp, "  uwLogoutPending: %04x\r", rec->uwLogoutPending);
fprintf(fp, "  uwICMPQueue: %08lx\r", rec->uwICMPQueue);
fprintf(fp, "  uwTCPQueue: %08lx\r", rec->uwTCPQueue);

fprintf(fp, "  uwTCPMaxSendSeg: %04x\r", rec->uwTCPMaxSendSeg);
fprintf(fp, "  uwTCPMaxReceiveSeg: %04x\r", rec->uwTCPMaxReceiveSeg);
fprintf(fp, "  uwTCPDataInQ: %08lx\r", rec->uwTCPDataInQ);
fprintf(fp, "  uwTCPDataIn: %08lx\r", rec->uwTCPDataIn);
fprintf(fp, "  uwTCPPushInFlag: %04x\r", rec->uwTCPPushInFlag);
fprintf(fp, "  uwTCPPushInOffset: %08lx\r", rec->uwTCPPushInOffset);
fprintf(fp, "  uwTCPPushOutFlag: %04x\r", rec->uwTCPPushOutFlag);
fprintf(fp, "  uwTCPPushOutSEQ: %08lx\r", rec->uwTCPPushOutSEQ);
fprintf(fp, "  uwTCPDataOut: %08lx\r", rec->uwTCPDataOut);
fprintf(fp, "  uwSND_UNA: %08lx\r", rec->uwSND_UNA);
fprintf(fp, "  uwSND_NXT: %08lx\r", rec->uwSND_NXT);
fprintf(fp, "  uwSND_WND: %04x\r", rec->uwSND_WND);
fprintf(fp, "  uwSND_UP: %04x\r", rec->uwSND_UP);
fprintf(fp, "  uwSND_WL1: %08lx\r", rec->uwSND_WL1);
fprintf(fp, "  uwSND_WL2: %08lx\r", rec->uwSND_WL2);
fprintf(fp, "  uwISS: %08lx\r", rec->uwISS);
fprintf(fp, "  uwRCV_NXT: %08lx\r", rec->uwRCV_NXT);
fprintf(fp, "  uwRCV_WND: %04x\r", rec->uwRCV_WND);
fprintf(fp, "  uwRCV_UP: %04x\r", rec->uwRCV_UP);
fprintf(fp, "  uwIRS: %08lx\r", rec->uwIRS);
fprintf(fp, "  uwTCP_State: %04x\r", rec->uwTCP_State);
fprintf(fp, "  uwTCP_StateTick: %08lx\r", rec->uwTCP_StateTick);
fprintf(fp, "  uwTCP_ErrCode: %04x\r", rec->uwTCP_ErrCode);
fprintf(fp, "  uwTCP_ICMPError: %04x\r", rec->uwTCP_ICMPError);
fprintf(fp, "  uwTCP_Server: %04x\r", rec->uwTCP_Server);
fprintf(fp, "  uwTCP_ChildList: %08lx\r", rec->uwTCP_ChildList);
fprintf(fp, "  uwTCP_ACKPending: %04x\r", rec->uwTCP_ACKPending);
fprintf(fp, "  uwTCP_ForceFIN: %04x\r", rec->uwTCP_ForceFIN);
fprintf(fp, "  uwTCP_FINSEQ: %08lx\r", rec->uwTCP_FINSEQ);
fprintf(fp, "  uwTCP_MyFINACKed: %04x\r", rec->uwTCP_MyFINACKed);
fprintf(fp, "  uwTCP_Timer: %08lx\r", rec->uwTCP_Timer);
fprintf(fp, "  uwTCP_TimerState: %04x\r", rec->uwTCP_TimerState);
fprintf(fp, "  uwTCP_rt_timer: %04x\r", rec->uwTCP_rt_timer);
fprintf(fp, "  uwTCP_2MSL_timer: %04x\r", rec->uwTCP_2MSL_timer);
fprintf(fp, "  uwTCP_SaveTTL: %04x\r", rec->uwTCP_SaveTTL);
fprintf(fp, "  uwTCP_SaveTOS: %04x\r", rec->uwTCP_SaveTOS);
fprintf(fp, "  uwTCP_TotalIN: %08lx\r", rec->uwTCP_TotalIN);
fprintf(fp, "  uwTCP_TotalOUT: %08lx\r", rec->uwTCP_TotalOUT);

fprintf(fp, "  uwUDP_Server : %04x\r", rec->uwUDP_Server);
fprintf(fp, "  uwUDPQueue : %08lx\r", rec->uwUDPQueue);
fprintf(fp, "  uwUDPError : %04x\r", rec->uwUDPError);
fprintf(fp, "  uwUDPErrorTick : %08lx\r", rec->uwUDPErrorTick);
fprintf(fp, "  uwUDPCount : %08lx\r", rec->uwUDPCount);

fprintf(fp, "  uwTriggers[0]: %08lx\r", rec->uwTriggers[0]);
fprintf(fp, "  uwSysTriggers[0]: %08lx\r", rec->uwSysTriggers[0]);
