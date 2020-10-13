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
    cube([ma+4,mc+4,mb+5],true);
    union()
    {
        cube([ma,mc,mb],true);//中间的空间
        translate([0,0,-1.5])//屏幕排线
            cube([ma-10,mc,mb],true);
        
        translate([0,3,0])//前面扣洞
            cube([ma-l2*2,mc,mb-l3*2],true);
        translate([0,3,mb/4+0.5])//前面再扣洞
            cube([ma-l2*2,mc,mb/2-l2*2],true);
        translate([0,-3,0])//背面扣洞
            cube([ma-ll,mc,mb-ll],true);
        
        translate([0,0,5])//上面扣洞
            cube([ma,mc,mb],true);
        translate([0,-3,10])//去掉一个梁
            cube([ma-ll,mc,mb-ll],true);

        translate([0,-3,mb/2+1])//做个槽放盖子
            cube([ma,mc,3.5],true);
    }
}
translate([0,-7.5,0])//背面
    cube([ma+4,2,mb+5],true);

difference()
{
    translate([0,-5,0])//背面
        cube([ma+4,3,mb+5],true);
    translate([0,-5,0])//背面
    union()
    {
        translate([0,0,5])//上面扣洞
            cube([ma,mc,mb+10],true);
        translate([0,0,3])//卡扣
            cube([ma+5,4,4],true);
    }
}

