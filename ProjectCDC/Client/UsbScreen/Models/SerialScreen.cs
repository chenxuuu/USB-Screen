using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Management;
using System.Runtime.CompilerServices;
using System.Text;
using System.Windows;
using System.Windows.Media.Imaging;

namespace UsbScreen.Models
{
	[PropertyChanged.AddINotifyPropertyChangedInterface]
	class SerialScreen
	{
		/// <summary>
		/// 屏幕分辨率宽度
		/// </summary>
		private const int width = 240;
		/// <summary>
		/// 屏幕分辨率高度
		/// </summary>
		private const int height = 240;

		/// <summary>
		/// 串口对象
		/// </summary>
		private SerialPort sp { get; set; } = new SerialPort();

		/// <summary>
		/// 预览图
		/// </summary>
		public Bitmap Priview { get; set; }
		private Graphics PriviewG;

		private bool _connected = false;
		/// <summary>
		/// 串口连接状态
		/// </summary>
		public bool IsConnected
		{
			get
			{
				IsConnected = sp.IsOpen;
				return _connected;
			}
			set => _connected = value;
		}

		/// <summary>
		/// 串口名
		/// </summary>
		public string Name
		{
			get => sp.PortName;
			set => sp.PortName = value;
		}

		public event EventHandler RefreshPriviewEvent;
		private void RefreshPriview()
		{
			try
			{
				RefreshPriviewEvent?.Invoke(null, EventArgs.Empty);
			}
			catch (Exception e)
			{
				Plugin.ErrorLogger(e.ToString());
			}
		}

		/// <summary>
		/// 初始化
		/// </summary>
		/// <param name="name">串口名</param>
		public SerialScreen(string name = "")
		{
			sp.PinChanged += (sender, e) => { _ = IsConnected; };
			if (name.Length > 0)
				Connect(name);
			Priview = new Bitmap(width, height);
			PriviewG = Graphics.FromImage(Priview);
			PriviewG.FillRectangle(Brushes.Black, 0, 0, width, height);
			RefreshPriview();
		}

		/// <summary>
		/// 列出所有设备
		/// </summary>
		/// <param name="isp">是否为isp设备</param>
		/// <returns>设备COM名列表</returns>
		public static string[] GetDeviceList(bool isp = false)
		{
			string[] devices = new string[0];
			using (ManagementObjectSearcher searcher = new ManagementObjectSearcher("select * from Win32_SerialPort"))
			{
				try
				{
					devices = (from ManagementObject entity in searcher.Get()
							   where ((string)entity["PNPDeviceID"]).EndsWith("ST1E6DIPSF0F0SPI3") ||
							   isp && ((string)entity["PNPDeviceID"]).EndsWith("ST1E6DIPSF0F0SPI3ISP")
							   select entity["DeviceID"] as string).ToArray();
				}
				catch (Exception e)
				{
					Debug.WriteLine($"[枚举串口出错] {e.Message}");
				}
			}
			return devices;
		}

		/// <summary>
		/// 连接设备
		/// </summary>
		/// <param name="name">串口名，可为空</param>
		/// <returns>是否成功连接</returns>
		public bool Connect(string name = null)
		{
			if (IsConnected)//已经连上了
				return true;
			if (name != null)//配置串口名
				Name = name;
			if (Name.Length == 0)
				return false;
			try
			{
				sp.Open();
				_ = IsConnected;
				return true;
			}
			catch
			{
				_ = IsConnected;
				return false;
			}
		}

		/// <summary>
		/// 断开连接
		/// </summary>
		/// <returns>是否成功断开</returns>
		public bool Disconnect()
		{
			if (!IsConnected)//已经连上了
				return true;
			try
			{
				sp.Close();
				_ = IsConnected;
				return true;
			}
			catch
			{
				_ = IsConnected;
				return false;
			}
		}

