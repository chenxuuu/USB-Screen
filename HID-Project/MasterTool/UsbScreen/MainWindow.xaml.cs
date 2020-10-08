using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Threading;
using HidApi;
using Microsoft.Win32;
using Microsoft.Win32.SafeHandles;
using Color = System.Drawing.Color;
using Point = System.Windows.Point;

namespace UsbScreen
{
	/// <summary>
	/// MainWindow.xaml 的交互逻辑
	/// </summary>
	public partial class MainWindow : Window
	{
		/// <summary>
		/// 缓存固件数据
		/// </summary>
		public static List<byte[]> Firmware { get; set; }
		/// <summary>
		/// USB设备类
		/// </summary>
		public class USBDevice
		{
			/// <summary>
			/// 设备地址
			/// </summary>
			public string Path { get; set; }
			/// <summary>
			/// 设备属性信息
			/// </summary>
			public HidAPI.HidAttributes Attr { get; set; }
			/// <summary>
			/// 设备功能信息
			/// </summary>
			public HidAPI.HIDP_CAPS Caps { get; set; }
			/// <summary>
			/// 设备数据读写通道
			/// </summary>
			public FileStream Channel { get; set; }
			/// <summary>
			/// 暂存待发送的数据
			/// </summary>
			public byte[] Buffer
			{
				get
				{
					return buffer;
				}
				set
				{
					buffer = value;
					SendDataBytes();
				}
			}
			private byte[] buffer = new byte[65];
			/// <summary>
			/// 发送数据到设备
			/// </summary>
			/// <param name="sendBytes">Byte[64]数组</param>
			public void SendDataBytes()
			{
				// 创建数据发送缓冲区
				byte[] dataBytes = Enumerable.Repeat((byte)0x00, Caps.OutputLength).ToArray();
				//dataBytes[0] = 0;                                                           // 设置 ReportID=0
				buffer.CopyTo(dataBytes, 1);												// 准备待发送数据
				try
				{
					Channel.Write(dataBytes, 0, Caps.OutputLength);                         // 发送数据到设备
				}
				catch (Exception e)
				{
					DebugPrint($"SendDataBytes操作出错:{e.Message}");
				}
			}
		}
		/// <summary>
		/// HID设备类集合
		/// </summary>
		public static List<USBDevice> HidDevice = new List<USBDevice>();
		/// <summary>
		/// 数据发送缓冲队列
		/// </summary>
		public static ConcurrentQueue<byte[]> SendBufferQueue = new ConcurrentQueue<byte[]>();
		///// <summary>
		///// 设备数据读写通道
		///// </summary>
		//public FileStream HidDevice { get; set; }
		///// <summary>
		///// 数据发送暂存区
		///// </summary>
		//private byte[] SendBytesTemp;
		/// <summary>
		/// 打印调试信息
		/// </summary>
		/// <param name="message">调试信息</param>
		public static void DebugPrint(object message)
		{
			Debug.WriteLine($"[{DateTime.Now:HH:mm:ss:fff}] {message}");
		}
		public MainWindow()
		{
			InitializeComponent();
			// 注册窗口移动事件
			TitleBar.MouseMove += (sender, e) =>
			{
				if (e.LeftButton == MouseButtonState.Pressed) this.DragMove();
			};
			// 注册窗口最小化按钮
			MinSize.Click += delegate { this.WindowState = WindowState.Minimized; };
			// 注册窗口关闭按钮
			AppExit.Click += delegate { this.Close(); };
			// 载入图片
			LoadImg.Click += delegate
			{
				OpenFileDialog FileLoader = new OpenFileDialog()
				{
					Filter = "位图文件|*.bmp;*.png;*.jpg"
				};
				if (FileLoader.ShowDialog().Value)
				{
					ShowCapture.Source = new BitmapImage();
					ImageBox.Source = new BitmapImage(new Uri(FileLoader.FileName));
				}
			};
			//Capture.PreviewMouseLeftButtonDown += delegate
			//{
			//	//if (Capture.AllowDrop)
			//	//{
			//	//	Capture.AllowDrop = false;
			//	//	Capture.Content = "屏幕捕获";
			//	//	ShowCapture.Source = new BitmapImage();
			//	//}
			//	//else
			//	//{
			//	//	Capture.AllowDrop = true;
			//	//	Capture.Content = "停止捕获";
			//	//	ShowCapture.Source = ScreenCapture.ScreenSnapshot();
			//	//}
			//	ShowCapture.Source = ScreenCapture.ScreenSnapshot();
			//};
			// 屏幕捕获
			Capture.PreviewMouseMove += (sender, e) =>
			{
				if (e.LeftButton == MouseButtonState.Pressed)
				{
					if (Capture.AllowDrop == false)
					{
						Capture.AllowDrop = true;
						ShowCapture.Source = ScreenCapture.ScreenSnapshot();
					}
					Point mousePoint = PointToScreen(e.GetPosition(this));
					ShowCapture.SetValue(Canvas.LeftProperty, 120 - mousePoint.X);
					ShowCapture.SetValue(Canvas.TopProperty, 120 - mousePoint.Y);
				}
				else
				{
					Capture.AllowDrop = false;
				}
			};
			// 刷新图像
			Refresh.Click += delegate
			{
				List<byte> ColorList = new List<byte>();

				RenderTargetBitmap bmp = new RenderTargetBitmap(240, 240, 96, 96, PixelFormats.Pbgra32);
				bmp.Render(Preview);
				using (var outStream = new MemoryStream())
				{
					BitmapEncoder encoder = new BmpBitmapEncoder();
					encoder.Frames.Add(BitmapFrame.Create(bmp));
					encoder.Save(outStream);
					using (Bitmap img = new Bitmap(outStream))
					{
						for (int h = 0; h < 240; ++h)
						{
							for (int v = 0; v < 240; ++v)
							{
								Color color = img.GetPixel(v, h);
								var rgb565 = color.R / 8 * 2048 + color.G / 4 * 32 + color.B / 8;
								ColorList.Add((byte)(rgb565 >> 8));
								ColorList.Add((byte)(rgb565 & 0xFF));
							}
						}
					}
				}

				byte spicmd = (byte)(sspi.IsChecked.Value ? 0xFB : 0xFA);
				List<byte[]> ImageArr = new List<byte[]>();
				for (int i = 0; i < ColorList.Count; i += 60)
				{
					List<byte> tmp = new List<byte> { spicmd, 0x3C, (byte)((i % 480) / 2), (byte)(i / 480) };
					tmp.AddRange(ColorList.Skip(i).Take(60));
					ImageArr.Add(tmp.ToArray());
				}
				ImageArr.ForEach(b => SendBufferQueue.Enqueue(b));
				if (SendBufferQueue.Count > 0) Transceiver(new byte[] { 0xB0 });// 启动数据收发器
			};
			// 更新固件
			LoadHex.Click += delegate
			{
				OpenFileDialog FileLoader = new OpenFileDialog()
				{
					Filter = "固件文件|*.hex"
				};
				if (FileLoader.ShowDialog() == false) return;
				Firmware = HexToBin(FileLoader.FileName);
				// 输出转译后的固件数据,调试使用
				//Firmware.ForEach(b =>
				//{
				//	Debug.WriteLine(string.Join(" ", b.Select(i => $"{i:X2}")));
				//});
				SendBufferQueue = new ConcurrentQueue<byte[]>();
				SendBufferQueue.Enqueue(new byte[] { 0xB1, 0x00 });// 进入Bootloader模式命令
				Transceiver(new byte[] { 0xB0 });// 启动数据收发器
			};
			this.Loaded += delegate
			{
				GetHidDeviceList();
			};
		}


