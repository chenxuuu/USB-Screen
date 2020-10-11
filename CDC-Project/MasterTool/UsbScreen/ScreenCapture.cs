using System;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Interop;
using System.Windows.Media.Imaging;

namespace UsbScreen
{
	/// <summary>
	/// 获取屏幕坐标颜色(需引用程序集：System.Drawing)
	/// </summary>
	public partial class ScreenCapture
	{
		/// <summary>
		/// 获取屏幕截图
		/// </summary>
		/// <returns></returns>
		public static BitmapSource ScreenSnapshot()
		{
			BitmapSource shotBmp;
			using (var bmp = new Bitmap((int)SystemParameters.PrimaryScreenWidth, (int)SystemParameters.PrimaryScreenHeight))
			{
				using (var g = Graphics.FromImage(bmp))
				{
					g.CopyFromScreen(0, 0, 0, 0, bmp.Size, CopyPixelOperation.SourceCopy);
					IntPtr hBitmap = bmp.GetHbitmap();
					try
					{
						shotBmp = Imaging.CreateBitmapSourceFromHBitmap(hBitmap, IntPtr.Zero, Int32Rect.Empty, BitmapSizeOptions.FromEmptyOptions());
					}
					finally
					{
						DeleteObject(hBitmap);
					}
				}
			}
			return shotBmp;
		}

		/// <summary>
		/// 释放资源
		/// </summary>
		/// <param name="hObject"></param>
		/// <returns></returns>
		[DllImport("gdi32.dll")]
		public static extern bool DeleteObject(IntPtr hObject);
	}
}
