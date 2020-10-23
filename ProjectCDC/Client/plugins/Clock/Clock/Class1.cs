using System;
using System.Collections.Generic;
using System.Drawing;
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
        Random r = new Random();
        public (Bitmap pic, int x, int y, long next) Refresh()
        {
            _gclock.FillRectangle(Brushes.Black, 0, 0, _pclock.Width, _pclock.Height);
            _gclock.DrawString($"{DateTime.Now:HH:mm:ss}", font, 
                new SolidBrush(Color.FromArgb(255, r.Next(50, 255), r.Next(50, 255), r.Next(50, 255))), 0, 0);
            return (_pclock, 0, _height / 2 - 32, 1000);
        }
    }
}
