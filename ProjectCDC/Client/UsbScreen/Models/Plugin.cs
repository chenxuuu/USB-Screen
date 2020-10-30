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
		public string Dir { get; set; } = null;
		/// <summary>
		/// 插件名称(文件名)
		/// </summary>
		public string Name{ get{ return Dir.Split('\\').Last(); } }
		/// <summary>
		/// 对象引用到插件
		/// </summary>
		public object Plugin { get; set; } = null;

		public Type PluginType { get; set; }
		/// <summary>
		/// 初始化插件
		/// </summary>
		public bool Init()
		{
			PluginType.GetMethod("InitializeComponent").Invoke(Plugin, new object[] { 240, 240 });
			return true;
		}
		/// <summary>
		/// 获取插件数据
		/// </summary>
		public ValueTuple<Bitmap, int, int, long> GetData()
		{
			return (ValueTuple<Bitmap, int, int, long>)PluginType.GetMethod("GetData").Invoke(Plugin, null);
		}
		/// <summary>
		/// 释放插件资源
		/// </summary>
		public void Dispose()
		{
			PluginType.GetMethod("Dispose").Invoke(Plugin, null);
		}

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
		private List<IPlugin> IPlugins = new List<IPlugin>();
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
					(Bitmap pic, int x, int y, long next) = IPlugins[PIndex].GetData();
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
			if (IPlugins.Count > 0)
			{
				PIndex = IPlugins.FindIndex(p => p.Dir == path);
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
			Assembly ass = Assembly.Load(File.ReadAllBytes(path));
			foreach (Type t in ass.GetExportedTypes())
			{
				if(t.GetInterface("IScreen") != null)
				{
					IPlugin newPlugin = new IPlugin
					{
						Plugin = ass.CreateInstance(t.FullName),
						PluginType = t,
						Dir = path
					};
					PIndex = IPlugins.Count;
					IPlugins.Add(newPlugin);
				}
			}

			if (PIndex > -1)
			{
				try
				{
					IPlugins[PIndex].Init();
					PluginTimer.Interval = 1;
					PluginTimer.Start();
					IsEnable = true;
					Debug.Print($"[插件加载成功] {IPlugins[PIndex].Name} Is Enabled");
					return true;
				}
				catch (Exception e)
				{
					IPlugins.RemoveAt(PIndex);
					Debug.Print($"[插件加载失败] {IPlugins[PIndex].Name} Is Enable Failed");
					ErrorLogger(e.ToString());
					Debug.Print(e.Message);
				}

			}
			PIndex = -1;
			return false;
		}

		public void Disable()
		{
			IPlugins[PIndex].Dispose();	// 伪卸载插件(其实还是在内存里面,只是为了能够加载更新的DLL文件)
			IPlugins.RemoveAt(PIndex);
			IsEnable = false;
			PIndex = -1;
			PluginTimer.Stop();
		}
	}
}

