.PHONY: build flash

build:
	@cd firmware/ && idf.py build

flash:
	@cd firmware/ && idf.py flash
