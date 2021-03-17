// Boot loader.
//
// Part of the boot block, along with bootasm.S, which calls bootmain().
// bootasm.S has put the processor into protected 32-bit mode and calls
// helloC() function. helloC() just prompts "OK!" on color graphics adapter
// (CGA) display.
#include "types.h"
#include "x86.h"

#define CRTPORT 0x3d4

static void vga_putchar(char c) {
  ushort *crt = (ushort*)0xb8000;  // CGA frame buffer (memory mapped address)
  int pos;

  // get cursor position: col + 80*row.
  outb(CRTPORT, 14);
  pos = inb(CRTPORT+1) << 8;
  outb(CRTPORT, 15);
  pos |= inb(CRTPORT+1);

  // put character on screen (white color on black background)
  crt[pos++] = (c & 0xff) | 0x0700;

  // update cursor position and put a space (move right)
  outb(CRTPORT, 14);
  outb(CRTPORT+1, pos>>8);
  outb(CRTPORT, 15);
  outb(CRTPORT+1, pos);
  crt[pos] = ' ' | 0x0700;
}

void hello(void)
{
    vga_putchar('O');
    vga_putchar('K');
    vga_putchar('!');

    // wait for ever
    for(;;) 
        ;
}
