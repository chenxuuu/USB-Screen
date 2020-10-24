using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Timers;

namespace UsbScreen.Models
{
	class IPlugin
	{
		/// <summary>
		/// 插件路径
		/// </summary>
		public string Path { get; set; } = null;
		/// <summary>
		/// 插件名称(文件名)
		/// </summary>
		public string Name{ get{ return Path.Split('\\').Last(); } }
		/// <summary>
		/// 对象引用到插件
		/// </summary>
		public object Plugin { get; set; } = null;
		/// <summary>
		/// 初始化插件
		/// </summary>
		public MethodInfo Init { get; set; } = null;
		/// <summary>
		/// 获取插件数据
		/// </summary>
		public MethodInfo GetData { get; set; } = null;
		/// <summary>
		/// 释放插件资源
		/// </summary>
		public MethodInfo Dispose { get; set; } = null;
	}

	[PropertyChanged.AddINotifyPropertyChangedInterface]
	class Plugin
	{
		/// <summary>
		/// 待操作的屏幕对象
		/// </summary>
		public SerialScreen screen { get; set; }
		/// <summary>
		/// 是否已启用了某插件？
		/// </summary>
		public bool IsEnable { get; set; } = false;
		/// <summary>
		/// 保存已加载的插件
		/// </summary>
		private List<IPlugin> Imlugins = new List<IPlugin>();
		/// <summary>
		/// 保存当前已启用插件的索引
		/// </summary>
		private int PIndex { get; set; } = -1;
		/// <summary>
		/// 插件访问定时器
		/// </summary>
		private readonly Timer PluginTimer = new Timer();

		public Plugin()
		{
			PluginTimer.AutoReset = false;
			PluginTimer.Elapsed += delegate
			{
				PluginTimer.Stop();
				if (!IsEnable) return;
				try
				{
					(Bitmap pic, int x, int y, long next) = (ValueTuple<Bitmap, int, int, long>)(Imlugins[PIndex].GetData).Invoke(Imlugins[PIndex].Plugin, new object[] { });
					screen?.Show(pic, x, y);
					PluginTimer.Interval = next;
					PluginTimer.Start();		// 准备获取下一帧数据
				}
				catch (Exception e)
				{
					ErrorLogger(e.ToString());
					Debug.Print(e.Message);
				}

			};
		}

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
			// 判断插件是否已加载
			if (Imlugins.Count > 0)
			{
				PIndex = Imlugins.FindIndex(p => p.Path == path);
				if (PIndex > -1)
				{
					PluginTimer.Interval = 1;
					PluginTimer.Start();
					IsEnable = true;
					return true;
				}
			}
			// 插件在加载前被删除了
			if (!File.Exists(path)) return false;
			// 加载新插件
			Assembly ass = Assembly.LoadFrom(path);
			foreach (Type t in ass.GetExportedTypes())
			{
				if(t.GetInterface("IScreen") != null)
				{
					IPlugin newPlugin = new IPlugin
					{
						Plugin = ass.CreateInstance(t.FullName),
						Init = t.GetMethod("InitializeComponent"),
						GetData = t.GetMethod("GetData"),
						Dispose = t.GetMethod("Dispose")
					};
					PIndex = Imlugins.Count;
					Imlugins.Add(newPlugin);
				}
			}

			if (PIndex > -1)
			{
				try
				{
					Imlugins[PIndex].Init.Invoke(Imlugins[PIndex].Plugin, new object[] { 240, 240 });
					PluginTimer.Interval = 1;
					PluginTimer.Start();
					IsEnable = true;
					Debug.Print($"[目标插件已启用] Plugin Is Enabled");
					return true;
				}
				catch (Exception e)
				{
					ErrorLogger(e.ToString());
					Debug.Print(e.Message);
				}

			}
			PIndex = -1;
			return false;
		}

		public void Disable()
		{
			IsEnable = false;
			PIndex = -1;
			PluginTimer.Stop();
		}
	}
}
