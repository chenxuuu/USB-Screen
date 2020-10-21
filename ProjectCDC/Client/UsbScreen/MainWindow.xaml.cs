using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Timers;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using UsbScreen.Models;
using Point = System.Drawing.Point;
using System.Windows.Interop;
using System.Threading;
using System.IO;
using Newtonsoft.Json;

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
        }

        [System.Runtime.InteropServices.DllImport("gdi32.dll")]
        private static extern bool DeleteObject(IntPtr hObject);
        private void ShowPicture(Bitmap bitmap)
        {
            IntPtr hBitmap = bitmap.GetHbitmap();
            ImageSource wpfBitmap = System.Windows.Interop.Imaging.CreateBitmapSourceFromHBitmap(
                hBitmap,
                IntPtr.Zero,
                Int32Rect.Empty,
                BitmapSizeOptions.FromEmptyOptions());
            if (!DeleteObject(hBitmap))
            {
                throw new System.ComponentModel.Win32Exception();
            }
            PriviewImage.Source = wpfBitmap;
        }

        private void MainWindow_OnLoaded(object sender, RoutedEventArgs e)
        {
            Directory.CreateDirectory("plugin");
            //加载配置文件
            if (File.Exists("settings.json"))
                setting = JsonConvert.DeserializeObject<Setting>(File.ReadAllText("settings.json"));
            else
                setting = new Setting();
            s.Name = setting.LastPort;
            plugin.screen = s;//屏幕对象传过去给它用
            plugin.RefreshPriviewEvent += (ss,ee) => ShowPicture(ss as Bitmap);
            ShowPicture(s.Priview);
            ConnectButton.DataContext = s;
            EnablePluginButton.DataContext = plugin;
            RefreshPluginButton.DataContext = plugin;
            RefreshPortList();
            RefreshPluginList();
            HwndSource source = PresentationSource.FromVisual(this) as HwndSource;
            source.AddHook(WndProc);  // 绑定事件监听,用于监听HID设备插拔
        }

        private IntPtr WndProc(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
        {
            if (msg == 0x219)// 监听USB设备插拔事件
            {
                Task.Run(() => 
                { 
                    for(int i=1;i<10;i++)
                    {
                        if (RefreshPortList())
                            break;
                        else
                            Task.Delay(100).Wait();
                    }
                });
                handled = true;
            }
            return IntPtr.Zero;
        }

        /// <summary>
        /// 刷新设备列表，并在断开恢复后重连
        /// </summary>
        /// <returns>串口列表是否成功获取</returns>
        private bool RefreshPortList()
        {
            _ = s.IsConnected;
            var list = SerialScreen.GetDeviceList();
            if (list == null)//列表获取失败了
                return false;
            Dispatcher.Invoke(new Action(delegate
            {
                PortComboBox.Items.Clear();
                foreach(var i in list)
                {
                    PortComboBox.Items.Add(i);
                    if (i == s.Name)
                    {
                        PortComboBox.Text = i;
                        if (!s.IsConnected && _lastStatus)//如果上次状态是已连接
                            Task.Run(() => s.Connect());//单独开个线程连防止连接时卡死界面
                    }
                }
                //如果没有项目，自动显示第一个端口
                if (PortComboBox.Text.Length == 0 && list.Length > 0)
                    PortComboBox.Text = list[0];
            }));
            return true;
        }

        /// <summary>
        /// 刷新插件列表
        /// </summary>
        private void RefreshPluginList()
        {
            var list = Plugin.GetPluginList();
            Dispatcher.Invoke(new Action(delegate
            {
                PluginComboBox.Items.Clear();
                foreach (var i in list)
                {
                    PluginComboBox.Items.Add(i);
                    if (i == setting.LastPlugin)
                    {
                        plugin.EnablePlugin(setting.LastPlugin);
                    }
                }
                //如果没有项目，自动显示第一个端口
                if (PluginComboBox.Text.Length == 0 && list.Length > 0)
                    PluginComboBox.Text = list[0];
            }));
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
            if(((string)PortComboBox.SelectedItem).Length > 0)
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
            if (plugin.IsEnable)
                plugin.Disable();
            else
                plugin.EnablePlugin((string)PluginComboBox.SelectedItem);
            setting.LastPlugin = plugin.IsEnable ? (string)PluginComboBox.SelectedItem : "";
        }
    }
}
