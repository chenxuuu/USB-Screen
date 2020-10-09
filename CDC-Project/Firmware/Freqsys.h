#ifndef __DELAY_H__
#define __DELAY_H__

#ifndef UINT8
	#define UINT8 unsigned char
#endif
#ifndef UINT16
	#define UINT16 unsigned short
#endif

extern void setFsys(void);
extern void mDelayuS(UINT16 n);
extern void mDelaymS(UINT16 n);

#endif