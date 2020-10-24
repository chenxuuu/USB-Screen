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
			Dial = new Bitmap(ScreenV, ScreenH);        // 设置表盘
			TimetHand = new Bitmap(ScreenV, ScreenH);   // 设置时针
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
			Pen cScale = new Pen(new SolidBrush(Color.FromArgb(19, 20, 25)), 3);            // 颜色和线宽
			Canvas.DrawArc(cScale, -radius, -radius, radius * 2, radius * 2, 0, 360);       // 表框

			// 绘制刻度
			Pen hScale = new Pen(new SolidBrush(Color.FromArgb(122, 179, 222)), 2);         // 时刻
			Pen mScale = new Pen(new SolidBrush(Color.FromArgb(77, 88, 124)), 1);           // 秒刻
			for (int i = 0; i <= 60; i++)
			{
				float angel = i * 6;
				PointF pointA = GetEndPoint(angel, radius - 5);
				if (i % 5 == 0)
				{
					PointF pointB = GetEndPoint(angel, radius - 15);
					Canvas.DrawLine(hScale, pointA.X, pointA.Y, pointB.X, pointB.Y);
				}
				else
				{
					PointF pointB = GetEndPoint(angel, radius - 10);
					Canvas.DrawLine(mScale, pointA.X, pointA.Y, pointB.X, pointB.Y);
				}
			}

			// 绘制文字
			Font scaleFont = new Font(new FontFamily("Consolas"), 18, FontStyle.Bold, GraphicsUnit.Pixel);  // 字体
			SolidBrush brush = new SolidBrush(Color.FromArgb(77, 88, 124));                                 // 颜色
			for (int i = 1; i <= 12; ++i)
			{
				SizeF fontSize = Canvas.MeasureString(i.ToString(), scaleFont);
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
			// 圆形绘图范围半径
			float radius = (TimetHand.Width < TimetHand.Height ? TimetHand.Width : TimetHand.Height) / 2 - 2;

			// 获取当前时间
			DateTime Time = DateTime.Now;
			int H = Time.Hour;
			int M = Time.Minute;
			int S = Time.Second;

			// 时针
			float angel = H * 30 + M * 30 / 60;
			Brush handColor = new SolidBrush(ColorTranslator.FromHtml("#424c50"));
			Canvas.RotateTransform(angel);
			Canvas.FillEllipse(handColor, -10, -10, 20, 20);
			Canvas.FillPolygon(handColor, new PointF[] { new PointF(0, -15), new PointF(-10, -24), new PointF(0, -33), new PointF(10, -24) });
			Canvas.FillPolygon(handColor, new PointF[] { new PointF(5, 15), new PointF(-5, 15), new PointF(-2, -60), new PointF(2, -60) });
			Canvas.RotateTransform(-angel);

			// 分针
			angel = M * 6 + S * 6 / 60;
			handColor = new SolidBrush(ColorTranslator.FromHtml("#a78e44"));
			Canvas.RotateTransform(angel);
			Canvas.FillEllipse(handColor, -8, -8, 16, 16);
			Canvas.DrawEllipse(new Pen(handColor, 2), -6, -40, 12, 12);
			Canvas.FillPolygon(handColor, new PointF[] { new PointF(4, 15), new PointF(-4, 15), new PointF(-2, -29), new PointF(2, -29) });
			Canvas.FillPolygon(handColor, new PointF[] { new PointF(2, -39), new PointF(-2, -39), new PointF(-1, -70), new PointF(1, -70) });
			Canvas.RotateTransform(-angel);

			// 秒针
			angel = S * 6;
			handColor = new SolidBrush(ColorTranslator.FromHtml("#4c221b"));
			Canvas.RotateTransform(angel);
			Canvas.FillEllipse(handColor, -6, -6, 12, 12);
			Canvas.FillPolygon(handColor, new PointF[] { new PointF(3, 20), new PointF(-3, 20), new PointF(-1, -90), new PointF(1, -90) });
			Canvas.FillEllipse(Brushes.Black, -4, -4, 8, 8);
			Canvas.RotateTransform(-angel);

			// 突出显示秒刻
			PointF pointA = GetEndPoint(270 + angel, radius - 5);
			PointF pointB = GetEndPoint(270 + angel, radius - (S % 5 == 0 ? 15 : 10));
			Canvas.DrawLine(new Pen(Brushes.Red, (S % 5 == 0 ? 2 : 1)), pointA.X, pointA.Y, pointB.X, pointB.Y);
		}
	}
}
