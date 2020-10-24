using System;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media.Imaging;
using Newtonsoft.Json;
using UsbScreen.Models;

namespace UsbScreen
{
	/// <summary>
	/// MainWindow.xaml 的交互逻辑
	/// </summary>
	public partial class MainWindow : Window
	{
		SerialScreen s = new SerialScreen();
		Plugin plugin = new Plugin();
		Setting setting;
		public MainWindow()
		{
			InitializeComponent();
			// 注册窗口移动事件
			TitleBar.MouseMove += (sender, e) => { if (e.LeftButton == MouseButtonState.Pressed) this.DragMove(); };
			// 注册窗口最小化按钮
			MinSize.Click += delegate { this.WindowState = WindowState.Minimized; };
			// 注册窗口关闭按钮
			AppExit.Click += delegate { this.Close(); };
		}

		private void ShowPicture(Bitmap bitmap)
		{
			MemoryStream ms = new MemoryStream();
			bitmap.Save(ms, System.Drawing.Imaging.ImageFormat.Bmp);
			byte[] bytes = ms.GetBuffer();  //byte[]   bytes=   ms.ToArray(); 这两句都可以
			ms.Close();
			//Convert it to BitmapImage
			BitmapImage image = new BitmapImage();
			image.BeginInit();
			image.StreamSource = new MemoryStream(bytes);
			image.EndInit();
			PriviewImage.Source = image;
		}

		private void MainWindow_OnLoaded(object sender, RoutedEventArgs e)
		{
			Directory.CreateDirectory("plugin");
			//加载配置文件
			if (File.Exists("settings.json"))
				setting = JsonConvert.DeserializeObject<Setting>(File.ReadAllText("settings.json"));
			else
				setting = new Setting();
			s.RefreshPriviewEvent += delegate { Dispatcher.Invoke(delegate { ShowPicture(s.Priview); }); };
			if (setting.LastPort.Length > 0)
				s.Name = setting.LastPort;
			plugin.screen = s;//屏幕对象传过去给它用
			ShowPicture(s.Priview);
			ConnectButton.DataContext = s;
			EnablePluginButton.DataContext = plugin;
			RefreshPluginButton.DataContext = plugin;
			RefreshPortList();
			RefreshPluginList();
			HwndSource source = PresentationSource.FromVisual(this) as HwndSource;
			source?.AddHook(WndProc);  // 绑定事件监听,用于监听HID设备插拔
		}

		private static int UsbPluginDeley = 0;
		private IntPtr WndProc(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
		{
			if (msg == 0x219)// 监听USB设备插拔消息
			{
				if (UsbPluginDeley == 0)
				{
					++UsbPluginDeley;   // Task启动需要准备时间,这里提前对公共变量加一
					Task.Run(() =>
					{
						do Task.Delay(100).Wait();
						while (++UsbPluginDeley < 10);
						UsbPluginDeley = 0;
						RefreshPortList();
						Debug.Print($"[USB拔插事件] {DateTime.Now:HH:mm:ss.fff}");
					});
				}
				else UsbPluginDeley = 1;
				handled = true;
			}
			else if (msg == 0x0320)// 监听系统主题颜色变更消息
			{
				TitleBar.Background = SystemParameters.WindowGlassBrush;
			}
			return IntPtr.Zero;
		}

		/// <summary>
		/// 刷新设备列表，并在断开恢复后重连
		/// </summary>
		/// <returns>串口列表是否成功获取</returns>
		private bool RefreshPortList()
		{
			bool result = true;
			Dispatcher.Invoke(delegate
			{
				PortComboBox.ItemsSource = SerialScreen.GetDeviceList();
				if (PortComboBox.Items.Count == 0)
				{
					result = false;
				}
				else if (PortComboBox.Items.Contains(s.Name))
				{
					PortComboBox.SelectedItem = s.Name;
					if (!s.IsConnected && _lastStatus)//如果上次状态是已连接
						Task.Run(() => s.Connect());//单独开个线程连防止连接时卡死界面
				}
				else PortComboBox.SelectedIndex = 0;
			});
			_ = s.IsConnected;
			return result;



			//	string[] list = SerialScreen.GetDeviceList();
			//if (list == null) return false;//列表获取失败了
			//Dispatcher.Invoke(delegate
			//{
			//	PortComboBox.ItemsSource = list;
			//	if (PortComboBox.Items.Count == 0) return;
			//	if (PortComboBox.Items.Contains(s.Name))
			//	{
			//		PortComboBox.SelectedItem = s.Name;
			//		if (!s.IsConnected && _lastStatus)//如果上次状态是已连接
			//			Task.Run(() => s.Connect());//单独开个线程连防止连接时卡死界面
			//	}
			//	else PortComboBox.SelectedIndex = 0;

			//	PortComboBox.Items.Clear();
			//	foreach (var i in list)
			//	{
			//		PortComboBox.Items.Add(i);
			//		if (i == s.Name)
			//		{
			//			PortComboBox.Text = i;
			//			if (!s.IsConnected && _lastStatus)//如果上次状态是已连接
			//				Task.Run(() => s.Connect());//单独开个线程连防止连接时卡死界面
			//		}
			//	}
			//	//如果没有项目，自动显示第一个端口
			//	if (PortComboBox.Text.Length == 0 && list.Length > 0)
			//		PortComboBox.Text = list[0];
			//});
			//return true;
		}

		/// <summary>
		/// 刷新插件列表
		/// </summary>
		private void RefreshPluginList()
		{
			PluginComboBox.ItemsSource = Plugin.GetPluginList();
			if (PluginComboBox.Items.Count == 0) return;
			if (PluginComboBox.Items.Contains(setting.LastPlugin))
			{
				PluginComboBox.SelectedItem = setting.LastPlugin;
				plugin.EnablePlugin(setting.LastPlugin);
			}
			else PluginComboBox.SelectedIndex = 0;
		}

		/// <summary>
		/// 上次的连接状态
		/// </summary>
		private bool _lastStatus = true;
		/// <summary>
		/// 连接、断开设备
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void ConnectButton_OnClick(object sender, RoutedEventArgs e)
		{
			if (s.IsConnected)
			{
				s.Disconnect();
				setting.LastPort = "";
				_lastStatus = false;
				return;
			}
			if (PortComboBox.SelectedItem == null)
				return;
			if (((string)PortComboBox.SelectedItem).Length > 0)
				s.Connect((string)PortComboBox.SelectedItem);
			setting.LastPort = (string)PortComboBox.SelectedItem;//设置里的串口刷新下
			_lastStatus = true;
		}

		private void RefreshPluginButton_Click(object sender, RoutedEventArgs e)
		{
			RefreshPluginList();
		}

		private void OpenPluginFolderButton_Click(object sender, RoutedEventArgs e)
		{
			Process.Start("explorer.exe", "plugin");
		}

		private void CheckUpdateButton_Click(object sender, RoutedEventArgs e)
		{
			MessageBox.Show("todo");
		}

		private void GetPluginButton_Click(object sender, RoutedEventArgs e)
		{
			Process.Start("https://github.com/chenxuuu/USB-Screen");
		}

		private void EnablePluginButton_Click(object sender, RoutedEventArgs e)
		{
			if (plugin.IsEnable) plugin.Disable();
			else plugin.EnablePlugin((string)PluginComboBox.SelectedItem);
			setting.LastPlugin = plugin.IsEnable ? (string)PluginComboBox.SelectedItem : "";
		}
	}
}
