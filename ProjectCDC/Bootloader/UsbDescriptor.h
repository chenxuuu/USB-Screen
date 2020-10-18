#ifndef __USBDESCRIPTOR_H__
#define __USBDESCRIPTOR_H__

// 供应商和产品信息
#define VendorId	0x1A86				// 供应商标识(VID)
#define ProductId	0x5722				// 产品标识(PID)
#define ReleaseNo	0x0100				// 发行版本(*.*.*.*)
#define LSB(x)		(x & 0xFF)
#define MSB(x)		((x & 0xFF00) >> 8)
#define VIDL		LSB(VendorId)  		// Vendor Id Low Byte (LSB)
#define VIDH		MSB(VendorId) 		// Vendor Id High Byte (MSB)
#define PIDL		LSB(ProductId) 		// Product Id Low Byte (LSB)
#define PIDH		MSB(ProductId) 		// Product Id High Byte (MSB)
#define REVL		LSB(ReleaseNo) 		// Release Number Low Byte (LSB)
#define REVH		MSB(ReleaseNo) 		// Release Number High Byte (MSB)

#define EP0SIZE		0x40				// 端点0数据包大小(只有8,16,32,64有效)
#define EP1SIZE		0x08				// 端点1数据包大小
#define EP2SIZE		0x40				// 端点2数据包大小
#define EP3SIZE		0x40				// 端点3数据包大小
#define EP4SIZE		0x40				// 端点4数据包大小

// 设备描述符
extern UINT8C DevReport[] = {
	0x12,		// 描述符长度，以字节为单位，固定为0x12
	0x01,   	// 设备描述符类型，固定为0x01
	0x00,0x02,	// USB协议版本,低位在前; 如USB2.0=0x0200，USB1.1=0x0110等。
	0x02,		// 设备类(0x00表示设备独立分属不同的设备类;0x01-0xFE表示标准HID设备,0x02表示CDC,0x03表示HID;0xFF表示供应商自定义设备)
	0x00,		// 设备子类
	0x00,		// 设备协议（由USB分配）．如果使用USB-IF组织定义的协议，就需要设置这里的值，否则直接设置为0。如果厂商自己定义的可以设置为FFH．
	EP0SIZE,	// 端点0数据包大小（只有8,16,32,64有效）
	VIDL,VIDH,	// Vendor ID; 供应商ID; 简称VID
	PIDL,PIDH,	// Product ID; 产品ID; 简称PID
	REVL,REVH,	// Device Release Version; 设备版本号; 简称Rev
	0x01,		// 厂商描述符字符串索引．若没有可为0
	0x02,		// 产品描述符字符串索引．若没有可为0
	0x03,		// 设备序列号字符串索引．若没有可为0
	0x01		// USB设备支持的配置数．指配置描述符的个数
};
// 配置描述符
extern UINT8C CfgReport[] =
{
// 配置描述符：配置描述符定义了设备的配置信息，一个设备可以有多个配置描述符
	0x09,		// 配置描述符长度．固定为0x09．
	0x02,		// 配置描述符类型．固定为0x02．
	0x43, 0x00,	// 配置描述符总长度(数据低位在前: 0x0043);
	0x02,		// 配置所支持的“接口数目”．也表示该配置下接口描述符数量．
	0x01,		// 这个配置的索引值
	0x00,		// 用于描述该配置字符串描述符的索引．
	0x80,		// 供电模式选择．Bit7:总线供电，Bit6:自供电，Bit5:远程唤醒，Bit4-0保留.
	0xFA,		// 总线供电的USB设备的最大消耗电流．以2mA为单位.(这里是2mA*0xFA=500mA)

	// 接口0描述符：接口描述符说明了接口所提供的配置，由配置描述符“接口数目”决定其数量
	0x09,		// 接口描述符长度．固定为0x09．
	0x04,		// 接口描述符类型．固定为0x04．
	0x00,		// 接口编号. 编号规则从0x00开始
	0x00,		// 备用编号. 用于为上一个字段选择可供替换的位置．
	0x01,		// 此接口使用的“端点数目”．不包括端点0．
	0x02,		// 主类型代码（由USB分配）
	0x02,		// 子类型代码（由USB分配）
	0x01,		// 协议代码（由USB分配）
	0x00,		// 字符串描述符的索引
	// 标头功能描述符 Header Functional Descriptor
	0x05,		// 描述符长度．固定为0x05．
	0x24,		// 主类型: CS_interface
	0x00,		// 子类型: 标题功能描述
	0x10,0x01,	// CDC协议版本: 规范发布号0x0110
	// 管理功能描述符 Call Management Functional Descriptor
	0x05,		// 描述符长度．固定为0x05．
	0x24,		// 主类型: CS_interface
	0x01,		// 子类型: 呼叫管理功能描述
	0x00,		// bm功能：D0+D1
	0x00,		// 数据接口：0 没有数据接口
	// ACM功能描述符
	0x04,		// 描述符长度．固定为0x04．
	0x24,		// 主类型: CS_interface
	0x02,		// 子类型: 抽象控件管理描述符
	0x02,		// 功能: (支持Set_Line_Coding、Set_Control_Line_State、Get_Line_Coding、Serial_State)
	// 联合功能描述符
	0x05,		// 描述符长度．固定为0x05．
	0x24,		// 主类型: CS_interface
	0x06,		// 子类型: 
	0x00,		// 主接口: 编号为0的通讯类接口
	0x01,		// 从接口: 编号为1的数据类接口
	// 端点1描述符
	0x07,		// 描述符长度．固定为0x07．
	0x05,		// 描述符类型．固定为0x05．
	0x81,		// 端点地址(这里为上传端点)．Bit7<1输入:0输出>，Bit6-4保留，Bit3-0端点号．
	0x03,		// 端点属性．Bit7-2保留，Bit1-0<0控制:1同步:2批量:3中断>.
	EP1SIZE,	// 端点最大通信包长度(数据低位在前: 0x0008,单位Byte);
	0x00,
	0x01,		// 端点的报告周期．批量传送和控制传输的端点忽略为0;同步传输的端点必须为1;中断传输的端点为1-255(单位ms)

	// 接口1描述符
	0x09,		// 描述符长度．固定为0x09．
	0x04,		// 描述符类型．固定为0x04．
	0x01,		// 接口编号. 编号规则从0x00开始
	0x00,		// 备用编号. 用于为上一个字段选择可供替换的位置．
	0x02,		// 此接口使用的“端点数目”.
	0x0A,		// 主类型代码(这里表示CDC)
	0x00,		// 子类型代码
	0x00,		// 协议代码
	0x00,		// 字符串描述符的索引
	// 端点描述符
	0x07,		// 描述符长度．固定为0x07．
	0x05,		// 描述符类型．固定为0x05．
	0x82,		// 端点地址(这里为上传端点)．Bit7<1输入:0输出>，Bit6-4保留，Bit3-0端点号．
	0x02,		// 端点属性．Bit7-2保留，Bit1-0<0控制:1同步:2批量:3中断>.
	EP2SIZE,	// 端点最大通信包长度(数据低位在前: 0x0040,单位Byte);
	0x00,
	0x00,		// 端点的报告周期．批量传送和控制传输的端点忽略为0;同步传输的端点必须为1;中断传输的端点为1-255(单位ms)
	// 端点描述符
	0x07,		// 描述符长度．固定为0x07．
	0x05,		// 描述符类型．固定为0x05．
	0x02,		// 端点地址(这里为下传端点)．Bit7<1输入:0输出>，Bit6-4保留，Bit3-0端点号．
	0x02,		// 端点属性．Bit7-2保留，Bit1-0<0控制:1同步:2批量:3中断>.
	EP2SIZE,	// 端点最大通信包长度(数据低位在前: 0x0040,单位Byte);
	0x00,
	0x00 		// 端点的报告周期．批量传送和控制传输的端点忽略为0;同步传输的端点必须为1;中断传输的端点为1-255(单位ms)	
};

