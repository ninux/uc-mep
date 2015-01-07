MAINFILE = mc-summary

.PHONY: all tex bib clean

all:
	pdflatex $(MAINFILE).tex
	bibtex $(MAINFILE).aux
	for i in {1..3}; do \
		pdflatex $(MAINFILE).tex; \
	done

tex:	
	for i in {1..3}; do \
		pdflatex $(MAINFILE).tex; \
	done

bib:
	bibtex $(MAINFILE).aux

clean:
	rm *.aux
	rm *.out
	rm *.log
	rm *.toc
	rm *.bbl
	rm *.blg


