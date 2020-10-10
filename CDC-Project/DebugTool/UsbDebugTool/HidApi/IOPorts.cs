using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Management;
using System.Text.RegularExpressions;

namespace HidApi
{
	class IOPorts
	{
		/// <summary>
		/// 即插即用设备信息结构
		/// </summary>
		public struct PnPEntityInfo
		{
			public String PNPDeviceID;      // 设备ID
			public String Name;             // 设备名称
			public String Description;      // 设备描述
			public String Service;          // 服务
			public String Status;           // 设备状态
			public UInt16 VendorID;         // 供应商标识
			public UInt16 ProductID;        // 产品编号 
			public Guid ClassGuid;          // 设备安装类GUID
		}

		/// <summary>
		/// 枚举win32 api
		/// </summary>
		public enum HardwareEnum
		{
			// 硬件
			Win32_Processor, // CPU 处理器
			Win32_PhysicalMemory, // 物理内存条
			Win32_Keyboard, // 键盘
			Win32_PointingDevice, // 点输入设备，包括鼠标。
			Win32_FloppyDrive, // 软盘驱动器
			Win32_DiskDrive, // 硬盘驱动器
			Win32_CDROMDrive, // 光盘驱动器
			Win32_BaseBoard, // 主板
			Win32_BIOS, // BIOS 芯片
			Win32_ParallelPort, // 并口
			Win32_SerialPort, // 串口
			Win32_SerialPortConfiguration, // 串口配置
			Win32_SoundDevice, // 多媒体设置，一般指声卡。
			Win32_SystemSlot, // 主板插槽 (ISA & PCI & AGP)
			Win32_USBController, // USB 控制器
			Win32_NetworkAdapter, // 网络适配器
			Win32_NetworkAdapterConfiguration, // 网络适配器设置
			Win32_Printer, // 打印机
			Win32_PrinterConfiguration, // 打印机设置
			Win32_PrintJob, // 打印机任务
			Win32_TCPIPPrinterPort, // 打印机端口
			Win32_POTSModem, // MODEM
			Win32_POTSModemToSerialPort, // MODEM 端口
			Win32_DesktopMonitor, // 显示器
			Win32_DisplayConfiguration, // 显卡
			Win32_DisplayControllerConfiguration, // 显卡设置
			Win32_VideoController, // 显卡细节。
			Win32_VideoSettings, // 显卡支持的显示模式。

			// 操作系统
			Win32_TimeZone, // 时区
			Win32_SystemDriver, // 驱动程序
			Win32_DiskPartition, // 磁盘分区
			Win32_LogicalDisk, // 逻辑磁盘
			Win32_LogicalDiskToPartition, // 逻辑磁盘所在分区及始末位置。
			Win32_LogicalMemoryConfiguration, // 逻辑内存配置
			Win32_PageFile, // 系统页文件信息
			Win32_PageFileSetting, // 页文件设置
			Win32_BootConfiguration, // 系统启动配置
			Win32_ComputerSystem, // 计算机信息简要
			Win32_OperatingSystem, // 操作系统信息
			Win32_StartupCommand, // 系统自动启动程序
			Win32_Service, // 系统安装的服务
			Win32_Group, // 系统管理组
			Win32_GroupUser, // 系统组帐号
			Win32_UserAccount, // 用户帐号
			Win32_Process, // 系统进程
			Win32_Thread, // 系统线程
			Win32_Share, // 共享
			Win32_NetworkClient, // 已安装的网络客户端
			Win32_NetworkProtocol, // 已安装的网络协议
			Win32_PnPEntity,//all device
		}
		public partial class HardWare
		{
			/// <summary>
			/// WMI取硬件信息
			/// </summary>
			/// <param name="hardType"></param>
			/// <param name="propKey"></param>
			/// <returns></returns>
			public static PnPEntityInfo[] MulGetHardwareInfo(HardwareEnum hardType)
			{

				List<PnPEntityInfo> Devices = new List<PnPEntityInfo>();
				//try
				{
					using (ManagementObjectSearcher searcher = new ManagementObjectSearcher("select * from " + hardType))
					{
						var hardInfos = searcher.Get();
						foreach (ManagementObject Entity in hardInfos)
						{
							string name = Entity.GetPropertyValue("PNPDeviceID").ToString();
							foreach (var ttt in Entity.Properties)
							{
								Console.WriteLine($"<{name}> {ttt.Name} = {ttt.Value}");
							}


							PnPEntityInfo Element = new PnPEntityInfo();

							//string Dependent = Entity["Dependent"].ToString();
							//// 过滤掉没有VID和PID的设备
							//Match match = Regex.Match(Dependent, "VID_[0-9|A-F]{4}&PID_[0-9|A-F]{4}");
							//if (match.Success)
							//{
							//	UInt16 theVendorID = Convert.ToUInt16(match.Value.Substring(4, 4), 16);   // 供应商标识
							//	UInt16 theProductID = Convert.ToUInt16(match.Value.Substring(13, 4), 16); // 产品编号
							//	Guid theClassGuid = new Guid(Entity["ClassGuid"] as String);    // 设备安装类GUID

							//	Element.PNPDeviceID = Entity["PNPDeviceID"] as String;  // 设备ID
							//	Element.VendorID = theVendorID;     // 供应商标识
							//	Element.ProductID = theProductID;   // 产品编号
							//	Element.ClassGuid = theClassGuid;   // 设备安装类GUID
							//}
							Element.Name = Entity.GetPropertyValue("DeviceID").ToString();                // 设备名称
							Element.Description = Entity.GetPropertyValue("PNPDeviceID").ToString();
							//Element.PNPDeviceID = Entity. GetPropertyValue("DEVPKEY_Device_BusReportedDeviceDesc").ToString();
							//Element.Service = Entity["Service"] as String;          // 服务
							//Element.Status = Entity["Status"] as String;            // 设备状态
							Devices.Add(Element);

						}
						searcher.Dispose();
					}
					return Devices.ToArray();
				}
				//catch(Exception e)
				//{
				//	Console.WriteLine(e.Message);
				//	Devices = null;
				//	return null;
				//}
			}
		}

