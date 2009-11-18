DEBUG=0

RST2HTML=rst2html.py
RST2LATEX=rst2latex.py
PDFLATEX=pdflatex

RSTFLAGS=-s -g -d
LATEXFLAGS=

ifeq ($(DEBUG),1)
	RSTFLAGS+=-v
endif

HTMLDOCS=README.html audio.html core.html gfx.html input.html io.html lua.html math.html physics.html time.html

LATEXDOCS=$(HTMLDOCS:.html=.tex)

PDFDOCS=$(HTMLDOCS:.html=.pdf)

ADD_RSTFLAGS=
ADD_LATEXFLAGS=
RSTFLAGS+=$(ADD_RSTFLAGS)
LATEXFLAGS+=$(ADD_LATEXFLAGS)

%.html: %
	$(RST2HTML) $(RSTFLAGS) $< > $@

%.html: %.txt
	$(RST2HTML) $(RSTFLAGS) $< > $@

%.tex: %
	$(RST2LATEX) $(RSTFLAGS) $< > $@

%.tex: %.txt
	$(RST2LATEX) $(RSTFLAGS) $< > $@

%.pdf: %.tex
	$(PDFLATEX) $(LATEXFLAGS) $<
	$(PDFLATEX) $(LATEXFLAGS) $<
	$(PDFLATEX) $(LATEXFLAGS) $<
	$(RM) $(@:.pdf=.aux) $(@:.pdf=.log) $(@:.pdf=.toc)

html: $(HTMLDOCS)

latex: $(LATEXDOCS)

pdf: $(PDFDOCS)

all: $(HTMLDOCS) $(PDFDOCS)

clean:
	$(RM) $(HTMLDOCS) $(LATEXDOCS) $(PDFDOCS)

.PHONY: all clean html latex pdf
