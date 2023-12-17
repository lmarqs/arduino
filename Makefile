init: update-index install-dependencies

update-index:
	arduino-cli core update-index --config-file arduino-cli.yaml
	arduino-cli lib  update-index --config-file arduino-cli.yaml

install-dependencies:
	arduino-cli core install esp32:esp32@2.0.14 --config-file arduino-cli.yaml
	arduino-cli core install esp32-bluepad32:esp32@3.10.2 --config-file arduino-cli.yaml
	arduino-cli core install arduino:avr@1.8.6 --config-file arduino-cli.yaml
	arduino-cli lib install --git-url https://github.com/RoboCore/RoboCore_Vespa.git#v1.1.0 --config-file arduino-cli.yaml
	arduino-cli lib install IRremote@4.2.0 --config-file arduino-cli.yaml

run: hexdump compile upload monitor

hexdump:
	find $(SKETCH) -type f -not -name '*.ino' -not -name '*.h' -not -name '*.c' -not -name '*.cpp' -not -name '.skip.*' | awk '{print "xxd --include " $$0 " > " $$0 ".h"}' | sh

compile:
	arduino-cli compile $(SKETCH) --build-path .build/$(SKETCH) --fqbn $(FBQN) --config-file arduino-cli.yaml

upload:
	sudo chmod a+rw $(PORT)
	arduino-cli upload $(SKETCH) --input-dir .build/$(SKETCH) --fqbn $(FBQN) --port $(PORT) --config-file arduino-cli.yaml

monitor:
	sudo chmod a+rw $(PORT)
	arduino-cli monitor --fqbn $(FBQN) --port $(PORT) --config-file arduino-cli.yaml

clean:
	rm -rf build

config-dump:
	arduino-cli config dump
