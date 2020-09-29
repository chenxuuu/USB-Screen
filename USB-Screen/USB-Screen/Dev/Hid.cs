using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HidLibrary;

namespace USB_Screen.Dev
{
    class Hid
    {
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
        public static bool SendBytes(byte[] data)
        {
            List<byte> d = new List<byte>(data);
            while (d.Count > 64)
            {
                Debug.WriteLine($"data to send length {d.Count}");
                var temp = d.GetRange(0, 64);
                d.RemoveRange(0,64);
                var result = Send(temp.ToArray());
                Debug.WriteLine($"data received length {result.Length}");
                if (result.Length != 64)
                    return false;
            }

            if (d.Count > 0)
            {
                var temp = d.GetRange(0, d.Count);
                for (int i= d.Count; i<64;i++)
                    temp.Add(0);
                var result = Send(temp.ToArray());
                if (result.Length != 64)
                    return false;
            }

            return true;
        }


        /// <summary>
        /// 发一包数据
        /// </summary>
        /// <param name="data">数据</param>
        /// <returns>返回的数据</returns>
        public static byte[] Send(byte [] data)
        {
            Debug.WriteLine($"pack data {ByteToHexString(data)}");
            var temp = new byte[data.Length + 1];
            temp[0] = 0;
            for (int i = 1; i < data.Length + 1; i++)
                temp[i] = data[i-1];
            try
            {
                HidDevice[] HidDeviceList;
                HidDevice HidDevice;

                // Enumerate the devices with the Vendor Id
                HidDeviceList = HidDevices.Enumerate(0x2333, 0x2434).ToArray();

                if (HidDeviceList.Length > 0)
                {
                    // Grab the first device
                    HidDevice = HidDeviceList[0];

                    // Check if connected...
                    //Debug.WriteLine("Connected: " + HidDevice.IsConnected.ToString());

                    HidDevice.Write(temp);

                    // Blocking read of report
                    HidDeviceData InData;

                    InData = HidDevice.Read();

                    var id = InData.Data;

                    var ti = new byte[id.Length - 1];
                    for (int i = 1; i < id.Length - 1; i++)
                        ti[i] = id[i + 1];
                    return ti;
                }
                else
                {
                    return new byte[] { };
                }
            }
            catch(Exception e)
            {
                Debug.WriteLine($"hid send error\r\n{e}");
                return new byte[] { };
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
