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
		private Bitmap _pic;
		private Graphics _g;
		private Bitmap _pclock;
		private Graphics _gclock;
		private int _width;
		private int _height;

		public Bitmap Enable(int width, int height)
		{
			_width = width;
			_height = height;
			_pic = new Bitmap(width, height);
			_g = Graphics.FromImage(_pic);
			_g.FillRectangle(Brushes.Black, 0, 0, _width, _height);
			_pclock = new Bitmap(240, 50);
			_gclock = Graphics.FromImage(_pclock);
			return _pic;
		}

		public void Disable()
		{
			_g.Dispose();
			_pic.Dispose();
			_gclock.Dispose();
			_pclock.Dispose();
		}

		Font font = new Font(new FontFamily("Consolas"), 50f, FontStyle.Bold, GraphicsUnit.Pixel);
		Color colorA = Color.FromArgb(0xF8, 0, 0), colorB = Color.FromArgb(0, 0xFC, 0);
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

		public (Bitmap pic, int x, int y, long next) Refresh()
		{
			_gclock.FillRectangle(Brushes.Black, 0, 0, _pclock.Width, _pclock.Height);
			_gclock.DrawString($"{DateTime.Now:HH:mm:ss}", font,
			new LinearGradientBrush(new Point(0, 0), new Point(240, 0), GetNextColor(ref colorA), GetNextColor(ref colorB))
		, 0, 0);
			return (_pclock, 0, _height / 2 - 32, 200);
		}
	}
}
