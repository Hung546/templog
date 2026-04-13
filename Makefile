.PHONY: build flash server dev pkg web app

build:
	@cd firmware/ && idf.py build

flash:
	@cd firmware/ && idf.py flash

server:
	@go run ./cmd/main.go

web:
	@cd web && ~/.bun/bin/bun run dev

app:
	@make server & cd web && bun run dev


pkg:
	@go mod tidy