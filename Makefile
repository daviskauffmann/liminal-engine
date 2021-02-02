PKGS = assimp bullet glew lua openal sdl2 sdl2_image sdl2_mixer

CXX = g++
CXXFLAGS = -ggdb -Iextern/imgui -Iextern/imgui/backends -Iextern/stb -std=c++17 -Wall -Wextra -Wpedantic -Wno-unused-parameter -Wno-type-limits `pkg-config --cflags $(PKGS)`
CPPFLAGS =
LDFLAGS = `pkg-config --libs $(PKGS)` -mconsole
LDLIBS = -lopengl32

SRC = \
	src/atlas.cpp \
	src/audio.cpp \
	src/camera.cpp \
	src/cubemap.cpp \
	src/directional_light.cpp \
	src/imgui.cpp \
	src/main.cpp \
	src/mesh.cpp \
	src/model.cpp \
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
	src/water.cpp
TARGET = bin/pk

.PHONY: all
all: $(TARGET)

$(TARGET): $(SRC:src/%.cpp=obj/%.o)
	@mkdir -p $(@D)
	$(CXX) $^ -o $@ $(LDFLAGS) $(LDLIBS)

obj/%.o: src/%.cpp
	@mkdir -p $(@D)
	@mkdir -p $(@D:obj%=dep%)
	$(CXX) -c $< -o $@ -MMD -MF $(@:obj/%.o=dep/%.d) $(CXXFLAGS) $(CPPFLAGS)

-include $(SRC:src/%.cpp=dep/%.d)

.PHONY: run
run: all
	./$(TARGET)

.PHONY: clean
clean:
	rm -rf bin obj dep
