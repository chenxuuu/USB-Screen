using System;
using System.Collections.Generic;
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
        public MainWindow()
        {
            InitializeComponent();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            UInt16 x = 4;
            UInt16 y = 4;

            List<byte> data = new List<byte>();

            Screen.AddData(data, new List<byte>() { 0x2a }, true);
            Screen.AddData(data, new List<byte>() { 0, 0 });
            Screen.AddData(data, new List<byte>() { (byte)((x - 1) >> 8), (byte)((x - 1) % 256) });
            Screen.AddData(data, new List<byte>() { 0x2b }, true);
            Screen.AddData(data, new List<byte>() { 0, 0 });
            Screen.AddData(data, new List<byte>() { (byte)((y - 1) >> 8), (byte)((y - 1) % 256) });
            Screen.AddData(data, new List<byte>() { 0x2c }, true);

            List<byte> sd = new List<byte>();
            for (int i = 0; i < x; i++)
            for (int j = 0; j < y; j++)
            {
                sd.Add(0xff);
                sd.Add(0xff);
            }
            Screen.AddData(data, sd);

            var r = Hid.SendBytes(data.ToArray());
            //MessageBox.Show(r.ToString());
        }
    }
}
