using System;
using System.Runtime.InteropServices;

namespace HidApi
{
	public class WinAPI
	{
		/// <summary>
		/// 设置函数返回值的错误信息
		/// </summary>
		/// <param name="dwErrCode">设置错误代码</param>
		[DllImport("kernel32.dll")]
		public static extern void SetLastError(uint dwErrCode);

		/// <summary>
		/// 读取函数返回的错误信息
		/// </summary>
		/// <returns>返回错误代码</returns>
		[DllImport("kernel32.dll")]
		public static extern uint GetLastError();

		/// <summary>
		/// 返回设备信息集合句柄，包括本地机器的设备信息元素
		/// </summary>
		/// <param name="ClassGuid">一个指向GUID的指针，此GUID可标识一个设备安装类或一个设备接口类。这个指针是可选的，并且可以为NULL。</param>
		/// <param name="Enumerator">一般为0；提供包含设备实例的枚举注册表分支下的键名，可以通过它获取设备信息。如果这个参数没有指定，则要从整个枚举树中获取所有设备实例的设备信息。</param>
		/// <param name="HwndParent">一般为IntPtr.Zero；提供顶级窗口的句柄，所有用户接口可以使用它来与成员联系。</param>
		/// <param name="Flags">通过此参数来过滤指定的设备信息集中的设备</param>
		/// <returns></returns>
		[DllImport("setupapi.dll", SetLastError = true)]
		public static extern IntPtr SetupDiGetClassDevs(ref Guid ClassGuid, uint Enumerator, IntPtr HwndParent, DIGCF Flags);
		/// <summary>
		/// 提供在设备信息结构中使用的控制选项
		/// <para>DIGCF_DEFAULT 系统默认设备接口相关联的设备。</para>
		/// <para>DIGCF_PRESENT 当前系统中存在的（已连接）设备。</para>
		/// <para>DIGCF_ALLCLASSES 所有已安装的设备(忽略ClassGuid参数)。</para>
		/// <para>DIGCF_PROFILE 当前硬件配置文件中的设备。</para>
		/// <para>DIGCF_DEVICEINTERFACE 支持指定设备接口类的设备。如果Enumerators参数指定了设备的实例ID，那么必须在Flags参数中设置此标志位。</para>
		/// </summary>
		public enum DIGCF
		{
			DIGCF_DEFAULT = 0x00000001,
			DIGCF_PRESENT = 0x00000002,
			DIGCF_ALLCLASSES = 0x00000004,
			DIGCF_PROFILE = 0x00000008,
			DIGCF_DEVICEINTERFACE = 0x00000010
		}

		/// <summary>
		/// 枚举设备信息集中包含的设备接口。(注意，本函数仅支持x86运行,x64会出现GetLastError=259)
		/// </summary>
		/// <param name="hDevInfo">指向包含要返回信息的设备接口的设备信息集的指针</param>
		/// <param name="devInfo">指向指定devInfo中的设备信息元素的SP_DEVINFO_DATA结构的指针</param>
		/// <param name="interfaceClassGuid">指定所请求接口的设备接口类的GUID</param>
		/// <param name="memberIndex">设备信息集中接口列表中从零开始的索引</param>
		/// <param name="deviceInterfaceData">包含完整的SP_DEVICE_INTERFACE_DATA结构的调用者分配的缓冲区，用于标识符合搜索参数的接口</param>
		/// <returns>如果函数顺利完成，则返回TRUE，如果有错误，则返回FALSE。</returns>
		[DllImport("setupapi.dll", CharSet = CharSet.Auto, SetLastError = true)]
		public static extern bool SetupDiEnumDeviceInterfaces(IntPtr deviceInfoSet, IntPtr deviceInfoData, ref Guid interfaceClassGuid, UInt32 memberIndex, ref SP_DEVICE_INTERFACE_DATA deviceInterfaceData);

		public struct SP_DEVICE_INTERFACE_DATA
		{
			public int cbSize;
			public Guid interfaceClassGuid;
			public int flags;
			public int reserved;
		}

