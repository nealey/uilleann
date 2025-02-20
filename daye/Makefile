default: chanter.stl

clean:
	rm -f *.stl

%.stl: %.scad
	openscad -o $@ $<

chanter/chanter-%.stl: chanter/chanter-%.scad chanter/chanter.scad
