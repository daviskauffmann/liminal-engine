CC := gcc
CFLAGS := -ggdb -std=c99 -Wall -Wextra -Wpedantic -Wno-unused-parameter -Wtype-limits
CPP := g++
CPPFLAGS := -ggdb -std=c++17 -Wall -Wextra -Wpedantic -Wno-unused-parameter
LDFLAGS :=

SRC	:= src
DEPS := deps
BUILD := build
BIN	:= bin
EXE := pk

CSOURCES := $(SRC)/stb_image.c
CPPSOURCES := \
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
COBJECTS := $(patsubst $(SRC)/%,$(BUILD)/%,$(CPPSOURCES:.cpp=.oo))
CPPOBJECTS := $(patsubst $(SRC)/%,$(BUILD)/%,$(CSOURCES:.c=.o))
CDEPENDENCIES := $(OBJECTS:.o=.d)
CPPDEPENDENCIES := $(OBJECTS:.oo=.dd)
INCLUDE :=
LIB :=
LIBRARIES := -lglew32 -lglu32 -lmingw32 -lopengl32 -lopenal -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer
TARGET := $(BIN)/$(EXE)

.PHONY: all
all: $(TARGET)

$(TARGET): $(COBJECTS) $(CPPOBJECTS)
	mkdir -p $(@D)
	$(CPP) $^ -o $@ $(LDFLAGS) $(LIB) $(LIBRARIES)

$(BUILD)/%.o: $(SRC)/%.c
	mkdir -p $(@D)
	$(CC) -c $< -o $@ -MMD -MF $(@:.o=.d) $(CFLAGS) $(INCLUDE)

$(BUILD)/%.oo: $(SRC)/%.cpp
	mkdir -p $(@D)
	$(CPP) -c $< -o $@ -MMD -MF $(@:.oo=.dd) $(CPPFLAGS) $(INCLUDE)

-include $(DEPENDENCIES)

.PHONY: run
run: all
	./$(TARGET)

.PHONY: clean
clean:
	rm -rf $(BIN) $(BUILD)
