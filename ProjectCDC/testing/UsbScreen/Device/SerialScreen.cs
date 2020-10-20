using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Management;
using System.Runtime.CompilerServices;
using System.Text;
using System.Windows.Media.Imaging;

namespace UsbScreen
{
    [PropertyChanged.AddINotifyPropertyChangedInterface]
    class SerialScreen
    {
        /// <summary>
        /// 屏幕分辨率宽度
        /// </summary>
        private int width = 240;
        /// <summary>
        /// 屏幕分辨率高度
        /// </summary>
        private int height = 240;

        /// <summary>
        /// 串口对象
        /// </summary>
        private SerialPort sp { get; set; } = new SerialPort();

        /// <summary>
        /// 串口连接状态
        /// </summary>
        public bool Connected
        {
            get
            {
                return sp.IsOpen;
            }
            set
            {
                throw new Exception("can not set connection status");
            }
        }

        /// <summary>
        /// 串口名
        /// </summary>
        public string Name
        {
            get
            {
                return sp.PortName;
            }
            set
            {
                sp.PortName = value;
            }
        }

        /// <summary>
        /// 初始化
        /// </summary>
        /// <param name="name">串口名</param>
        public SerialScreen(string name)
        {
            Connect(name);
        }
        public SerialScreen()
        {
            
        }

        /// <summary>
        /// 列出所有设备
        /// </summary>
        /// <param name="isp">是否为isp设备</param>
        /// <returns>设备COM名列表</returns>
        public static string[] GetDeviceList(bool isp = false)
        {
            string[] devices;
            using (ManagementObjectSearcher searcher = new ManagementObjectSearcher("select * from Win32_SerialPort"))
            {
                try
                {
                    devices = (from ManagementObject Entity in searcher.Get()
                               where (Entity["PNPDeviceID"] as string).EndsWith("ST1E6DIPSF0F0SPI3") ||
                               (isp && (Entity["PNPDeviceID"] as string).EndsWith("ST1E6DIPSF0F0SPI3ISP"))
                               select Entity["DeviceID"] as string).ToArray();
                }
                catch (Exception e)
                {
                    Debug.WriteLine($"[枚举串口出错] {e.Message}");
                    devices = new string[0];
                }
            }
            return devices;
        }

        /// <summary>
        /// 连接设备
        /// </summary>
        /// <param name="name">串口名，可为空</param>
        /// <returns>是否成功连接</returns>
        public bool Connect(string name = null)
        {
            if (Connected)//已经连上了
                return true;
            if (name != null)//配置串口名
                Name = name;
            try
            {
                sp.Open();
                return true;
            }
            catch
            {
                return false;
            }
        }

        /// <summary>
        /// 断开连接
        /// </summary>
        /// <returns>是否成功断开</returns>
        public bool Disconnect()
        {
            if (!Connected)//已经连上了
                return true;
            try
            {
                sp.Close();
                return true;
            }
            catch
            {
                return false;
            }
        }

        /// <summary>
        /// 发送串口数据
        /// </summary>
        /// <param name="d"></param>
        /// <param name="start"></param>
        /// <param name="len"></param>
        /// <returns></returns>
        private bool sendBytes(byte[] d, int start, int len)
        {
            if (!Connected)
            {
                Debug.WriteLine("not connected");
                return false;
            }
            try
            {
                Debug.WriteLine($"{len},{BitConverter.ToString(d)}");
                sp.Write(d, start, len);
                return true;
            }
            catch
            {
                return false;
            }
        }

