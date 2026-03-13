#include <dos.h>

typedef void interrupt far (*fp)();
fp old1c;

unsigned char last_sec = 0;

unsigned char far *vid = (unsigned char far *)0xB8000000L;

void interrupt tick() {
    unsigned char h, m, s;
    int x=72*2,y=0;

    asm cli;
    asm pushf;
    asm push ax; asm push bx; asm push cx; asm push dx;
    asm push si; asm push di; asm push bp; asm push es;

    asm mov ah, 0x02;
    asm int 1AH;

    /* Transfer BCD time values from registers to C vars:
     * CH=hours(0-23), CL=minutes(0-59), DH=seconds(0-59) */
    h = _CH; m = _CL; s = _DH;

    if (s != last_sec) {
        last_sec = s;

        vid[x+ 0] = (h>>4) & 0x0F | '0'; vid[x+ 1]=0x1E;
        vid[x+ 2] = (h>>0) & 0x0F | '0'; vid[x+ 3]=0x1E;
        vid[x+ 4] =                 ':'; vid[x+ 5]=0x1E;
        vid[x+ 6] = (m>>4) & 0x0F | '0'; vid[x+ 7]=0x1E;
        vid[x+ 8] = (m>>0) & 0x0F | '0'; vid[x+ 9]=0x1E;
        vid[x+10] =                 ':'; vid[x+11]=0x1E;
        vid[x+12] = (s>>4) & 0x0F | '0'; vid[x+13]=0x1E;
        vid[x+14] = (s>>0) & 0x0F | '0'; vid[x+15]=0x1E;
    }

    asm pop es; asm pop bp; asm pop di; asm pop si;
    asm pop dx; asm pop cx; asm pop bx; asm pop ax;
    asm popf;
    asm sti;

    (*old1c)();
}

void main(void) {
    old1c = getvect(0x1C);
    setvect(0x1C, tick);
    keep(0,512);
}
