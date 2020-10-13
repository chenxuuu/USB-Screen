using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HidLibrary;

namespace USB_Screen.Dev
{
    [PropertyChanged.AddINotifyPropertyChangedInterface]
    class Hid
    {
        /// <summary>
        /// 发送进度
        /// </summary>
        public int Progress { get; set; } = 0;

        /// <summary>
        /// 是否存在设备，目前仅支持单设备
        /// </summary>
        /// <returns>是否存在</returns>
        public static bool HasDevice() => HidDevices.Enumerate(0x2333, 0x2434).ToArray().Length > 0;

        /// <summary>
        /// 发送多字节数据
        /// </summary>
        /// <param name="data">数据</param>
        /// <returns>是否成功</returns>
        public bool SendBytes(byte[] data)
        {
            Progress = 0;//进度清零


            HidDevice[] HidDeviceList;
            HidDevice HidDevice;

            // Enumerate the devices with the Vendor Id
            HidDeviceList = HidDevices.Enumerate(0x2333, 0x2434).ToArray();
            if (HidDeviceList.Length <= 0)
                return false;

            HidDevice = HidDeviceList[0];
            List<byte> d = new List<byte>(data);
            while (d.Count > 64)
            {
                var temp = d.GetRange(0, 64);
                d.RemoveRange(0,64);
                if (!Send(temp.ToArray(), HidDevice))
                    return false;
                Progress = 100 - (int)(100 * (double)d.Count / data.Length);//更新进度
            }

            if (d.Count > 0)
            {
                var temp = d.GetRange(0, d.Count);
                for (int i= d.Count; i<64;i++)
                    temp.Add(0);
                if (!Send(temp.ToArray(), HidDevice))
                    return false;
                Progress = 100 - (int)(100 * (double)d.Count / data.Length);//更新进度
            }

            Progress = 100;//更新进度
            return true;
        }


        /// <summary>
        /// 发一包数据
        /// </summary>
        /// <param name="data">数据</param>
        /// <returns>是否成功</returns>
        private static bool Send(byte [] data, HidDevice HidDevice)
        {
            var temp = new byte[data.Length + 1];
            temp[0] = 0;
            for (int i = 1; i < data.Length + 1; i++)
                temp[i] = data[i-1];
            try
            {
                HidDevice.Write(temp);
                return true;
            }
            catch(Exception e)
            {
                Debug.WriteLine($"hid send error\r\n{e}");
                return false;
            }
        }

        public static string ByteToHexString(byte[] bytes)
        {
            string str = string.Empty;
            if (bytes != null)
            {
                for (int i = 0; i < bytes.Length; i++)
                {
                    str += bytes[i].ToString("X2");
                }
            }
            return str;
        }
    }
}
