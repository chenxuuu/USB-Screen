using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Threading.Tasks;

namespace AnalogClock
{
    interface IScreen
    {
        /// <summary>
        /// 启用插件，返回第一屏
        /// </summary>
        /// <param name="width"></param>
        /// <param name="height"></param>
        /// <returns></returns>
        Bitmap Enable(int width, int height);

        /// <summary>
        /// 获取刷新后的数据
        /// </summary>
        /// <returns>位图、起始位置(x,y)、下次刷新间隔时间ms</returns>
        (Bitmap pic, int x, int y, long next) Refresh();

        /// <summary>
        /// 停用插件，释放所有占用的资源
        /// </summary>
        /// <returns></returns>
        void Disable();
    }
}
