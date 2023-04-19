// This is the button on the bottom of the chanter.
// You put a magnet in the bottom of it,
// then jam a unipolar linear hall effect sensor in the body,
// and you have a cheap and reliable way to detect insertion depth.

// Make circles lovely and round
$fa = 1; $fs = 0.5;

magnet_od = 17.5; // Outside diameter of the magnet you're using
magnet_id = 7;
magnet_h = 6; // I use two ceramic magnets stacked together, to increase the field strength
throw = 15; // How far you can depress the button
gap = 2; // How much space we need for the gap. OH59E is 1.57mm thick
wall = 1.6; // Wall depth
leg_angle = 30; // How wide a leg should be, in degrees
tab_w = 2; // Tab height

module magnet() {
    color("SlateGray") {
        difference() {
            cylinder(h=magnet_h, d=magnet_od);
            translate([0, 0, -0.1]) cylinder(h=magnet_h+0.2, d=magnet_id);
        }
    }
}

// Cup to hold the magnets on the bottom
magnet_cup_tolerance = 1;
cup_id = magnet_od + magnet_cup_tolerance;
cup_od = cup_id + wall*2;
cup_leg_h = wall + throw;
peg_d = magnet_id - 0.3;

module cup() {
    union() {
        cylinder(h=wall, d=cup_od);

        // A peg in the middle to make it easier to assemble.
        // This goes all the way so we can print without supports
        cylinder(h=wall+magnet_h, d=peg_d);

        // The tabs on the sides
        for (i = [0, 120, 240]) {
            rotate(i) {
                rotate_extrude(angle=leg_angle) {
                    translate([cup_id/2, 0, 0]) square([wall, cup_leg_h]);
                }
            }
            rotate(i + leg_angle*0.1) {
                rotate_extrude(angle=leg_angle*0.8) {
                    translate([cup_od/2, cup_leg_h-tab_w, 0]) square(tab_w);
                }
            }
        }
    }
}

// The thing the cup slides into
cup_container_tolerance = 0.5;
container_id = cup_od + cup_container_tolerance;
container_od = container_id + wall*4;
container_h = wall + throw + gap + wall; // Height of the container that the cup goes into

module channel(h) {
    rotate_extrude(angle=leg_angle) {
        translate([cup_od/2, 0, 0]) square([tab_w, h]);
    }
}
module container() {
    difference() {
        union() {
            difference() {
                cylinder(h=container_h, d=container_od);

                // Carve out space for the cup
                translate([0, 0, wall]) cylinder(h=container_h, d=container_id);
            
                // Carve out channels for the tabs
                channel_w = tab_w + 0.5;
                for(i = [0, 120, 240]) {
                    translate([0, 0, wall]) {
                        // Channel for the throw
                        rotate(i) channel(throw + wall);

                        // Channel for the screw-in.
                        // This is going to droop when printing, so we make it a bit wider
                        rotate(i+leg_angle) channel(tab_w*1.5);

                        // Channel for insertion
                        rotate(i+leg_angle+leg_angle) channel(container_h);
                    }
                }
            }
            // Another post to help center magnets
            cylinder(h=wall+magnet_h, d=peg_d);
        }

        // Now drill holes for the sensors
        for (x = [-1.27, 0, 1.27]) {
            translate([x, 2, 0]) cube([0.8,0.8, container_h], center=true);
        }
    }
}
