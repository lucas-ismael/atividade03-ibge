#include <math.h>
#include <stdio.h>
#include <string.h>
#include "atividade.h"

enum { TOTAL = 19 };

/* Coordenadas da tabela da questão 8. Acari fica no índice zero. */
static const Municipio cidades[TOTAL] = {
    {.codigo = 2400109, .nome = "Acari", .latitude = -6.423950, .longitude = -36.639350},
    {.codigo = 2400208, .nome = "Assú", .latitude = -5.592250, .longitude = -37.005600},
    {.codigo = 2400307, .nome = "Afonso Bezerra", .latitude = -5.484150, .longitude = -36.650150},
    {.codigo = 2400406, .nome = "Água Nova", .latitude = -6.208850, .longitude = -38.314950},
    {.codigo = 2400505, .nome = "Alexandria", .latitude = -6.365650, .longitude = -37.984400},
    {.codigo = 2400604, .nome = "Almino Afonso", .latitude = -6.156850, .longitude = -37.748900},
    {.codigo = 2400703, .nome = "Alto do Rodrigues", .latitude = -5.342750, .longitude = -36.687700},
    {.codigo = 2400802, .nome = "Angicos", .latitude = -5.642400, .longitude = -36.516700},
    {.codigo = 2400901, .nome = "Antônio Martins", .latitude = -6.213250, .longitude = -37.920050},
    {.codigo = 2401008, .nome = "Apodi", .latitude = -5.628800, .longitude = -37.865950},
    {.codigo = 2401107, .nome = "Areia Branca", .latitude = -5.021650, .longitude = -37.004300},
    {.codigo = 2401206, .nome = "Arez", .latitude = -6.195250, .longitude = -35.204450},
    {.codigo = 2401404, .nome = "Baía Formosa", .latitude = -6.419300, .longitude = -35.047900},
    {.codigo = 2401453, .nome = "Baraúna", .latitude = -5.081900, .longitude = -37.627650},
    {.codigo = 2401503, .nome = "Barcelona", .latitude = -5.985350, .longitude = -35.933300},
    {.codigo = 2401602, .nome = "Bento Fernandes", .latitude = -5.670900, .longitude = -35.786900},
    {.codigo = 2401651, .nome = "Bodó", .latitude = -5.954100, .longitude = -36.440700},
    {.codigo = 2401701, .nome = "Bom Jesus", .latitude = -6.005900, .longitude = -35.593800},
    {.codigo = 2401800, .nome = "Brejinho", .latitude = -6.215850, .longitude = -35.387900},
};

typedef struct {
    double pesos[TOTAL][TOTAL];
    int caminho[TOTAL];
    int melhorCaminho[TOTAL];
    int visitado[TOTAL];
    double melhorDistancia;
} Caixeiro;

/* Uma rota válida inicial dá à busca um valor para tentar melhorar. */
static void rotaInicial(Caixeiro *dados)
{
    int usados[TOTAL] = {1};
    int atual = 0;
    dados->melhorCaminho[0] = 0;
    for (int pos = 1; pos < TOTAL; pos++) {
        double menor = INFINITY;
        int proximo = 0;
        for (int cidade = 1; cidade < TOTAL; cidade++) {
            if (!usados[cidade] && dados->pesos[atual][cidade] < menor) {
                menor = dados->pesos[atual][cidade];
                proximo = cidade;
            }
        }
        dados->melhorCaminho[pos] = proximo;
        dados->melhorDistancia += menor;
        usados[proximo] = 1;
        atual = proximo;
    }
    dados->melhorDistancia += dados->pesos[atual][0];
}

/* Vértices intermediários ainda precisam de duas arestas.
   A ponta do caminho e Acari precisam de uma cada. Na raiz, Acari precisa de duas.
   Cada aresta futura é contada nos dois extremos, por isso a divisão por dois. */
