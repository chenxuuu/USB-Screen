using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace UsbScreen.Models
{
    [PropertyChanged.AddINotifyPropertyChangedInterface]
    class Setting
    {
        /// <summary>
        /// 保存配置
        /// </summary>
        private void Save()
        {
            File.WriteAllText("settings.json", JsonConvert.SerializeObject(this));
        }

        private string _lastPort = "";
        /// <summary>
        /// 上次使用的串口
        /// </summary>
        public string LastPort
        {
            get => _lastPort;
            set
            {
                _lastPort = value;
                Save();
            }
        }

        private string _lastPlugin = "";
        /// <summary>
        /// 上次使用的插件
        /// </summary>
        public string LastPlugin
        {
            get => _lastPlugin;
            set
            {
                _lastPlugin = value;
                Save();
            }
        }
    }
}
