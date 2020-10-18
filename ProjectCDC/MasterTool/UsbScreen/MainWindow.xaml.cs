using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Management;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Threading;
using Microsoft.Win32;
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
		/// 暂存串口资源
		/// </summary>
		public static SerialPort serialport = new SerialPort();
		public static bool IsAutoCapture = false;
		/// <summary>
		/// 缓存图片颜色数据(设置成全局变量，重复使用避免GC)
		/// </summary>
		public static List<byte> ImageData = new List<byte>();
		/// <summary>
		/// 缓存固件数据
		/// </summary>
		public static ConcurrentQueue<byte[]> Firmware = new ConcurrentQueue<byte[]>();
		/// <summary>
		/// 数据发送缓冲队列
		/// </summary>
		public static ConcurrentQueue<byte[]> SendBufferQueue = new ConcurrentQueue<byte[]>();
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

					// 输出图片数据到下位机
					if (serialport.IsOpen) return;
					AutoCapture.IsChecked = false;
					Rectangle rectangle = new Rectangle()
					{
						X = (int)Preview.PointToScreen(new Point(0, 0)).X,
						Y = (int)Preview.PointToScreen(new Point(0, 0)).Y,
						Width = (int)Preview.Width,
						Height = (int)Preview.Height
					};
					SetDeviceData(rectangle);
				}
			};
			// 屏幕捕获
			Capture.PreviewMouseLeftButtonDown += delegate
			{
				Capture.CaptureMouse();
				ScreenCapture.ScreenSnapshot(out BitmapSource bitmapSource);
				ShowCapture.Source = bitmapSource;
			};
			// 移动捕获坐标
			Capture.PreviewMouseMove += (sender, e) =>
			{
				if (e.LeftButton == MouseButtonState.Pressed)
				{
					Point mousePoint = PointToScreen(e.GetPosition(this));
					ShowCapture.SetValue(Canvas.LeftProperty, 120 - mousePoint.X);
					ShowCapture.SetValue(Canvas.TopProperty, 120 - mousePoint.Y);
				}
			};
			// 向下位机发送捕获结果
			Capture.PreviewMouseLeftButtonUp += delegate
			{
				if (Capture.IsMouseCaptured && (serialport.IsOpen == false))
				{
					Capture.ReleaseMouseCapture();
					Rectangle rect = new Rectangle()
					{
						X = (int)Preview.PointToScreen(new Point(0, 0)).X,
						Y = (int)Preview.PointToScreen(new Point(0, 0)).Y,
						Width = (int)Preview.Width,
						Height = (int)Preview.Height
					};
					if (AutoCapture.IsChecked.Value)
					{
						rect.X = 0 - Convert.ToInt32(ShowCapture.GetValue(Canvas.LeftProperty));
						rect.Y = 0 - Convert.ToInt32(ShowCapture.GetValue(Canvas.TopProperty));
					}
					SetDeviceData(rect);
				}
			};
			// 自动捕获
			AutoCapture.Checked += delegate { IsAutoCapture = AutoCapture.IsChecked.Value; };
			AutoCapture.Unchecked += delegate { IsAutoCapture = AutoCapture.IsChecked.Value; };
			// 刷新图像
			//Refresh.Click += delegate
			//{
			//	if (serialport.IsOpen) return;
			//	Rectangle rectangle = new Rectangle()
			//	{
			//		X = 0 - Convert.ToInt32(ShowCapture.GetValue(Canvas.LeftProperty)),
			//		Y = 0 - Convert.ToInt32(ShowCapture.GetValue(Canvas.TopProperty)),
			//		Width = (int)Preview.Width,
			//		Height = (int)Preview.Height
			//	};
			//	SetDeviceData(rectangle);
			//};
			// 更新固件
			LoadHex.Click += delegate
			{
				if (serialport.IsOpen)
				{
					MessageBox.Show("请等待当前传输完成后再试!");
					return;
				}
				OpenFileDialog FileLoader = new OpenFileDialog()
				{
					Filter = "固件文件|*.hex"
				};
				if (FileLoader.ShowDialog() == false) return;
				Firmware = new ConcurrentQueue<byte[]>();
				HexToBin(FileLoader.FileName).ForEach(b => Firmware.Enqueue(b));
				// 输出转译后的固件数据,调试使用
				//Firmware.ForEach(b =>
				//{
				//	Debug.WriteLine(string.Join(" ", b.Select(i => $"{i:X2}")));
				//});
				try
				{
					serialport.PortName = DeviceComboBox.Text;
					serialport.Open();
					serialport.Write(new byte[] { 0xB1 }, 0, 1);    // 进入Bootloader模式命令
					SysPanel.IsEnabled = false;
					serialport.Close();
				}
				catch (Exception e)
				{
					Debug.Print(e.Message);
				}
			};
			this.Loaded += delegate
			{
				GetDeviceList();
			};
		}


		public void SetDeviceData(Rectangle rect)
		{
			if (DeviceComboBox.SelectedIndex == -1) return;
			serialport.PortName = DeviceComboBox.Text;
			Task.Factory.StartNew(() =>
			{
				serialport.Open();
				Stopwatch sw = new Stopwatch();
				do
				{
					CaptureArea(ref ImageData, rect);
					SendBufferQueue.Enqueue(new byte[] { 0x00, 0x00, 0xEF, 0x00, 0xEF });
					SendBufferQueue.Enqueue(ImageData.ToArray());
					sw.Restart();
					Debug.Print($"{DateTime.Now:HH:mm:ss.fff} [传输开始] 共计:{ImageData.Count + 5}Byte");
					sw.Start();
					while (SendBufferQueue.TryDequeue(out byte[] buff))
					{
						try
						{
							serialport.Write(buff, 0, buff.Length);
						}
						catch (Exception e)
						{
							Debug.Print($"{DateTime.Now:HH:mm:ss.fff} [传输失败] {e.Message}");
							return;
						}
					}
					sw.Stop();
					Debug.Print($"{DateTime.Now:HH:mm:ss.fff} [传输完成] 耗时:{sw.ElapsedMilliseconds}ms");
				}
				while (IsAutoCapture);
				serialport.Close();
			});
		}

		/// <summary>
		/// 捕获屏幕区域
		/// </summary>
		/// <param name="ImageArr">输出图片数据</param>
		public void CaptureArea(ref List<byte> colors, Rectangle rect)
		{
			ScreenCapture.GetScreenCapture(out Bitmap bmp, rect);
			colors.Clear();
			for (int h = 0; h < 240; ++h)
			{
				for (int v = 0; v < 240; ++v)
				{
					Color color = bmp.GetPixel(v, h);
					var rgb565 = color.R / 8 * 2048 + color.G / 4 * 32 + color.B / 8;
					colors.Add((byte)(rgb565 >> 8));
					colors.Add((byte)(rgb565 & 0xFF));
				}
			}
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
						GetDeviceList();
						ConnectingTimer.Stop();
					}
				};
			}
			ConnectingTimer.Start();
		}

		/// <summary>
		/// 获取串口设备列表
		/// </summary>
		private void GetDeviceList()
		{
			List<string> SerialPorts = new List<string>();
			// 枚举串口设备
			using (ManagementObjectSearcher searcher = new ManagementObjectSearcher("select * from Win32_SerialPort"))
			{
				try
				{
					foreach (ManagementObject Entity in searcher.Get())
					{
						if ((Entity["PNPDeviceID"] as string).EndsWith("ST1E6DIPSF0F0SPI3"))            // 筛选目标设备
						{
							SerialPorts.Add(Entity["DeviceID"] as string);
						}
						else if ((Entity["PNPDeviceID"] as string).EndsWith("ST1E6DIPSF0F0SPI3ISP"))    // 筛选ISP设备
						{
							UpdateFirmware(Entity["DeviceID"] as string);
						}
					}
				}
				catch (Exception e)
				{
					Console.WriteLine($"[枚举串口出错] {e.Message}");
				}
			}
			// 将设备列表加入下拉框
			this.Dispatcher.Invoke((Action)delegate
			{
				if (SerialPorts.Count > 0)
				{
					DeviceComboBox.ItemsSource = SerialPorts;
					DeviceComboBox.SelectedIndex = 0;
				}
				else
				{
					DeviceComboBox.SelectedIndex = -1;
					DeviceComboBox.ItemsSource = null;
				}
			});
		}

		/// <summary>
		/// 固件更新(必须符合:PNPDeviceID=ST1E6DIPSF0F0SPI3ISP)
		/// </summary>
		/// <param name="DeviceID">串口号</param>
		public void UpdateFirmware(string DeviceID)
		{
			SerialPort upgradePort = new SerialPort()
			{
				PortName = DeviceID
			};
			if (Firmware.IsEmpty)
			{
				OpenFileDialog FileLoader = new OpenFileDialog()
				{
					Filter = "固件文件|*.hex"
				};
				if (FileLoader.ShowDialog() == false) return;
				HexToBin(FileLoader.FileName).ForEach(b => Firmware.Enqueue(b));
			}
			upgradePort.Open();
			if (Firmware.IsEmpty)
			{
				try
				{
					upgradePort.Write(new byte[] { 0x1B, 0x00 }, 0, 2);
					upgradePort.Close();
				}
				catch (Exception e)
				{
					this.Dispatcher.Invoke((Action)delegate { SysPanel.IsEnabled = true; });
					Debug.Print(e.Message);
				}
				return;
			}

			this.Dispatcher.Invoke((Action)delegate
			{
				progress.Maximum = Firmware.Count;
				progress.Value = 0;
			});
			upgradePort.DataReceived += UpgradePort_DataReceived;
			if (Firmware.TryDequeue(out byte[] result))
			{
				try
				{
					upgradePort.Write(result, 0, result.Length);
					this.Dispatcher.Invoke((Action)delegate
					{
						++progress.Value;
						if (progress.Value == progress.Maximum) progress.Value = 0;
					});
				}
				catch (Exception e)
				{
					Firmware = new ConcurrentQueue<byte[]>();
					this.Dispatcher.Invoke((Action)delegate { SysPanel.IsEnabled = true; });
					Debug.Print($"{DateTime.Now:HH:mm:ss.fff} [传输失败] {e.Message}");
					return;
				}
			}
		}

		private void UpgradePort_DataReceived(object sender, SerialDataReceivedEventArgs e)
		{
			SerialPort upgradePort = (SerialPort)sender;
			if (upgradePort.BytesToRead == 0) return;                       // 忽略零长度数据包
			byte[] recvData = new byte[upgradePort.BytesToRead];
			upgradePort.Read(recvData, 0, upgradePort.BytesToRead);
			DebugPrint(string.Join(" ", recvData.Select(d => $"{d:X2}")));  // 打印收到的数据,调试使用
			if (ResultCommand(recvData[1]))
			{
				if (Firmware.TryDequeue(out byte[] result))
				{
					try
					{
						upgradePort.Write(result, 0, result.Length);
						this.Dispatcher.Invoke((Action)delegate
						{
							++progress.Value;
							if (progress.Value == progress.Maximum) progress.Value = 0;
						});
					}
					catch (Exception err)
					{
						Firmware = new ConcurrentQueue<byte[]>();
						Debug.Print($"{DateTime.Now:HH:mm:ss.fff} [传输失败] {err.Message}");
						return;
					}
				}
			}
			else
			{
				Firmware = new ConcurrentQueue<byte[]>();
			}
			if (Firmware.IsEmpty)
			{
				this.Dispatcher.Invoke((Action)delegate
				{
					SysPanel.IsEnabled = true;
				});
				upgradePort.DataReceived -= UpgradePort_DataReceived;
			}
		}
		/// <summary>
		/// 命令操作结果判断
		/// </summary>
		private bool ResultCommand(byte ReCode)
		{
			switch (ReCode)
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
				case 0x42:
					DebugPrint("[任务中止] Flash编程地址无效");
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