        /// <summary>
        /// 显示图片
        /// </summary>
        /// <param name="pic">图片数据</param>
        /// <param name="x">起始x</param>
        /// <param name="y">起始y</param>
        /// <param name="tc">是否黑白图（黑白图模式仅支持8倍数长宽，否则默认x会被裁减）</param>
        /// <returns>是否成功执行</returns>
        public bool Show(Bitmap pic,int x = 0, int y = 0, bool tc = false)
        {
            if (x >= width || y >= height)//超过屏幕高度宽度
                return false;
            //停止的位置，防止超出画面
            int ex = (pic.Width + x > width ? width : pic.Width + x) - 1;
            int ey = (pic.Height + y > height ? height : pic.Height + y) - 1;
            //处理后的长宽
            int tx = (ex - x + 1), ty = (ey - y + 1);
            if (!tc)//彩色图片
            {
                //待发送数据
                byte[] data = new byte[64];
                data[1] = (byte)x; data[2] = (byte)ex; data[3] = (byte)y; data[4] = (byte)ey;
                if (tx * ty <= 29)//一帧就能发完
                {
                    data[0] = (byte)(tx * ty * 2);//包长度
                    for(int i=0;i< tx * ty; i++)
                    {
                        var c = pic.GetPixel(i % tx, i / tx);
                        data[5 + i * 2] = (byte)((c.R >> 3 << 3) + (c.G >> 5));
                        data[6 + i * 2] = (byte)((((c.G >> 2) % 0x1000) << 5) + (c.B >> 3));
                    }
                    return sendBytes(data, 0, data[0] + 5);
                }
                else//要分开发的
                {
                    //第一包，带设置参数
                    data[0] = 63 - 5;
                    for (int i = 0; i < (63 - 5) / 2; i++)
                    {
                        var c = pic.GetPixel(i % tx, i / tx);
                        data[5 + i * 2] = (byte)((c.R >> 3 << 3) + (c.G >> 5));
                        data[6 + i * 2] = (byte)((((c.G >> 2) % 0x1000) << 5) + (c.B >> 3));
                    }
                    if (!sendBytes(data, 0, data[0] + 5))
                        return false;
                    int sent = (63 - 5) / 2;//已发送的像素个数
                    while (tx * ty - sent >= 64/2)//直到发到少于64字节
                    {
                        for(int i = 0; i < 64 / 2; i++)
                        {
                            var c = pic.GetPixel((i + sent) % tx, (i + sent) / tx);
                            data[i * 2] = (byte)((c.R >> 3 << 3) + (c.G >> 5));
                            data[i * 2 + 1] = (byte)((((c.G >> 2) % 0x1000) << 5) + (c.B >> 3));
                        }
                        if (!sendBytes(data, 0, 64))
                            return false;
                        sent += 64 / 2;
                    }
                    //最后一包，纯数据，数据长度小于63字节
                    data[0] = (byte)((tx * ty - sent) * 2 | 0x40);
                    for (int i = 0; i < (tx * ty - sent); i++)
                    {
                        var c = pic.GetPixel((i + sent) % tx, (i + sent) / tx);
                        data[i * 2 + 1] = (byte)((c.R >> 3 << 3) + (c.G >> 5));
                        data[i * 2 + 2] = (byte)((((c.G >> 2) % 0x1000) << 5) + (c.B >> 3));
                    }
                    if (!sendBytes(data, 0, (data[0] & 0x3f) + 1))
                        return false;
                }
            }
            else//黑白图片
            {
                if ((ex + 1 - x) * (ey + 1 - y) % 8 != 0)//不是8的倍数像素点
                {
                    ex = ex - (ex + 1 - x) % 8;
                    tx = (ex - x + 1);
                    if (ex - x == -1)//没啥可显示的
                    {
                        Debug.WriteLine("no pic to show");
                        return true;
                    }
                }
                //待发送数据
                byte[] data = new byte[64];
                data[1] = (byte)x; data[2] = (byte)ex; data[3] = (byte)y; data[4] = (byte)ey;
                if (tx * ty / 8 <= 63 - 5)//一帧就能发完
                {
                    data[0] = (byte)(tx * ty / 8 | 0x80);//包长度
                    for (int i = 0; i < tx * ty / 8; i++)
                    {
                        data[5 + i] = 0;
                        for (int j = 0; j < 8; j++)
                        {
                            var p = i * 8 + j;//到哪个点了
                            var c = pic.GetPixel(p % tx, p / tx);
                            if (0.3 * c.R + 0.59 * c.G + 0.11 * c.B > 128)//黑色
                                data[5 + i] |= (byte)(1 << (8 - j));
                        }
                    }
                    return sendBytes(data, 0, tx * ty / 8 + 5);
                }
                else//要分开发的
                {
                    //第一包，带设置参数
                    data[0] = 63 - 5;
                    for (int i = 0; i < 63 - 5; i++)
                    {
                        data[5 + i] = 0;
                        for (int j = 0; j < 8; j++)
                        {
                            var p = i * 8 + j;//到哪个点了
                            var c = pic.GetPixel(p % tx, p / tx);
                            if (0.3 * c.R + 0.59 * c.G + 0.11 * c.B > 128)//黑色
                                data[5 + i] |= (byte)(1 << (8 - j));
                        }
                    }
                    if (!sendBytes(data, 0, data[0] + 5))
                        return false;
                    int sent = (63 - 5) * 8;//已发送的像素个数
                    while (tx * ty - sent >= 64 * 8)//直到发到少于64字节
                    {
                        for (int i = 0; i < 64 ; i++)
                        {
                            data[i] = 0;
                            for (int j = 0; j < 8; j++)
                            {
                                var p = i * 8 + j;//到哪个点了
                                var c = pic.GetPixel(p % tx, p / tx);
                                if (0.3 * c.R + 0.59 * c.G + 0.11 * c.B > 128)//黑色
                                    data[i] |= (byte)(1 << (8 - j));
                            }
                        }
                        if (!sendBytes(data, 0, 64))
                            return false;
                        sent += 64 * 8;
                    }
                    //最后一包，纯数据，数据长度小于63字节
                    data[0] = (byte)((tx * ty - sent) / 8 | 0xC0);
                    for (int i = 0; i < (tx * ty - sent) / 8; i++)
                    {
                        data[i + 1] = 0;
                        for (int j = 0; j < 8; j++)
                        {
                            var p = i * 8 + j;//到哪个点了
                            var c = pic.GetPixel(p % tx, p / tx);
                            if (0.3 * c.R + 0.59 * c.G + 0.11 * c.B > 128)//黑色
                                data[i + 1] |= (byte)(1 << (8 - j));
                        }
                    }
                    if (!sendBytes(data, 0, (data[0] & 0x3f) + 1))
                        return false;
                }
            }
            return true;
        }

    }
}
