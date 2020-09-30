using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
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
using USB_Screen.Dev;

namespace USB_Screen
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        Dev.Hid hid = new Dev.Hid();

        public MainWindow()
        {
            InitializeComponent();
            // 当窗口后台运行时半透明显示
            this.Deactivated += delegate { this.Opacity = 0.8; };
            // 当窗口前台运行时取消半透明
            this.Activated += delegate { this.Opacity = 1; };
            // 注册窗口移动事件
            TitleBar.MouseMove += (object sender, MouseEventArgs e) => { if (e.LeftButton == MouseButtonState.Pressed) this.DragMove(); };
            // 注册窗口关闭按钮
            AppExit.Click += delegate { this.Close(); };



            TestButton.Click += Button_Click;


            progressBar.DataContext = hid;
        }

        object sendLock = new object();
        private void Button_Click(object sender, RoutedEventArgs e)
        {
            lock(sendLock)
            {
                Task.Run(() =>
                {
                    byte x = 240;
                    byte y = 240;

                    List<byte> data = new List<byte>();

                    Screen.AddData(data, new List<byte>() { 0x2a }, true);
                    Screen.AddData(data, new List<byte>() { 0, 0 });
                    Screen.AddData(data, new List<byte>() { 0, (byte)(x - 1) });
                    Screen.AddData(data, new List<byte>() { 0x2b }, true);
                    Screen.AddData(data, new List<byte>() { 0, 0 });
                    Screen.AddData(data, new List<byte>() { 0, (byte)(y - 1) });
                    Screen.AddData(data, new List<byte>() { 0x2c }, true);

                    List<byte> sd = new List<byte>();
                    using (Bitmap img = new Bitmap(@"1.png"))
                    {
                        for (int i = 0; i < x; i++)
                            for (int j = 0; j < y; j++)
                            {
                                var color = img.GetPixel(y - i - 1, j);
                                //rrrr rggg gggb bbbb
                                var rgb565 = color.R / 8 * 2048 + color.G / 4 * 32 + color.B / 8;
                                sd.Add((byte)(rgb565 / 256));
                                sd.Add((byte)(rgb565 % 256));
                            }
                    }

                    //MessageBox.Show(sd.Count.ToString());
                    Screen.AddData(data, sd);

                    var r = hid.SendBytes(data.ToArray());
                });
            }

        }

    }
}
