# vim:set syntax=make filetype=make:
#
# The contents of this Makefile is in the public domain.

OBJS=

include global.make

PARTS=audio.a gfx.a input.a io.a main.a math.a

BIN=6dof

all: common.h.gch all-no-pch

all-no-pch: $(BIN)

$(BIN): $(PARTS)
	$(CC) $(LDFLAGS) $(PARTS) -o $(BIN) $(LIBS)

$(PARTS): | common.h.gch
	$(MAKE) -C $(@:.a=)

clean:
	$(RM) $(PARTS) $(BIN) common.h.gch

doc:
	$(MAKE) -C doc

.PHONY: all all-no-pch clean doc
