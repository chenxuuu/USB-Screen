using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.Remoting.Messaging;
using System.Text;
using System.Threading.Tasks;

namespace USB_Screen.Dev
{
    class Screen
    {
        /// <summary>
        /// 向list插入数据
        /// </summary>
        /// <param name="list">数据list</param>
        /// <param name="data">数据，注意会被销毁</param>
        /// <param name="command">是否为命令</param>
        public static void AddData(List<byte> list, List<byte> data, bool command = false)
        {
            while (data.Count > 0)
            {
                var len = 63 - list.Count % 64;
                if(len == 0)
                    list.Add(0);
                else
                {
                    if (len > data.Count)
                        len = data.Count;
                    list.Add((byte)((command ? 0x00 : 0x80) + len));
                    list.AddRange(data.GetRange(0,len));
                    data.RemoveRange(0,len);
                }
            }
        }
    }
}
