.PHONY: build start exec stop restart

build:
	docker compose build

start: build
	docker compose up -d

exec: 
	docker compose exec doom-depth bash -c "gcc -o EXEC *.c -std=gnu11 -lsqlite3 && ./EXEC"

stop:
	docker compose down --remove-orphans --volumes --timeout 0

restart: stop start
