include <button.scad>;

cup();
translate([0, 0, wall]) magnet();

translate([40, 0, 0]) container();
translate([40, 0, wall]) magnet();