		/// <summary>
		/// 创建Windows事件监听器
		/// </summary>
		/// <param name="e"></param>
		protected override void OnSourceInitialized(EventArgs e)
		{
			base.OnSourceInitialized(e);
			HwndSource source = PresentationSource.FromVisual(this) as HwndSource;
			source.AddHook(WndProc);  // 绑定事件监听,用于监听HID设备插拔
		}
		/// <summary>
		/// 监听Windows事件消息
		/// </summary>
		/// <param name="hwnd"></param>
		/// <param name="msg"></param>
		/// <param name="wParam"></param>
		/// <param name="lParam"></param>
		/// <param name="handled"></param>
		/// <returns></returns>
		private IntPtr WndProc(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
		{
			if (msg == 0x219)           // 监听USB设备插拔事件
			{
				DebugPrint($"<USB拔插事件> msg:0x{Convert.ToString(msg, 16)} | wParam:0x{Convert.ToString(wParam.ToInt32(), 16)}");
				RunConnectingTimer();
				handled = true;
			}
			return IntPtr.Zero;
		}
		// USB设备拔插延时任务(延迟处理USB插拔事件,提高通信稳定性)
		private static DispatcherTimer ConnectingTimer = null;
		private static uint ConnectingDelay = 0;
		private void RunConnectingTimer()
		{
			ConnectingDelay = 0;
			if (ConnectingTimer == null)
			{
				ConnectingTimer = new DispatcherTimer()
				{
					Interval = new TimeSpan(0, 0, 0, 0, 100)    // 定时器触发间隔100ms
				};
				ConnectingTimer.Tick += delegate
				{
					if (++ConnectingDelay >= 10)
					{
						GetHidDeviceList();
						ConnectingTimer.Stop();
					}
				};
			}
			ConnectingTimer.Start();
		}

		/// <summary>
		/// 获取HID设备列表
		/// </summary>
		private void GetHidDeviceList()
		{
			// 获取HID设备列表
			List<string> deviceList = HidAPI.GetHidDeviceList();
			DebugPrint($"[遍历HID设备列表] 找到 {deviceList.Count} 个HID标准设备");
			// 对列表排序
			deviceList.Sort();
			// 筛选符合要求的设备
			deviceList = deviceList.Select(s => s.ToUpper()).Where(str => str.Contains("VID_2333")).ToList();
			DebugPrint($"[筛选HID设备列表] 找到 {deviceList.Count} 个符合要求的设备");
			// 筛选新添加的设备
			if (HidDevice != null)
			{
				deviceList = deviceList.Where(str => !HidDevice.Exists(dev => dev.Path == str)).ToList();
			}
			DebugPrint($"[筛选HID设备列表] 找到 {deviceList.Count} 个刚添加的新设备");
			// 连接新添加的设备
			if (deviceList.Count > 0)
			{
				DeviceList.ItemsSource = deviceList;
				DeviceList.SelectedIndex = 0;
				ConnectDevice(deviceList);
			}
			else
			{
				DeviceList.ItemsSource = null;
			}
		}

		/// <summary>
		/// 连接HID设备
		/// </summary>
		/// <param name="devicePath">目标设备地址</param>
		private void ConnectDevice(List<string> deviceList)
		{
			deviceList.ForEach(devicePath =>
			{
				DebugPrint($"<连接设备> {devicePath}");
				// 使用共享模式打开设备读写句柄
				IntPtr devHandle = WinAPI.CreateFile(devicePath.ToLower(), WinAPI.Generic.READ | WinAPI.Generic.WRITE, (uint)FileShare.ReadWrite, 0, WinAPI.CreationDisposition.OPEN_EXISTING, WinAPI.FileFlag.OVERLAPPED, 0);
				// 若打开设备句柄失败则返回失败标志
				if (devHandle == new IntPtr(-1))
				{
					DebugPrint($"<连接设备失败> {devicePath.ToUpper()}");
					DebugPrint($"<GetLastError()={WinAPI.GetLastError()}> {WinAPI.ErrorCode(WinAPI.GetLastError())}");
					return;
				}
				// 获取设备属性 (若不需要可删除)
				HidAPI.HidD_GetAttributes(devHandle, out HidAPI.HidAttributes devAttr);
				// 找到对应的HID设备信息
				HidAPI.HidD_GetPreparsedData(devHandle, out IntPtr preparseData);
				HidAPI.HidP_GetCaps(preparseData, out HidAPI.HIDP_CAPS devCaps);
				HidAPI.HidD_FreePreparsedData(preparseData);
				// 创建设备读写通道
				HidDevice.Add(new USBDevice()
				{
					Path = devicePath,
					Attr = devAttr,
					Caps = devCaps,
					Channel = new FileStream(new SafeFileHandle(devHandle, false), FileAccess.ReadWrite, devCaps.InputLength, true)
				});
			});
			if (HidDevice.Count > 0)
			{
				HidDevice.ForEach(dev =>
				{
					DebugPrint($"设备已连接-> {dev.Path}");
					UsbReadAsync(dev); // 开始异步读取设备数据
				});
				// 通知设备已连接
				OnDeviceConnected(HidDevice[0].Attr.PID);
			}
		}
		/// <summary>
		/// 异步读数据
		/// </summary>
		private void UsbReadAsync(USBDevice dev)
		{
			byte[] inputBuffer = new byte[dev.Caps.InputLength];
			dev.Channel.BeginRead(inputBuffer, 0, dev.Caps.InputLength, (iResult) =>
			{
				byte[] readBuffer = (byte[])(iResult.AsyncState);
				try
				{
					dev.Channel.EndRead(iResult);                                       // 等待读数据结束
					byte[] readBytes = readBuffer.Skip(1).ToArray();                    // 读取接收的数据（丢弃ReportID,固定为第0位）
					Transceiver(readBytes);                                             // 转存已读取的数据
					UsbReadAsync(dev);                                                  // 启动下一次读操作
				}
				catch (Exception e)
				{
					DebugPrint($"DeviceReadAsync操作出错:{e.Message}");
					dev.Channel.Close();                                                // 关闭设备读写通道
					HidDevice.Remove(dev);                                              // 移除已断开的设备
					OnDeviceRemoved(dev);                                               // 通知设备已断开
				}
			}, inputBuffer);
		}
		/// <summary>
		/// 数据收发器
		/// </summary>
		/// <param name="readByters">Byte[]数组</param>
		public void Transceiver(byte[] readBytes)
		{
			// 识别收发器启动命令
			if (readBytes.Length == 1 && readBytes[0] == 0xB0)
			{
				// 初始化进度条
				this.Dispatcher.Invoke((Action)delegate
				{
					progress.Maximum = SendBufferQueue.Count;
					progress.Value = 0;
					progress.Visibility = Visibility.Visible;
				});
				// 开始统计时间
				DebugPrint($"<数据传输开始> 需要发送 {SendBufferQueue.Count} 个数据包");// 数据传输耗时统计
				watch.Restart();
				// 开始发送数据(多通道并行发送)
				HidDevice.ForEach(dev =>
				{
					Task.Factory.StartNew(() =>
					{
						if (SendBufferQueue.TryDequeue(out byte[] result)) dev.Buffer = result;
					});
				});
				return;
			}
			// 数据长度错误
			if (readBytes.Length != 64)
			{
				DebugPrint("<数据长度错误> " + string.Join(" ", readBytes.Select(d => $"{d:X2}"))); // 打印收到的数据,调试使用
			}
			// 判断反馈的命令操作结果
			if (ResultCommand(readBytes) == false)
			{
				DebugPrint(string.Join(" ", readBytes.Select(d => $"{d:X2}")));     // 打印收到的数据,调试使用
				this.Dispatcher.Invoke((Action)delegate { progress.Value = 0; });   // 清空进度条
				watch.Stop();
				DebugPrint($"<数据传输结束> 成功发送 {progress.Value} 个数据包,耗时 {watch.Elapsed}");
				return;
			}
			// 处理收到的数据
			HidDevice.ForEach(dev =>
			{
				byte[] lastBytes = dev.Buffer;
				if ((lastBytes != null) && (lastBytes[0] == readBytes[0]) && (lastBytes[2] == readBytes[2]) && (lastBytes[3] == readBytes[3]))
				{
					if (SendBufferQueue.TryDequeue(out byte[] result))
					{
						dev.Buffer = result;
					}
					this.Dispatcher.Invoke((Action)delegate
					{
						++progress.Value;
						if (progress.Value == progress.Maximum)
						{
							watch.Stop();
							DebugPrint($"<数据传输结束> 成功发送 {progress.Value} 个数据包,耗时 {watch.Elapsed}");
							progress.Visibility = Visibility.Collapsed;
						}
					});
				}
			});
		}
		/// <summary>
		/// Stopwatch计时器
		/// </summary>
		private static readonly Stopwatch watch = new Stopwatch();
		/// <summary>
		/// 命令操作结果判断
		/// </summary>
		private bool ResultCommand(byte[] CommandBytes)
		{
			switch (CommandBytes[1])
			{
				case 0x00:  // 操作成功
					return true;
				case 0x01:
					DebugPrint("[任务中止] 操作超时");
					break;
				case 0x02:
					DebugPrint("[任务中止] 操作出错");
					break;
				case 0x1D:
					DebugPrint("[任务中止] 设备唯一ID码校验失败");
					break;
				case 0x40:
					DebugPrint("[任务中止] 地址无效");
					break;
				case 0xDE:
					DebugPrint("[任务中止] 数据长度错误");
					break;
				case 0xE0:
					DebugPrint("[任务中止] 命令无效");
					break;
				case 0xFF:
					DebugPrint("[任务中止] 数据校验失败");
					break;
				default:
					DebugPrint("[任务中止] 未知错误");
					break;
			}
			return false;
		}
		/// <summary>
		/// 设备已连接事件
		/// </summary>
		private void OnDeviceConnected(int pid)
		{
			this.Dispatcher.Invoke((Action)delegate
			{
				progress.Value = 0;
				if (pid == 0x2324)
				{
					Refresh.IsEnabled = true;
					LoadHex.IsEnabled = true;
				}
				else if (pid == 0xC551 && Firmware != null && Firmware.Count > 0)
				{
					Firmware.ForEach(b => SendBufferQueue.Enqueue(b));
					Firmware.Clear();
					Transceiver(new byte[] { 0xB0 });// 启动数据收发器
				}
			});
		}
		/// <summary>
		/// 设备已断开事件
		/// </summary>
		private void OnDeviceRemoved(USBDevice dev)
		{
			this.Dispatcher.Invoke((Action)delegate
			{
				DebugPrint($"连接已断开-> {dev.Path}");
				Refresh.IsEnabled = false;
				LoadHex.IsEnabled = false;
			});
		}

		/// <summary>
		/// 转化Hex文件为Bin数据
		/// </summary>
		/// <param name="FilePath">文件路径</param>
		/// <returns>待下载的指令集合List&lt;byte[]&gt;</returns>
		public List<byte[]> HexToBin(string FilePath)
		{
			// 缓存转化后的数据
			List<byte[]> BinLines = new List<byte[]>();
			// 创建Hex数据缓存区
			List<string> HexLines = new List<string>();
			// 载入固件数据
			HexLines.AddRange(File.ReadAllLines(FilePath));
			DebugPrint($"已载入文件: {FilePath.Split('\\').Last()}");
			// 获取文件尾标志
			int footer = HexLines.FindIndex(str => str.Equals(":00000001FF"));
			if (footer != -1)
			{
				HexLines.RemoveAt(footer);
			}
			else
			{
				DebugPrint("Hex数据已损坏,导入失败!");
				return BinLines;
			}
			// 校验Hex数据
			foreach (string line in HexLines)
			{
				if (line.StartsWith(":") && (line.Length & 0x1) == 0x1)
				{
					byte checkSum = 0x00;
					for (int i = 1, len = line.Length; i < len; i += 2)
					{
						checkSum += Convert.ToByte(line.Substring(i, 2), 16);
					}
					if (checkSum != 0)
					{
						DebugPrint("Hex数据已损坏,导入失败!");
						return BinLines;
					}
				}
				else
				{
					DebugPrint("Hex数据已损坏,导入失败!");
					return BinLines;
				}
			}
			// 提取Hex有效数据(排除数据头和校验和)
			for (int i = 0, len = HexLines.Count; i < len; ++i)
			{
				HexLines[i] = HexLines[i].Substring(3, HexLines[i].Length - 3 - 2);
			}
			// 将数据按照地址从小到大排序
			HexLines.Sort();
			// 识别BootLoader固件(若是Boot固件,则需要对Start跳转命令进行处理)
			ushort bootAddr = Convert.ToUInt16(HexLines[1].Substring(0, 4), 16);
			if (bootAddr > 0x1000 && (byte)bootAddr == 0x04)
			{
				HexLines[0] = (bootAddr & 0xFC00).ToString("X4") + HexLines[0].Substring(4);
			}
			// 获取固件数据起始地址(WCH单片机CH55X系列Flash最大64K,所以只有16位地址)
			ushort startAddr = Convert.ToUInt16(HexLines[0].Substring(0, 4), 16);
			DebugPrint(startAddr == 0x00 ? "已载入用户程序固件" : "已载入引导程序固件");
			// 将分段的Hex数据转化为连续的Bin数据
			List<byte> binData = new List<byte>();
			HexLines.ForEach(line =>
			{
				int strAddr = Convert.ToInt32(line.Substring(0, 4), 16);
				while (binData.Count < (strAddr - startAddr))
				{
					binData.Add(0xFF);  // 为数据中间的空位填补0xFF占位
				}
				for (int i = 6, len = line.Length; i < len; i += 2)
				{
					binData.Add(Convert.ToByte(line.Substring(i, 2), 16));
				}
			});
			// 将连续的Bin数据分割为指定的长度(每段长 0x20 byte)
			List<List<byte>> TempList = new List<List<byte>>();
			for (int i = 0, len = binData.Count; i < len; i += 0x20)
			{
				TempList.Add(binData.Skip(i).Take(0x20).ToList());
			}
			// 使用0xFF数据补齐最后一行数据的长度,使其能够达到32位长(注:Flash校验限制长度为0x20)
			List<byte> patchBuffer = Enumerable.Repeat((byte)0xFF, 0x20 - TempList.Last().Count).ToList();
			TempList[TempList.Count - 1].AddRange(patchBuffer);
			// 计算数据长度和地址
			for (int i = 0, addr = startAddr, len = TempList.Count; i < len; ++i, addr += 0x20)
			{
				int sLen = TempList[i].Count();
				TempList[i].InsertRange(0, new List<byte> { 0x00, (byte)sLen, (byte)addr, (byte)(addr >> 8) });
			}
			// 将数据压入操作队列 {1byte操作命令,1byte数据长度,2byte操作地址,0x20byte数据内容}
			// 添加Flash写入命令
			TempList.ForEach(buf =>
			{
				buf[0] = 0x82;
				BinLines.Add(buf.ToArray());
			});
			// 添加Flash校验命令(判断数据内容是否为BootLoader,如果是,则不需要添加校验命令,用户程序会自动校验)
			if (startAddr == 0x0000)
			{
				TempList.ForEach(buf =>
				{
					buf[0] = 0x83;
					BinLines.Add(buf.ToArray());
				});
				// 添加设备重启命令
				BinLines.Add(new byte[] { 0x1B, 0x00 });
			}
			return BinLines;
		}
	}
}
