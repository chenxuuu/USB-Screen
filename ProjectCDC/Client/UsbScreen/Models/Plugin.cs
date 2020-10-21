using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using UsbScreen.Interface;

namespace UsbScreen.Models
{
    [PropertyChanged.AddINotifyPropertyChangedInterface]
    class Plugin
    {
        /// <summary>
        /// 是否已启用了某插件？
        /// </summary>
        public bool IsEnable { get; set; } = false;

        private MethodInfo _enable = null;
        private MethodInfo _refresh = null;
        private MethodInfo _disable = null;
        private object _o = null;

        public static string[] GetPluginList()
        {
            return Directory.GetFiles("plugin", "*.dll");
        }

        public bool EnablePlugin(string path)
        {
            path = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, path);
            if (!File.Exists(path))
                return false;
            bool valid = false;
            try
            {
                Assembly asm = Assembly.LoadFile(path);
                foreach (Type ta in asm.GetExportedTypes())
                {
                    Type t = asm.GetType(ta.FullName);
                    if (t == null
                        || !t.IsClass
                        || !t.IsPublic
                        || t.GetInterface("IScreen") == null
                       )
                        continue;
                    _o = Activator.CreateInstance(t, null);
                    if (_o == null)
                        return false;
                    _enable = ta.GetMethod("Enable");
                    _refresh = ta.GetMethod("Refresh");
                    _disable = ta.GetMethod("Disable");
                    valid = true;
                    break;
                }
            }
            catch
            {
                return false;
            }
            if (!valid)//插件无效
                return false;
            IsEnable = true;//插件状态改为启用
            Debug.WriteLine("plugin enabled");
            Debug.WriteLine(((Bitmap)_enable.Invoke(_o, new object[] { 240,240 })).ToString());
            return true;
        }

        public void Disable()
        {
            if(_o != null && _disable != null)
            {
                try
                {
                    _disable.Invoke(_o, null);
                    Debug.WriteLine("plugin disabled");
                }
                catch
                {

                }
            }
            IsEnable = false;
        }
    }
}
