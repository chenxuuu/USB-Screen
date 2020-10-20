using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
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

namespace UsbScreen
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        readonly SerialScreen s = new SerialScreen();
        public MainWindow()
        {
            InitializeComponent();
        }

        private void testButton_Click(object sender, RoutedEventArgs e)
        {
            if (!s.Connect(SerialScreen.GetDeviceList()[0]))
                MessageBox.Show("connect error");
            Stopwatch sw = new Stopwatch();
            Bitmap catchBmp = new Bitmap(240, 240);
            Graphics g = Graphics.FromImage(catchBmp);

            while (true)
            {
                g.CopyFromScreen(new Point(0, 0), new Point(0, 0), new System.Drawing.Size(240, 240));
                sw.Start();
                if (!s.Show(catchBmp, 0, 0))
                {
                    MessageBox.Show("show error");
                    break;
                }
                //break;
                sw.Stop();
                Debug.Print($"{DateTime.Now:HH:mm:ss.fff} [传输完成] 耗时:{sw.ElapsedMilliseconds}ms");
                sw.Restart();
            }
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

        private void testpicButton_Click(object sender, RoutedEventArgs e)
        {
            if (!s.Connect(SerialScreen.GetDeviceList()[0]))
                MessageBox.Show("connect error");
            Stopwatch sw = new Stopwatch();
            Bitmap p1 = new Bitmap("1.png");

            if (!s.Show(p1, 100, 100, false))
            {
                MessageBox.Show("show error");
                return;
            }
            ShowPicture(s.Priview);
        }

        private void MainWindow_OnLoaded(object sender, RoutedEventArgs e)
        {
            ShowPicture(s.Priview);
            PortComboBox.ItemsSource = SerialScreen.GetDeviceList();
            ConnectButton.DataContext = s;
        }

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
                return;
            }
            if (PortComboBox.SelectedItem == null)
                return;
            if(((string)PortComboBox.SelectedItem).Length > 0)
                s.Connect((string)PortComboBox.SelectedItem);
        }
    }
}
