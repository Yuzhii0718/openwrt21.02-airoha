#ifndef _SGMII_GLOBALDEF_H_
#define _SGMII_GLOBALDEF_H_


//To define variable type
#define UNSIGNEDINT64TYPE unsigned long long
#define UNSIGNEDINT32TYPE unsigned int
#define UNSIGNEDINT16TYPE unsigned short
#define UNSIGNEDINT8TYPE  unsigned char
#define INT32TYPE  signed int
#define INT16TYPE  signed short
#define INT8TYPE   signed char

typedef UNSIGNEDINT64TYPE      uint64;
typedef UNSIGNEDINT32TYPE      uint32;
typedef UNSIGNEDINT16TYPE      uint16;
typedef UNSIGNEDINT8TYPE       uint8;
typedef INT32TYPE              int32;
typedef INT16TYPE              int16;
typedef INT8TYPE               int8;

//To define every bit of 32bits
#define Bit0  0x00000001
#define Bit1  0x00000002
#define Bit2  0x00000004
#define Bit3  0x00000008
#define Bit4  0x00000010
#define Bit5  0x00000020
#define Bit6  0x00000040
#define Bit7  0x00000080
#define Bit8  0x00000100
#define Bit9  0x00000200
#define Bit10 0x00000400
#define Bit11 0x00000800
#define Bit12 0x00001000
#define Bit13 0x00002000
#define Bit14 0x00004000
#define Bit15 0x00008000
#define Bit16 0x00010000
#define Bit17 0x00020000
#define Bit18 0x00040000
#define Bit19 0x00080000
#define Bit20 0x00100000
#define Bit21 0x00200000
#define Bit22 0x00400000
#define Bit23 0x00800000
#define Bit24 0x01000000
#define Bit25 0x02000000
#define Bit26 0x04000000
#define Bit27 0x08000000
#define Bit28 0x10000000
#define Bit29 0x20000000
#define Bit30 0x40000000
#define Bit31 0x80000000

//To define custom using
#define PASS 1
#define FAIL 0 


struct BIT {
	u32 b0:1;
	u32 b1:1;
	u32 b2:1;
	u32 b3:1;
	u32 b4:1;
	u32 b5:1;
	u32 b6:1;
	u32 b7:1;

	u32 b8:1;
	u32 b9:1;
	u32 b10:1;
	u32 b11:1;
	u32 b12:1;
	u32 b13:1;
	u32 b14:1;
	u32 b15:1;

	u32 b16:1;
	u32 b17:1;
	u32 b18:1;
	u32 b19:1;
	u32 b20:1;
	u32 b21:1;
	u32 b22:1;
	u32 b23:1;

	u32 b24:1;
	u32 b25:1;
	u32 b26:1;
	u32 b27:1;
	u32 b28:1;
	u32 b29:1;
	u32 b30:1;
	u32 b31:1;
};

typedef union {
	u32 value;
	struct BIT bit;
}RGDATA_t;	

typedef struct {
	void __iomem* base;
	void __iomem* base_start;
	void __iomem* addr;
	RGDATA_t data;
}REG_t;

typedef void __iomem* RgAddr; 
#endif //_SGMII_GLOBALDEF_H_
