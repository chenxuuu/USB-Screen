using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace AnalogClock
{
	public class AnalogClock : IScreen
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
		/// 表盘位图
		/// </summary>
		private Bitmap Dial { get; set; }
		/// <summary>
		/// 绘图画布
		/// </summary>
		private Graphics Canvas { get; set; }
		/// <summary>
		/// 时针位图
		/// </summary>
		private Bitmap TimetHand { get; set; }

		public Bitmap Enable(int width, int height)
		{
			ScreenV = width;
			ScreenH = height;
			Dial = new Bitmap(ScreenV, ScreenH);		// 设置表盘
			TimetHand = new Bitmap(ScreenV, ScreenH);	// 设置时针
			DrawBackImage();
			return Dial;
		}

		public void Disable()
		{
			Dial.Dispose();
			Canvas.Dispose();
		}

		public (Bitmap pic, int x, int y, long next) Refresh()
		{
			DrawPointer();
			return (TimetHand, (240 - TimetHand.Width) / 2, (240 - TimetHand.Height) / 2, 200);
		}

		/// <summary>
		/// 以坐标源点,用三角函数求坐标终点
		/// </summary>
		/// <param name="angle">角度</param>
		/// <param name="length">长度</param>
		/// <returns></returns>
		private static PointF GetEndPoint(double angle, double length)
		{
			// 角度转弧度
			double radian = (angle * Math.PI) / 180;
			// 返回终点坐标
			return new PointF((float)(length * Math.Cos(radian)), (float)(length * Math.Sin(radian)));
		}

		/// <summary>
		/// 绘制时钟表盘
		/// </summary>
		private void DrawBackImage()
		{
			// 设置画布
			Canvas = Graphics.FromImage(Dial);
			Canvas.FillRectangle(Brushes.Black, 0, 0, Dial.Width, Dial.Height);
			// 设置2D绘图质量
			Canvas.SmoothingMode = SmoothingMode.HighQuality;
			// 设置画布起点(这里设置为画布中心)
			Canvas.TranslateTransform(Dial.Width / 2, Dial.Height / 2);

			// 以指定半径和线宽绘制圆弧
			float radius = (Dial.Width < Dial.Height ? Dial.Width : Dial.Height) / 2 - 2;   // 半径
			Pen cScale = new Pen(new SolidBrush(Color.FromArgb(19, 20, 25)), 3);            // 表框
			Canvas.DrawArc(cScale, -radius, -radius, radius * 2, radius * 2, 0, 360);

			// 绘制刻度
			Pen hScale = new Pen(new SolidBrush(Color.FromArgb(122, 179, 222)), 2);         // 时刻
			Pen mScale = new Pen(new SolidBrush(Color.FromArgb(77, 88, 124)), 1);           // 秒刻
			for (int i = 0; i <= 60; i++)
			{
				float angel = i * 6;
				if (i % 5 == 0)
				{
					PointF pointA = GetEndPoint(angel, radius - 5);
					PointF pointB = GetEndPoint(angel, radius - 15);
					Canvas.DrawLine(hScale, pointA.X, pointA.Y, pointB.X, pointB.Y);
				}
				else
				{
					PointF pointA = GetEndPoint(angel, radius - 5);
					PointF pointB = GetEndPoint(angel, radius - 10);
					Canvas.DrawLine(mScale, pointA.X, pointA.Y, pointB.X, pointB.Y);
				}
			}

			// 绘制文字
			Font scaleFont = new Font(new FontFamily("Consolas"), 18, FontStyle.Bold, GraphicsUnit.Pixel);  // 字体
			SizeF fontSize = Canvas.MeasureString("12", scaleFont);
			SolidBrush brush = new SolidBrush(Color.FromArgb(77, 88, 124));                                 // 颜色
			for (int i = 1; i <= 12; ++i)
			{
				PointF pointT = GetEndPoint(270 + i * 30, radius - 18 - fontSize.Width / 2);
				Canvas.DrawString(i.ToString(), scaleFont, brush, pointT.X - fontSize.Width / 2, pointT.Y - fontSize.Width / 2);
			}
		}

		/// <summary>
		/// 绘制时钟指针
		/// </summary>
		private void DrawPointer()
		{
			// 设置画布
			Canvas = Graphics.FromImage(TimetHand);
			Canvas.DrawImage(Dial, new Point(0, 0));
			// 设置2D绘图质量
			Canvas.SmoothingMode = SmoothingMode.HighQuality;
			// 设置画布起点(这里设置为画布中心)
			Canvas.TranslateTransform(TimetHand.Width / 2, TimetHand.Height / 2);

			// 获取当前时间
			DateTime Time = DateTime.Now;
			int H = Time.Hour;
			int M = Time.Minute;
			int S = Time.Second;

			// 时针
			float radius = 60;
			float angel = 30 * H + 30 * M / 60;
			List<PointF> handPoins = new List<PointF>();
			handPoins.Add(GetEndPoint(270, radius));
			handPoins.Add(GetEndPoint(180, 9));
			handPoins.Add(GetEndPoint(90, 9));
			handPoins.Add(GetEndPoint(0, 9));
			Canvas.RotateTransform(angel);
			Canvas.FillPolygon(new SolidBrush(ColorTranslator.FromHtml("#990033")), handPoins.ToArray());
			Canvas.RotateTransform(-angel);

			// 分针
			radius = 70;
			angel = 6 * M + 6 * S / 60;
			handPoins.Clear();
			handPoins.Add(GetEndPoint(270, radius));
			handPoins.Add(GetEndPoint(180, 7));
			handPoins.Add(GetEndPoint(90, 7));
			handPoins.Add(GetEndPoint(0, 7));
			Canvas.RotateTransform(angel);
			Canvas.FillPolygon(new SolidBrush(ColorTranslator.FromHtml("#336633")), handPoins.ToArray());
			Canvas.RotateTransform(-angel);

			// 秒针
			radius = 90;
			angel = 6 * S;
			handPoins.Clear();
			handPoins.Add(GetEndPoint(270, radius));
			handPoins.Add(GetEndPoint(180, 5));
			handPoins.Add(GetEndPoint(90, 5));
			handPoins.Add(GetEndPoint(0, 5));
			Canvas.RotateTransform(angel);
			Canvas.FillPolygon(new SolidBrush(ColorTranslator.FromHtml("#FFCC99")), handPoins.ToArray());
			Canvas.RotateTransform(-angel);
		}

	}
}
