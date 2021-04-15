
bootblock.o:     file format elf32-i386


Disassembly of section .text:

00007c00 <start>:
# with %cs=0 %ip=7c00.

.code16                       # Assemble for 16-bit mode
.globl start
start:
  cli                         # BIOS enabled interrupts; disable
    7c00:	fa                   	cli    

  # Zero data segment registers DS, ES, and SS.
  xorw    %ax,%ax             # Set %ax to zero
    7c01:	31 c0                	xor    %eax,%eax
  movw    %ax,%ds             # -> Data Segment
    7c03:	8e d8                	mov    %eax,%ds
  movw    %ax,%es             # -> Extra Segment
    7c05:	8e c0                	mov    %eax,%es
  movw    %ax,%ss             # -> Stack Segment
    7c07:	8e d0                	mov    %eax,%ss

00007c09 <seta20.1>:

  # Physical address line A20 is tied to zero so that the first PCs 
  # with 2 MB would run software that assumed 1 MB.  Undo that.
seta20.1:
  inb     $0x64,%al               # Wait for not busy
    7c09:	e4 64                	in     $0x64,%al
  testb   $0x2,%al
    7c0b:	a8 02                	test   $0x2,%al
  jnz     seta20.1
    7c0d:	75 fa                	jne    7c09 <seta20.1>

  movb    $0xd1,%al               # 0xd1 -> port 0x64
    7c0f:	b0 d1                	mov    $0xd1,%al
  outb    %al,$0x64
    7c11:	e6 64                	out    %al,$0x64

00007c13 <seta20.2>:

seta20.2:
  inb     $0x64,%al               # Wait for not busy
    7c13:	e4 64                	in     $0x64,%al
  testb   $0x2,%al
    7c15:	a8 02                	test   $0x2,%al
  jnz     seta20.2
    7c17:	75 fa                	jne    7c13 <seta20.2>

  movb    $0xdf,%al               # 0xdf -> port 0x60
    7c19:	b0 df                	mov    $0xdf,%al
  outb    %al,$0x60
    7c1b:	e6 60                	out    %al,$0x60

  # Switch from real to protected mode.  Use a bootstrap GDT that makes
  # virtual addresses map directly to physical addresses so that the
  # effective memory map doesn't change during the transition.
  lgdt    gdtdesc
    7c1d:	0f 01 16             	lgdtl  (%esi)
    7c20:	78 7c                	js     7c9e <vga_putchar+0x20>
  movl    %cr0, %eax
    7c22:	0f 20 c0             	mov    %cr0,%eax
  orl     $CR0_PE, %eax
    7c25:	66 83 c8 01          	or     $0x1,%ax
  movl    %eax, %cr0
    7c29:	0f 22 c0             	mov    %eax,%cr0

//PAGEBREAK!
  # Complete the transition to 32-bit protected mode by using a long jmp
  # to reload %cs and %eip.  The segment descriptors are set up with no
  # translation, so that the mapping is still the identity mapping.
  ljmp    $(SEG_KCODE<<3), $start32
    7c2c:	ea                   	.byte 0xea
    7c2d:	31 7c 08 00          	xor    %edi,0x0(%eax,%ecx,1)

00007c31 <start32>:

.code32  # Tell assembler to generate 32-bit code now.
start32:
  # Set up the protected-mode data segment registers
  movw    $(SEG_KDATA<<3), %ax    # Our data segment selector
    7c31:	66 b8 10 00          	mov    $0x10,%ax
  movw    %ax, %ds                # -> DS: Data Segment
    7c35:	8e d8                	mov    %eax,%ds
  movw    %ax, %es                # -> ES: Extra Segment
    7c37:	8e c0                	mov    %eax,%es
  movw    %ax, %ss                # -> SS: Stack Segment
    7c39:	8e d0                	mov    %eax,%ss
  movw    $0, %ax                 # Zero segments not ready for use
    7c3b:	66 b8 00 00          	mov    $0x0,%ax
  movw    %ax, %fs                # -> FS
    7c3f:	8e e0                	mov    %eax,%fs
  movw    %ax, %gs                # -> GS
    7c41:	8e e8                	mov    %eax,%gs

  # Set up the stack pointer and call into C.
  movl    $start, %esp
    7c43:	bc 00 7c 00 00       	mov    $0x7c00,%esp
  call    hello
    7c48:	e8 ab 00 00 00       	call   7cf8 <hello>

  # If bootmain returns (it shouldn't), trigger a Bochs
  # breakpoint if running under Bochs, then loop.
  movw    $0x8a00, %ax            # 0x8a00 -> port 0x8a00
    7c4d:	66 b8 00 8a          	mov    $0x8a00,%ax
  movw    %ax, %dx
    7c51:	66 89 c2             	mov    %ax,%dx
  outw    %ax, %dx
    7c54:	66 ef                	out    %ax,(%dx)
  movw    $0x8ae0, %ax            # 0x8ae0 -> port 0x8a00
    7c56:	66 b8 e0 8a          	mov    $0x8ae0,%ax
  outw    %ax, %dx
    7c5a:	66 ef                	out    %ax,(%dx)

00007c5c <spin>:
spin:
  jmp     spin
    7c5c:	eb fe                	jmp    7c5c <spin>
    7c5e:	66 90                	xchg   %ax,%ax

