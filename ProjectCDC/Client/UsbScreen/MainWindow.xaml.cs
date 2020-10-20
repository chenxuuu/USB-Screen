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

        SerialScreen s = new SerialScreen();
        public MainWindow()
        {
            InitializeComponent();
            ShowPicture(new Bitmap("1.png"));
        }

        private void testButton_Click(object sender, RoutedEventArgs e)
        {
            if (!s.Connect(SerialScreen.GetDeviceList()[0]))
                MessageBox.Show("connect error");
            Stopwatch sw = new Stopwatch();
            Bitmap CatchBmp = new Bitmap(240, 240);
            Graphics g = Graphics.FromImage(CatchBmp);

            while (true)
            {
                g.CopyFromScreen(new Point(0, 0), new Point(0, 0), new System.Drawing.Size(240, 240));
                sw.Start();
                if (!s.Show(CatchBmp, 0, 0))
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
            Bitmap p2 = new Bitmap("2.png");

            bool last = false;
            while (true)
            {
                sw.Start();
                if (!s.Show(last ? p1 : p2, 100, 100,false))
                {
                    MessageBox.Show("show error");
                    break;
                }
                ShowPicture(s.Priview);
                break;
                sw.Stop();
                Debug.Print($"{DateTime.Now:HH:mm:ss.fff} [传输完成] 耗时:{sw.ElapsedMilliseconds}ms");
                sw.Restart();
                last = !last;
            }
        }
    }
}
