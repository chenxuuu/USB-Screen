using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Threading;
using HidApi;
using Microsoft.Win32;
using Microsoft.Win32.SafeHandles;

namespace UsbDebugTool
{
	/// <summary>
	/// MainWindow.xaml 的交互逻辑
	/// </summary>
	public partial class MainWindow : Window
	{
		public HidAPI.HidAttributes DeviceAttr;     // 设备属性信息
		public HidAPI.HIDP_CAPS DeviceCaps;         // 设备功能信息
		public FileStream HidDevice;                // 设备读写通道
		/// <summary>
		/// 暂存固件数据
		/// </summary>
		public List<byte[]> Firmware = new List<byte[]>();
		/// <summary>
		/// 暂存图片数据
		/// </summary>
		public List<byte[]> ImageArr = new List<byte[]>();
		/// <summary>
		/// 数据发送缓冲队列
		/// </summary>
		public Queue<byte[]> SendBufferQueue = new Queue<byte[]>();

		// 串口类
		internal SerialPort SerialPortBase = new SerialPort();

		/// <summary>
		/// 数据发送暂存区
		/// </summary>
		private byte[] SendBytesTemp;
		/// <summary>
		/// 打印调试信息
		/// </summary>
		/// <param name="info">调试信息</param>
		public void DebugPrint(object info)
		{
			Debug.WriteLine($"{DateTime.Now:HH:mm:ss:fff} {info}");
			Dispatcher.BeginInvoke((Action)(() =>
			{
				ReadBox.AppendText($"{info}{Environment.NewLine}");
			}));
		}
		/// <summary>
		/// 应用程序主入口
		/// </summary>
		public MainWindow()
		{
			InitializeComponent();
			// 注册窗口移动事件
			TitleBar.MouseMove += (sender, e) =>
			{
				if (e.LeftButton == MouseButtonState.Pressed) this.DragMove();
			};
			// 注册窗口关闭按钮
			AppExit.Click += delegate { this.Close(); };
			// 窗口加载完成事件
			this.Loaded += delegate
			{
				GetDeviceList();
			};
			ScanList.Click += delegate
			{
				GetDeviceList();
			};
			LinkRead.Click += delegate
			{
				if (LinkRead.Content.Equals("连接设备"))
				{
					ConnectDevice(DeviceList.SelectedItem.ToString());  // 连接指定路径设备
				}
				else
				{
					HidDevice.Close();                                  // 关闭设备读写通道
					OnDeviceRemoved();
				}
			};
			LoadImg.Click += delegate
			{
				ImageArr.Clear();
				OpenFileDialog FileLoader = new OpenFileDialog()
				{
					Filter = "位图文件|*.bmp;*.png;*.jpg"
				};
				if (FileLoader.ShowDialog() == false) return;

				List<byte> ColorList = new List<byte>();
				using (var img = new System.Drawing.Bitmap(FileLoader.FileName))
				{
					for (int h = 0; h < 240; ++h)
					{
						for (int v = 0; v < 240; ++v)
						{
							var color = img.GetPixel(v, h);
							var rgb565 = color.R / 8 * 2048 + color.G / 4 * 32 + color.B / 8;
							ColorList.Add((byte)(rgb565 >> 8));
							ColorList.Add((byte)(rgb565 & 0xFF));
						}
					}
				}
				//for (int i = 0; i < ColorList.Count; i += 60)
				//{
				//	List<byte> tmp = new List<byte> { 0xFA, 0x3C, (byte)((i % 480) / 2), (byte)(i / 480) };
				//	tmp.AddRange(ColorList.Skip(i).Take(60));
				//	ImageArr.Add(tmp.ToArray());
				//}
				for (int i = 0; i < ColorList.Count; i += 64)
				{
					ImageArr.Add(ColorList.Skip(i).Take(64).ToArray());
				}
				// 输出转译后的图片数据,调试使用
				ImageArr.ForEach(b =>
				{
					Debug.WriteLine(string.Join(" ", b.Select(i => $"{i:X2}")));
				});

				if (ImageArr.Count > 0) LoadImg.Background = Brushes.GreenYellow;
			};
			LoadData.Click += delegate
			{
				Firmware.Clear();
				OpenFileDialog FileLoader = new OpenFileDialog()
				{
					Filter = "固件文件|*.hex;*.apk"
				};
				if (FileLoader.ShowDialog() == false) return;
				Firmware = HexToBin(FileLoader.FileName);
				// 输出转译后的固件数据,调试使用
				Firmware.ForEach(b =>
				{
					Debug.WriteLine(string.Join(" ", b.Select(i => $"{i:X2}")));
				});
			};
			SendData.Click += delegate
			{
				if (Firmware.Count > 0)                                         // 如果已载入固件,将固件存入发送缓冲队列
				{
					Firmware.ForEach(b => SendBufferQueue.Enqueue(b));
					Firmware.Clear();
				}
				else if (ImageArr.Count > 0)
				{
					ImageArr.ForEach(b => SendBufferQueue.Enqueue(b));
					ImageArr.Clear();
				}
				else if (SendBox.Text.Length >= 2)
				{
					for (int i = 0; i < SendBox.LineCount; ++i)
					{
						try
						{
							byte[] SendCmd = SendBox.GetLineText(i).Trim().Split(' ').Select(s => Convert.ToByte(s, 16)).ToArray();
							SendBufferQueue.Enqueue(SendCmd);                   // 加入数据发送暂存区
						}
						catch (Exception e)
						{
							SendBufferQueue.Clear();                            // 清空数据发送暂存区
							DebugPrint($"SendCmd错误: {e.Message}");
							return;
						}
					}
				}
				if (SendBufferQueue.Count > 0) Transceiver(new byte[] { 0xB0 });// 启动数据收发器
			};
			ClearBox.Click += delegate
			{
				ReadBox.Clear();
			};


			//注册对串口接收数据的响应方法
			SerialPortBase.DataReceived += delegate
			{
				//获取接收缓冲区中数据的字节数
				byte[] buf = new byte[SerialPortBase.BytesToRead];
				//将数据读入buf数组中
				SerialPortBase.Read(buf, 0, buf.Length);
				Dispatcher.BeginInvoke((Action)(() =>
				{
					ReadBox.AppendText(string.Join(" ", buf.Select(b => b.ToString("X2"))));
				}));
			};
			ScanUart.Click += delegate
			{
				//UartList.ItemsSource = SerialPort.GetPortNames();
				//UartList.SelectedIndex = UartList.Items.Count - 1;

				var DeviceArr = IOPorts.HardWare.MulGetHardwareInfo(IOPorts.HardwareEnum.Win32_SerialPort);
				var DeviceStr = DeviceArr.Select(dev =>
				{
					return $"设备ID:{dev.PNPDeviceID}|VID:{dev.VendorID}|PID:{dev.ProductID}|名称:{dev.Name}|描述:{dev.Description}";
				});
				UartList.ItemsSource = DeviceStr;
				UartList.SelectedIndex = UartList.Items.Count - 1;


			};
			OpenUart.Click += delegate
			{
				//如果串口已经是打开状态，则此按钮关闭串口
				if (SerialPortBase.IsOpen)
				{
					SerialPortBase.Close();
				}
				else
				{
					try
					{
						SerialPortBase.PortName = UartList.Text;    // 串口名称
						SerialPortBase.BaudRate = 115200;           // 波特率
						SerialPortBase.DataBits = 8;                // 数据位
						SerialPortBase.StopBits = StopBits.One;     // 停止位
						SerialPortBase.Parity = Parity.None;        // 奇偶校验
						SerialPortBase.WriteBufferSize = 1024 * 1024 * 1;   /* 输出缓冲区的大小为 = 1MB */
						SerialPortBase.ReadBufferSize = 1024 * 1024 * 2;    /* 输入缓冲区的大小为 = 2MB */

						SerialPortBase.ReadTimeout = 5000;        // 超时ms
						SerialPortBase.WriteTimeout = 1000;     //
						SerialPortBase.Open();

						DebugPrint($"<{SerialPortBase.PortName}> {(SerialPortBase.IsOpen ? "打开成功" : "打开失败")}");
						DebugPrint($"<{SerialPortBase.PortName}> 波特率: {SerialPortBase.BaudRate} | 数据位: {SerialPortBase.DataBits} | 停止位: {SerialPortBase.StopBits} | 校验位: {SerialPortBase.Parity}");
						DebugPrint($"<{SerialPortBase.PortName}> 传输协议: {SerialPortBase.Handshake}");
						DebugPrint($"<{SerialPortBase.PortName}> 中断模式: {SerialPortBase.BreakState}");
						DebugPrint($"<{SerialPortBase.PortName}> 可以发送: {SerialPortBase.CtsHolding}");
						DebugPrint($"<{SerialPortBase.PortName}> 忽略Null: {SerialPortBase.DiscardNull}");
						DebugPrint($"<{SerialPortBase.PortName}> 输入缓冲区: {SerialPortBase.ReadBufferSize} | 输出缓冲区: {SerialPortBase.WriteBufferSize}");
						DebugPrint($"<{SerialPortBase.PortName}> ReadTimeOut:{SerialPortBase.ReadTimeout} | WriteTimeOut:{SerialPortBase.WriteTimeout}");
						DebugPrint($"<{SerialPortBase.PortName}> DSR:{SerialPortBase.DsrHolding} DTR:{SerialPortBase.DtrEnable}");
						DebugPrint($"<{SerialPortBase.PortName}> RTS:{SerialPortBase.RtsEnable}");
					}
					catch (Exception ex)
					{
						DebugPrint($"<打开串口失败> {ex.Message}");
					}

				}

				//按钮显示文字转变
				OpenUart.Content = SerialPortBase.IsOpen ? "关闭串口" : "打开串口";
				//发送按钮功能使能
				SendUart.IsEnabled = SerialPortBase.IsOpen;
			};
			SendUart.Click += delegate
			{
				if (SendBox.Text.Length >= 2)
				{
					for (int i = 0; i < SendBox.LineCount; ++i)
					{
						try
						{
							byte[] SendCmd = SendBox.GetLineText(i).Trim().Split(' ').Select(s => Convert.ToByte(s, 16)).ToArray();
							int sendLength = SerialPortBase.BytesToWrite;

							DebugPrint($"<发送数据> {string.Join(" ", SendCmd.Select(d => d.ToString("X2")))}");
							SerialPortBase.Encoding = Encoding.ASCII;
							//comm.Write(SendCmd, 0, sendLength);
							SerialPortBase.BaseStream.Write(SendCmd, 0, sendLength);
							SerialPortBase.BaseStream.BeginWrite(SendCmd, 0, sendLength, (result) =>
							{
								if (result.IsCompleted)
								{
									DebugPrint($"<数据发送成功>");
								}
							}, null);

						}
						catch (Exception e)
						{
							DebugPrint($"<串口发送失败> {e.Message}");
							return;
						}
					}
				}

			};
		}