		/// <summary>
		/// 基于WMI获取USB设备信息
		/// </summary>
		public partial class USB
		{
			#region UsbDevice
			/// <summary>
			/// 获取所有的USB设备实体（过滤没有VID和PID的设备）
			/// </summary>
			public static PnPEntityInfo[] AllUsbDevices
			{
				get
				{
					return WhoUsbDevice(UInt16.MinValue, UInt16.MinValue, Guid.Empty);
				}
			}

			/// <summary>
			/// 查询USB设备实体（设备要求有VID和PID）
			/// </summary>
			/// <param name="VendorID">供应商标识，MinValue忽视</param>
			/// <param name="ProductID">产品编号，MinValue忽视</param>
			/// <param name="ClassGuid">设备安装类Guid，Empty忽视</param>
			/// <returns>设备列表</returns>
			public static PnPEntityInfo[] WhoUsbDevice(UInt16 VendorID, UInt16 ProductID, Guid ClassGuid)
			{
				List<PnPEntityInfo> UsbDevices = new List<PnPEntityInfo>();

				// 获取USB控制器及其相关联的设备实体
				ManagementObjectCollection USBControllerDeviceCollection = new ManagementObjectSearcher("SELECT * FROM Win32_USBControllerDevice").Get();
				if (USBControllerDeviceCollection != null)
				{
					foreach (ManagementObject USBControllerDevice in USBControllerDeviceCollection)
					{   // 获取设备实体的DeviceID
						String Dependent = (USBControllerDevice["Dependent"] as String).Split(new Char[] { '=' })[1];

						// 过滤掉没有VID和PID的USB设备
						Match match = Regex.Match(Dependent, "VID_[0-9|A-F]{4}&PID_[0-9|A-F]{4}");
						if (match.Success)
						{
							UInt16 theVendorID = Convert.ToUInt16(match.Value.Substring(4, 4), 16);   // 供应商标识
							if (VendorID != UInt16.MinValue && VendorID != theVendorID) continue;

							UInt16 theProductID = Convert.ToUInt16(match.Value.Substring(13, 4), 16); // 产品编号
							if (ProductID != UInt16.MinValue && ProductID != theProductID) continue;

							ManagementObjectCollection PnPEntityCollection = new ManagementObjectSearcher("SELECT * FROM Win32_PnPEntity WHERE DeviceID=" + Dependent).Get();
							if (PnPEntityCollection != null)
							{
								foreach (ManagementObject Entity in PnPEntityCollection)
								{
									Guid theClassGuid = new Guid(Entity["ClassGuid"] as String);    // 设备安装类GUID
									if (ClassGuid != Guid.Empty && ClassGuid != theClassGuid) continue;

									PnPEntityInfo Element;
									Element.PNPDeviceID = Entity["PNPDeviceID"] as String;  // 设备ID
									Element.Name = Entity["Name"] as String;                // 设备名称
									Element.Description = Entity["Description"] as String;  // 设备描述
									Element.Service = Entity["Service"] as String;          // 服务
									Element.Status = Entity["Status"] as String;            // 设备状态
									Element.VendorID = theVendorID;     // 供应商标识
									Element.ProductID = theProductID;   // 产品编号
									Element.ClassGuid = theClassGuid;   // 设备安装类GUID

									UsbDevices.Add(Element);
								}
							}
						}
					}
				}

				if (UsbDevices.Count == 0) return null; else return UsbDevices.ToArray();
			}

