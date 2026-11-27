#include <math.h>
#include <stdio.h>
#include <string.h>
#include "atividade.h"

enum { MAX_REGIONAL = 1000 };

/* As duas buscas usam a mesma matriz, uma questão por vez. */
static double pesos[MAX_REGIONAL][MAX_REGIONAL];

static int mostrarCaminho(const ListaMunicipios *lista, int origem, int destino,
                          const int anterior[], int comTrechos)
{
    int caminho[MAX_REGIONAL];
    int tamanho = 0;
    for (int atual = destino; atual != -1; atual = anterior[atual]) {
        caminho[tamanho++] = atual;
        if (atual == origem)
            break;
    }
    if (caminho[tamanho - 1] != origem)
        return 0;

    for (int i = tamanho - 1; i >= 0; i--) {
        const Municipio *cidade = &lista->cidades[caminho[i]];
        if (comTrechos && i != tamanho - 1) {
            const Municipio *partida = &lista->cidades[caminho[i + 1]];
            printf("  → %s/%s — %.2f km\n", cidade->nome, cidade->uf,
                   distanciaKm(partida, cidade));
        } else {
            printf("%s/%s%s", cidade->nome, cidade->uf,
                   !comTrechos && i > 0 ? " → " : "\n");
        }
    }
    return 1;
}

static void preencherMatriz(const ListaMunicipios *lista)
{
    int n = lista->total;
    for (int i = 0; i < n; i++) {
        pesos[i][i] = 0.0;
        for (int j = i + 1; j < n; j++) {
            pesos[i][j] = distanciaKm(&lista->cidades[i], &lista->cidades[j]);
            pesos[j][i] = pesos[i][j];
        }
    }
}

int rotaRN(const ListaMunicipios *lista)
{
    static ListaMunicipios rn;
    rn.total = 0;
    for (int i = 0; i < lista->total; i++)
        if (strcmp(lista->cidades[i].uf, "RN") == 0)
            rn.cidades[rn.total++] = lista->cidades[i];

    int origem = indiceMunicipio(&rn, PAU_DOS_FERROS);
    int destino = indiceMunicipio(&rn, NATAL);
    if (origem < 0 || destino < 0)
        return 0;

    int n = rn.total;
    if (n > MAX_REGIONAL)
        return 0;
    preencherMatriz(&rn);

    int fila[MAX_REGIONAL];
    int anterior[MAX_REGIONAL];
    int visitado[MAX_REGIONAL] = {0};
    int inicio = 0;
    int fim = 0;

    for (int i = 0; i < n; i++)
        anterior[i] = -1;
    fila[fim++] = origem;
    visitado[origem] = 1;
    while (inicio < fim && !visitado[destino]) {
        int atual = fila[inicio++];
        for (int vizinho = 0; vizinho < n; vizinho++) {
            if (!visitado[vizinho] && pesos[atual][vizinho] <= 20.0) {
                visitado[vizinho] = 1;
                anterior[vizinho] = atual;
                fila[fim++] = vizinho;
            }
        }
    }

    puts("\n5. Rota de Pau dos Ferros a Natal (BFS, limite de 20 km)");
    if (visitado[destino])
        mostrarCaminho(&rn, origem, destino, anterior, 0);
    else
        puts("As conexões de até 20 km não ligam Pau dos Ferros a Natal.");
    return 1;
}

static double dijkstra(int n, int origem, int destino,
                       int anterior[], int calcularLimite)
{
    double custo[MAX_REGIONAL];
    int visitado[MAX_REGIONAL] = {0};
    for (int i = 0; i < n; i++) {
        custo[i] = INFINITY;
        anterior[i] = -1;
    }
    custo[origem] = 0.0;

    for (int passo = 0; passo < n; passo++) {
        int atual = -1;
        double menor = INFINITY;
        for (int i = 0; i < n; i++) {
            if (!visitado[i] && custo[i] < menor) {
                menor = custo[i];
                atual = i;
            }
        }
        if (atual < 0 || atual == destino)
            break;

        visitado[atual] = 1;
        for (int vizinho = 0; vizinho < n; vizinho++) {
            double trecho = pesos[atual][vizinho];
            if (visitado[vizinho] || (!calcularLimite && trecho > 30.0))
                continue;

            /* O menor limite possível é o menor valor do maior trecho. */
            double novo = calcularLimite ? fmax(custo[atual], trecho)
                                         : custo[atual] + trecho;
            if (novo < custo[vizinho]) {
                custo[vizinho] = novo;
                anterior[vizinho] = atual;
            }
        }
    }
    return custo[destino];
}

int menorRota(const ListaMunicipios *lista)
{
    int origem = indiceMunicipio(lista, PAU_DOS_FERROS);
    int destino = indiceMunicipio(lista, JOAO_PESSOA);
    if (origem < 0 || destino < 0)
        return 0;

    int n = lista->total;
    if (n > MAX_REGIONAL)
        return 0;
    preencherMatriz(lista);

    int anterior[MAX_REGIONAL];
    double total = dijkstra(n, origem, destino, anterior, 0);
    puts("\n6. Menor rota de Pau dos Ferros a João Pessoa (Dijkstra, limite de 30 km)");
    if (isfinite(total)) {
        mostrarCaminho(lista, origem, destino, anterior, 1);
        printf("Distância total: %.2f km\n", total);
    } else {
        puts("Não há caminho com conexões de até 30 km.");
        double limite = dijkstra(n, origem, destino, anterior, 1);
        printf("Menor limite para existir caminho: aproximadamente %.6f km.\n", limite);
    }
    return 1;
}

void maisVizinhos(const ListaMunicipios *lista)
{
    int grau[MAX_MUNICIPIOS] = {0};
    int deOutraUF[MAX_MUNICIPIOS] = {0};

    /* Cada par com distância de até 15 km representa uma aresta do grafo.
       Para esta questão basta acumular os graus dos dois extremos. */
    for (int i = 0; i < lista->total; i++) {
        const Municipio *a = &lista->cidades[i];
        for (int j = i + 1; j < lista->total; j++) {
            const Municipio *b = &lista->cidades[j];
            if (distanciaKm(a, b) <= 15.0) {
                grau[i]++;
                grau[j]++;
                if (strcmp(a->uf, b->uf) != 0) {
                    deOutraUF[i]++;
                    deOutraUF[j]++;
                }
            }
        }
    }

    int maiorGrau[100] = {0};
    for (int i = 0; i < lista->total; i++) {
        int uf = lista->cidades[i].codigo / 100000;
        if (grau[i] > maiorGrau[uf])
            maiorGrau[uf] = grau[i];
    }

    puts("\n7. Município com mais vizinhos por UF (limite de 15 km)");
    puts("UF | Município | Código IBGE | Vizinhos | De outra UF");
    for (int i = 0; i < lista->total; i++) {
        const Municipio *cidade = &lista->cidades[i];
        if (grau[i] == maiorGrau[cidade->codigo / 100000])
            printf("%s | %s | %d | %d | %d\n",
                   cidade->uf, cidade->nome, cidade->codigo, grau[i], deOutraUF[i]);
    }
}
