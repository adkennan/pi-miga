
#include "type.h"
#include "font.xbm"
#include "gpu.h"
#include "debug.h"
#include "uart.h"

#define CHAR_WIDTH 8
#define CHAR_HEIGHT 15

extern void Put32(uint32, uint32);
extern uint32 Get32(uint32);
extern void _DataMemoryBarrier(void);

typedef struct {
	uint32 x;
	uint32 y;
} Cursor_t;

typedef struct {
	uint32 Width;
	uint32 Height;
	uint32 Depth;
	uint32 Pitch;
	uint8* Buffer;
} Screen_t;


uint32 _MailboxRead = 0x2000B880;
uint32 _MailboxStatus = 0x2000B898; 
uint32 _MailboxWrite = 0x2000B8A0;

volatile uint32 mailbuffer[256] __attribute__((aligned (16)));
Cursor_t Cursor;
Screen_t Screen;

void MailboxWrite(uint32 box, uint32 data) {

	_DataMemoryBarrier();

	while(TRUE) {
		if((Get32(_MailboxStatus) & 0x80000000) == 0) {
			break;
		}
	}

	Put32(_MailboxWrite, data + box);
	_DataMemoryBarrier();
}

uint32 MailboxRead(uint32 box) {

	_DataMemoryBarrier();
	uint32 value;
	box &= 0xF;
	while(TRUE) {
		while(TRUE) {
			if((Get32(_MailboxStatus) & 0x40000000) == 0) {
				break;
			}
		}
		value = Get32(_MailboxRead);
		if( (value & 0xF) == box ) {
			break;
		}
	}
	
	return value - box;
}

void SetPowerState(uint32 deviceId, bool state) {

	uint32 mb = (uint32)&mailbuffer | 0x40000000;

	mailbuffer[0] = 8 * 4; // Total size
	mailbuffer[1] = 0; // Request
	mailbuffer[2] = 0x28001; // Power
	mailbuffer[3] = 8; // Buffer size
	mailbuffer[4] = 8; // Buffer size
	mailbuffer[5] = deviceId;
	mailbuffer[6] = state == TRUE ? 3 : 0,
	mailbuffer[7] = 0; // End tag

	MailboxWrite(8, mb);
	uint32 result = MailboxRead(8);
	DebugPrintf("SetPowerState = %x\n", mailbuffer[1]);
}

int SetScreenMode(uint32 width, uint32 height) {

	uint32 mb = (uint32)&mailbuffer | 0x40000000;

	mailbuffer[0] = 8 * 4; // Total size
	mailbuffer[1] = 0; // Request
	mailbuffer[2] = 0x40003; // Display size
	mailbuffer[3] = 8; // Buffer size
	mailbuffer[4] = 0; // Request size
	mailbuffer[5] = 0; // Space for horizontal resolution
	mailbuffer[6] = 0; // Space for vertical resolution
	mailbuffer[7] = 0; // End tag

	MailboxWrite(8, mb);
	uint32 result = MailboxRead(8);
	
	if( mailbuffer[1] != 0x80000000 ) {
		return 0;
	}

	Screen.Width = width; //mailbuffer[5];
	Screen.Height = height; //mailbuffer[6];
	Screen.Depth = 24;

	uint32 c = 1;
	mailbuffer[c++] = 0; // Request
	mailbuffer[c++] = 0x00048003; // Tag id (set physical size)
	mailbuffer[c++] = 8; // Value buffer size (bytes)
	mailbuffer[c++] = 8; // Req. + value length (bytes)
	mailbuffer[c++] = Screen.Width; // Horizontal resolution
	mailbuffer[c++] = Screen.Height; // Vertical resolution
	mailbuffer[c++] = 0x00048004; // Tag id (set virtual size)
	mailbuffer[c++] = 8; // Value buffer size (bytes)
	mailbuffer[c++] = 8; // Req. + value length (bytes)
	mailbuffer[c++] = Screen.Width; // Horizontal resolution
	mailbuffer[c++] = Screen.Height; // Vertical resolution
	mailbuffer[c++] = 0x00048005; // Tag id (set depth)
	mailbuffer[c++] = 4; // Value buffer size (bytes)
	mailbuffer[c++] = 4; // Req. + value length (bytes)
	mailbuffer[c++] = Screen.Depth; // 16 bpp
	mailbuffer[c++] = 0x00040001; // Tag id (allocate framebuffer)
	mailbuffer[c++] = 8; // Value buffer size (bytes)
	mailbuffer[c++] = 4; // Req. + value length (bytes)
	mailbuffer[c++] = 16; // Alignment = 16
	mailbuffer[c++] = 0; // Space for response
	mailbuffer[c++] = 0; // Terminating tag
	mailbuffer[0] = c*4; // Buffer size

	MailboxWrite(8, mb);

	result = MailboxRead(8);

	if( mailbuffer[1] != 0x80000000 ) {
		return 0;
	}

	uint32 n = 2;
	while( (result = mailbuffer[n])) {
		if( result == 0x40001 ) {
			break;
		}
		n += 3+(mailbuffer[n+1]>>2);
		if(n>c) {
			return 0;
		}
	}
	if(mailbuffer[n+2] != 0x80000008) {
		return 0;
	}

	Screen.Buffer = (uint8*)mailbuffer[n + 3];

	if( Screen.Buffer == 0 ) {
		return 0;
	}

	/* Get the framebuffer pitch (bytes per line) */
	mailbuffer[0] = 7 * 4; // Total size
	mailbuffer[1] = 0; // Request
	mailbuffer[2] = 0x40008; // Display size
	mailbuffer[3] = 4; // Buffer size
	mailbuffer[4] = 0; // Request size
	mailbuffer[5] = 0; // Space for pitch
	mailbuffer[6] = 0; // End tag

	MailboxWrite(8, mb);
	result = MailboxRead(8);
	
	/* 4 bytes, plus MSB set to indicate a response */
	if(mailbuffer[4] != 0x80000004) {
		return 0;
	}

	Screen.Pitch = mailbuffer[5];
	if(Screen.Pitch == 0) {
		return 0;
	}

	Cursor.x = 0;
	Cursor.y = 0;

	Fill(0, 0, Screen.Width, Screen.Height, 0x1f, 0x50, 0xa7);

	DebugPrintf("\nInitialized Framebuffer:\n  Width: %x\n  Height: %x\n  Depth: %x\n  Buffer: %x\n\n",
				Screen.Width, Screen.Height, Screen.Depth, Screen.Buffer);

	return -1;
}

