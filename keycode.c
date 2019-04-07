#pragma lint - 1
#pragma optimize - 1

#pragma cda "Key Code" StartUp ShutDown

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <misctool.h>

#include <adb.h>

unsigned key = 0;
unsigned mod = 0;

asm void ReadKey(void) {
  lda #0
  sep #0x20
  loop:
  lda >0xe0c000
  bpl loop
  sta >0xe0c010
  and #0x7f
  sta key
  lda >0xe0c025
  sta mod
  rep #0x20
  rtl
}

static unsigned char adb_data[128];

static unsigned char adb_data_size = 0;

asm unsigned ReadADBKey(void) {

loop:
	wai
	lda adb_data_size
	beq loop
	stz adb_data_size
	rtl
}

/*
 * stack:
 * rtl1 [3]
 * rtl2 [3]
 * dataptr [4]
 * CALLED IN 8-BIT NATIVE MODE!!!
 */
asm void adb_poll(void) {

	phb
	phd
	phk
	plb
	tsc
	tcd
	lda [10] ; count
	tax
	stx adb_data_size	
	beq exit

	tay
loop:
	lda [10],y
	bmi clear
	tax
	inc adb_data,x
	bra advance
clear:
	and #0xea7f
	tax
	stz adb_data,x
advance:
	dey
	bne loop

exit:
	pld
	plb
	clc
	rtl
}

void RawADB(unsigned on) {

	static unsigned char buffer = 0x01;

	if (on) {
		SRQPoll((Pointer)adb_poll, 2);
		SendInfo(1, (Pointer)&buffer, setModes);
	} else {
		SendInfo(1, (Pointer)&buffer, clearModes);
		/* need to delay this... */
		WaitUntil(0, 960 * 2); /* 1/8th second */
		//ClearSRQTable();
		SRQRemove(2);		
	}

	putchar(30);
	putchar(32 + 0);
	putchar(32 + 23);
	putchar(29); /* clear line */
	fputs("Raw ADB ", stdout);
	fputs(on ? "On" : "Off", stdout);

}

void Clear(void) {
	putchar(30);
	putchar(32 + 0);
	putchar(32 + 11);
	putchar(29); /* clear line */
	putchar('\r');
	putchar(29); /* clear line */
}

void StartUp(void) {

	unsigned adb = 0;
	unsigned adb_cmd = 0;

	putchar(0x06); /* cursor off */
	putchar(0x0c); /* clear screen */
	fputs("Command-Q: Quit     Command-A: Toggle ADB\r", stdout);
	RawADB(0);

	for(;;) {
		if (adb) {
			unsigned i, n, c;
			n = ReadADBKey();

			if (n) {

				putchar(30);
				putchar(32 + 0);
				putchar(32 + 11);
				putchar(29); /* clear line */

				fputs("ADB Data: ", stdout);
				for (i = 0; i < 128; ++i) {
					if (adb_data[i])
						printf("%02x ", i);
				}

				/* command-A */
				if (adb_data[0x37] && adb_data[0x00]) {
					RawADB(0);
					adb = 0;
					Clear();
					ReadKey();
					continue;
				}
				/* command-Q */
				if (adb_data[0x37] && adb_data[0x0c]) {
					RawADB(0);
					adb = 0;
					Clear();
					ReadKey();
					return;
				}
			}
		} else {
			ReadKey();
			if (key == 'q' || key == 'Q') {
				if (mod & 0x80) return;
			}
			if (key == 'a' || key == 'A') {
				if (mod & 0x80) {
					RawADB(1);

					adb = 1;
					adb_cmd = 0;

					memset(adb_data, 0, sizeof(adb_data));
					Clear();
					continue;
				}
			}

			/* dca */
			putchar(30);
			putchar(32 + 0);
			putchar(32 + 11);
			putchar(29); /* clear line */
			printf("Char: $%02x ", key);
			if (isprint(key)) printf("%c", key);
			putchar('\r');
			putchar(29); /* clear line */
			printf("Mod:  $%02x ", mod);
			if (mod & 0x80) fputs("command ", stdout);
			if (mod & 0x40) fputs("option ", stdout);
			if (mod & 0x20) fputs("updated ", stdout);
			if (mod & 0x10) fputs("keypad ", stdout);
			if (mod & 0x08) fputs("repeat ", stdout);
			if (mod & 0x04) fputs("caps lock ", stdout);
			if (mod & 0x02) fputs("control ", stdout);
			if (mod & 0x01) fputs("shift ", stdout);
		}
	}

}

void ShutDown(void) {

}
