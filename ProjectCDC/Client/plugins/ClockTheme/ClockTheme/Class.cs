using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.IO;
using System.Linq;
using System.Reflection;

namespace ClockTheme
{
	public class ClockTheme : IScreen
	{
		readonly string ThemePath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, $@"plugin\{Assembly.GetExecutingAssembly().GetName().Name}");
		private int ScreenV { get; set; }
		private int ScreenH { get; set; }
		private Dictionary<string, Bitmap> Images { get; set; } = new Dictionary<string, Bitmap>
		{
			{ "Background", null },
			{ "Backdrop", null },
			{ "Dial", null },
			{ "PointerHour", null },
			{ "PointerMinute", null },
			{ "PointerSecond", null }
		};

		private Bitmap Dial { get; set; }
		private Bitmap TimetNow { get; set; }
		private Graphics Canvas { get; set; }
		private bool Initialized { get; set; } = false;

		public void InitializeComponent(int width, int height)
		{
			if (Initialized == false)
			{
				Initialized = true;
				ScreenV = width;
				ScreenH = height;
				Directory.CreateDirectory(ThemePath);
				ExportResources(ThemePath);
				string FilePath;
				string[] fileList = Images.Keys.ToArray();
				foreach (string fileName in fileList)
				{
					FilePath = Path.Combine(ThemePath, $"{fileName}.png");
					if (File.Exists(FilePath))
					{
						Image img = Image.FromFile(FilePath);
						Images[fileName] = new Bitmap(ScreenV < img.Width ? ScreenV : img.Width, ScreenH < img.Height ? ScreenH : img.Height);
						Graphics g = Graphics.FromImage(Images[fileName]);
						g.DrawImage(img, 0, 0, Images[fileName].Width, Images[fileName].Height);
						g.Dispose();
						img.Dispose();
					}
				}
				Dial = new Bitmap(ScreenV, ScreenH);
				DrawDial();
			}
		}

		public (Bitmap pic, int x, int y, long next) GetData()
		{
			if (Initialized == false) InitializeComponent(240, 240);
			DrawPointer();
			return (TimetNow, (240 - TimetNow.Width) / 2, (240 - TimetNow.Height) / 2, 200);
		}

		public void Dispose()
		{
			foreach (Bitmap b in Images.Values) if (b != null) b.Dispose();
			Images.Clear();
			Dial.Dispose();
			Canvas.Dispose();
			TimetNow.Dispose();
		}

