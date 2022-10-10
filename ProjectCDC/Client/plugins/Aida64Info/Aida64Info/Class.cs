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
        /// �������
        /// </summary>
        private int ScreenV { get; set; }
        /// <summary>
        /// �����߶�
        /// </summary>
        private int ScreenH { get; set; }
        /// <summary>
        /// ����ͼ
        /// </summary>
        private Bitmap Background { get; set; }
        /// <summary>
        /// ʱ��λͼ
        /// </summary>
        private Bitmap BgImg { get; set; }
        /// <summary>
        /// ��ͼ����
        /// </summary>
        private Graphics Canvas { get; set; }
        /// <summary>
        /// ���ʼ�����
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

            // ��ȡAIDA64�����ڴ�����
            using MemoryMappedFile mmf = MemoryMappedFile.OpenExisting("AIDA64_SensorValues");
            using MemoryMappedViewAccessor accessor = mmf.CreateViewAccessor();
            byte[] Buffer = new byte[accessor.Capacity];
            accessor.ReadArray(0, Buffer, 0, Buffer.Length);
            string Aida64XML = Encoding.ASCII.GetString(Buffer).Replace("\0", "");
            // ����AIDA64�����ڴ�����
            XElement AidaInfos = XDocument.Parse($"<AIDA64>{Aida64XML}</AIDA64>").Element("AIDA64");
            IEnumerable<XElement> NodeS = AidaInfos.Elements("sys");
            IEnumerable<XElement> NodeT = AidaInfos.Elements("temp");
            IEnumerable<XElement> NodeF = AidaInfos.Elements("fan");
            IEnumerable<XElement> NodeD = AidaInfos.Elements("duty");
            IEnumerable<XElement> NodeV = AidaInfos.Elements("volt");
            IEnumerable<XElement> NodeP = AidaInfos.Elements("pwr");
            XElement DefaultXML = XDocument.Parse($"<NoData><id>Null</id><value>??</value></NoData>").Element("NoData");

            // �����к�����
            int Index = -1;

            string SUSEDMEM = (NodeS.FirstOrDefault(x => x.Element("id").Value == "SUSEDMEM") ?? DefaultXML).Element("value").Value;
            Canvas.DrawString($"RAM����{SUSEDMEM,8}MB", font, Brushes.Orange, 0, ++Index * FontSize);
            string SFREEMEM = (NodeS.FirstOrDefault(x => x.Element("id").Value == "SFREEMEM") ?? DefaultXML).Element("value").Value;
            Canvas.DrawString($"RAM����{SFREEMEM,8}MB", font, Brushes.ForestGreen, 0, ++Index * FontSize);

            string SCPUUTI = (NodeS.FirstOrDefault(x => x.Element("id").Value == "SCPUUTI") ?? DefaultXML).Element("value").Value;
            Canvas.DrawString($"CPUռ��{SCPUUTI,9}%", font, Brushes.Yellow, 0, ++Index * FontSize);
            string FCPU = (NodeF.FirstOrDefault(x => x.Element("id").Value == "FCPU") ?? DefaultXML).Element("value").Value;
            Canvas.DrawString($"CPU����{FCPU,7}RPM", font, Brushes.Orange, 0, ++Index * FontSize);
            string TCPUDIO = (NodeT.FirstOrDefault(x => x.Element("id").Value == "TCPUDIO") ?? DefaultXML).Element("value").Value;
            Canvas.DrawString($"CPU�¶�{TCPUDIO,8}��C", font, Brushes.ForestGreen, 0, ++Index * FontSize);
            string PCPUPKG = (NodeP.FirstOrDefault(x => x.Element("id").Value == "PCPUPKG") ?? DefaultXML).Element("value").Value;
            Canvas.DrawString($"CPU����{PCPUPKG,9}W", font, Brushes.WhiteSmoke, 0, ++Index * FontSize);

            string SGPU1UTI = (NodeS.FirstOrDefault(x => x.Element("id").Value == "SGPU1UTI") ?? DefaultXML).Element("value").Value;
            Canvas.DrawString($"GPUռ��{SCPUUTI,9}%", font, Brushes.Yellow, 0, ++Index * FontSize);
            string FGPU1 = (NodeF.FirstOrDefault(x => x.Element("id").Value == "FGPU1") ?? DefaultXML).Element("value").Value;
            Canvas.DrawString($"GPU����{FGPU1,7}RPM", font, Brushes.Orange, 0, ++Index * FontSize);
            string TGPU1DIO = (NodeT.FirstOrDefault(x => x.Element("id").Value == "TGPU1DIO") ?? DefaultXML).Element("value").Value;
            Canvas.DrawString($"GPU�¶�{TGPU1DIO,8}��C", font, Brushes.ForestGreen, 0, ++Index * FontSize);
            string PGPU1 = (NodeP.FirstOrDefault(x => x.Element("id").Value == "PGPU1") ?? DefaultXML).Element("value").Value;
            Canvas.DrawString($"GPU����{PGPU1,9}W", font, Brushes.WhiteSmoke, 0, ++Index * FontSize);

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
