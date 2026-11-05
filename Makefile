CXX ?= g++
CPPFLAGS += -I.deps/raylib/src
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra
LDLIBS += -lGL -lm -lpthread -ldl -lrt -lX11

RAYLIB_VERSION := 6.0
RAYLIB_DIR := .deps/raylib
RAYLIB_LIB := $(RAYLIB_DIR)/src/libraylib.a
TARGET := build/raylib_window
SOURCES := $(wildcard raylib-quickstart/src/*.cpp)

.PHONY: all run clean
all: $(TARGET)

$(RAYLIB_DIR)/src/Makefile:
	mkdir -p .deps
	git clone --depth 1 --branch $(RAYLIB_VERSION) https://github.com/raysan5/raylib.git $(RAYLIB_DIR)

$(RAYLIB_LIB): $(RAYLIB_DIR)/src/Makefile
	$(MAKE) -C $(RAYLIB_DIR)/src PLATFORM=PLATFORM_DESKTOP RAYLIB_LIBTYPE=STATIC

$(TARGET): $(SOURCES) $(RAYLIB_LIB) Makefile
	mkdir -p build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(SOURCES) $(RAYLIB_LIB) $(LDFLAGS) $(LDLIBS) -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	$(RM) $(TARGET)
