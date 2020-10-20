using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;

namespace UsbScreen
{
    class Convert
    {
    }

    /// <summary>
    /// 是否显示
    /// </summary>
    [ValueConversion(typeof(bool), typeof(string))]
    public class ConnectOrDisconnect : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if ((bool) value)
                return "断开";
            else
                return "连接";
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
