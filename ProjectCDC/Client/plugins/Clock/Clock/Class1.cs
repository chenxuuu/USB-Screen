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
        private Bitmap _pic = null;
        private Graphics _g;
        private int _width;
        private int _height;

        public Bitmap Enable(int width, int height)
        {
            _width = width;
            _height = height;
            _pic = new Bitmap(width, height);
            _g = Graphics.FromImage(_pic);
            _g.FillRectangle(Brushes.Gray, 0, 0, _width, _height);
            return _pic;
        }

        public void Disable()
        {
            _g.Dispose();
            _pic.Dispose();
        }

        Random r = new Random();
        public (Bitmap pic, int x, int y, long next) Refresh()
        {
            _g.FillRectangle(
                new SolidBrush(Color.FromArgb(255, r.Next(0, 255), r.Next(0, 255), r.Next(0, 255))),
                0, 0, _width, _height);
            return (_pic, 0, 0, 1000);
        }
    }
}