void DrawPixel(uint32 x, uint32 y, uint8 r, uint8 g, uint8 b) {
	
	switch( Screen.Depth ) {
		case 16: {
			uint16 *p = (uint16*)(Screen.Buffer + (y * Screen.Pitch + (x * 2)));
			*p = (uint16)(((r >> 3) << 16) | ((g >> 3) << 11) | (b >> 3));
			break;
		}

		case 24: {
			uint8* p = (Screen.Buffer + (y * Screen.Pitch + (x * 3 )));
			*(p++) = r;
			*(p++) = g;
			*(p++) = b;
			break;
		}

		case 32: {
			uint32* p = (uint32*)(Screen.Buffer + (y * Screen.Pitch) + (x * 4));
			*p = r + (g << 8) + (b << 16);
			break;
		}
	}
}

void Fill(uint32 x, uint32 y, uint32 w, uint32 h, uint8 r, uint8 g, uint8 b) {

	for( uint32 yy = y; yy < y + h - 1; yy++ ) {
		for( uint32 xx = x; xx < x + w - 1; xx++ ) {
			DrawPixel(yy, xx, r, g, b);
		}
	}
}

void GetCharPos(uint32 c, uint32* x, uint32* y) {
	*x = c * CHAR_WIDTH;
	*y = 0;
	while( *x >= BMP_width - CHAR_WIDTH ) {
		*x -= (BMP_width - CHAR_WIDTH);
		*y += CHAR_HEIGHT + 1;
	}	
}

void WriteChar(uint32 c, uint32 x, uint32 y, uint8 r, uint8 g, uint8 b) {

	uint32 cx, cy;
	GetCharPos(c, &cx, &cy);
	uint32 pitch = BMP_width >> 3;

	for( uint32 py = 0; py < CHAR_HEIGHT; py++ ) {
		uint8 sp = BMP_bits[((py + cy) * pitch) + (cx >> 3)];
		for( uint32 px = 0; px < CHAR_WIDTH; px++ ) {
			if( sp & (1 << px)) {
				DrawPixel(x + px, y + py, r, g, b);
			}
		}
	}
}

void PutChar(uint32 c) {

	uart_putc(c);

	if( Screen.Buffer == NULL ) {
		return;
	}
	
	if( c == '\n' ) {
		Cursor.y++;
		Cursor.x = 0;
	}
	else {
		if( c > 32 ) {
			WriteChar(c, Cursor.x * CHAR_WIDTH, Cursor.y * CHAR_HEIGHT, 255, 255, 255);
		}

		Cursor.x++;
		if( Cursor.x >= Screen.Width / CHAR_WIDTH ) {
			Cursor.x = 0;
			Cursor.y++;
		}
	}

	if( Cursor.y >= Screen.Height / CHAR_HEIGHT ) {
		Cursor.y = 0;
	}
}