			/// <summary>
			/// 查询USB设备实体（设备要求有VID和PID）
			/// </summary>
			/// <param name="VendorID">供应商标识，MinValue忽视</param>
			/// <param name="ProductID">产品编号，MinValue忽视</param>
			/// <returns>设备列表</returns>
			public static PnPEntityInfo[] WhoUsbDevice(UInt16 VendorID, UInt16 ProductID)
			{
				return WhoUsbDevice(VendorID, ProductID, Guid.Empty);
			}

			/// <summary>
			/// 查询USB设备实体（设备要求有VID和PID）
			/// </summary>
			/// <param name="ClassGuid">设备安装类Guid，Empty忽视</param>
			/// <returns>设备列表</returns>
			public static PnPEntityInfo[] WhoUsbDevice(Guid ClassGuid)
			{
				return WhoUsbDevice(UInt16.MinValue, UInt16.MinValue, ClassGuid);
			}

			/// <summary>
			/// 查询USB设备实体（设备要求有VID和PID）
			/// </summary>
			/// <param name="PNPDeviceID">设备ID，可以是不完整信息</param>
			/// <returns>设备列表</returns>        
			public static PnPEntityInfo[] WhoUsbDevice(String PNPDeviceID)
			{
				List<PnPEntityInfo> UsbDevices = new List<PnPEntityInfo>();

				// 获取USB控制器及其相关联的设备实体
				ManagementObjectCollection USBControllerDeviceCollection = new ManagementObjectSearcher("SELECT * FROM Win32_USBControllerDevice").Get();
				if (USBControllerDeviceCollection != null)
				{
					foreach (ManagementObject USBControllerDevice in USBControllerDeviceCollection)
					{   // 获取设备实体的DeviceID
						String Dependent = (USBControllerDevice["Dependent"] as String).Split(new Char[] { '=' })[1];
						if (!String.IsNullOrEmpty(PNPDeviceID))
						{   // 注意：忽视大小写
							if (Dependent.IndexOf(PNPDeviceID, 1, PNPDeviceID.Length - 2, StringComparison.OrdinalIgnoreCase) == -1) continue;
						}

						// 过滤掉没有VID和PID的USB设备
						Match match = Regex.Match(Dependent, "VID_[0-9|A-F]{4}&PID_[0-9|A-F]{4}");
						if (match.Success)
						{
							ManagementObjectCollection PnPEntityCollection = new ManagementObjectSearcher("SELECT * FROM Win32_PnPEntity WHERE DeviceID=" + Dependent).Get();
							if (PnPEntityCollection != null)
							{
								foreach (ManagementObject Entity in PnPEntityCollection)
								{
									PnPEntityInfo Element;
									Element.PNPDeviceID = Entity["PNPDeviceID"] as String;  // 设备ID
									Element.Name = Entity["Name"] as String;                // 设备名称
									Element.Description = Entity["Description"] as String;  // 设备描述
									Element.Service = Entity["Service"] as String;          // 服务
									Element.Status = Entity["Status"] as String;            // 设备状态
									Element.VendorID = Convert.ToUInt16(match.Value.Substring(4, 4), 16);   // 供应商标识   
									Element.ProductID = Convert.ToUInt16(match.Value.Substring(13, 4), 16); // 产品编号                         // 产品编号
									Element.ClassGuid = new Guid(Entity["ClassGuid"] as String);            // 设备安装类GUID

									UsbDevices.Add(Element);
								}
							}
						}
					}
				}

				if (UsbDevices.Count == 0) return null; else return UsbDevices.ToArray();
			}

