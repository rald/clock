#include <dos.h>
#include <conio.h>
#include <stdlib.h>

typedef void interrupt (*fp)(void);
fp old1c;

unsigned char last_sec = 0;
unsigned char far *vid = (unsigned char far *)0xB8000000L;

void interrupt tick() {
	unsigned char h, m, s;

	asm cli;
	asm pushf;
	asm push ax; asm push bx; asm push cx; asm push dx;
	asm push si; asm push di; asm push bp; asm push ds;

	asm mov ah, 0x02;
	asm mov dl, 0x00;
	asm int 1AH;

	h = _CH; m = _CL; s = _DH;

	if (s != last_sec) {
		last_sec = s;
		vid[ 0] = h/10+'0'; vid[ 1]=0x17;
		vid[ 2] = h%10+'0'; vid[ 3]=0x17;
		vid[ 4] = ':';      vid[ 5]=0x17;
		vid[ 6] = m/10+'0'; vid[ 7]=0x17;
		vid[ 8] = m%10+'0'; vid[ 9]=0x17;
	}

	asm pop ds; asm pop bp; asm pop di; asm pop si;
	asm pop dx; asm pop cx; asm pop bx; asm pop ax;
	asm popf;
	asm sti;

	(*old1c)();
}

int main(void) {
	old1c = getvect(0x1C);
	setvect(0x1C, tick);
	keep(0, 2000/16);
	return 0;
}