		/// <summary>
		/// 遍历HID设备列表
		/// </summary>
		public void GetDeviceList()
		{
			// 获取HID设备列表
			List<string> deviceList = HidAPI.GetHidDeviceList();
			// 对列表排序
			deviceList.Sort();
			// 对列表反序
			deviceList.Reverse();
			// 筛选符合要求的设备
			deviceList = deviceList.Select(s => s.ToUpper()).ToList();
			//deviceList = deviceList.Select(s => s.ToUpper()).Where(s => s.Contains("VID_001C")).ToList();


			//ManagementObjectCollection collection;
			//using (var searcher = new ManagementObjectSearcher(@"Select * From Win32_USBHub")) collection = searcher.Get();
			//foreach (var device in collection)
			//{
			//	deviceList.Add(device.GetPropertyValue("DeviceID").ToString());
			//}

			if (deviceList.Count > 0)
			{
				DeviceList.ItemsSource = deviceList;    // 添加设备列表到下拉框
				DeviceList.SelectedIndex = 0;           // 选中第一个设备
				LinkRead.IsEnabled = true;
			}
			else
			{
				DeviceList.ItemsSource = null;
				LinkRead.IsEnabled = false;
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
				DebugPrint($"[连接设备失败] {devicePath.ToUpper()}");
				DebugPrint($"[GetLastError()={WinAPI.GetLastError()}] {WinAPI.ErrorCode(WinAPI.GetLastError())}");
				return;
			}
			// 获取设备属性 (若不需要可删除)
			HidAPI.HidD_GetAttributes(devHandle, out DeviceAttr);
			// 找到对应的HID设备信息
			HidAPI.HidD_GetPreparsedData(devHandle, out IntPtr preparseData);
			HidAPI.HidP_GetCaps(preparseData, out DeviceCaps);
			HidAPI.HidD_FreePreparsedData(preparseData);

			// 创建设备读写通道
			HidDevice = new FileStream(new SafeFileHandle(devHandle, false), FileAccess.ReadWrite, DeviceCaps.InputReportByteLength, true);
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
			byte[] inputBuffer = new byte[DeviceCaps.InputReportByteLength];
			HidDevice.BeginRead(inputBuffer, 0, DeviceCaps.InputReportByteLength, (iResult) =>
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
			byte[] dataBytes = Enumerable.Repeat((byte)0x00, DeviceCaps.OutputReportByteLength).ToArray();
			//dataBytes[0] = 0;                                                           // 设置 ReportID=0
			sendBytes.CopyTo(dataBytes, 1);                                             // 准备待发送数据
			try
			{
				HidDevice.Write(dataBytes, 0, DeviceCaps.OutputReportByteLength);       // 发送数据到设备
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
				if (Firmware.Count > 0)                                                 // 固件下载状态判断
				{
					byte[] FirmwareEnd = Firmware.Last();
					if (FirmwareEnd[0] == readBytes[0] && FirmwareEnd[2] == readBytes[2] && FirmwareEnd[3] == readBytes[3])
					{
						Firmware.Clear();
						FirmwareCMD(readBytes[0], (byte)0);
					}
					else
					{
						FirmwareCMD(readBytes[0], SendBufferQueue.Count > 0 ? SendBufferQueue.Peek().First() : (byte)0);
					}
				}
				else if (ImageArr.Count > 0)
				{
				}
				else
				{
					DebugPrint(string.Join(" ", readBytes.Select(d => $"{d:X2}")));     // 打印收到的数据,调试使用
				}
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
					Firmware.Clear();
					ImageArr.Clear();
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
		/// 固件下载结果反馈
		/// </summary>
		/// <param name="LastCmd">上一次发送的命令</param>
		/// <param name="NextCmd">下一次发送的命令</param>
		private void FirmwareCMD(byte LastCmd, byte NextCmd)
		{
			if (LastCmd == 0x81 && NextCmd == 0x82)
			{
				DebugPrint("[FirmwareCMD] Flash擦除完成!");
			}
			else if (LastCmd == 0x82 && NextCmd == 0x83)
			{
				DebugPrint("[FirmwareCMD] Flash写入完成!");
			}
			else if (LastCmd == 0x83 && NextCmd == 0x00)
			{
				DebugPrint("[FirmwareCMD] Flash校验完成!");
			}
			else if (LastCmd == 0x82 && NextCmd == 0x00)
			{
				DebugPrint("[FirmwareCMD] Bootloader写入完成!");
			}
		}
		/// <summary>
		/// 设备已连接事件
		/// </summary>
		private void OnDeviceConnected()
		{
			this.Dispatcher.Invoke((Action)delegate
			{
				SendData.IsEnabled = true;
				LoadData.IsEnabled = true;
				LinkRead.Content = "断开设备";
				ReadBox.Clear();
				DebugPrint($"设备已连接-> VID:{DeviceAttr.VID:X4} PID:{DeviceAttr.PID:X4} REV:{DeviceAttr.VER:X4} ReportIO:{DeviceCaps.InputReportByteLength}/{DeviceCaps.OutputReportByteLength}");
			});
		}
		/// <summary>
		/// 设备已断开事件
		/// </summary>
		private void OnDeviceRemoved()
		{
			this.Dispatcher.Invoke((Action)delegate
			{
				SendData.IsEnabled = false;
				LoadData.IsEnabled = false;
				LinkRead.Content = "连接设备";
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

		/// <summary>
		///  AES 解密
		/// </summary>
		/// <param name="str">密文（待解密）</param>
		/// <param name="key">密钥</param>
		/// <returns></returns>
		public string AesDecrypt(string str, string key = "6211723196ea6885")
		{
			if (string.IsNullOrEmpty(str)) return null;
			byte[] toEncryptArray = Convert.FromBase64String(str);

			RijndaelManaged rm = new RijndaelManaged
			{
				Key = Encoding.UTF8.GetBytes(key),
				Mode = CipherMode.ECB,
				Padding = PaddingMode.PKCS7
			};

			ICryptoTransform cTransform = rm.CreateDecryptor();
			byte[] resultArray = cTransform.TransformFinalBlock(toEncryptArray, 0, toEncryptArray.Length);

			return Encoding.UTF8.GetString(resultArray);
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
			// 判断文件类型,若为apk则需要解码
			if (FilePath.ToLower().EndsWith(".apk"))
			{
				// 数据内容解码
				for (int i = 0, len = HexLines.Count; i < len; ++i)
				{
					HexLines[i] = AesDecrypt(HexLines[i]);
				}
				// 获取文件头信息
				string[] header = HexLines[0].Split('|');
				// 校验数据行数是否正确
				if (Convert.ToInt32(header[1]) != (HexLines.Count - 1))
				{
					DebugPrint($"文件已损坏: {FilePath.Split('\\').Last()}");
					return BinLines;
				}
				// 文件头数据不再使用,因此删除
				HexLines.RemoveAt(0);
				DebugPrint($"已载入文件: {header[0]}");
			}
			else
			{
				DebugPrint($"已载入文件: {FilePath.Split('\\').Last()}");
			}
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
			List<byte> dataTop = TempList.First();      /* 获取数据开始行,用于获取擦除Flash开始地址 */
			List<byte> dataEnd = TempList.Last();       /* 获取数据结束行,用于获取擦除Flash结束地址 */
			// 添加Flash擦除命令
			BinLines.Add(new byte[] { 0x81, 0x20, dataTop[2], dataTop[3], dataEnd[2], dataEnd[3] });
			// 判断数据内容是否为BootLoader
			if (startAddr != 0x0000)
			{
				// 为了兼容旧版固件,需要添加额外的擦除命令
				BinLines.Add(new byte[] { 0x81, 0x20, dataTop[2], (byte)(dataTop[3] + 0x04), 0x00, 0x00 });
			}
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
				BinLines.Add(new byte[] { 0x1B, 0x20 });
			}
			return BinLines;
		}
	}
}
