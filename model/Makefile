default: chanter.stl button-container.stl button-cup.stl

button-%.stl: button-%.scad button.scad
	openscad -o $@ $<

%.stl: %.scad
	openscad -o $@ $<
