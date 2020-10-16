# USB-Screen

低成本PC资源监控小屏幕。如果感兴趣，点个星星呗。

PC monitor screen, only cost $5.

> only Chinese readme now.

<img height="300" src="https://source.papapoi.com/wp-content/uploads/2020/10/pic-scaled.jpg"/>

## 硬件

- 主控：CH551
- 屏幕：1.4寸IPS彩屏
- 接口：microUSB、typec、排针
- 外壳：3d打印

## 使用的开发软件/框架

- 硬件设计：KICAD
- 嵌入式软件：标准C51 (Keil/SDCC)
- 外壳：OpenSCAD
- PC客户端：WPF (.net framework 4.0)
  - 可根据通讯协议自行开发客户端

## 通讯协议

目前通过`USB CDC`进行通讯。

设备插入后，会被识别为串口设备，设备实例路径为`USB\VID_1A86&PID_5722\ST1E6DIPSF0F0SPI3`

数据包分为两种：

### 包长度小于64字节

这种类型的数据包，将作为`刷新区域设置`+`像素信息`使用，刷新区域的范围为**0~239**，格式如下：

```txt
     1byte         1byte        1byte        1byte         1byte
+-------------+-------------+-----------+--------------+------------+
| pack length | width start | width end | height start | height end |
+-------------+-------------+-----------+--------------+------------+

        1byte                1byte
+-------------------+------------------+------
| rgb565 high 8 bit | rgb565 low 8 bit |......
+-------------------+------------------+------
```

像素数据顺序为从左至右从上至下刷新，每个像素点的颜色为2字节，遵循`rgb565`色彩格式，如下

```txt
    1byte       1byte
+-----------+-----------+
| rrrr rggg | gggb bbbb |
+-----------+-----------+
```

### 包长度等于64字节

64字节数据将全部作为像素数据发送至屏幕

```txt
        1byte               1byte              1byte               1byte
+-------------------+------------------+-------------------+------------------+---
| rgb565 high 8 bit | rgb565 low 8 bit | rgb565 high 8 bit | rgb565 low 8 bit |...
+-------------------+------------------+-------------------+------------------+---
```

## 功能

PC控制端正在制作中

欢迎[提建议](https://github.com/chenxuuu/USB-Screen/issues)或直接参与开发。

