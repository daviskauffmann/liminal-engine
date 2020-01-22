CC := gcc
CFLAGS := -ggdb -std=c99 -Wall -Wextra -Wpedantic -Wno-unused-parameter -Wno-type-limits
CXX := g++
CXXFLAGS := -ggdb -std=c++17 -Wall -Wextra -Wpedantic -Wno-unused-parameter
LDFLAGS :=

SRC	:= src
DEPS := deps
BUILD := build
BIN	:= bin
EXE := pk

CSOURCES := $(SRC)/stb_image.c
CCSOURCES := \
	$(SRC)/atlas.cc \
	$(SRC)/audio.cc \
	$(SRC)/camera.cc \
	$(SRC)/cubemap.cc \
	$(SRC)/directional_light.cc \
	$(SRC)/display.cc \
	$(SRC)/main.cc \
	$(SRC)/material.cc \
	$(SRC)/mesh.cc \
	$(SRC)/object.cc \
	$(SRC)/point_light.cc \
	$(SRC)/program.cc \
	$(SRC)/renderer.cc \
	$(SRC)/sound.cc \
	$(SRC)/source.cc \
	$(SRC)/spot_light.cc \
	$(SRC)/sprite.cc \
	$(SRC)/terrain.cc \
	$(SRC)/texture.cc \
	$(SRC)/vertex.cc \
	$(SRC)/water.cc
COBJECTS := $(patsubst $(SRC)/%,$(BUILD)/%,$(CSOURCES:.c=.o))
CCOBJECTS := $(patsubst $(SRC)/%,$(BUILD)/%,$(CCSOURCES:.cc=.oo))
CDEPENDENCIES := $(COBJECTS:.o=.d)
CCDEPENDENCIES := $(CCOBJECTS:.oo=.dd)
INCLUDE :=
LIB :=
LIBRARIES := -lglew32 -lglu32 -lmingw32 -lopengl32 -lopenal -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer
TARGET := $(BIN)/$(EXE)

.PHONY: all
all: $(TARGET)

$(TARGET): $(COBJECTS) $(CCOBJECTS)
	mkdir -p $(@D)
	$(CXX) $^ -o $@ $(LDFLAGS) $(LIB) $(LIBRARIES)

$(BUILD)/%.o: $(SRC)/%.c
	mkdir -p $(@D)
	$(CC) -c $< -o $@ -MMD -MF $(@:.o=.d) $(CFLAGS) $(INCLUDE)

-include $(CDEPENDENCIES)

$(BUILD)/%.oo: $(SRC)/%.cc
	mkdir -p $(@D)
	$(CXX) -c $< -o $@ -MMD -MF $(@:.oo=.dd) $(CXXFLAGS) $(INCLUDE)

-include $(CCDEPENDENCIES)

.PHONY: run
run: all
	./$(TARGET)

.PHONY: clean
clean:
	rm -rf $(BIN) $(BUILD)
