*=================================================
*
* cda.asm - Main code for TCP Snooper CDA
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
*=================================================

	case on
	copy /lang/orca/libraries/ainclude/e16.gsos
	copy /lang/orca/libraries/ainclude/e16.event
	copy /lang/orca/libraries/ainclude/e16.memory
                      
	mcopy cda.mac


OS_KIND	gequ	$e100bc	
RD80VID	gequ	$e0c01f
SET80VID gequ	$e0c00d


StackSize gequ	$0300

CDARoot	start
	
	dw	'TCP Snooper'
	dc	i4'CDAStart'
	dc	i4'CDAShutDown'
	end

CDAStart	start

	using CDAData

	phb
	phk
	plb

	lda >OS_KIND		;; 00 = prodos, 01 == gsos
	and #$00ff
	bne gsos
	brl exit

gsos	anop

* allocate $300 of stack space.

	pha
	_MMStartUp
	pla
	sta MyID

	pha
	pha
	~NewHandle #StackSize,MyID,#attrLocked+attrFixed+attrBank+attrPage,#0
	bcc gotstack
	pla
	pla
	brl exit
	
gotstack anop

	tsc
	sta OldStack
	tcd
	lda [1]
	tcd
	clc
	adc #StackSize-1

* copy old stack to new stack
	pha
	sei
	ldx #$fe
sloop	lda >$000100,x
	sta stack,x
	dex
	dex
	bpl sloop
	cli
	pla
	tcs
	

*	short m
*	sta >SET80VID
*	long m

	pha
	pha
	pha
	_GetInputDevice

	pha
	pha
	_GetInGlobals

	pha
	pha
	pha
	_GetOutputDevice

	pha
	pha
	_GetOutGlobals
*
	~SetInputDevice #0,#3
	~SetInGlobals #$ff,#$80

	~SetOutputDevice #0,#3
	~SetOutGlobals #$ff,#$80
	
	~InitTextDev #0
	~InitTextDev #1


	~WriteChar #$0C

	jsl netstat

* restore, arguments already on stack.

	_SetOutGlobals
	_SetOutputDevice
	_SetInGlobals
	_SetInputDevice

	~InitTextDev #0
	~InitTextDev #1


* restore old stack
	sei
	ldx #$fe
rloop	lda stack,x
	sta >$000100,x
	dex
	dex
	bpl rloop
	lda OldStack
	tcs
	cli
	_DisposeHandle	; * handle still on stack.


exit	anop
	plb

CDAShutDown	entry
	rtl
	end

GetKey	start
	using CDAData

	pea 0
	_EMStatus
	pla
	beq metal

evloop	anop
	pha
	~GetNextEvent #keyDownMask+autoKeyMask,#Event
	pla
	beq evloop
	lda Event+omessage
	rtl	
	
metal	anop

	short m

mloop	anop

	lda >$e0c0000
	bpl mloop
	sta >$e0c010

	long m
	and #$007f	
	rtl
	end



CDAData	data

MyID	ds 2
OldStack ds 2

Event	ds 16
stack	ds 256
	end
