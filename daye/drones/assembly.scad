use <tenor/assembly.scad>
use <baritone/assembly.scad>
use <bass/assembly.scad>

translate([10, 10, 0]) drone_tenor();
translate([-10, 10, 0]) drone_baritone();
translate([-10, -10, 0]) drone_bass();