			/// <summary>
			/// 根据服务定位USB设备
			/// </summary>
			/// <param name="ServiceCollection">要查询的服务集合</param>
			/// <returns>设备列表</returns>
			public static PnPEntityInfo[] WhoUsbDevice(String[] ServiceCollection)
			{
				if (ServiceCollection == null || ServiceCollection.Length == 0)
					return WhoUsbDevice(UInt16.MinValue, UInt16.MinValue, Guid.Empty);

				List<PnPEntityInfo> UsbDevices = new List<PnPEntityInfo>();

				// 获取USB控制器及其相关联的设备实体
				ManagementObjectCollection USBControllerDeviceCollection = new ManagementObjectSearcher("SELECT * FROM Win32_USBControllerDevice").Get();
				if (USBControllerDeviceCollection != null)
				{
					foreach (ManagementObject USBControllerDevice in USBControllerDeviceCollection)
					{   // 获取设备实体的DeviceID
						String Dependent = (USBControllerDevice["Dependent"] as String).Split(new Char[] { '=' })[1];

						// 过滤掉没有VID和PID的USB设备
						Match match = Regex.Match(Dependent, "VID_[0-9|A-F]{4}&PID_[0-9|A-F]{4}");
						if (match.Success)
						{
							ManagementObjectCollection PnPEntityCollection = new ManagementObjectSearcher("SELECT * FROM Win32_PnPEntity WHERE DeviceID=" + Dependent).Get();
							if (PnPEntityCollection != null)
							{
								foreach (ManagementObject Entity in PnPEntityCollection)
								{
									String theService = Entity["Service"] as String;          // 服务
									if (String.IsNullOrEmpty(theService)) continue;

									foreach (String Service in ServiceCollection)
									{   // 注意：忽视大小写
										if (String.Compare(theService, Service, true) != 0) continue;

										PnPEntityInfo Element;
										Element.PNPDeviceID = Entity["PNPDeviceID"] as String;  // 设备ID
										Element.Name = Entity["Name"] as String;                // 设备名称
										Element.Description = Entity["Description"] as String;  // 设备描述
										Element.Service = theService;                           // 服务
										Element.Status = Entity["Status"] as String;            // 设备状态
										Element.VendorID = Convert.ToUInt16(match.Value.Substring(4, 4), 16);   // 供应商标识   
										Element.ProductID = Convert.ToUInt16(match.Value.Substring(13, 4), 16); // 产品编号
										Element.ClassGuid = new Guid(Entity["ClassGuid"] as String);            // 设备安装类GUID

										UsbDevices.Add(Element);
										break;
									}
								}
							}
						}
					}
				}

				if (UsbDevices.Count == 0) return null; else return UsbDevices.ToArray();
			}
			#endregion

			#region PnPEntity
			/// <summary>
			/// 所有即插即用设备实体（过滤没有VID和PID的设备）
			/// </summary>
			public static PnPEntityInfo[] AllPnPEntities
			{
				get
				{
					return WhoPnPEntity(UInt16.MinValue, UInt16.MinValue, Guid.Empty);
				}
			}

