include ../global.make

OBJS=input.o

LIB=../input.a

ifeq ($(INPUT_SDL),1)
	OBJS+= input_sdl.o
endif

all: $(LIB)($(OBJS))

clean:
	$(RM) $(LIB)
