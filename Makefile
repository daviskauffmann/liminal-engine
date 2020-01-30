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
OBJ := $(SRC:src/%.cpp=build/%.o)
DEP := $(OBJ:%.o=%.d)
TGT := bin/pk

.PHONY: all
all: $(TGT)

$(TGT): $(OBJ)
	mkdir -p $(@D)
	$(CXX) $^ -o $@ $(LDFLAGS) $(LDLIBS)

build/%.o: src/%.cpp
	mkdir -p $(@D)
	$(CXX) -c $< -o $@ -MMD -MF $(@:.o=.d) $(CXXFLAGS) $(CPPFLAGS)

-include $(DEP)

.PHONY: run
run: all
	./$(TGT)

.PHONY: clean
clean:
	rm -rf bin build
