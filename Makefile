FQBN = adafruit:samd:adafruit_trellis_m4
UF2_MOUNT = /media/neale/TRELM4BOOT
ARDUINO_DIR = /opt/arduino-1.8.13

default: build/uilleann.ino.uf2
install: build/uilleann.ino.uf2
	./install.sh $< $(UF2_MOUNT)

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
		-tools $(ARDUINO_DIR)/tools-builder \
		-tools ~/.arduino15/packages \
		-hardware $(ARDUINO_DIR)/hardware \
		-hardware ~/.arduino15/packages \
		-built-in-libraries $(ARDUINO_DIR)/libraries \
		-libraries ~/Arduino/libraries \
		-compile \
		$<
	
upload: uilleann.ino
	arduino --upload --board $(FQBN) $<
