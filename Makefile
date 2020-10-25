FQBN = adafruit:samd:adafruit_trellis_m4

default: build/uilleann.ino.uf2

# uf2conv.py is covered by an MIT license.
build/uf2conv.py:
	mkdir -p build
	curl -L https://raw.githubusercontent.com/microsoft/uf2/master/utils/uf2conv.py > $@
	chmod +x $@

%.uf2: %.bin build/uf2conv.py
	build/uf2conv.py -b 0x4000 -c -o $@ $<

build/%.bin: % *.cpp *.h
	arduino-builder \
		-build-cache ~/.cache/arduino \
		-build-path build \
		-core-api-version 10813 \
		-fqbn $(FQBN) \
		-hardware ~/.arduino15/packages \
		-tools /app/Arduino/tools-builder \
		-tools ~/.arduino15/packages \
		-hardware /app/Arduino/hardware \
		-hardware ~/.arduino15/packages \
		-built-in-libraries /app/Arduino/libraries \
		-libraries ~/Arduino/libraries \
		-compile \
		$<
	
upload: uilleann.ino
	arduino --upload --board $(FQBN) $<
