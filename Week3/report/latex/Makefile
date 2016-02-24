TEX = $(wildcard *.tex)
PDF = $(TEX:.tex=.pdf)

all : $(PDF)

%.pdf : %.tex
	latex $^ > /dev/null
	latex $^ > /dev/null
	dvips $(basename $^).dvi > /dev/null
	ps2pdf $(basename $^).ps > /dev/null

.PHONY : clean
clean :
	rm -f *.ps *.dvi *.log *.toc *.aux *.pdf *.fdb* *.fls *.out
