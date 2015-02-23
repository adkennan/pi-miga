
#include "type.h"
#include "font.xbm"

#define CHAR_WIDTH 8
#define CHAR_HEIGHT 15

typedef struct {
	uint32 Width;
	uint32 Height;
	uint32 VWidth;
	uint32 VHeight;
	uint32 Pitch;
	uint32 Depth;
	uint32 XOffset;
	uint32 YOffset;
	uint8* Buffer;
	uint32 Size;
} FrameBuffer_t __attribute__ ((aligned (16)));

typedef struct {
	uint32 Read;
	uint32 Pad[3];
	uint32 Peek;
	uint32 Sender;
	uint32 Status;
	uint32 Config;
	uint32 Write;
} Mailbox_t;

typedef struct {
	uint32 x;
	uint32 y;
} Cursor_t;

volatile static Mailbox_t* Mailbox = (Mailbox_t*)0x2000b880;

static FrameBuffer_t FrameBuffer;

static Cursor_t Cursor;

void MailboxWrite(uint32 box, uint32 data) {

	while(Mailbox->Status & 0x80000000);

	Mailbox->Write = (data & 0xFFFFFFF0) | (box & 0xF);
}

uint32 MailboxRead(uint32 box) {

	uint32 value;
	box &= 0xF;
	while(TRUE) {
		while(Mailbox->Status & 0x40000000);
		value = Mailbox->Read;
		if( (value & 0xF) == box ) {
			break;
		}
	}
	
	return value & 0xFFFFFFF0;
}

int SetScreenMode(uint32 width, uint32 height) {

	FrameBuffer.Width = width;
	FrameBuffer.Height = height;
	FrameBuffer.Depth = 24;
	FrameBuffer.VWidth = width;
	FrameBuffer.VHeight = height;
	FrameBuffer.XOffset = 0;
	FrameBuffer.YOffset = 0;
			
	MailboxWrite(1, (uint32)&FrameBuffer);

	uint32 reply = MailboxRead(1);
	if( reply == 0 ) {
		return 0;
	}

	Cursor.x = 0;
	Cursor.y = 0;

	return -1;
}

void Fill(uint32 x, uint32 y, uint32 w, uint32 h, uint8 r, uint8 g, uint8 b) {

	for( uint32 yy = y; yy < y + h; yy++ ) {
		for( uint32 xx = x; xx < x + w; xx++ ) {
			uint8* p = FrameBuffer.Buffer + (yy * FrameBuffer.Pitch) + (xx * 3);
			*(p++) = r;
			*(p++) = g;
			*(p++) = b;
		}
	}
}

void GetCharPos(uint32 c, uint32* x, uint32* y) {
	*x = c * CHAR_WIDTH;
	*y = 0;
	while( *x >= BMP_width - CHAR_WIDTH ) {
		*x -= (BMP_width - CHAR_WIDTH);
		*y += CHAR_HEIGHT;
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
				uint8* dp = FrameBuffer.Buffer + ((y + py) * FrameBuffer.Pitch) + ((x + px) * 3);
				*(dp++) = r;
				*(dp++) = g;
				*dp = b;
			}
		}
	}
}

void PutChar(uint32 c) {

	if( c == '\n' ) {
		Cursor.y++;
		Cursor.x = 0;
	}
	else if( c > 32 ) {
		WriteChar(c, Cursor.x * CHAR_WIDTH, Cursor.y * CHAR_HEIGHT, 255, 255, 255);
	}

	Cursor.x++;
	if( Cursor.x >= FrameBuffer.Width / CHAR_WIDTH ) {
		Cursor.x = 0;
		Cursor.y++;
	}

	if( Cursor.y >= FrameBuffer.Height / CHAR_HEIGHT ) {
		Cursor.y = 0;
	}
}

