TARGET := Chess

BUILD := build
BUILD_DEBUG := $(BUILD)/debug
BUILD_RELEASE := $(BUILD)/release

QMAKE := /opt/Qt/5.15.2/gcc_64/bin/qmake

debug:
	$(QMAKE) \
		Chess.pro -o $(BUILD_DEBUG)/ \
		-spec linux-g++ CONFIG+=debug CONFIG+=qml_debug
	cd $(BUILD_DEBUG) && make -j

run-debug:
	make debug
	./$(BUILD_DEBUG)/$(TARGET)

release:
	$(QMAKE) \
		Chess.pro \
		-o $(BUILD_RELEASE)/ \
		-spec linux-g++ CONFIG+=release CONFIG+=qml_release
	cd $(BUILD_RELEASE) && make -j

run-release:
	make release
	./$(BUILD_RELEASE)/$(TARGET)

clean:
	rm -r $(BUILD)
