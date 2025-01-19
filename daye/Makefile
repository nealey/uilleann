default: chanter.stl

clean:
	rm -f *.stl

%.stl: %.scad
	openscad -o $@ $<
