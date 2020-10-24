using System.Drawing;

namespace UsbScreen.Interface
{
	interface IScreen
    {
        /// <summary>
        /// 使用指定的宽高初始化插件
        /// </summary>
        /// <param name="width">画布宽度</param>
        /// <param name="height">画布高度</param>
        void InitializeComponent(int width, int height);

        /// <summary>
        /// 获取插件数据
        /// </summary>
        /// <returns>图片，开始位置(x,y)，下次刷新等待时间</returns>
        (Bitmap pic,int x,int y,long next) GetData();

        /// <summary>
        /// 停用插件
        /// </summary>
        /// <returns></returns>
        void Dispose();
    }
}
