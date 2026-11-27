#ifndef ATIVIDADE_H
#define ATIVIDADE_H

#include "ibge_api.h"

enum {
    MAX_MUNICIPIOS = 6000,
    PAU_DOS_FERROS = 2409407,
    MOSSORO = 2408003,
    NATAL = 2408102,
    JOAO_PESSOA = 2507507
};

typedef struct {
    Municipio cidades[MAX_MUNICIPIOS];
    int total;
} ListaMunicipios;

double distanciaKm(const Municipio *a, const Municipio *b);
int indiceMunicipio(const ListaMunicipios *lista, int codigo);
int carregarMunicipios(ListaMunicipios *lista, int apenasRN, int todoBrasil,
                       int offline);
int consultarMunicipio(int codigo, Municipio *cidade, int offline);

int rotaRN(const ListaMunicipios *lista);
int menorRota(const ListaMunicipios *lista);
void maisVizinhos(const ListaMunicipios *lista);
int caixeiroViajante(void);

#endif
