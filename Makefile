PKGS = assimp bullet glew lua openal sdl2 sdl2_image sdl2_mixer

CXX = g++
CXXFLAGS = -ggdb -Iextern/entt/single_include -Iextern/imgui -Iextern/imgui/backends -Iextern/json/single_include -Iextern/stb -std=c++17 -Wall -Wextra -Wpedantic -Wno-unused-parameter -Wno-type-limits `pkg-config --cflags $(PKGS)`
CPPFLAGS =
LDFLAGS = `pkg-config --libs $(PKGS)` -mconsole
LDLIBS = -lopengl32

SRC = \
	src/audio/sound.cpp \
	src/components/audio_source.cpp \
	src/components/directional_light.cpp \
	src/components/point_light.cpp \
	src/components/script.cpp \
	src/components/spot_light.cpp \
	src/components/sprite.cpp \
	src/components/terrain.cpp \
	src/components/transform.cpp \
	src/components/water.cpp \
	src/core/assets.cpp \
	src/core/platform.cpp \
	src/core/scene.cpp \
	src/graphics/camera.cpp \
	src/graphics/cubemap.cpp \
	src/graphics/mesh.cpp \
	src/graphics/model.cpp \
	src/graphics/program.cpp \
	src/graphics/renderer.cpp \
	src/graphics/skybox.cpp \
	src/graphics/texture.cpp \
	src/graphics/vertex.cpp \
	src/imgui.cpp \
	src/main.cpp \
	src/stb_image.cpp \
	src/stb_include.cpp
TARGET = bin/liminal

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
