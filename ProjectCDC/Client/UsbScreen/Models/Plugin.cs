using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.Remoting.Channels;
using System.Text;
using System.Threading.Tasks;
using System.Timers;
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
		private Timer _t = new Timer();

		public Plugin()
		{
			_t.AutoReset = false;
			_t.Elapsed += (sender, ee) =>
			{
				if (!IsEnable)
					return;
				_t.Stop();
				try
				{
					//刷新第一屏
					(Bitmap pic, int x, int y, long next) =
						(ValueTuple<Bitmap, int, int, long>)_refresh.Invoke(_o, new object[] { });
					Task.Run(() =>
					{
						screen?.Show(pic, x, y);
						_t.Interval = next;
						_t.Start();
					});//异步刷，防止卡
				}
				catch (Exception e)
				{
					ErrorLogger(e.ToString());
				}
			};
		}

		/// <summary>
		/// 待操作的屏幕对象
		/// </summary>
		public SerialScreen screen;

		private MethodInfo _enable = null;
		private MethodInfo _refresh = null;
		private MethodInfo _disable = null;
		private object _o = null;

		public static string[] GetPluginList()
		{
			return Directory.GetFiles("plugin", "*.dll");
		}

		public static void ErrorLogger(string err)
		{
			File.AppendAllText("error_log.txt", $"{DateTime.Now:[yyyy-MM-dd HH:mm:ss:ffff]}\r\n{err}\r\n\r\n");
		}

		public bool EnablePlugin(string path)
		{
			path = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, path);
			if (!File.Exists(path))
				return false;
			bool valid = false;
			try
			{
				Assembly ass = Assembly.LoadFile(path);
				foreach (Type ta in ass.GetExportedTypes())
				{
					Type t = ass.GetType(ta.FullName);
					if (t == null || !t.IsClass || !t.IsPublic || t.GetInterface("IScreen") == null) continue;
					_o = Activator.CreateInstance(t, null);
					if (_o == null) return false;
					_enable = ta.GetMethod("Enable");
					_refresh = ta.GetMethod("Refresh");
					_disable = ta.GetMethod("Disable");
					valid = true;
					break;
				}
			}
			catch (Exception e)
			{
				ErrorLogger(e.ToString());
				return false;
			}
			if (!valid) return false;//插件无效
			IsEnable = true;//插件状态改为启用
			Debug.WriteLine($"[目标插件已启用] ${path} is running...");
			try
			{
				//刷新第一屏
				var pic = (Bitmap)_enable.Invoke(_o, new object[] { 240, 240 });
				Task.Run(() =>
				{
					screen?.Show(pic);
				});//异步刷，防止卡
				_t.Interval = 1;
				_t.Start();
			}
			catch (Exception e)
			{
				ErrorLogger(e.ToString());
			}
			return true;
		}

		public void Disable()
		{
			IsEnable = false;
			_t.Stop();
			if (_o != null && _disable != null)
			{
				try
				{
					_disable.Invoke(_o, null);
					Debug.WriteLine("plugin disabled");
				}
				catch (Exception e)
				{
					ErrorLogger(e.ToString());
				}
			}
		}
	}
}
