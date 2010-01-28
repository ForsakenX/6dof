# vim:set syntax=make filetype=make:
#
# The contents of this Makefile is in the public domain.

# Perhaps these should later be moved to a separate configuration file.
DEBUG=0
STRICT=0

AUDIO_SDLMIXER=0
GFX_SDL=1
INPUT_SDL=1
SCALAR_PRECISION=0

CC=gcc

# Common compile/link flags
FLAGS=-pipe -march=native
CFLAGS=$(FLAGS)
LDFLAGS=$(FLAGS)
ifeq ($(DEBUG),1)
	FLAGS+=-g
	CFLAGS+=-Wall -Wcast-qual -Wdeclaration-after-statement -Wfloat-equal -Wextra -Wmissing-prototypes -Wswitch-default -Wunreachable-code -Wunsafe-loop-optimizations
else
	CFLAGS+=-O2
	LDFLAGS+=-s
endif
ifeq ($(STRICT),1)
	CFLAGS+=-Wconversion -Wsign-conversion
endif
LIBS=-lm -llua

# Set options to indicate whether SDL and/or OpenGL are used.
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

# Additional compile/link options based on settings.
ifeq ($(SDL),1)
	CFLAGS+= -DSDL
	LIBS+= -lSDL
endif
ifeq ($(OPENGL),1)
	CFLAGS+= -DOPENGL
	LIBS+= -lGL -lGLU
endif
ifeq ($(AUDIO_SDLMIXER),1)
	CFLAGS+= -DAUDIO_SDLMIXER
	LIBS+= -lSDL_mixer
endif
ifeq ($(INPUT_SDL),1)
	CFLAGS+= -DINPUT_SDL
endif

ifeq ($(SCALAR_PRECISION),0)
	CFLAGS+= -DSCALAR_FLOAT
else ifeq ($(SCALAR_PRECISION),1)
	CFLAGS+= -DSCALAR_DOUBLE
else ifeq ($(SCALAR_PRECISION),2)
	CFLAGS+= -DSCALAR_LONG_DOUBLE
endif

ifeq ($(MAKELEVEL),0)
	INCLDIR=include
else
	INCLDIR=../include
endif

ADD_FLAGS=
ADD_CFLAGS=
ADD_LDFLAGS=
FLAGS+=$(ADD_FLAGS)
CFLAGS+=$(ADD_CFLAGS)
LDFLAGS+=$(ADD_LDFLAGS)

INCLUDE:=audio.h common.h config.h debug.h endian.h game.h gfx.h global.h input.h lua_binread.h mem.h model.h quaternion.h scalar.h time.h types.h vector.h

PCH=$(INCLDIR)/common.h.gch

INCLUDE:=$(patsubst %,$(INCLDIR)/%,$(INCLUDE))

$(OBJS): $(INCLUDE) | $(PCH)

$(PCH): $(INCLUDE)
	$(CC) $(CFLAGS) -o $(PCH) $(INCLDIR)/common.h

.DEFAULT_GOAL := all
