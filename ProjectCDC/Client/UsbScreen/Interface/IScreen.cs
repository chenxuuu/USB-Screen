using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace UsbScreen.Interface
{
    interface IScreen
    {
        /// <summary>
        /// 启用插件，返回第一屏
        /// </summary>
        /// <param name="width"></param>
        /// <param name="height"></param>
        /// <returns></returns>
        Bitmap Enable(int width,int height);

        /// <summary>
        /// 获取刷新后的数据
        /// </summary>
        /// <returns>图片、起始位置、下次刷新等待时间</returns>
        (Bitmap pic,int x,int y,long next) Refresh();

        /// <summary>
        /// 停用插件
        /// </summary>
        /// <returns></returns>
        void Disable();
    }
}
