using System;
using System.Diagnostics;
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
		/// 捕获屏幕区域
		/// </summary>
		/// <param name="bmp">输出Bitmap</param>
		/// <param name="rect">区域范围</param>
		public static void GetScreenCapture(out Bitmap bmp, Rectangle rect)
		{
			bmp = new Bitmap(rect.Width, rect.Height);
			using (Graphics g = Graphics.FromImage(bmp))
			{
				g.CopyFromScreen(rect.X, rect.Y, 0, 0, bmp.Size);
				g.DrawImage(bmp, 0, 0, rect, GraphicsUnit.Pixel);
			}
		}
		/// <summary>
		/// 获取屏幕截图
		/// </summary>
		/// <returns></returns>
		public static void ScreenSnapshot(out BitmapSource Snapshot)
		{
			GetScreenCapture(out Bitmap bmp, new Rectangle()
			{
				X = 0,
				Y = 0,
				Width = (int)SystemParameters.PrimaryScreenWidth,
				Height = (int)SystemParameters.PrimaryScreenHeight
			});

			IntPtr hBitmap = bmp.GetHbitmap();
			Snapshot = Imaging.CreateBitmapSourceFromHBitmap(hBitmap, IntPtr.Zero, Int32Rect.Empty, BitmapSizeOptions.FromEmptyOptions());
			DeleteObject(hBitmap);
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
