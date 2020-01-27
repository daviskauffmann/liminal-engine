CXX := g++
CXXFLAGS := -ggdb -std=c++17 -Wall -Wextra -Wpedantic -Wno-unused-parameter -Wno-type-limits `pkg-config --cflags assimp bullet glew openal sdl2 sdl2_image sdl2_mixer`
LDFLAGS := `pkg-config --libs assimp bullet glew openal sdl2 sdl2_image sdl2_mixer` -mconsole

SRC	:= src
EXTERN := extern
BUILD := build
BIN	:= bin

SOURCES := \
	$(SRC)/atlas.cpp \
	$(SRC)/audio.cpp \
	$(SRC)/camera.cpp \
	$(SRC)/cubemap.cpp \
	$(SRC)/directional_light.cpp \
	$(SRC)/display.cpp \
	$(SRC)/main.cpp \
	$(SRC)/material.cpp \
	$(SRC)/mesh.cpp \
	$(SRC)/object.cpp \
	$(SRC)/point_light.cpp \
	$(SRC)/program.cpp \
	$(SRC)/renderer.cpp \
	$(SRC)/sound.cpp \
	$(SRC)/source.cpp \
	$(SRC)/spot_light.cpp \
	$(SRC)/sprite.cpp \
	$(SRC)/stb_image.cpp \
	$(SRC)/stb_include.cpp \
	$(SRC)/terrain.cpp \
	$(SRC)/texture.cpp \
	$(SRC)/vertex.cpp \
	$(SRC)/water.cpp
OBJECTS := $(SOURCES:$(SRC)/%.cpp=$(BUILD)/%.o)
DEPENDENCIES := $(OBJECTS:%.o=%.d)
INCLUDE := -I$(EXTERN)/stb
LIB :=
LIBRARIES := -lopengl32
TARGET := $(BIN)/pk

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJECTS)
	mkdir -p $(@D)
	$(CXX) $^ -o $@ $(LDFLAGS) $(LIB) $(LIBRARIES)

$(BUILD)/%.o: $(SRC)/%.cpp
	mkdir -p $(@D)
	$(CXX) -c $< -o $@ -MMD -MF $(@:.o=.d) $(CXXFLAGS) $(INCLUDE)

-include $(DEPENDENCIES)

.PHONY: run
run: all
	./$(TARGET)

.PHONY: clean
clean:
	rm -rf $(BIN) $(BUILD)
