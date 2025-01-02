using System.Collections.Generic;
using System.Drawing;
using System.IO.MemoryMappedFiles;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Aida64Info
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
        private Bitmap BgImg { get; set; }
        /// <summary>
        /// 绘图画布
        /// </summary>
        private Graphics Canvas { get; set; }
        /// <summary>
        /// 插件始化标记
        /// </summary>
        private int Initialvalue { get; set; } = -1;
        private const int BgWidth = 240, BgHeight = 240;
        private const int FontSize = 24;

        public void InitializeComponent(int width, int height)
        {
            if (Initialvalue < 0)
            {
                Initialvalue = 0;
                ScreenV = width;
                ScreenH = height;
                BgImg = new Bitmap(BgWidth, BgHeight);
                Canvas = Graphics.FromImage(BgImg);
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
            Canvas.FillRectangle(Brushes.Black, 0, 0, BgImg.Width, BgImg.Height);

            // 读取AIDA64共享内存数据
            using MemoryMappedFile mmf = MemoryMappedFile.OpenExisting("AIDA64_SensorValues");
            using MemoryMappedViewAccessor accessor = mmf.CreateViewAccessor();
            byte[] Buffer = new byte[accessor.Capacity];
            accessor.ReadArray(0, Buffer, 0, Buffer.Length);
            string Aida64XML = Encoding.ASCII.GetString(Buffer).Replace("\0", "");
            // 解析AIDA64共享内存数据
            XElement AidaInfos = XDocument.Parse($"<AIDA64>{Aida64XML}</AIDA64>").Element("AIDA64");
            IEnumerable<XElement> NodeS = AidaInfos.Elements("sys");
            IEnumerable<XElement> NodeT = AidaInfos.Elements("temp");
            IEnumerable<XElement> NodeF = AidaInfos.Elements("fan");
            IEnumerable<XElement> NodeD = AidaInfos.Elements("duty");
            IEnumerable<XElement> NodeV = AidaInfos.Elements("volt");
            IEnumerable<XElement> NodeP = AidaInfos.Elements("pwr");
            XElement DefaultXML = XDocument.Parse($"<NoData><id>Null</id><value>??</value></NoData>").Element("NoData");

            // 设置行号索引
            int Index = -1;

            string SUSEDMEM = (NodeS.FirstOrDefault(x => x.Element("id").Value == "SUSEDMEM") ?? DefaultXML).Element("value").Value;
            Canvas.DrawString($"RAM已用{SUSEDMEM,8}MB", font, Brushes.Orange, 0, ++Index * FontSize);
            string SFREEMEM = (NodeS.FirstOrDefault(x => x.Element("id").Value == "SFREEMEM") ?? DefaultXML).Element("value").Value;
            Canvas.DrawString($"RAM可用{SFREEMEM,8}MB", font, Brushes.ForestGreen, 0, ++Index * FontSize);

            string SCPUUTI = (NodeS.FirstOrDefault(x => x.Element("id").Value == "SCPUUTI") ?? DefaultXML).Element("value").Value;
            Canvas.DrawString($"CPU占用{SCPUUTI,9}%", font, Brushes.Yellow, 0, ++Index * FontSize);
            string FCPU = (NodeF.FirstOrDefault(x => x.Element("id").Value == "FCPU") ?? DefaultXML).Element("value").Value;
            Canvas.DrawString($"CPU风扇{FCPU,7}RPM", font, Brushes.Orange, 0, ++Index * FontSize);
            string TCPUDIO = (NodeT.FirstOrDefault(x => x.Element("id").Value == "TCPUDIO") ?? DefaultXML).Element("value").Value;
            Canvas.DrawString($"CPU温度{TCPUDIO,8}°C", font, Brushes.ForestGreen, 0, ++Index * FontSize);
            string PCPUPKG = (NodeP.FirstOrDefault(x => x.Element("id").Value == "PCPUPKG") ?? DefaultXML).Element("value").Value;
            Canvas.DrawString($"CPU功耗{PCPUPKG,9}W", font, Brushes.WhiteSmoke, 0, ++Index * FontSize);

            string SGPU1UTI = (NodeS.FirstOrDefault(x => x.Element("id").Value == "SGPU1UTI") ?? DefaultXML).Element("value").Value;
            Canvas.DrawString($"GPU占用{SCPUUTI,9}%", font, Brushes.Yellow, 0, ++Index * FontSize);
            string FGPU1 = (NodeF.FirstOrDefault(x => x.Element("id").Value == "FGPU1") ?? DefaultXML).Element("value").Value;
            Canvas.DrawString($"GPU风扇{FGPU1,7}RPM", font, Brushes.Orange, 0, ++Index * FontSize);
            string TGPU1DIO = (NodeT.FirstOrDefault(x => x.Element("id").Value == "TGPU1DIO") ?? DefaultXML).Element("value").Value;
            Canvas.DrawString($"GPU温度{TGPU1DIO,8}°C", font, Brushes.ForestGreen, 0, ++Index * FontSize);
            string PGPU1 = (NodeP.FirstOrDefault(x => x.Element("id").Value == "PGPU1") ?? DefaultXML).Element("value").Value;
            Canvas.DrawString($"GPU功耗{PGPU1,9}W", font, Brushes.WhiteSmoke, 0, ++Index * FontSize);

            return (BgImg, (240 - BgImg.Width) / 2, (240 - BgImg.Height) / 2, 1000);
        }

        public void Dispose()
        {
            Background.Dispose();
            Canvas.Dispose();
            BgImg.Dispose();
        }

        readonly Font font = new Font(new FontFamily("Consolas"), FontSize, FontStyle.Regular, GraphicsUnit.Pixel);
    }
}
