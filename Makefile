APP_TARGET := Chess
TEST_TARGET := test

BUILD := build
BUILD_DEBUG := $(BUILD)/debug
BUILD_RELEASE := $(BUILD)/release
BUILD_TEST := $(BUILD)/test

INCLUDE := include
SRC:= src
FORMS := forms
RES := res

all: debug release tests cloc

binaries: debug release

debug:
	$(QMAKE) \
		Chess.pro -o $(BUILD_DEBUG)/ \
		-spec linux-g++ \
		CONFIG+=debug CONFIG+=qml_debug
	cd $(BUILD_DEBUG) && make -j$(nproc)
	cp -nr $(RES) $(BUILD_DEBUG)/

run-debug:
	make debug
	./$(BUILD_DEBUG)/$(APP_TARGET)

release:
	$(QMAKE) \
		Chess.pro \
		-o $(BUILD_RELEASE)/ \
		-spec linux-g++ \
		CONFIG+=release CONFIG+=qml_release
	cd $(BUILD_RELEASE) && make -j$(nproc)
	cp -nr $(RES) $(BUILD_RELEASE)/

run-release:
	make release
	./$(BUILD_RELEASE)/$(APP_TARGET)

tests:
	$(QMAKE) \
		test.pro \
		-o $(BUILD_TEST)/ \
		-spec linux-g++ \
		CONFIG+=release CONFIG+=qml_release
	cd $(BUILD_TEST) && make -j$(nproc)
	./$(BUILD_TEST)/$(TEST_TARGET)

cloc:
	cloc $(SRC) $(INCLUDE) $(FORMS)

clean:
	rm -r $(BUILD)
