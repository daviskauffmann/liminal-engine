CC := gcc
CFLAGS := -ggdb -std=c99 -Wall -Wextra -Wpedantic -Wno-unused-parameter -Wno-type-limits
CXX := g++
CXXFLAGS := -ggdb -std=c++17 -Wall -Wextra -Wpedantic -Wno-unused-parameter
LDFLAGS :=

SRC	:= src
BUILD := build
BIN	:= bin

CSOURCES := $(SRC)/stb_image.c
CXXSOURCES := \
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
	$(SRC)/terrain.cpp \
	$(SRC)/texture.cpp \
	$(SRC)/vertex.cpp \
	$(SRC)/water.cpp
COBJECTS := $(CSOURCES:$(SRC)/%.c=$(BUILD)/%.o)
CXXOBJECTS := $(CXXSOURCES:$(SRC)/%.cpp=$(BUILD)/%.oo)
CDEPENDENCIES := $(COBJECTS:%.o=%.d)
CXXDEPENDENCIES := $(OBJECTS:%.oo=%.dd)
INCLUDE :=
LIB :=
LIBRARIES := -lglew32 -lglu32 -lmingw32 -lopengl32 -lopenal -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer
TARGET := $(BIN)/pk+

.PHONY: all
all: $(TARGET)

$(TARGET): $(COBJECTS) $(CXXOBJECTS)
	mkdir -p $(@D)
	$(CXX) $^ -o $@ $(LDFLAGS) $(LIB) $(LIBRARIES)

$(BUILD)/%.o: $(SRC)/%.c
	mkdir -p $(@D)
	$(CC) -c $< -o $@ -MMD -MF $(@:.o=.d) $(CFLAGS) $(INCLUDE)

-include $(CDEPENDENCIES)

$(BUILD)/%.oo: $(SRC)/%.cpp
	mkdir -p $(@D)
	$(CXX) -c $< -o $@ -MMD -MF $(@:.oo=.dd) $(CXXFLAGS) $(INCLUDE)

-include $(CXXDEPENDENCIES)

.PHONY: run
run: all
	./$(TARGET)

.PHONY: clean
clean:
	rm -rf $(BIN) $(BUILD)
