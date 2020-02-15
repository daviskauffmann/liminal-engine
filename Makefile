CXX := g++
CXXFLAGS := -ggdb -Iextern/stb -std=c++17 -Wall -Wextra -Wpedantic -Wno-unused-parameter -Wno-type-limits `pkg-config --cflags assimp bullet glew openal sdl2 sdl2_image sdl2_mixer`
CPPFLAGS :=
LDFLAGS := `pkg-config --libs assimp bullet glew openal sdl2 sdl2_image sdl2_mixer` -mconsole
LDLIBS := -lopengl32

SRC := \
	src/atlas.cpp \
	src/audio.cpp \
	src/camera.cpp \
	src/cubemap.cpp \
	src/directional_light.cpp \
	src/display.cpp \
	src/main.cpp \
	src/material.cpp \
	src/mesh.cpp \
	src/object.cpp \
	src/point_light.cpp \
	src/program.cpp \
	src/renderer.cpp \
	src/skybox.cpp \
	src/sound.cpp \
	src/source.cpp \
	src/spot_light.cpp \
	src/sprite.cpp \
	src/stb_image.cpp \
	src/stb_include.cpp \
	src/terrain.cpp \
	src/texture.cpp \
	src/vertex.cpp \
	src/water.cpp
TARGET := bin/pk

.PHONY: all
all: $(TARGET)

$(TARGET): $(SRC:src/%.cpp=build/%.o)
	mkdir -p $(@D)
	$(CXX) $^ -o $@ $(LDFLAGS) $(LDLIBS)

build/%.o: src/%.cpp
	mkdir -p $(@D)
	$(CXX) -c $< -o $@ -MMD -MF $(@:.o=.d) $(CXXFLAGS) $(CPPFLAGS)

-include $(SRC:src/%.cpp=build/%.d)

.PHONY: run
run: all
	./$(TARGET)

.PHONY: clean
clean:
	rm -rf bin build
