#ifndef TypeDefines_H
#define TypeDefines_H

#ifndef TRUE
#define TRUE    1
#define FALSE	0
#endif
#ifndef True
#define True	1
#define False	0
#endif
#ifndef true
#define true	1
#define false	0
#endif

#ifndef NULL
#define NULL    0
#endif
#ifndef Null
#define Null	0
#endif
#ifndef null
#define null	0
#endif

#ifndef Bool
typedef bit		Bool;
#endif
#ifndef bool
typedef bit		bool;
#endif

#ifndef UINT8
#define UINT8	unsigned char
#endif
#ifndef UINT8X
#define UINT8X	unsigned char  xdata
#endif

#ifndef	U8I
#define	U8I	unsigned char idata
#endif
#ifndef	U16I
#define	U16I unsigned short idata
#endif


#endif /* TypeDefines_H */