/*以下内容为字符串描述符，用于描述设备接口的用途*/
// 语言类型 (0x0409: U.S. English)
UINT8C StrLangID[] = { 0x04, 0x03, 0x09, 0x04 };
// 制造商 (注:因KeilC的UINT16是大端数据,所以要用联合体把UINT16字符转化成小端数据)
code struct { UINT8 bLength; UINT8 bDscType; union{UINT8 b;UINT16 w;} string[7]; } StrVendor = {
sizeof(StrVendor),0x03,
{'A','n','t','e','c','e','r'}};
// 产品型号
code struct { UINT8 bLength; UINT8 bDscType; union{UINT8 b;UINT16 w;} string[12]; } StrProduct = {
sizeof(StrProduct),0x03,
{'U','s','b','S','c','r','e','e','n','I','S','P'}};
// 产品序列号
code struct { UINT8 bLength; UINT8 bDscType; union{UINT8 b;UINT16 w;} string[20]; } StrSerial = {
sizeof(StrSerial),0x03,
{'S','T','1','E','6','D','I','P','S','F','0','F','0','S','P','I','3','I','S','P'}};
// 将所有字符串描述符纳入一个指针数组
PUINT8C StrReports[]={	
	(PUINT8C)&StrLangID,
	(PUINT8C)&StrVendor,
	(PUINT8C)&StrProduct,
	(PUINT8C)&StrSerial
};

// CDC初始配置参数
UINT8C LineCoding[7] = {
	0x00,0x48,0x00,0x00,	// 波特率: 19200 (数据低位在前0x4800)
	0x00,					// 停止位: 1 (可选 {1,1.5,2})
	0x00,					// 校验位: 无 (可选 {无,奇,偶,标志,空格})
	0x08					// 数据位: 8 (可选 {0x04,0x05,0x06,0x07,0x08})
};
#define SET_LINE_CODING		0X20	// 主机写CDC配置命令
#define GET_LINE_CODING		0X21	// 主机读CDC配置命令
#define SET_LINE_STATE		0X22	// 主机写CDC状态


#endif