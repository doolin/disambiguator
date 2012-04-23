
INKSCAPE = /Applications/Inkscape.app/Contents/Resources/bin/inkscape

PDFS = algorithm.pdf

pdfs: $(PDFS)

%.pdf : %.svg
	$(INKSCAPE) --export-area-drawing -f $< --export-pdf $@


clean:
	rm -rf *~ *.pdf
