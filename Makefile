all: README.html5 README.pdf

%.pdf : %.md
	pandoc $< -s -o $@

%.html5 : %.md
	pandoc $< -s --toc --number-sections -c cmdline.css -o $@

clean:
	rm -f README.pdf README.html5
