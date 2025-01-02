// Based on O'Flynn Rowsome Chanter Measurements
// http://pipers.ie/source/media/?mediaId=31307&galleryId=1353

// Diameter of screw hole
Screwhole = 4; // [1:10]

module metal() {
    color("silver") children();
}

module leather() {
    color("sienna") children();
}

module ivory() {
    color("wheat") children();
}

module wood() {
    color("saddlebrown") children();
}

// A shape like a hamburger patty
module patty(h, d) {
    intersection() {
        cylinder(h=h, d=d);
        translate([0, 0, h/2]) {
            resize([d, d, h*3]) {
                sphere(d=d);
            }
        }
    }
}    

// A cylinder with something like a compression fitting around it
module ringyding(h, id, od) {
    margin = h * 0.1;
    union() {
        leather() cylinder(h=h, d=id);
        translate([0, 0, margin]) ivory() patty(h=h*0.8, d=od);
    }
}

// A fillet is a sort of trumpet bell shape
module fillet(h, d1, d2) {
    r = abs(d1-d2)/2;
    resize([d1, d1, h]) {
        rotate_extrude() {
            translate([d2/2, 0, 0]) {
                difference() {
                    square([r, r]);
                    translate([r, r]) circle(r=r);
                }
            }
        }
    }
}

// An upside-down fillet
module tellif(h, d2, d1) {
    translate([0, 0, h]) mirror([0, 0, 1]) fillet(h, d1, d2);
}

// Absolutely nothing: helps make the code look better
module nothing(h) {
}

// Just a rotated cylinder
// h: height of the *top* of the protrusion
// d: height of the protrusion (diameter?)
// protrusion: amount of protrusion
module bumpout(h, d, protrusion) {
    intersection() {
        translate([0, -protrusion, h-d]) {
            cylinder(h=d, d=20.4);
        }
        translate([0, -protrusion, h-d/2]) {
            sphere(d=protrusion*4);
        }
        translate([0, 0, h-d]) {
            cylinder(h=d, d1=19, d2=50);
        }
    }
}


// A tonehole with :
// * height=h
module tonehole(h) {
    translate([0, 5, h]) {
        rotate(a=90, v=[1, 0, 0]) {
            resize([Screwhole, Screwhole, 100]) {
                cylinder(h=100, d=100);
            }
        }
    }
}

module chanter() {
    difference() {
        union() {
            translate([0, 0,  0]) metal() cylinder(h=22.0, d=17.1);
            translate([0, 0, 22]) wood() cylinder(h=23.5, d=17.1);  // Rings go around this
            
            // Decorative stuff on the bottom
            translate([0, 0, 32]) {          
                translate([0, 0, 0.0]) ivory() patty(h=3.4, d=28.7);
                translate([0, 0, 3.4]) leather() fillet(h=1.8, d1=27, d2=22);
                translate([0, 0, 4.1]) ringyding(h=4.1, id=21, od=24);
                translate([0, 0, 8.2]) ringyding(h=5.3, id=21, od=24);
            }

            // A taper on that bottom ring so it will print nicely
            translate([0, 0, 12]) wood() tellif(h=20, d1=25.5, d2=100);
            
            // Main body
            translate([0, 0, 45.5]) wood() cylinder(h=244.9, d1=20.4, d2=18);
            
            // Top decoration
            translate([0, 0, 290.4]) {
                color("silver") cylinder(h=40.8, d=17);
                
                translate([0, 0,  0.0]) ringyding(h=5.5, id=19, od=21);
                translate([0, 0,  5.5]) nothing(h=9.7); // metal
                translate([0, 0, 15.2]) ringyding(h=4.3, id=18, od=20.7);
                translate([0, 0, 19.5]) nothing(h=6.7); // metal
                translate([0, 0, 26.2]) ivory() patty(h=2, d=20.2);
                translate([0, 0, 28.2]) leather() tellif(h=8, d2=19, d1=23);
                translate([0, 0, 36.2]) ivory() patty(h=4.6, d=25.4);
            }
            
            // This protects the reed and provides a place for tubing to connect
            translate([0, 0, 324.5]) metal() cylinder(h=32.7, d=14.8);
            
            // Bumpouts
            // These angles are my best guess based on photos
            rotate(220) wood() bumpout(161.2, 14.8, 6); // protrusion guessed
        }

        // Inner bore
        union() {
            translate([0, 0, -0.01]) { // Go just a bit past the ends
                translate([0, 0,   0]) cylinder(h=337.01, d1=13.2, d2=5.51);
                translate([0, 0, 337]) cylinder(h=21, d1=5.51, d2=7.1);
            }
        }

        // Tone Holes!
        translate([0, 0, 5]) { // This offset is specified nowhere. I'm guessing to make it fit the bumpouts.
            rotate(180) tonehole(263.0); // back D
            rotate(0) tonehole(246.4); // C♯
            rotate(0) tonehole(216.2); // B
            rotate(0) tonehole(182.0); // A
            rotate(0) tonehole(147.4); //  G
            rotate(0) tonehole(116.2); // F♯
            rotate(0) tonehole(84.7); // E
            rotate(-10) tonehole(53.3); // E♭
        }
    }
}

//intersection() {
    //chanter();
    //cube(400);
//}
chanter();
