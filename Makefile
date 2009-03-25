# Perhaps these should later be moved to a separate configuration file.
DEBUG=0
AUDIO_SDLMIXER=0
GFX_SDL=1
INPUT_SDL=1
SCALAR_PRECISION=0

CC=gcc

FLAGS=-pipe
CFLAGS=$(FLAGS)
LDFLAGS=$(FLAGS)
ifeq ($(DEBUG),1)
	FLAGS+=-g
	CFLAGS+=-Wall
else
	CFLAGS+=-O2
	LDFLAGS+=-s
endif
LIBS=-lm -llua

SDL=0
OPENGL=0
ifeq ($(AUDIO_SDLMIXER),1)
	SDL=1
endif
ifeq ($(GFX_SDL),1)
	SDL=1
	OPENGL=1
endif
ifeq ($(INPUT_SDL),1)
	SDL=1
endif

ifeq ($(SDL),1)
	CFLAGS+= -DSDL
	LIBS+= -lSDL
endif

ifeq ($(OPENGL),1)
	CFLAGS+= -DOPENGL
	LIBS+= -lGL -lGLU
endif

OBJS=audio.o audio_null.o config.o file.o game.o gfx.o global.o input.o lua_funcs.o main.o model.o quaternion.o time.o vector.o
ifeq ($(AUDIO_SDLMIXER),1)
	CFLAGS+= -DAUDIO_SDLMIXER
	OBJS+= audio_sdlmixer.o
	LIBS+= -lSDL_mixer
endif
ifeq ($(GFX_SDL),1)
	OBJS+= gfx_sdl.o
endif
ifeq ($(GFX_SDL),1)
	CFLAGS+= -DINPUT_SDL
	OBJS+= input_sdl.o
endif

ifeq ($(SCALAR_PRECISION),0)
	CFLAGS+= -DSCALAR_FLOAT
else ifeq ($(SCALAR_PRECISION),1)
	CFLAGS+= -DSCALAR_DOUBLE
else ifeq ($(SCALAR_PRECISION),2)
	CFLAGS+= -DSCALAR_LONG_DOUBLE
endif

ADD_FLAGS=
ADD_CFLAGS=
ADD_LDFLAGS=
FLAGS+=$(ADD_FLAGS)
CFLAGS+=$(ADD_CFLAGS)
LDFLAGS+=$(ADD_LDFLAGS)

BIN=6dof

all: common.h.gch all-no-pch

all-no-pch: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(BIN) $(LIBS)

$(OBJS): | common.h.gch

common.h.gch: audio.h common.h config.h file.h game.h gfx.h global.h input.h lua_funcs.h model.h quaternion.h scalar.h types.h vector.h
	$(CC) $(CFLAGS) -o common.h.gch common.h

clean:
	$(RM) $(OBJS) $(BIN) common.h.gch luafuncs.so README.html

luafuncs: luafuncs.so

luafuncs.so: lua_funcs.c
	$(CC) -shared -fPIC -pipe -O2 $(ADD_FLAGS) -DSTANDALONE -s -o luafuncs.so lua_funcs.c -llua

README.html: README
	rst2html.py -s -v -g -d README > README.html

.PHONY: luafuncs clean
