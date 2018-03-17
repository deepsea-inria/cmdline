all: README.html README.pdf

%.pdf : %.md
	pandoc $< -s -o $@

%.html : %.md
	pandoc $< -s --toc --number-sections -c cmdline.css -o $@

clean:
	rm -f README.pdf README.html
