.PHONY: build flash

build:
	@idf.py build

flash:
	@idf.py flash