			/// <summary>
			/// 根据VID和PID及设备安装类GUID定位即插即用设备实体
			/// </summary>
			/// <param name="VendorID">供应商标识，MinValue忽视</param>
			/// <param name="ProductID">产品编号，MinValue忽视</param>
			/// <param name="ClassGuid">设备安装类Guid，Empty忽视</param>
			/// <returns>设备列表</returns>
			/// <remarks>
			/// HID：{745a17a0-74d3-11d0-b6fe-00a0c90f57da}
			/// Imaging Device：{6bdd1fc6-810f-11d0-bec7-08002be2092f}
			/// Keyboard：{4d36e96b-e325-11ce-bfc1-08002be10318} 
			/// Mouse：{4d36e96f-e325-11ce-bfc1-08002be10318}
			/// Network Adapter：{4d36e972-e325-11ce-bfc1-08002be10318}
			/// USB：{36fc9e60-c465-11cf-8056-444553540000}
			/// </remarks>
			public static PnPEntityInfo[] WhoPnPEntity(UInt16 VendorID, UInt16 ProductID, Guid ClassGuid)
			{
				List<PnPEntityInfo> PnPEntities = new List<PnPEntityInfo>();

				// 枚举即插即用设备实体
				String VIDPID;
				if (VendorID == UInt16.MinValue)
				{
					if (ProductID == UInt16.MinValue)
						VIDPID = "'%VID[_]____&PID[_]____%'";
					else
						VIDPID = "'%VID[_]____&PID[_]" + ProductID.ToString("X4") + "%'";
				}
				else
				{
					if (ProductID == UInt16.MinValue)
						VIDPID = "'%VID[_]" + VendorID.ToString("X4") + "&PID[_]____%'";
					else
						VIDPID = "'%VID[_]" + VendorID.ToString("X4") + "&PID[_]" + ProductID.ToString("X4") + "%'";
				}

				String QueryString;
				if (ClassGuid == Guid.Empty)
					QueryString = "SELECT * FROM Win32_PnPEntity WHERE PNPDeviceID LIKE" + VIDPID;
				else
					QueryString = "SELECT * FROM Win32_PnPEntity WHERE PNPDeviceID LIKE" + VIDPID + " AND ClassGuid='" + ClassGuid.ToString("B") + "'";

				ManagementObjectCollection PnPEntityCollection = new ManagementObjectSearcher(QueryString).Get();
				if (PnPEntityCollection != null)
				{
					foreach (ManagementObject Entity in PnPEntityCollection)
					{
						String PNPDeviceID = Entity["PNPDeviceID"] as String;
						Match match = Regex.Match(PNPDeviceID, "VID_[0-9|A-F]{4}&PID_[0-9|A-F]{4}");
						if (match.Success)
						{
							PnPEntityInfo Element;

							Element.PNPDeviceID = PNPDeviceID;                      // 设备ID
							Element.Name = Entity["Name"] as String;                // 设备名称
							Element.Description = Entity["Description"] as String;  // 设备描述
							Element.Service = Entity["Service"] as String;          // 服务
							Element.Status = Entity["Status"] as String;            // 设备状态
							Element.VendorID = Convert.ToUInt16(match.Value.Substring(4, 4), 16);   // 供应商标识
							Element.ProductID = Convert.ToUInt16(match.Value.Substring(13, 4), 16); // 产品编号
							Element.ClassGuid = new Guid(Entity["ClassGuid"] as String);            // 设备安装类GUID

							PnPEntities.Add(Element);
						}
					}
				}

				if (PnPEntities.Count == 0) return null; else return PnPEntities.ToArray();
			}

			/// <summary>
			/// 根据VID和PID定位即插即用设备实体
			/// </summary>
			/// <param name="VendorID">供应商标识，MinValue忽视</param>
			/// <param name="ProductID">产品编号，MinValue忽视</param>
			/// <returns>设备列表</returns>
			public static PnPEntityInfo[] WhoPnPEntity(UInt16 VendorID, UInt16 ProductID)
			{
				return WhoPnPEntity(VendorID, ProductID, Guid.Empty);
			}

			/// <summary>
			/// 根据设备安装类GUID定位即插即用设备实体
			/// </summary>
			/// <param name="ClassGuid">设备安装类Guid，Empty忽视</param>
			/// <returns>设备列表</returns>
			public static PnPEntityInfo[] WhoPnPEntity(Guid ClassGuid)
			{
				return WhoPnPEntity(UInt16.MinValue, UInt16.MinValue, ClassGuid);
			}

