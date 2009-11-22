include ../global.make

OBJS=gfx.o

LIB=../gfx.a

ifeq ($(GFX_SDL),1)
	OBJS+= gfx_sdl.o
endif

all: $(LIB)($(OBJS))

clean:
	$(RM) $(LIB)
