using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Clock
{
	public class Clock : IScreen
	{
		/// <summary>
		/// 画布宽度
		/// </summary>
		private int ScreenV { get; set; }
		/// <summary>
		/// 画布高度
		/// </summary>
		private int ScreenH { get; set; }
		/// <summary>
		/// 背景图
		/// </summary>
		private Bitmap Background { get; set; }
		/// <summary>
		/// 时间位图
		/// </summary>
		private Bitmap Clocker { get; set; }
		/// <summary>
		/// 绘图画布
		/// </summary>
		private Graphics Canvas { get; set; }
		/// <summary>
		/// 插件始化标记
		/// </summary>
		private int Initialvalue { get; set; } = -1;
		private const int ClockWidth = 240, ClockHeight = 50;

		public void InitializeComponent(int width, int height)
		{
			if (Initialvalue < 0)
			{
				Initialvalue = 0;
				ScreenV = width;
				ScreenH = height;
				Clocker = new Bitmap(ClockWidth, ClockHeight);
				Canvas = Graphics.FromImage(Clocker);
			}
		}

		public (Bitmap pic, int x, int y, long next) GetData()
		{
			if (Initialvalue < 0) InitializeComponent(240, 240);
			if (Initialvalue == 0)
			{
				Initialvalue = 1;
				Background = new Bitmap(ScreenV, ScreenH);
				Graphics bg = Graphics.FromImage(Background);
				bg.FillRectangle(Brushes.Black, 0, 0, Background.Width, Background.Height);
				bg.Dispose();
				return (Background, 0, 0, 1);
			}
			Canvas.FillRectangle(Brushes.Black, 0, 0, Clocker.Width, Clocker.Height);
			Brush brush = new LinearGradientBrush(new Point(0, 0), new Point(240, 0), GetNextColor(ref colorA), GetNextColor(ref colorB));
			Canvas.DrawString($"{DateTime.Now:HH:mm:ss}", font, brush, 0, 0);
			return (Clocker, (240 - Clocker.Width) / 2, (240 - Clocker.Height) / 2, 200);
		}

		public void Dispose()
		{
			Background.Dispose();
			Canvas.Dispose();
			Clocker.Dispose();
		}


		Font font = new Font(new FontFamily("Consolas"), ClockHeight, FontStyle.Bold, GraphicsUnit.Pixel);
		static Color colorA = Color.FromArgb(0xF8, 0, 0), colorB = Color.FromArgb(0, 0xFC, 0);
		private Color GetNextColor(ref Color c)
		{
			byte R = c.R, G = c.G, B = c.B, offset = 4;
			if (R == 0xF8 && G < 0xFC && B == 0x00) G += offset;
			else if (R > 0x00 && G == 0xFC && B == 0x00) R -= offset;
			else if (R == 0x00 && G == 0xFC && B < 0xF8) B += offset;
			else if (R == 0x00 && G > 0x00 && B == 0xF8) G -= offset;
			else if (R < 0xF8 && G == 0x00 && B == 0xF8) R += offset;
			else if (R == 0xF8 && G == 0x00 && B > 0x00) B -= offset;
			c = Color.FromArgb(R, G, B);
			return c;
		}
	}
}