			/// <summary>
			/// 根据设备ID定位设备
			/// </summary>
			/// <param name="PNPDeviceID">设备ID，可以是不完整信息</param>
			/// <returns>设备列表</returns>
			/// <remarks>
			/// 注意：对于下划线，需要写成“[_]”，否则视为任意字符
			/// </remarks>
			public static PnPEntityInfo[] WhoPnPEntity(String PNPDeviceID)
			{
				List<PnPEntityInfo> PnPEntities = new List<PnPEntityInfo>();

				// 枚举即插即用设备实体
				String QueryString;
				if (String.IsNullOrEmpty(PNPDeviceID))
				{
					QueryString = "SELECT * FROM Win32_PnPEntity WHERE PNPDeviceID LIKE '%VID[_]____&PID[_]____%'";
				}
				else
				{   // LIKE子句中有反斜杠字符将会引发WQL查询异常
					QueryString = "SELECT * FROM Win32_PnPEntity WHERE PNPDeviceID LIKE '%" + PNPDeviceID.Replace('\\', '_') + "%'";
				}

				ManagementObjectCollection PnPEntityCollection = new ManagementObjectSearcher(QueryString).Get();
				if (PnPEntityCollection != null)
				{
					foreach (ManagementObject Entity in PnPEntityCollection)
					{
						String thePNPDeviceID = Entity["PNPDeviceID"] as String;
						Match match = Regex.Match(thePNPDeviceID, "VID_[0-9|A-F]{4}&PID_[0-9|A-F]{4}");
						if (match.Success)
						{
							PnPEntityInfo Element;

							Element.PNPDeviceID = thePNPDeviceID;                   // 设备ID
							Element.Name = Entity["Name"] as String;                // 设备名称
							Element.Description = Entity["Description"] as String;  // 设备描述
							Element.Service = Entity["Service"] as String;          // 服务
							Element.Status = Entity["Status"] as String;            // 设备状态
							Element.VendorID = Convert.ToUInt16(match.Value.Substring(4, 4), 16);   // 供应商标识
							Element.ProductID = Convert.ToUInt16(match.Value.Substring(13, 4), 16); // 产品编号
							Element.ClassGuid = new Guid(Entity["ClassGuid"] as String);            // 设备安装类GUID

							PnPEntities.Add(Element);
						}
					}
				}

				if (PnPEntities.Count == 0) return null; else return PnPEntities.ToArray();
			}

			/// <summary>
			/// 根据服务定位设备
			/// </summary>
			/// <param name="ServiceCollection">要查询的服务集合，null忽视</param>
			/// <returns>设备列表</returns>
			/// <remarks>
			/// 跟服务相关的类：
			///     Win32_SystemDriverPNPEntity
			///     Win32_SystemDriver
			/// </remarks>
			public static PnPEntityInfo[] WhoPnPEntity(String[] ServiceCollection)
			{
				if (ServiceCollection == null || ServiceCollection.Length == 0)
					return WhoPnPEntity(UInt16.MinValue, UInt16.MinValue, Guid.Empty);

				List<PnPEntityInfo> PnPEntities = new List<PnPEntityInfo>();

				// 枚举即插即用设备实体
				String QueryString = "SELECT * FROM Win32_PnPEntity WHERE PNPDeviceID LIKE '%VID[_]____&PID[_]____%'";
				ManagementObjectCollection PnPEntityCollection = new ManagementObjectSearcher(QueryString).Get();
				if (PnPEntityCollection != null)
				{
					foreach (ManagementObject Entity in PnPEntityCollection)
					{
						String PNPDeviceID = Entity["PNPDeviceID"] as String;
						Match match = Regex.Match(PNPDeviceID, "VID_[0-9|A-F]{4}&PID_[0-9|A-F]{4}");
						if (match.Success)
						{
							String theService = Entity["Service"] as String;            // 服务
							if (String.IsNullOrEmpty(theService)) continue;

							foreach (String Service in ServiceCollection)
							{   // 注意：忽视大小写
								if (String.Compare(theService, Service, true) != 0) continue;

								PnPEntityInfo Element;

								Element.PNPDeviceID = PNPDeviceID;                      // 设备ID
								Element.Name = Entity["Name"] as String;                // 设备名称
								Element.Description = Entity["Description"] as String;  // 设备描述
								Element.Service = theService;                           // 服务
								Element.Status = Entity["Status"] as String;            // 设备状态
								Element.VendorID = Convert.ToUInt16(match.Value.Substring(4, 4), 16);   // 供应商标识
								Element.ProductID = Convert.ToUInt16(match.Value.Substring(13, 4), 16); // 产品编号
								Element.ClassGuid = new Guid(Entity["ClassGuid"] as String);            // 设备安装类GUID

								PnPEntities.Add(Element);
								break;
							}
						}
					}
				}

				if (PnPEntities.Count == 0) return null; else return PnPEntities.ToArray();
			}
			#endregion
		}
	}
}