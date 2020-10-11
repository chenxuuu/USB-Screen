#ifndef	UINT8C
#define	UINT8C	const unsigned char code
#endif

/*HID设备报告描述符*/
#ifndef	UsbReports_H
#define	UsbReports_H

// 供应商和产品信息
#define VendorId	0x2333  // 供应商标识(VID)
#define ProductId	0xC551  // 产品标识(PID)
#define ReleaseNo	0x1005  // 发行版本
#define LSB(x) (x & 0xFF)
#define MSB(x) ((x & 0xFF00) >> 8)
#define VIDL LSB(VendorId)  // Vendor Id Low Byte (LSB)
#define VIDH MSB(VendorId)  // Vendor Id High Byte (MSB)
#define PIDL LSB(ProductId) // Product Id Low Byte (LSB)
#define PIDH MSB(ProductId) // Product Id High Byte (MSB)
#define REVL LSB(ReleaseNo) // Release Number Low Byte (LSB)
#define REVH MSB(ReleaseNo) // Release Number High Byte (MSB)

// HID类报表描述符
UINT8C	HidReport[] = {
    0x06, 0x00, 0xFF,		// USAGE_PAGE (Vendor Defined Page 1)
    0x09, 0x01,				// USAGE (Vendor Usage 1)
    0xA1, 0x01,				// COLLECTION (Application)
	0x09, 0x02,     		//   Usage ID - vendor defined
	0x15, 0x00,     		//   Logical Minimum (0)
	0x25, 0xFF,				//   Logical Maximum (255)
	0x75, 0x08,     		//   Report Size (8 bits)
	0x95, 0x40,     		//   Report Count(64)
	0x81, 0x06,     		//   Input (Data, Variable, Absolute)
	0x09, 0x02,     		//   Usage ID - vendor defined
	0x15, 0x00,     		//   Logical Minimum (0)
	0x25, 0xFF,				//   Logical Maximum (255)
	0x75, 0x08,     		//   Report Size (8 bits)
	0x95, 0x40,     		//   Report Count(64)
	0x91, 0x06,      		//   Output (Data, Variable, Absolute)
    0xc0                	// END_COLLECTION
};

// 配置描述符
UINT8C	CfgReport[] = 
{
// 配置描述符：配置描述符定义了设备的配置信息，一个设备可以有多个配置描述符
	0x09,	// 配置描述符长度．固定为0x09．
	0x02,	// 配置描述符类型．固定为0x02．
	0x29,	// 配置描述符总长度(低8位);指此配置返回的配置描述符，接口描述符以及端点描述符的全部大小．
	0x00,	// 配置描述符总长度(高8位)
	0x01,	// 配置所支持的“接口数目”．也表示该配置下接口描述符数量．
	0x01,	// 这个配置的索引值
	0x04,	// 用于描述该配置字符串描述符的索引．
	0xA0,	// 供电模式选择．Bit7:总线供电，Bit6:自供电，Bit5:远程唤醒，Bit4-0保留.
	0xC8,	// 总线供电的USB设备的最大消耗电流．以2mA为单位.(这里是2mA*0xC8=400mA)
	
// 接口描述符：接口描述符说明了接口所提供的配置，由配置描述符“接口数目”决定其数量
	0x09,	// 接口描述符长度．固定为0x09．
	0x04,	// 接口描述符类型．固定为0x04．
	0x00,	// 接口编号. 编号规则从0x00开始
	0x00,	// 备用编号. 用于为上一个字段选择可供替换的位置．
	0x02,	// 此接口使用的“端点数目”．不包括端点0．
	0x03,	// 类型代码（由USB分配）
	0x00,	// 子类型代码（由USB分配）
	0x00,	// 协议代码（由USB分配）
	0x03,	// 字符串描述符的索引
// HID类描述符
	0x09,   // HID描述符长度．固定为0x09．
    0x21,   // HID描述符类型．固定为0x21．
    0x10,   // USB规格发布号LSB低位; 表示了本设备能适用于哪种协议，
    0x01,   // USB规格发布号MSB高位; 如USB2.0=0x0200，USB1.1=0x0110等。
    0x00,   // 国家代码（0x00不支持）
    0x01,   // 下级描述符数量，通常至少需要一个报告描述符。
    0x22,   // 下级描述符类型，例如报告描述符。
    sizeof(HidReport),	// Report Size LSB 下级描述符长度  (HIDReport[]长度)
    0x00,   // Report Size LSB
// 端点描述符：USB设备中的每个端点都有自己的端点描述符，由接口描述符“端点数目”决定其数量
	0x07,	// 端点描述符大小．固定为0x07．
	0x05,	// 端点描述符类型．固定为0x05．
	0x84,	// 端点地址(这里为输入端点)．Bit7<1输入:0输出>，Bit6-4保留，Bit3-0端点号．
	0x03,	// 端点属性．Bit7-2保留，Bit1-0<0控制:1同步:2批量:3中断>.
	0x40,	// Max Packet Size LSB 端点最大通信包长度(单位byte)
	0x00,	// Max Packet Size MSB
	0x01,	// 端点的报告周期．批量传送和控制传输的端点忽略为0;同步传输的端点必须为1;中断传输的端点为1-255(单位ms)．
// 端点描述符：USB设备中的每个端点都有自己的端点描述符，由接口描述符“端点数目”决定其数量
	0x07,	// 端点描述符大小．固定为0x07．
	0x05,	// 端点描述符类型．固定为0x05．
	0x04,	// 端点地址(这里为输出端点)．Bit7<1输入:0输出>，Bit6-4保留，Bit3-0端点号．
	0x03,	// 端点属性．Bit7-2保留，Bit1-0<0控制:1同步:2批量:3中断>.
	0x40,	// Max Packet Size LSB 端点最大通信包长度(单位byte)
	0x00,	// Max Packet Size MSB
	0x01	// 端点的报告周期．批量传送和控制传输的端点忽略为0;同步传输的端点必须为1;中断传输的端点为1-255(单位ms)．
};