		/// <summary>
		/// 返回有关设备接口的详细信息。（必须调用两次 第1次返回长度 第2次获取数据）
		/// </summary>
		/// <param name="deviceInfoSet">指向设备信息集的指针，它包含了所要接收信息的接口。该句柄通常由SetupDiGetClassDevs函数返回。</param>
		/// <param name="deviceInterfaceData">一个指向 SP_DEVICE_INTERFACE_DATA结构的指针，该结构指定了 DeviceInfoSet 参数中设备的接口。这个类型的指针通常由 SetupDiEnumDeviceInterfaces 函数返回。</param>
		/// <param name="deviceInterfaceDetailData">一个指向SP_DEVICE_INTERFACE_DETAIL_DATA结构的指针，该结构用于接收指定接口的信息。该参数是可选的且可以为NULL。如果DeviceInterfaceDetailSize 参数为0，该参数必须为NULL。如果该参数被指定，主调者必须在调用该函数之前，设置 SP_DEVICE_INTERFACE_DETAIL_DATA 结构的 cbSize 成员为 sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA)。cbSize 成员总是包含数据结构的固定部分的长度。</param>
		/// <param name="deviceInterfaceDetailDataSize">DeviceInterfaceDetailData 参数指定的缓冲的大小。该缓冲的大小不能小于 (offsetof(SP_DEVICE_INTERFACE_DETAIL_DATA, DevicePath) + sizeof(TCHAR)) 字节。	如果 DeviceInterfaceDetailData 参数为NULL,该参数必须为0.</param>
		/// <param name="requiredSize">一个指向变量的指针，该变量接收请求的 DeviceInterfaceDetailData 缓冲的大小。这个大小包含了结构的固定部分的大小再加上设备路径字符串的长度。该参数是可选的，也可以是NULL。</param>
		/// <param name="deviceInfoData">一个指向缓冲的指针，该缓冲接收关于支持请求的接口的设备的信息。主调者必须设置 DeviceInfoData.cbSize 成员为 sizeof(SP_DEVINFO_DATA)。该参数是可选的，也可以为NULL。</param>
		/// <returns>如果函数顺利完成，则返回TRUE，如果有错误，则返回FALSE。</returns>
		[DllImport("setupapi.dll", SetLastError = true, CharSet = CharSet.Auto)]
		public static extern bool SetupDiGetDeviceInterfaceDetail(IntPtr deviceInfoSet, ref SP_DEVICE_INTERFACE_DATA deviceInterfaceData, IntPtr deviceInterfaceDetailData, int deviceInterfaceDetailDataSize, ref int requiredSize, SP_DEVINFO_DATA deviceInfoData);
		[StructLayout(LayoutKind.Sequential)]
		public class SP_DEVINFO_DATA
		{
			public int cbSize = Marshal.SizeOf(typeof(SP_DEVINFO_DATA));
			public Guid classGuid = Guid.Empty; // temp
			public int devInst = 0; // dumy
			public int reserved = 0;
		}

		[StructLayout(LayoutKind.Sequential, Pack = 2)]
		public struct SP_DEVICE_INTERFACE_DETAIL_DATA
		{
			internal int cbSize;
			internal short devicePath;
		}

		/// <summary>
		/// 删除设备信息集并释放所有关联的内存。
		/// </summary>
		/// <param name="DeviceInfoSet">设备信息的句柄设置为删除。</param>
		/// <returns>成功返回True，失败返回False</returns>
		[DllImport("setupapi.dll", CharSet = CharSet.Auto, SetLastError = true)]
		public static extern bool SetupDiDestroyDeviceInfoList(IntPtr deviceInfoSet);

		/// <summary>
		/// 创建设备读写文件
		/// </summary>
		/// <param name="devicePath">设备路径(仅支持小写字母,可使用ToLower()转化)</param>
		/// <param name="desiredAccess">访问模式</param>
		/// <param name="shareMode">共享模式</param>
		/// <param name="securityAttributes">安全属性</param>
		/// <param name="creationDisposition">如何创建</param>
		/// <param name="flagsAndAttributes">文件属性</param>
		/// <param name="templateFile">模板文件的句柄</param>
		/// <returns>返回文件句柄</returns>
		[DllImport("kernel32.dll", SetLastError = true)]
		public static extern IntPtr CreateFile(string devicePath, uint desiredAccess, uint shareMode, uint securityAttributes, uint creationDisposition, uint flagsAndAttributes, uint templateFile);

		/// <summary>
		/// 关闭CreateFile创建的句柄
		/// </summary>
		/// <param name="hObject">设备句柄，即CreateFile的返回值</param>
		/// <returns>成功返回True，失败返回False</returns>
		[DllImport("kernel32.dll")]
		public static extern bool CloseHandle(IntPtr hObject);

		/// <summary>
		/// 对象的访问类型 
		///</summary>
		public static class Generic
		{
			public const uint READ = 0x80000000;
			public const uint WRITE = 0x40000000;
			public const uint EXECUTE = 0x20000000;
			public const uint ALL = 0x10000000;
		}

		/// <summary>
		/// 文件存在与不存在时采取的行动
		/// </summary>
		public static class CreationDisposition
		{
			public const uint CREATE_NEW = 1;
			public const uint CREATE_ALWAYS = 2;
			public const uint OPEN_EXISTING = 3;                // 文件不存在时，返回失败
			public const uint OPEN_ALWAYS = 4;
			public const uint TRUNCATE_EXISTING = 5;
		}

		/// <summary>
		/// 文件的属性和标志。
		/// </summary>
		public static class FileFlag
		{
			public const uint WRITE_THROUGH = 0x80000000;
			public const uint OVERLAPPED = 0x40000000;   // 以重叠（异步）模式打开
			public const uint NO_BUFFERING = 0x20000000;
			public const uint RANDOM_ACCESS = 0x10000000;
			public const uint SEQUENTIAL_SCAN = 0x08000000;
			public const uint DELETE_ON_CLOSE = 0x04000000;
			public const uint BACKUP_SEMANTICS = 0x02000000;
			public const uint POSIX_SEMANTICS = 0x01000000;
			public const uint OPEN_REPARSE_POINT = 0x00200000;
			public const uint OPEN_NO_RECALL = 0x00100000;
			public const uint FIRST_PIPE_INSTANCE = 0x00080000;
		}

		/// <summary>
		/// 查询 GetLastError() 的错误描述
		/// </summary>
		/// <returns>错误代码的解释</returns>
		public static string ErrorCode(uint error)
		{
			return error switch
			{
				0x02 => "文件不存在！",
				0x05 => "访问被拒绝。",
				0x20 => "文件正在被其它进程占用！",
				_ => "未知错误！"
			};
		}
	}
}