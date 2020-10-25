BOARD = --board adafruit:samd:adafruit_trellis_m4

verify: uilleann.ino
	arduino --verify $(BOARD) $<
	
install: uilleann.ino
	arduino --upload $(BOARD) $<

