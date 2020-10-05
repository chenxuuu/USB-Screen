using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Threading;
using HidApi;
using Microsoft.Win32;
using Microsoft.Win32.SafeHandles;
using Point = System.Windows.Point;

namespace UsbScreen
{
	/// <summary>
	/// MainWindow.xaml 的交互逻辑
	/// </summary>
	public partial class MainWindow : Window
	{
		/// <summary>
		/// 设备属性信息
		/// </summary>
		public HidAPI.HidAttributes DeviceAttr { get; set; }
		/// <summary>
		/// 设备功能信息
		/// </summary>
		public HidAPI.HIDP_CAPS DeviceCaps { get; set; }
		/// <summary>
		/// 设备数据读写通道
		/// </summary>
		public FileStream HidDevice { get; set; }
		/// <summary>
		/// 数据发送缓冲队列
		/// </summary>
		public Queue<byte[]> SendBufferQueue = new Queue<byte[]>();
		/// <summary>
		/// 数据发送暂存区
		/// </summary>
		private byte[] SendBytesTemp;
		/// <summary>
		/// 打印调试信息
		/// </summary>
		/// <param name="message">调试信息</param>
		private void DebugPrint(object message)
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
				RenderTargetBitmap bmp = new RenderTargetBitmap(240, 240, 96, 96, System.Windows.Media.PixelFormats.Pbgra32);
				bmp.Render(Preview);
				MemoryStream stream = new MemoryStream();
				BitmapEncoder encoder = new BmpBitmapEncoder();
				encoder.Frames.Add(BitmapFrame.Create(bmp));
				encoder.Save(stream);

				List<byte> ColorList = new List<byte>();
				using (Bitmap img = new Bitmap(stream))
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
				stream.Dispose();

				List<byte[]> ImageArr = new List<byte[]>();
				for (int i = 0; i < ColorList.Count; i += 60)
				{
					List<byte> tmp = new List<byte> { 0xFA, 0x3C, (byte)(i % 120), (byte)(i / 120) };
					tmp.AddRange(ColorList.Skip(i).Take(60));
					ImageArr.Add(tmp.ToArray());
				}
				ImageArr.ForEach(b => SendBufferQueue.Enqueue(b));
				if (SendBufferQueue.Count > 0) Transceiver(new byte[] { 0xB0 });// 启动数据收发器
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
			DebugPrint($"[遍历HID设备列表] 找到 {deviceList.Count} 个设备");
			// 对列表排序
			deviceList.Sort();
			// 筛选符合要求的设备
			deviceList = deviceList.Select(s => s.ToUpper()).Where(str => str.Contains("VID_2333")).ToList();
			DebugPrint($"[筛选HID设备列表] 找到 {deviceList.Count} 个符合要求的设备");
			if (deviceList.Count > 0)
			{
				DeviceList.ItemsSource = deviceList;
				DeviceList.SelectedIndex = 0;
				ConnectDevice(deviceList[0]);
			}
			else
			{
				DeviceList.ItemsSource = null;
			}

		}

