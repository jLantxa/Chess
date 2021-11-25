APP_TARGET := Chess
TEST_TARGET := test

BUILD := build
BUILD_DEBUG := $(BUILD)/debug
BUILD_RELEASE := $(BUILD)/release
BUILD_TEST := $(BUILD)/test

INCLUDE := include
SRC:= src
FORMS := forms
TEST := test
RES := res

all: debug release doc tests
	@make cloc

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

pack-release:
	cd $(BUILD_RELEASE) && zip -r $(APP_TARGET).zip $(APP_TARGET) $(RES)

tests:
	$(QMAKE) \
		test.pro \
		-o $(BUILD_TEST)/ \
		-spec linux-g++ \
		CONFIG+=release CONFIG+=qml_release
	cd $(BUILD_TEST) && make -j$(nproc)
	./$(BUILD_TEST)/$(TEST_TARGET)

cloc:
	cloc $(SRC) $(INCLUDE) $(FORMS) $(TEST)

format:
	clang-format --style=Google -i \
		$(SRC)/*.cpp \
		$(INCLUDE)/*.h $(INCLUDE)/*.hpp \
		$(TEST)/*.cpp

doc:
	@doxygen

clean:
	rm -r $(BUILD)
