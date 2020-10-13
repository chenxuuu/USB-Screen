//长宽高
ma=32.3;//横着
mb=34.3;//竖着
mc=4.2;//厚度

l1=1.6;//屏幕顶部
l2=2;//两边
l3=4.8;//屏幕底部

ll=3;//默认槽宽


difference()
{
    union()
    {
        translate([0,-2,mb/2+1])
            cube([ma-0.4,mc+4-0.2,2.8],true);
        difference()//两边的脚
        {
            translate([0,-5,mb/2-7])
                cube([ma+2,2,15],true);
            translate([0,-5,mb/2-7])
                cube([ma-3,2.1,18],true);
        }
    }
    union()
    {
        include<usb_screen.scad>;
        translate([6,-2.5,5])
            cube([12,5,mb],true);//usb口
    }
}