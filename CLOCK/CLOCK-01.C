#include <dos.h>
#include <conio.h>

/***********************************************
 * DOS real-time clock display using timer tick
 * interrupt (INT 1C). Updates time display in
 * upper left corner of VGA text mode screen.
 ***********************************************/

typedef void interrupt far (*fp)();
fp old1c;

/* Last second value to detect time changes */
unsigned char last_sec = 0;

/* VGA text memory at B800:0000 (color text mode) */
unsigned char far *vid = (unsigned char far *)0xB8000000L;

/************************************************
 * Timer interrupt handler - INT 1C (18.2Hz)
 * Updates screen clock display every second
 ************************************************/
void interrupt tick() {
    unsigned char h, m, s;
    
    /* Save all registers - critical section */
    asm cli;
    asm pushf; 
    asm push ax; asm push bx; asm push cx; asm push dx;
    asm push si; asm push di; asm push bp; asm push es;
    
    /* Get current time from DOS INT 1A function 02h */
    asm mov ah, 0x02;
    asm int 1AH;
    
    /* Transfer BCD time values from registers to C vars:
     * CH=hours(0-23), CL=minutes(0-59), DH=seconds(0-59) */
    h = _CH; m = _CL; s = _DH;
    
    /* Update display only when second changes */
    if (s != last_sec) {
        last_sec = s;
        
        /* Write HH:MM to screen positions 0-9 (5 chars 2 bytes) */
        vid[ 0] = h/10+'0'; vid[ 1]=0x07;  /* Hour tens */
        vid[ 2] = h%10+'0'; vid[ 3]=0x07;  /* Hour ones */
        vid[ 4] = ':';      vid[ 5]=0x07;  /* Colon */
        vid[ 6] = m/10+'0'; vid[ 7]=0x07;  /* Min tens */
        vid[ 8] = m%10+'0'; vid[ 9]=0x07;  /* Min ones */
    }
    
    /* Restore registers and chain to original handler */
    asm pop es; asm pop bp; asm pop di; asm pop si;
    asm pop dx; asm pop cx; asm pop bx; asm pop ax;
    asm popf;
    asm sti;
    
    (*old1c)();  /* Call original INT 1C handler */
}

/**********************************************************
 * Main program - Installs interrupt handler and waits
 * indefinitely for keypress to exit gracefully
 **********************************************************/
void main(void) {
    /* Install custom timer interrupt handler */
    old1c = getvect(0x1C);
    setvect(0x1C, tick);
    keep(0,2000/16);
}
