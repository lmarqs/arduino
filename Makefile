init: update-index install-dependencies

update-index:
	arduino-cli core update-index --config-file arduino-cli.yaml
	arduino-cli lib  update-index --config-file arduino-cli.yaml

install-dependencies:
	arduino-cli core install arduino:avr@1.8.6 --config-file arduino-cli.yaml
	arduino-cli core install digistump:avr@1.6.7 --config-file arduino-cli.yaml
	arduino-cli core install esp32-bluepad32:esp32@3.10.2 --config-file arduino-cli.yaml
	arduino-cli core install esp32:esp32@2.0.14 --config-file arduino-cli.yaml
	arduino-cli lib install IRremote@4.2.0 --config-file arduino-cli.yaml
	arduino-cli lib install LiquidCrystal@1.0.7 --config-file arduino-cli.yaml
	arduino-cli lib install 'RoboCore - Vespa@1.1.0' --config-file arduino-cli.yaml
	arduino-cli lib install Servo@1.2.1 --config-file arduino-cli.yaml
	arduino-cli lib install ESP32Servo@1.1.1 --config-file arduino-cli.yaml
	arduino-cli lib install WiFiManager@v2.0.16-rc.2 --config-file arduino-cli.yaml

run: compile upload monitor

hexdump:
	cd $(SKETCH); find data -type f -not -name '*.h' | awk '{print "xxd --include " $$0 " > " $$0 ".h"}' | sh

compile:
	arduino-cli compile $(SKETCH) --build-path .build/$(SKETCH) --fqbn $(FBQN) --library library --config-file arduino-cli.yaml

filesystem.spiffs:
	sudo chmod a+rw $(PORT)
	# values from ./.arduino15/data/packages/esp32/hardware/esp32/2.0.14/tools/partitions/default.csv
	.arduino15/data/packages/esp32/tools/mkspiffs/0.2.3/mkspiffs --create $(SKETCH)/data --page 256 --block 4096 --size 917504 .build/$(SKETCH)/filesystem.spiffs
	python .arduino15/data/packages/esp32/tools/esptool_py/4.5.1/esptool.py --baud 460800 --port $(PORT) --before default_reset --after hard_reset write_flash 0x310000 .build/$(SKETCH)/filesystem.spiffs

upload:
	sudo chmod a+rw $(PORT)
	arduino-cli upload $(SKETCH) --input-dir .build/$(SKETCH) --fqbn $(FBQN) --port $(PORT) --config-file arduino-cli.yaml

monitor:
	sudo chmod a+rw $(PORT)
	arduino-cli monitor --config baudrate=115200 --fqbn $(FBQN) --port $(PORT) --config-file arduino-cli.yaml

monitor-describe:
	sudo chmod a+rw $(PORT)
	arduino-cli monitor --describe --fqbn $(FBQN) --port $(PORT) --config-file arduino-cli.yaml

clean:
	rm -rf build

config-dump:
	arduino-cli config dump
