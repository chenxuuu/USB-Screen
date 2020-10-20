# USB-Screen

![Build firmware](https://github.com/chenxuuu/USB-Screen/workflows/Build%20firmware/badge.svg)
![Build client](https://github.com/chenxuuu/USB-Screen/workflows/Build%20ProjectCDC/badge.svg)

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
| data length | width start | width end | height start | height end |
+-------------+-------------+-----------+--------------+------------+
       ↓
|<--length-------------------------------......-->|
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

#### `data length`的使用特例

因为`data length`包最长也只能为`59`，所以进行了额外定义：

1. `data length`的最高位`0x80`如果为1，则表示当前模式为黑白模式，此模式下1字节代表8个像素点，设备会将所有数据按黑白模式进行处理

2. `data length`的倒数第二高位`0x40`如果为1，则表示后面的数据全部为像素数据，同时`data length`剩下字节表示后面跟随数据的长度，注意总包长度必须小于64字节

```txt
     1byte
+---------------+
| 0x40 & length |
+---------------+
           ↓
|<-------length-----------------------......-->|
        1byte                1byte
+-------------------+------------------+------
| rgb565 high 8 bit | rgb565 low 8 bit |......
+-------------------+------------------+------
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