		/// <summary>
		/// 发送串口数据
		/// </summary>
		/// <param name="d"></param>
		/// <param name="start"></param>
		/// <param name="len"></param>
		/// <returns></returns>
		private bool SendBytes(byte[] d, int start, int len)
		{
			if (!IsConnected)
			{
				Debug.WriteLine("not connected, try reconnect");
				if (sp.PortName.Length > 0 && !Connect())
					return false;
			}
			try
			{
				sp.Write(d, start, len);
				_ = IsConnected;
				return true;
			}
			catch
			{
				_ = IsConnected;
				return false;
			}
		}

		/// <summary>
		/// 显示图片
		/// </summary>
		/// <param name="pic">图片数据</param>
		/// <param name="x">起始x</param>
		/// <param name="y">起始y</param>
		/// <param name="tc">是否黑白图（黑白图模式仅支持8倍数长宽，否则默认x会被裁减）</param>
		/// <returns>是否成功执行</returns>
		public bool Show(Bitmap pic, int x = 0, int y = 0, bool tc = false)
		{
			if (x >= width || y >= height) return false;    // 超过屏幕高度宽度
															// 停止的位置，防止超出画面
			int ex = (pic.Width + x > width ? width : pic.Width + x) - 1;
			int ey = (pic.Height + y > height ? height : pic.Height + y) - 1;
			// 处理后的长宽
			int tx = ex - x + 1, ty = ey - y + 1;
			// 更新预览图
			PriviewG.DrawImage(pic, x, y);
			RefreshPriview();
			// 提取图片像素颜色数据
			List<Color> ColorList = new List<Color>();
			for (int h = 0; h < tx; ++h)
			{
				for (int v = 0; v < tx; ++v)
				{
					ColorList.Add(pic.GetPixel(v, h));
				}
			}
			// 根据图片颜色预处理数据
			List<byte> dataList = new List<byte>();
			if (!tc)	// 彩色图
			{
				ColorList.ForEach(c =>
				{
					dataList.Add((byte)((c.R & 0xF8) | (c.G >> 5)));
					dataList.Add((byte)((c.G & 0xE0) | (c.B >> 3)));
				});
			}
			else		// 黑白图
			{
				// 像素少于8个,补齐到8个
				if (ColorList.Count < 8)
				{
					do ColorList.AddRange(ColorList);
					while (ColorList.Count < 8);
					ColorList = ColorList.Take(8).ToList();
				}
				// 像素不是8的倍数,补齐数量
				if ((ColorList.Count % 8) != 0)
				{
					ColorList.AddRange(ColorList.Take(8 - (ColorList.Count % 8)));
				}
				// 提取数据
				for (int n = 0; n < ColorList.Count;)
				{
					byte pixel8 = 0x00;
					for (byte i = 0x80; i != 0; i >>= 1)
					{
						Color c = ColorList[n];
						if ((0.3 * c.R + 0.59 * c.G + 0.11 * c.B) > 128) pixel8 |= i;   // 黑色
						++n;
					}
					dataList.Add(pixel8);
				}
			}
			// 设置图片显示范围命令 (以及添加黑白标志)
			List<byte> sendCMD = new List<byte>() { (byte)(tc ? 080 : 0x00), (byte)x, (byte)ex, (byte)y, (byte)ey };
			int sortCount = dataList.Count % 64;                                    // 获取零头数据长度
			if (sortCount <= 58)
			{
				sendCMD[0] |= (byte)sortCount;                                      // 有效数据长度
				sendCMD.AddRange(dataList.Take(sortCount));                         // 将数据拼接到命令后面
			}
			if (!SendBytes(sendCMD.ToArray(), 0, sendCMD.Count)) return false;      // 发送带命令参数的零头数据
			if ((58 < sortCount) && (sortCount <= 62))
			{
				sendCMD.Clear();
				sendCMD.Add((byte)(sortCount | 0x40));
				sendCMD.AddRange(dataList.Take(sortCount));
				if (!SendBytes(sendCMD.ToArray(), 0, sendCMD.Count)) return false;  // 发送无命令参数的零头数据
			}
			dataList = dataList.Skip(sortCount).ToList();
			if (dataList.Count != 0)
			{
				return SendBytes(dataList.ToArray(), 0, dataList.Count);            // 剩下的所有数据
			}
			return true;
		}
	}
}
