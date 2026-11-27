#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ibge_api.h"

static int lerCodigo(const char *texto, int *codigo)
{
    if (strlen(texto) != 7 || strspn(texto, "0123456789") != 7 ||
        texto[0] == '0') {
        return 0;
    }
    *codigo = (int)strtol(texto, NULL, 10);
    return 1;
}

int main(int argc, char *argv[])
{
    Municipio municipio;
    IbgeStatus status;
    int codigo;

    if (argc != 2 || !lerCodigo(argv[1], &codigo)) {
        fprintf(stderr, "Uso: %s <código IBGE de 7 dígitos>\n", argv[0]);
        return 1;
    }

    status = ibgeBuscarMunicipio(codigo, &municipio);
    if (status != IBGE_OK) {
        fprintf(stderr, "Erro na consulta: %s\n",
                ibgeStatusMensagem(status));
        return 1;
    }

    printf("Código: %d\n", municipio.codigo);
    printf("Município: %s\n", municipio.nome);
    printf("UF: %s\n", municipio.uf);
    printf("Região: %s\n", municipio.regiao);
    printf("Latitude aproximada: %.6f\n", municipio.latitude);
    printf("Longitude aproximada: %.6f\n", municipio.longitude);
    return 0;
}