		/// <summary>
		/// 释放内嵌资源文件到目标文件夹
		/// </summary>
		/// <param name="path">文件夹路径</param>
		private void ExportResources(string path)
		{
			Assembly ass = Assembly.GetExecutingAssembly();
			string resourceName = "ReadMe.md";
			using (StreamReader sr = new StreamReader(ass.GetManifestResourceStream($"ClockTheme.Resources.{resourceName}")))
			{
				File.WriteAllText(Path.Combine(path, resourceName), sr.ReadToEnd());
			}
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
		private void DrawDial()
		{
			// 设置画布
			Graphics bg = Graphics.FromImage(Dial);
			// 默认底色为黑色
			bg.FillRectangle(Brushes.Black, 0, 0, Dial.Width, Dial.Height);
			// 设置画布起点(这里设置为画布中心)
			bg.TranslateTransform(Dial.Width / 2, Dial.Height / 2);
			// 设置2D绘图质量
			bg.SmoothingMode = SmoothingMode.HighQuality;
			// 标记画布状态
			bool ImgIsEmpty = true;
			// 绘制底色,背景,表盘
			string[] imgList = new string[] { "Background", "Backdrop", "Dial" };
			foreach (string imgName in imgList)
			{
				if (Images[imgName] == null) continue;
				ImgIsEmpty = false;
				Bitmap img = Images[imgName];
				bg.DrawImage(img, -img.Width / 2, -img.Height / 2);
			}
			// 是否已加载图片资源
			if (ImgIsEmpty)
			{
				// 绘制表框
				float radius = (Dial.Width < Dial.Height ? Dial.Width : Dial.Height) / 2 - 2;
				Pen cScale = new Pen(new SolidBrush(Color.FromArgb(19, 20, 25)), 3);
				bg.DrawArc(cScale, -radius, -radius, radius * 2, radius * 2, 0, 360);
				// 绘制刻度
				Pen hScale = new Pen(new SolidBrush(Color.FromArgb(122, 179, 222)), 2); // 时刻
				Pen mScale = new Pen(new SolidBrush(Color.FromArgb(77, 88, 124)), 1);   // 秒刻
				for (int i = 0; i <= 60; i++)
				{
					float angel = i * 6;
					PointF pointA = GetEndPoint(angel, radius - 5);
					PointF pointB = GetEndPoint(angel, radius - (i % 5 == 0 ? 15 : 10));
					bg.DrawLine(i % 5 == 0 ? hScale : mScale, pointA, pointB);
				}
				// 绘制文字
				Font scaleFont = new Font(new FontFamily("Consolas"), 18, FontStyle.Bold, GraphicsUnit.Pixel);
				SolidBrush brush = new SolidBrush(Color.FromArgb(77, 88, 124));
				for (int i = 1; i <= 12; ++i)
				{
					SizeF fontSize = bg.MeasureString(i.ToString(), scaleFont);
					PointF pointT = GetEndPoint(270 + i * 30, radius - 18 - fontSize.Width / 2);
					bg.DrawString(i.ToString(), scaleFont, brush, pointT.X - fontSize.Width / 2, pointT.Y - fontSize.Width / 2);
				}
			}
			bg.Dispose();

			// 设置时钟宽高
			TimetNow = new Bitmap(Dial.Width, Dial.Height);
			// 时针画布
			Canvas = Graphics.FromImage(TimetNow);
			// 设置2D绘图质量
			Canvas.SmoothingMode = SmoothingMode.HighQuality;
			// 设置画布起点(这里设置为画布中心)
			Canvas.TranslateTransform(TimetNow.Width / 2, TimetNow.Height / 2);
		}

		/// <summary>
		/// 绘制时钟指针
		/// </summary>
		private void DrawPointer()
		{
			// 绘制表盘背景
			Canvas.DrawImage(Dial, -Dial.Width / 2, -Dial.Height / 2);
			// 获取当前时间
			DateTime Time = DateTime.Now;
			float H = Time.Hour;
			float M = Time.Minute;
			float S = Time.Second;

			// 标记画布状态
			bool ImgIsEmpty = true;
			// 绘制时针,分针,秒针
			float angel = 0;
			string[] imgList = new string[] { "PointerHour", "PointerMinute", "PointerSecond" };
			for (int i = 0; i < imgList.Length; ++i)
			{
				if (Images[imgList[i]] == null) continue;
				ImgIsEmpty = false;
				if (i == 0) angel = H * 30 + M * 30 / 60 + S * 30 / 60 / 60;
				else if (i == 1) angel = M * 6 + S * 6 / 60;
				else if (i == 2) angel = S * 6;
				Bitmap img = Images[imgList[i]];
				Canvas.RotateTransform(angel);
				Canvas.DrawImage(img, -img.Width / 2, -img.Height / 2);
				Canvas.RotateTransform(-angel);
			}
			if (ImgIsEmpty == false) return;

			// 圆形绘图范围半径
			float radius = (Dial.Width < Dial.Height ? Dial.Width : Dial.Height) / 2 - 2;
			// 绘制文字
			Font scaleFont = new Font(new FontFamily("Bahnschrift Light"), 18, FontStyle.Regular, GraphicsUnit.Pixel);
			SolidBrush brush = new SolidBrush(ColorTranslator.FromHtml("#A0CA6924"));
			string date = $"{Time:yyyy年MM月dd日}";
			SizeF fontSize = Canvas.MeasureString(date, scaleFont);
			Canvas.DrawString(date, scaleFont, brush, -fontSize.Width / 2, -10 - fontSize.Height);
			string week = $"{Time:dddd}";
			fontSize = Canvas.MeasureString(week, scaleFont);
			Canvas.DrawString(week, scaleFont, brush, -fontSize.Width / 2, 15);
			// 时针
			angel = H * 30 + M * 30 / 60;
			Brush handColor = new SolidBrush(ColorTranslator.FromHtml("#D0424C50"));
			Canvas.RotateTransform(angel);
			Canvas.FillEllipse(handColor, -10, -10, 20, 20);
			Canvas.DrawArc(new Pen(handColor, 5), -24, -24, 48, 48, 270 - 8, -(30 - 8));
			Canvas.DrawArc(new Pen(handColor, 5), -24, -24, 48, 48, 270 + 8, 30 - 8);
			Canvas.FillPolygon(handColor, new PointF[] { new PointF(5, 15), new PointF(-5, 15), new PointF(-2, -60), new PointF(2, -60) });
			Canvas.RotateTransform(-angel);
			// 分针
			angel = M * 6 + S * 6 / 60;
			handColor = new SolidBrush(ColorTranslator.FromHtml("#D0A78E44"));
			Canvas.RotateTransform(angel);
			Canvas.FillEllipse(handColor, -8, -8, 16, 16);
			Canvas.DrawEllipse(new Pen(handColor, 2), -6, -40, 12, 12);
			Canvas.FillPolygon(handColor, new PointF[] { new PointF(4, 15), new PointF(-4, 15), new PointF(-2, -29), new PointF(2, -29) });
			Canvas.FillPolygon(handColor, new PointF[] { new PointF(2, -39), new PointF(-2, -39), new PointF(-1, -70), new PointF(1, -70) });
			Canvas.RotateTransform(-angel);
			// 秒针
			angel = S * 6;
			handColor = new SolidBrush(ColorTranslator.FromHtml("#D04C221B"));
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