00007c60 <gdt>:
	...
    7c68:	ff                   	(bad)  
    7c69:	ff 00                	incl   (%eax)
    7c6b:	00 00                	add    %al,(%eax)
    7c6d:	9a cf 00 ff ff 00 00 	lcall  $0x0,$0xffff00cf
    7c74:	00                   	.byte 0x0
    7c75:	92                   	xchg   %eax,%edx
    7c76:	cf                   	iret   
	...

00007c78 <gdtdesc>:
    7c78:	17                   	pop    %ss
    7c79:	00 60 7c             	add    %ah,0x7c(%eax)
	...

00007c7e <vga_putchar>:
#include "types.h"
#include "x86.h"

#define CRTPORT 0x3d4

static void vga_putchar(char c) {
    7c7e:	55                   	push   %ebp
    7c7f:	89 e5                	mov    %esp,%ebp
    7c81:	57                   	push   %edi
    7c82:	56                   	push   %esi
    7c83:	53                   	push   %ebx
    7c84:	83 ec 04             	sub    $0x4,%esp
    7c87:	89 45 f0             	mov    %eax,-0x10(%ebp)
}

static inline void
outb(ushort port, uchar data)
{
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
    7c8a:	be d4 03 00 00       	mov    $0x3d4,%esi
    7c8f:	b8 0e 00 00 00       	mov    $0xe,%eax
    7c94:	89 f2                	mov    %esi,%edx
    7c96:	ee                   	out    %al,(%dx)
  asm volatile("in %1,%0" : "=a" (data) : "d" (port));
    7c97:	bb d5 03 00 00       	mov    $0x3d5,%ebx
    7c9c:	89 da                	mov    %ebx,%edx
    7c9e:	ec                   	in     (%dx),%al
  ushort *crt = (ushort*)0xb8000;  // CGA frame buffer (memory mapped address)
  int pos;

  // get cursor position: col + 80*row.
  outb(CRTPORT, 14);
  pos = inb(CRTPORT+1) << 8;
    7c9f:	0f b6 c8             	movzbl %al,%ecx
    7ca2:	c1 e1 08             	shl    $0x8,%ecx
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
    7ca5:	b8 0f 00 00 00       	mov    $0xf,%eax
    7caa:	89 f2                	mov    %esi,%edx
    7cac:	ee                   	out    %al,(%dx)
  asm volatile("in %1,%0" : "=a" (data) : "d" (port));
    7cad:	89 da                	mov    %ebx,%edx
    7caf:	ec                   	in     (%dx),%al
  outb(CRTPORT, 15);
  pos |= inb(CRTPORT+1);
    7cb0:	0f b6 c0             	movzbl %al,%eax
    7cb3:	09 c1                	or     %eax,%ecx

  // put character on screen (white color on black background)
  crt[pos++] = (c & 0xff) | 0x0700;
    7cb5:	8d 79 01             	lea    0x1(%ecx),%edi
    7cb8:	01 c9                	add    %ecx,%ecx
    7cba:	0f b6 45 f0          	movzbl -0x10(%ebp),%eax
    7cbe:	80 cc 07             	or     $0x7,%ah
    7cc1:	66 89 81 00 80 0b 00 	mov    %ax,0xb8000(%ecx)
  asm volatile("out %0,%1" : : "a" (data), "d" (port));
    7cc8:	b8 0e 00 00 00       	mov    $0xe,%eax
    7ccd:	89 f2                	mov    %esi,%edx
    7ccf:	ee                   	out    %al,(%dx)

  // update cursor position and put a space (move right)
  outb(CRTPORT, 14);
  outb(CRTPORT+1, pos>>8);
    7cd0:	89 fa                	mov    %edi,%edx
    7cd2:	c1 fa 08             	sar    $0x8,%edx
    7cd5:	89 d0                	mov    %edx,%eax
    7cd7:	89 da                	mov    %ebx,%edx
    7cd9:	ee                   	out    %al,(%dx)
    7cda:	b8 0f 00 00 00       	mov    $0xf,%eax
    7cdf:	89 f2                	mov    %esi,%edx
    7ce1:	ee                   	out    %al,(%dx)
    7ce2:	89 f8                	mov    %edi,%eax
    7ce4:	89 da                	mov    %ebx,%edx
    7ce6:	ee                   	out    %al,(%dx)
  outb(CRTPORT, 15);
  outb(CRTPORT+1, pos);
  crt[pos] = ' ' | 0x0700;
    7ce7:	66 c7 81 02 80 0b 00 	movw   $0x720,0xb8002(%ecx)
    7cee:	20 07 
}
    7cf0:	83 c4 04             	add    $0x4,%esp
    7cf3:	5b                   	pop    %ebx
    7cf4:	5e                   	pop    %esi
    7cf5:	5f                   	pop    %edi
    7cf6:	5d                   	pop    %ebp
    7cf7:	c3                   	ret    

00007cf8 <hello>:

void hello(void)
{
    7cf8:	55                   	push   %ebp
    7cf9:	89 e5                	mov    %esp,%ebp
    vga_putchar('O');
    7cfb:	b8 4f 00 00 00       	mov    $0x4f,%eax
    7d00:	e8 79 ff ff ff       	call   7c7e <vga_putchar>
    vga_putchar('K');
    7d05:	b8 4b 00 00 00       	mov    $0x4b,%eax
    7d0a:	e8 6f ff ff ff       	call   7c7e <vga_putchar>
    vga_putchar('!');
    7d0f:	b8 21 00 00 00       	mov    $0x21,%eax
    7d14:	e8 65 ff ff ff       	call   7c7e <vga_putchar>

    // wait for ever
    for(;;) 
    7d19:	eb fe                	jmp    7d19 <hello+0x21>