// 设备描述符
UINT8C	DevReport[] = 
{
	0x12,	// 描述符长度，以字节为单位，固定为0x12
	0x01,	// 设备描述符类型，固定为0x01
	0x10,	// USB规格发布号LSB低位; 表示了本设备能适用于哪种协议，
	0x01,	// USB规格发布号MSB高位; 如USB2.0=0x0200，USB1.1=0x0110等。
	0x00,	// 类型代码（由USB指定）。当它的值是0时，表示所有接口在配置描述符里，并且所有接口是独立的。当它的值是1到FEH时，表示不同的接口关联的。当它的值是FFH时，它是厂商自己定义的．
	0x00,	// 子类型代码（由USB分配）．如果类型代码值是0，一定要设置为0．其它情况就跟据USB-IF组织定义的编码．
	0x00,	// 协议代码（由USB分配）．如果使用USB-IF组织定义的协议，就需要设置这里的值，否则直接设置为0。如果厂商自己定义的可以设置为FFH．
	0x08,	// 端点0数据包大小（只有8,16,32,64有效）
	VIDL,	// Vendor ID LSB; 供应商ID
	VIDH,	// Vendor ID MSB; 俗称VID
	PIDL,	// Product ID LSB; 产品ID
	PIDH,	// Product ID MSB; 俗称PID
	REVL,	// Device release number LSB; 设备版本号
	REVH,	// Device release number MSB
	0x01,	// 厂商描述符字符串索引．索引到对应的字符串描述符． 为0则表示没有.
	0x02,	// 产品描述符字符串索引．同上．
	0x00,	// 设备序列号字符串索引．同上．
	0x01	// 可能的配置数．指配置字符串的个数
};

// 语言类型
const struct { UINT8 bLength; UINT8 bDscType; UINT16 string[1]; } code HidStrLangID = { sizeof(HidStrLangID),0x03,
{0x0409} };	// LangID = 0x0409: U.S. English
// 厂家信息
const struct { UINT8 bLength; UINT8 bDscType; UINT16 string[7]; } code HidStrVendor = { sizeof(HidStrVendor),0x03,
{'A','n','t','e','c','e','r'} };
// 产品型号
const struct { UINT8 bLength; UINT8 bDscType; UINT16 string[9]; } code HidStrSerial = { sizeof(HidStrSerial),0x03,
{'U', 's', 'b', 'S', 'c', 'r', 'e', 'e', 'n'}};
// 产品名称
const struct { UINT8 bLength; UINT8 bDscType; UINT16 string[8]; } code HidStrProduct = { sizeof(HidStrProduct),0x03,
{'C', 'H', '5', '5', '1', 'I', 'A', 'P'}};
// 将所有字符串描述符纳入一个指针数组
PUINT8C StringReports[]=
{	
	(PUINT8C)(&HidStrLangID),
	(PUINT8C)(&HidStrVendor),
	(PUINT8C)(&HidStrSerial),
	(PUINT8C)(&HidStrProduct)
};
#endif	/* UsbReports_H */