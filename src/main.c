#include <stdio.h>
#include <string.h>
#include "atividade.h"

typedef struct {
    int indice;
    double distancia;
} MunicipioDistancia;

static void mostrarMunicipio(const Municipio *cidade)
{
    printf("%s (%d) — latitude %.6f, longitude %.6f\n",
           cidade->nome, cidade->codigo, cidade->latitude, cidade->longitude);
}

static int distanciaEntreCidades(int offline)
{
    Municipio pauDosFerros;
    Municipio mossoro;
    if (!consultarMunicipio(PAU_DOS_FERROS, &pauDosFerros, offline) ||
        !consultarMunicipio(MOSSORO, &mossoro, offline))
        return 0;

    puts("\n1. Distância entre Pau dos Ferros e Mossoró");
    mostrarMunicipio(&pauDosFerros);
    mostrarMunicipio(&mossoro);
    printf("Distância de grande círculo: %.2f km. O cálculo segue a superfície da Terra.\n",
           distanciaKm(&pauDosFerros, &mossoro));
    puts("Para uma viagem por rodovias, a distância depende das estradas usadas.");
    return 1;
}

static int dezMaisProximos(const ListaMunicipios *lista)
{
    int origem = indiceMunicipio(lista, PAU_DOS_FERROS);
    if (origem < 0 || lista->total < 11)
        return 0;

    MunicipioDistancia ranking[MAX_MUNICIPIOS];
    int total = 0;
    for (int i = 0; i < lista->total; i++) {
        if (i == origem)
            continue;

        MunicipioDistancia atual = {
            i, distanciaKm(&lista->cidades[origem], &lista->cidades[i])
        };
        /* Inserção direta: abre espaço para manter o vetor ordenado. */
        int posicao = total;
        while (posicao > 0 && ranking[posicao - 1].distancia > atual.distancia) {
            ranking[posicao] = ranking[posicao - 1];
            posicao--;
        }
        ranking[posicao] = atual;
        total++;
    }

    puts("\n2. Dez municípios mais próximos de Pau dos Ferros (RN, CE e PB)");
    for (int i = 0; i < 10; i++) {
        const Municipio *cidade = &lista->cidades[ranking[i].indice];
        printf("%2d. %s/%s (%d) — %.2f km\n", i + 1, cidade->nome,
               cidade->uf, cidade->codigo, ranking[i].distancia);
    }
    return 1;
}

static int cidadesNoRaio(const ListaMunicipios *lista)
{
    int pau = indiceMunicipio(lista, PAU_DOS_FERROS);
    int mossoro = indiceMunicipio(lista, MOSSORO);
    if (pau < 0 || mossoro < 0)
        return 0;

    puts("\n3. Municípios do RN a até 100 km dos dois pontos");
    puts("Município | até Mossoró | até Pau dos Ferros");
    int encontrados = 0;
    for (int i = 0; i < lista->total; i++) {
        const Municipio *cidade = &lista->cidades[i];
        if (strcmp(cidade->uf, "RN") != 0)
            continue;
        double ateMossoro = distanciaKm(cidade, &lista->cidades[mossoro]);
        double atePau = distanciaKm(cidade, &lista->cidades[pau]);
        if (ateMossoro <= 100.0 && atePau <= 100.0) {
            printf("%s | %.2f km | %.2f km\n", cidade->nome, ateMossoro, atePau);
            encontrados++;
        }
    }
    printf("Municípios encontrados: %d\n", encontrados);
    return 1;
}

int main(int argc, char *argv[])
{
    static ListaMunicipios lista;
    int offline = 0;
    int questao = 0;
    for (int i = 1; i < argc; i++) {
        if (!offline && strcmp(argv[i], "--offline") == 0) {
            offline = 1;
        } else if (!questao && strlen(argv[i]) == 1 && strchr("1235678", argv[i][0])) {
            questao = argv[i][0] - '0';
        } else {
            fprintf(stderr, "Uso: %s [--offline] [1|2|3|5|6|7|8]\n", argv[0]);
            return 1;
        }
    }

    puts("Atividade 03 — Grafos e localização de municípios");
    if (offline)
        puts("Fonte dos dados: dados/municipios.csv (modo offline).");

    if ((questao == 0 || questao == 1) && !distanciaEntreCidades(offline))
        return 1;

    if (questao == 0 || (questao >= 2 && questao <= 7)) {
        /* A execução completa reaproveita a lista nacional nas demais questões. */
        if (!carregarMunicipios(&lista, questao == 3 || questao == 5,
                               questao == 0 || questao == 7, offline))
            return 1;
        printf("Municípios carregados: %d\n", lista.total);
    }

    /* As questões 2 e 6 consideram apenas CE, RN e PB. */
    static ListaMunicipios regional;
    if (questao == 0 || questao == 2 || questao == 6) {
        for (int i = 0; i < lista.total; i++) {
            int uf = lista.cidades[i].codigo / 100000;
            if (uf == 23 || uf == 24 || uf == 25)
                regional.cidades[regional.total++] = lista.cidades[i];
        }
    }

    if (((questao == 0 || questao == 2) && !dezMaisProximos(&regional)) ||
        ((questao == 0 || questao == 3) && !cidadesNoRaio(&lista)) ||
        ((questao == 0 || questao == 5) && !rotaRN(&lista)) ||
        ((questao == 0 || questao == 6) && !menorRota(&regional))) {
        fputs("Não foi possível resolver a questão com os municípios carregados.\n",
              stderr);
        return 1;
    }

    if (questao == 0 || questao == 7)
        maisVizinhos(&lista);
    if (questao == 0 || questao == 8)
        return caixeiroViajante() ? 0 : 1;
    return 0;
}