		/// <summary>
		/// 连接指定地址的HID设备
		/// </summary>
		/// <param name="devicePath">目标设备地址</param>
		private void ConnectDevice(string devicePath)
		{

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
			DeviceAttr = devAttr;
			// 找到对应的HID设备信息
			HidAPI.HidD_GetPreparsedData(devHandle, out IntPtr preparseData);
			HidAPI.HidP_GetCaps(preparseData, out HidAPI.HIDP_CAPS devCaps);
			DeviceCaps = devCaps;
			HidAPI.HidD_FreePreparsedData(preparseData);

			// 创建设备读写通道
			HidDevice = new FileStream(new SafeFileHandle(devHandle, false), FileAccess.ReadWrite, DeviceCaps.InputLength, true);
			// 开始异步读取设备数据
			DeviceReadAsync();
			// 通知设备已连接
			OnDeviceConnected();
		}
		/// <summary>
		/// 异步读数据
		/// </summary>
		private void DeviceReadAsync()
		{
			byte[] inputBuffer = new byte[DeviceCaps.InputLength];
			HidDevice.BeginRead(inputBuffer, 0, DeviceCaps.InputLength, (iResult) =>
			{
				byte[] readBuffer = (byte[])(iResult.AsyncState);
				try
				{
					HidDevice.EndRead(iResult);                                         // 等待读数据结束
					byte[] readBytes = readBuffer.Skip(1).ToArray();                    // 读取接收的数据（丢弃ReportID,固定为第0位）
					Transceiver(readBytes);                                             // 转存已读取的数据
					DeviceReadAsync();                                                  // 启动下一次读操作
				}
				catch (Exception e)
				{
					DebugPrint($"DeviceReadAsync操作出错:{e.Message}");
					HidDevice.Close();                                                  // 关闭设备读写通道
					OnDeviceRemoved();                                                  // 通知设备已断开
				}
			}, inputBuffer);
		}
		/// <summary>
		/// 发送数据到设备
		/// </summary>
		/// <param name="sendBytes">Byte[64]数组</param>
		private void SendDataBytes(byte[] sendBytes)
		{
			// 创建数据发送缓冲区
			byte[] dataBytes = Enumerable.Repeat((byte)0x00, DeviceCaps.OutputLength).ToArray();
			//dataBytes[0] = 0;                                                         // 设置 ReportID=0
			sendBytes.CopyTo(dataBytes, 1);												// 准备待发送数据
			try
			{
				HidDevice.Write(dataBytes, 0, DeviceCaps.OutputLength);					// 发送数据到设备
			}
			catch (Exception e)
			{
				DebugPrint($"SendDataBytes操作出错:{e.Message}");
				DataSendingTimer.Stop();                                                // 关闭数据发送超时定时器
			}
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
				RunSendingTimer();                                                      // 启动数据发送超时定时器
				return;
			}
			// 处理收到的数据
			if (SendBytesTemp != null && readBytes.Length == 64 && readBytes[0] == SendBytesTemp[0])
			{
				DebugPrint(string.Join(" ", readBytes.Select(d => $"{d:X2}")));			// 打印收到的数据,调试使用
				if (ResultCommand(readBytes) && SendBufferQueue.Count > 0)              // 判断反馈的命令操作结果
				{
					SendBytesTemp = SendBufferQueue.Dequeue();
					SendDataBytes(SendBytesTemp);
					DelayTimerValue = 0;                                                // 更新数据发送超时检测定时器阈值
				}
				else
				{
					DataSendingTimer.Stop();
					SendBufferQueue.Clear();
				}
			}
			else
			{
				DebugPrint(":: " + string.Join(" ", readBytes.Select(d => $"{d:X2}"))); // 打印收到的数据,调试使用
			}
		}
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
			DebugPrint(string.Join(" ", CommandBytes.Select(d => $"{d:X2}")));         // 打印收到的数据,调试使用
			return false;
		}
		/// <summary>
		/// 设备已连接事件
		/// </summary>
		private void OnDeviceConnected()
		{
			this.Dispatcher.Invoke((Action)delegate
			{
				Refresh.IsEnabled = true;
				DebugPrint($"设备已连接-> VID:{DeviceAttr.VID:X4} PID:{DeviceAttr.PID:X4} REV:{DeviceAttr.VER:X4} ReportIO:{DeviceCaps.InputLength}/{DeviceCaps.OutputLength}");
			});
		}
		/// <summary>
		/// 设备已断开事件
		/// </summary>
		private void OnDeviceRemoved()
		{
			this.Dispatcher.Invoke((Action)delegate
			{
				Refresh.IsEnabled = false;
				DebugPrint($"连接已断开-> VID:{DeviceAttr.VID:X4} PID:{DeviceAttr.PID:X4} REV:{DeviceAttr.VER:X4}");
			});
		}
		/// <summary>
		/// 数据发送超时检测定时器
		/// </summary>
		private DispatcherTimer DataSendingTimer = null;
		private uint DelayTimerValue = 0;
		private void RunSendingTimer()
		{
			DelayTimerValue = 0;
			if (DataSendingTimer == null)
			{
				DataSendingTimer = new DispatcherTimer()
				{
					Interval = new TimeSpan(0, 0, 0, 0, 100)    // 定时器触发间隔100ms
				};
				DataSendingTimer.Tick += delegate
				{
					if (SendBytesTemp[0] == 0xB1 || SendBytesTemp[0] == 0x1B)
					{
						DataSendingTimer.Stop();                // 重启指令不会得到反馈结果(关闭数据发送超时定时器)
						return;
					}
					if (++DelayTimerValue < 10) return;
					DebugPrint("数据传输超时,将立即重试...");
					DelayTimerValue = 0;
					SendDataBytes(SendBytesTemp);
				};
			}
			if (!DataSendingTimer.IsEnabled)
			{
				if (SendBufferQueue.Count == 0)
				{
					DataSendingTimer.Stop();
					return;
				}
				SendBytesTemp = SendBufferQueue.Dequeue();
				SendDataBytes(SendBytesTemp);
				DataSendingTimer.Start();
			}
		}

	}
}
