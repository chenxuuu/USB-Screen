difference()
{
    cube([2.2,0.6,2.2],true);
    union()
    {
        cube([2,0.4,2],true);
        translate([0,0.2,0])
            cube([1.8,0.6,1.8],true);
    }
}