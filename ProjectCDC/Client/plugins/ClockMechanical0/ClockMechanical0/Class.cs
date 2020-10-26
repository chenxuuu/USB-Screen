using System;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace ClockMechanical0
{
	public class ClockMechanical0 : IScreen
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
		/// 时针位图
		/// </summary>
		private Bitmap TimetHand { get; set; }
		/// <summary>
		/// 绘图画布
		/// </summary>
		private Graphics Canvas { get; set; }
		/// <summary>
		/// 插件始化标记
		/// </summary>
		private bool Initialized = false;

		public void InitializeComponent(int width, int height)
		{
			if (Initialized == false)
			{
				Initialized = true;
				ScreenV = width;
				ScreenH = height;
				Dial = new Bitmap(ScreenV, ScreenH);        // 设置表盘
				TimetHand = new Bitmap(ScreenV, ScreenH);   // 设置时针
				DrawBackImage();                            // 绘制表盘
			}
		}

		public (Bitmap pic, int x, int y, long next) GetData()
		{
			if (Initialized == false) InitializeComponent(240, 240);
			DrawPointer();
			return (TimetHand, (240 - TimetHand.Width) / 2, (240 - TimetHand.Height) / 2, 200);
		}

		public void Dispose()
		{
			Canvas.Dispose();
			Dial.Dispose();
			TimetHand.Dispose();
		}

		/// <summary>
		/// 以坐标源点、角度、距离,求坐标终点
		/// </summary>
		/// <param name="angle">角度</param>
		/// <param name="length">距离</param>
		/// <param name="center">源点坐标</param>
		/// <returns>终点坐标</returns>
		private static PointF GetEndPoint(double angle, double length, PointF center)
		{
			// 角度转弧度
			double radian = (angle * Math.PI) / 180;
			// 返回终点坐标
			return new PointF(center.X + (float)(length * Math.Cos(radian)), center.Y + (float)(length * Math.Sin(radian)));
		}
		/// <summary>
		/// 以坐标源点、角度、距离,求坐标终点
		/// </summary>
		/// <param name="angle">角度</param>
		/// <param name="length">距离</param>
		/// <returns>终点坐标</returns>
		private static PointF GetEndPoint(double angle, double length)
		{
			return GetEndPoint(angle, length, new PointF(0, 0));
		}

		/// <summary>
		/// 绘制时钟表盘
		/// </summary>
		private void DrawBackImage()
		{
			// 设置画布
			Graphics bg = Graphics.FromImage(Dial);
			bg.FillRectangle(Brushes.Black, 0, 0, Dial.Width, Dial.Height);
			// 设置2D绘图质量
			bg.SmoothingMode = SmoothingMode.HighQuality;
			// 设置画布起点(这里设置为画布中心)
			bg.TranslateTransform(Dial.Width / 2, Dial.Height / 2);

			// 以指定半径和线宽绘制圆弧
			float radius = (Dial.Width < Dial.Height ? Dial.Width : Dial.Height) / 2;
			// 外框
			Brush brush = new SolidBrush(ColorTranslator.FromHtml("#434343"));
			bg.FillEllipse(brush, -radius, -radius, radius * 2, radius * 2);
			// 底色
			brush = new SolidBrush(ColorTranslator.FromHtml("#202020"));
			radius -= 6;
			bg.FillEllipse(brush, -radius, -radius, radius * 2, radius * 2);
			// 刻度
			brush = new SolidBrush(ColorTranslator.FromHtml("#434343"));
			radius -= 8;
			bg.DrawArc(new Pen(brush, 2), -radius, -radius, radius * 2, radius * 2, 0, 360);
			radius -= 9;
			bg.DrawArc(new Pen(brush, 2), -radius, -radius, radius * 2, radius * 2, 0, 360);
			for (int i = 0; i < 60; i++)
			{
				float angel = 6 * i;
				PointF pointA = GetEndPoint(angel, radius);
				PointF pointB = GetEndPoint(angel, radius + 9);
				bg.DrawLine(i % 5 == 0 ? new Pen(brush, 4) : new Pen(brush, 2), pointA.X, pointA.Y, pointB.X, pointB.Y);
			}

			// 绘制文字
			Font scaleFont = new Font(new FontFamily("Microsoft YaHei"), 32, FontStyle.Regular, GraphicsUnit.Pixel);  // 字体
			string[] scaleNums = new string[] { "Ⅰ", "Ⅱ", "Ⅲ", "Ⅳ", "Ⅴ", "Ⅵ", "Ⅶ", "Ⅷ", "Ⅸ", "Ⅹ", "Ⅺ", "Ⅻ" };
			for (int i = 0; i < 12; ++i)
			{
				bg.RotateTransform(30);
				SizeF fontSize = bg.MeasureString(scaleNums[i], scaleFont);
				PointF pointT = GetEndPoint(270, radius - fontSize.Width / 2);
				bg.DrawString(scaleNums[i], scaleFont, brush, pointT.X - fontSize.Width / 2, pointT.Y - fontSize.Width / 2);
			}

			radius -= 50;
			bg.DrawArc(new Pen(brush, 4), -radius, -radius, radius * 2, radius * 2, 0, 360);
			bg.Dispose();

			// 时针画布
			Canvas = Graphics.FromImage(TimetHand);
			// 设置2D绘图质量
			Canvas.SmoothingMode = SmoothingMode.HighQuality;
			// 设置画布中心为绘图源点
			Canvas.TranslateTransform(TimetHand.Width / 2, TimetHand.Height / 2);
		}

		/// <summary>
		/// 绘制时钟指针
		/// </summary>
		private void DrawPointer()
		{
			// 绘制表盘背景
			Canvas.DrawImage(Dial, new Point(-TimetHand.Width / 2, -TimetHand.Height / 2));

			// 获取当前时间
			DateTime Time = DateTime.Now;
			int H = Time.Hour;
			int M = Time.Minute;
			int S = Time.Second;

			// 时针
			Brush handColor = new SolidBrush(ColorTranslator.FromHtml("#505221"));
			float angel = H * 30 + M * 30 / 60;
			Canvas.RotateTransform(angel);
			Canvas.FillEllipse(handColor, -10, -10, 20, 20);
			Canvas.DrawArc(new Pen(handColor, 5), -24, -24, 48, 48, 270 - 8, -(30 - 8));
			Canvas.DrawArc(new Pen(handColor, 5), -24, -24, 48, 48, 270 + 8, 30 - 8);
			Canvas.FillPolygon(handColor, new PointF[] { new PointF(5, 15), new PointF(-5, 15), new PointF(-2, -60), new PointF(2, -60) });
			Canvas.RotateTransform(-angel);

			// 分针
			angel = M * 6 + S * 6 / 60;
			handColor = new SolidBrush(ColorTranslator.FromHtml("#E0A78E44"));
			Canvas.RotateTransform(angel);
			Canvas.FillEllipse(handColor, -8, -8, 16, 16);
			Canvas.DrawEllipse(new Pen(handColor, 2), -6, -40, 12, 12);
			Canvas.FillPolygon(handColor, new PointF[] { new PointF(4, 15), new PointF(-4, 15), new PointF(-2, -29), new PointF(2, -29) });
			Canvas.FillPolygon(handColor, new PointF[] { new PointF(2, -39), new PointF(-2, -39), new PointF(-1, -70), new PointF(1, -70) });
			Canvas.RotateTransform(-angel);

			// 秒针
			angel = S * 6;
			handColor = new SolidBrush(ColorTranslator.FromHtml("#E04C221B"));
			Canvas.RotateTransform(angel);
			Canvas.FillEllipse(handColor, -6, -6, 12, 12);
			Canvas.FillPolygon(handColor, new PointF[] { new PointF(3, 20), new PointF(-3, 20), new PointF(-1, -90), new PointF(1, -90) });
			Canvas.FillEllipse(Brushes.Black, -4, -4, 8, 8);
			Canvas.RotateTransform(-angel);
		}
	}
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
		(Bitmap pic, int x, int y, long next) GetData();

		/// <summary>
		/// 停用插件
		/// </summary>
		/// <returns></returns>
		void Dispose();
	}
}