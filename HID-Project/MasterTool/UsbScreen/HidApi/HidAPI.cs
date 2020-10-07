/*注意：HIDAPI只能在“平台目标:x86”模式下工作,否则无法获取到USB设备*/
using Microsoft.Win32.SafeHandles;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;

namespace HidApi
{
	public class HidAPI
	{
		private const int MaxHidDevices = 256;          // HID最大设备数量
		/// <summary>
		/// 结构体包含有关HIDClass设备的供应商信息
		/// <para>Size 属性长度(sizeof(HidAttributes))</para>
		/// <para>VID 供应商代码(VendorID)</para>
		/// <para>PID 产品代码(ProductID)</para>
		/// <para>VER 产品版本号(VersionNumber)</para>
		/// </summary>
		public struct HidAttributes
		{
			public int Size;
			public ushort VID;
			public ushort PID;
			public ushort VER;
		}

		/// <summary>
		/// USB设备信息结构体
		/// <para>UsagePage</para>
		/// <para>Usage</para>
		/// <para>InputLength USB上传数据包长度(含ReportID)。</para>
		/// <para>OutputLength USB下传数据包长度(含ReportID)。</para>
		/// <para>Reserved 保留供内部系统使用。</para>
		/// <para>NumberLinkCollectionNodes</para>
		/// <para>NumberInputButtonCaps</para>
		/// <para>NumberInputValueCaps</para>
		/// <para>NumberInputDataIndices</para>
		/// <para>NumberOutputButtonCaps</para>
		/// <para>NumberOutputValueCaps</para>
		/// <para>NumberOutputDataIndices</para>
		/// <para>NumberFeatureButtonCaps</para>
		/// <para>NumberFeatureValueCaps</para>
		/// <para>NumberFeatureDataIndices</para>
		/// </summary>
		public struct HIDP_CAPS
		{
			public ushort UsagePage;
			public ushort Usage;
			public ushort InputLength;
			public ushort OutputLength;
			[MarshalAs(UnmanagedType.ByValArray, SizeConst = 17)]
			public ushort[] Reserved;
			public ushort NumberLinkCollectionNodes;
			public ushort NumberInputButtonCaps;
			public ushort NumberInputValueCaps;
			public ushort NumberInputDataIndices;
			public ushort NumberOutputButtonCaps;
			public ushort NumberOutputValueCaps;
			public ushort NumberOutputDataIndices;
			public ushort NumberFeatureButtonCaps;
			public ushort NumberFeatureValueCaps;
			public ushort NumberFeatureDataIndices;
		}

		/// <summary>
		/// 获取HID的全局GUID
		/// </summary>
		/// <param name="HidGuid">指向调用者分配的Guid容器</param>
		[DllImport("hid.dll")]
		public static extern void HidD_GetHidGuid(out Guid HidGuid);

		/// <summary>
		/// 返回HID设备的属性集合
		/// </summary>
		/// <param name="HidDeviceObject">设备句柄，即CreateFile的返回值</param>
		/// <param name="Attributes">返回指定设备的属性集合</param>
		/// <returns>成功返回True，失败返回False</returns>
		[DllImport("hid.dll")]
		public static extern bool HidD_GetAttributes(IntPtr hidDeviceObject, out HidAttributes attributes);

		/// <summary>
		/// 返回HID顶级集合的预处理数据。
		/// </summary>
		/// <param name="HidDeviceObject">设备句柄，即CreateFile的返回值</param>
		/// <param name="PreparsedData">指向包含_HIDP_PREPARSED_DATA结构中的集合预处理数据的例程分配缓冲区的地址。</param>
		/// <returns>成功返回True，失败返回False</returns>
		[DllImport("hid.dll")]
		public static extern bool HidD_GetPreparsedData(IntPtr HidDeviceObject, out IntPtr PreparsedData);

		/// <summary>
		/// 释放资源
		/// </summary>
		/// <param name="PreparsedData"></param>
		/// <returns></returns>
		[DllImport("hid.dll")]
		public static extern bool HidD_FreePreparsedData(IntPtr PreparsedData);

		/// <summary>
		/// 设置HID接收缓冲区大小
		/// </summary>
		/// <param name="HidDeviceObject">设备句柄，即CreateFile的返回值</param>
		/// <param name="NumberBuffers">缓冲区大小参数（65*N）</param>
		/// <returns></returns>
		[DllImport("hid.dll")]
		public static extern bool HidD_SetNumInputBuffers(IntPtr HidDeviceObject, int NumberBuffers);

		[DllImport("hid.dll")]
		public static extern uint HidP_GetCaps(IntPtr PreparsedData, out HIDP_CAPS Capabilities);

		/// <summary>
		/// 获取已连接电脑的HID设备列表
		/// </summary>
		/// <returns>返回HID设备集合字符串List</returns>
		public static List<string> GetHidDeviceList()
		{
			List<string> deviceList = new List<string>();
			HidD_GetHidGuid(out Guid HIDGuid);  // 获取HID设备的全局GUID，用于筛选所有已连接设备中的HID设备。
			IntPtr HIDInfoSet = WinAPI.SetupDiGetClassDevs(ref HIDGuid, 0, IntPtr.Zero, WinAPI.DIGCF.DIGCF_PRESENT | WinAPI.DIGCF.DIGCF_DEVICEINTERFACE);    //获取包含所有HID接口信息集合的句柄
			if (HIDInfoSet != IntPtr.Zero)
			{
				WinAPI.SP_DEVICE_INTERFACE_DATA interfaceInfo = new WinAPI.SP_DEVICE_INTERFACE_DATA();
				interfaceInfo.cbSize = Marshal.SizeOf(interfaceInfo);
				for (uint index = 0; index < MaxHidDevices; index++)
				{
					if (!WinAPI.SetupDiEnumDeviceInterfaces(HIDInfoSet, IntPtr.Zero, ref HIDGuid, index, ref interfaceInfo)) continue;
					int buffsize = 0;
					// 第一次读取接口详细信息(取得信息缓冲区的大小)
					WinAPI.SetupDiGetDeviceInterfaceDetail(HIDInfoSet, ref interfaceInfo, IntPtr.Zero, buffsize, ref buffsize, null);
					IntPtr pDetail = Marshal.AllocHGlobal(buffsize);
					WinAPI.SP_DEVICE_INTERFACE_DETAIL_DATA detail = new WinAPI.SP_DEVICE_INTERFACE_DETAIL_DATA()
					{
						cbSize = Marshal.SizeOf(typeof(WinAPI.SP_DEVICE_INTERFACE_DETAIL_DATA))
					};
					Marshal.StructureToPtr(detail, pDetail, false);
					// 第二次读取接口详细信息
					if (WinAPI.SetupDiGetDeviceInterfaceDetail(HIDInfoSet, ref interfaceInfo, pDetail, buffsize, ref buffsize, null))
					{
						deviceList.Add(Marshal.PtrToStringAuto((IntPtr)((int)pDetail + 4)));    //通常数据为小写字符串
					}
					Marshal.FreeHGlobal(pDetail);
				}
			}
			WinAPI.SetupDiDestroyDeviceInfoList(HIDInfoSet);
			return deviceList;
		}
	}
}