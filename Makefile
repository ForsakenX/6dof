# vim:set syntax=make filetype=make:
#
# The contents of this Makefile is in the public domain.

OBJS=

include global.make

PARTS=audio.a gfx.a input.a io.a main.a math.a

BIN=6dof

all: $(PCH) all-no-pch

all-no-pch: $(BIN)

$(BIN): $(PARTS)
	$(CC) $(LDFLAGS) '-Wl,-(' $(PARTS) '-Wl,-)' -o $(BIN) $(LIBS)

$(PARTS): | $(PCH)
	$(MAKE) -C $(@:.a=)

# Is there a better way to do this?
clean:
	$(RM) $(BIN) $(PCH)
	$(MAKE) -C audio clean
	$(MAKE) -C doc clean
	$(MAKE) -C gfx clean
	$(MAKE) -C input clean
	$(MAKE) -C io clean
	$(MAKE) -C main clean
	$(MAKE) -C math clean

doc:
	$(MAKE) -C doc

.PHONY: all all-no-pch clean doc
