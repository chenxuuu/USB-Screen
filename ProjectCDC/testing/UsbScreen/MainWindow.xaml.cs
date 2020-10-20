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
            testTextBlock.DataContext = s;
            if (!s.Connect(SerialScreen.GetDeviceList()[0]))
                MessageBox.Show("connect error");
        }

        private void testButton_Click(object sender, RoutedEventArgs e)
        {
            Random r = new Random();
            Stopwatch sw = new Stopwatch();
            while (true)
            {
                sw.Start();
                if (!s.Show(new Bitmap(@"1.png"), r.Next(0, 200), r.Next(0, 200)))
                {
                    MessageBox.Show("show error");
                    break;
                }
                break;
                sw.Stop();
                Debug.Print($"{DateTime.Now:HH:mm:ss.fff} [传输完成] 耗时:{sw.ElapsedMilliseconds}ms");
                sw.Restart();
            }
        }
    }
}
