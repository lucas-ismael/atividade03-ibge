CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2 -Iinclude
FONTES = src/main.c src/municipios.c src/grafo.c src/caixeiro_viajante.c

build/atividade: $(FONTES) include/ibge_api.h include/atividade.h lib/libibge.so
	mkdir -p build
	$(CC) $(CFLAGS) $(FONTES) -Llib -libge -lm -Wl,-rpath,'$$ORIGIN/../lib' -o $@

.PHONY: executar
executar: build/atividade
	./build/atividade
