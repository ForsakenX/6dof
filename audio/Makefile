include ../global.make

OBJS=audio.o audio_null.o

LIB=../audio.a

ifeq ($(AUDIO_SDLMIXER),1)
	OBJS+= audio_sdlmixer.o
endif

all: $(LIB)($(OBJS))

clean:
	$(RM) $(LIB)
