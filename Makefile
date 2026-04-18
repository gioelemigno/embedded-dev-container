.PHONY: help build up down restart logs ps clean

# Default target: shows available commands
help:
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@echo "  build    Build or rebuild services"
	@echo "  run      Start containers (up and down)"
	@echo "  clean    Delete volumes"

build:
	USER_UID=$$(id -u) USER_GID=$$(id -g) docker compose build

run:
	xhost +local:docker && \
	USER_UID=$$(id -u) USER_GID=$$(id -g) docker compose up && \
	USER_UID=$$(id -u) USER_GID=$$(id -g) docker compose down

clean:
	docker compose down --volumes