static double estimativaRestante(const Caixeiro *dados, int atual)
{
    double soma = 0.0;
    for (int i = 0; i < TOTAL; i++) {
        if (dados->visitado[i] && i != atual && i != 0)
            continue;
        double primeira = INFINITY;
        double segunda = INFINITY;
        for (int j = 0; j < TOTAL; j++) {
            if (j == i || (dados->visitado[j] && j != atual && j != 0))
                continue;
            if (dados->pesos[i][j] < primeira) {
                segunda = primeira;
                primeira = dados->pesos[i][j];
            } else if (dados->pesos[i][j] < segunda) {
                segunda = dados->pesos[i][j];
            }
        }
        soma += primeira;
        if (!dados->visitado[i] || atual == 0)
            soma += segunda;
    }
    return soma / 2.0;
}

static void buscarMelhorRota(Caixeiro *dados, int pos, int atual, double parcial)
{
    if (pos == TOTAL) {
        double fechada = parcial + dados->pesos[atual][0];
        if (fechada < dados->melhorDistancia) {
            dados->melhorDistancia = fechada;
            memcpy(dados->melhorCaminho, dados->caminho, sizeof dados->caminho);
        }
        return;
    }
    if (parcial + estimativaRestante(dados, atual) > dados->melhorDistancia + 1e-9)
        return;

    for (int cidade = 1; cidade < TOTAL; cidade++) {
        if (dados->visitado[cidade])
            continue;
        dados->caminho[pos] = cidade;
        dados->visitado[cidade] = 1;
        buscarMelhorRota(dados, pos + 1, cidade, parcial + dados->pesos[atual][cidade]);
        dados->visitado[cidade] = 0;
    }
}

static int gravarMapa(const Caixeiro *dados)
{
    const char *caminho = "build/rota-acari.kml";
    FILE *arquivo = fopen(caminho, "w");
    if (!arquivo) {
        perror(caminho);
        return 0;
    }

    fputs("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
          "<kml xmlns=\"http://www.opengis.net/kml/2.2\"><Document>\n"
          "<name>Atividade 03 — Rota de Acari</name>\n"
          "<Style id=\"rota\"><LineStyle><color>ffbd6321</color>"
          "<width>4</width></LineStyle></Style>\n", arquivo);
    for (int pos = 0; pos < TOTAL; pos++) {
        const Municipio *cidade = &cidades[dados->melhorCaminho[pos]];
        fprintf(arquivo, "<Placemark><name>%d. %s</name><Point><coordinates>"
                "%.6f,%.6f,0</coordinates></Point></Placemark>\n",
                pos + 1, cidade->nome, cidade->longitude, cidade->latitude);
    }
    fputs("<Placemark><name>Percurso geográfico</name><styleUrl>#rota</styleUrl>"
          "<LineString><tessellate>1</tessellate><coordinates>\n", arquivo);
    for (int pos = 0; pos <= TOTAL; pos++) {
        const Municipio *cidade = &cidades[dados->melhorCaminho[pos % TOTAL]];
        fprintf(arquivo, "%.6f,%.6f,0\n", cidade->longitude, cidade->latitude);
    }
    fputs("</coordinates></LineString></Placemark></Document></kml>\n", arquivo);
    if (fclose(arquivo) != 0) {
        perror(caminho);
        return 0;
    }
    printf("Mapa gerado em %s. Importe o KML no Google My Maps ou no Google Earth.\n",
           caminho);
    return 1;
}

int caixeiroViajante(void)
{
    Caixeiro dados = {0};
    for (int i = 0; i < TOTAL; i++) {
        for (int j = i + 1; j < TOTAL; j++) {
            dados.pesos[i][j] = distanciaKm(&cidades[i], &cidades[j]);
            dados.pesos[j][i] = dados.pesos[i][j];
        }
    }

    rotaInicial(&dados);
    dados.visitado[0] = 1;
    buscarMelhorRota(&dados, 1, 0, 0.0);

    puts("\n8. Caixeiro viajante — circuito com saída e chegada em Acari");
    puts("Saída: Acari/RN");
    for (int pos = 1; pos <= TOTAL; pos++) {
        int anterior = dados.melhorCaminho[pos - 1];
        int atual = dados.melhorCaminho[pos % TOTAL];
        printf("%2d. %s → %s — %.2f km\n", pos, cidades[anterior].nome,
               cidades[atual].nome, dados.pesos[anterior][atual]);
    }
    printf("Distância total do circuito: %.2f km\n", dados.melhorDistancia);
    return gravarMapa(&dados);
}
