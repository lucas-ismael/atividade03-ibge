#include <math.h>
#include <stdio.h>
#include <string.h>
#include "atividade.h"

static const char *const ARQUIVO_IBGE = "build/consultas-ibge.csv";
static const char *const ARQUIVO_OFFLINE = "dados/municipios.csv";
enum { TENTATIVAS_IBGE = 3 };

double distanciaKm(const Municipio *a, const Municipio *b)
{
    const double grausParaRadianos = 3.14159265358979323846 / 180.0;
    double latA = a->latitude * grausParaRadianos;
    double latB = b->latitude * grausParaRadianos;
    double meiaLat = sin((latB - latA) / 2.0);
    double meiaLon = sin((b->longitude - a->longitude) * grausParaRadianos / 2.0);
    double h = meiaLat * meiaLat + cos(latA) * cos(latB) * meiaLon * meiaLon;

    /* Evita que o arredondamento deixe 1 - h ligeiramente negativo. */
    if (h > 1.0)
        h = 1.0;
    return 6371.0 * 2.0 * atan2(sqrt(h), sqrt(1.0 - h));
}

int indiceMunicipio(const ListaMunicipios *lista, int codigo)
{
    for (int i = 0; i < lista->total; i++)
        if (lista->cidades[i].codigo == codigo)
            return i;
    return -1;
}

static int lerArquivo(ListaMunicipios *lista, const char *caminho)
{
    FILE *arquivo = fopen(caminho, "r");
    if (!arquivo)
        return 0;

    char linha[256];
    while (fgets(linha, sizeof linha, arquivo)) {
        Municipio cidade = {0};
        if (sscanf(linha, "%d;%2[A-Z];%lf;%lf;%127[^\r\n]",
                   &cidade.codigo, cidade.uf, &cidade.latitude,
                   &cidade.longitude, cidade.nome) != 5)
            continue;
        int indice = indiceMunicipio(lista, cidade.codigo);
        if (indice < 0) {
            if (lista->total == MAX_MUNICIPIOS) {
                fclose(arquivo);
                return 0;
            }
            indice = lista->total++;
        }
        lista->cidades[indice] = cidade;
    }
    int sucesso = !ferror(arquivo);
    fclose(arquivo);
    return sucesso;
}

int consultarMunicipio(int codigo, Municipio *cidade, int offline)
{
    if (offline) {
        static ListaMunicipios dados;
        if (!dados.total && !lerArquivo(&dados, ARQUIVO_OFFLINE)) {
            fprintf(stderr, "Não foi possível ler %s.\n", ARQUIVO_OFFLINE);
            return 0;
        }
        int i = indiceMunicipio(&dados, codigo);
        if (i < 0) {
            fprintf(stderr, "Código %d ausente na base local.\n", codigo);
            return 0;
        }
        *cidade = dados.cidades[i];
        return 1;
    }

    IbgeStatus status;
    for (int tentativa = 1; ; tentativa++) {
        status = ibgeBuscarMunicipio(codigo, cidade);
        /* A biblioteca também pode retornar erro de GPS se a malha não chegar. */
        if (tentativa == TENTATIVAS_IBGE ||
            (status != IBGE_ERRO_CONEXAO && status != IBGE_ERRO_GPS))
            break;
        fprintf(stderr, "IBGE, município %d: %s. Tentando novamente.\n",
                codigo, ibgeStatusMensagem(status));
    }
    if (status != IBGE_OK) {
        /* A API lista Boa Esperança do Norte, mas a consulta pode falhar.
           Nesse caso usamos o ponto da malha oficial incluído no projeto. */
        if (codigo == 5101837 &&
            (status == IBGE_ERRO_RESPOSTA || status == IBGE_ERRO_GPS)) {
            if (consultarMunicipio(codigo, cidade, 1)) {
                fprintf(stderr, "IBGE, município %d: usando a malha oficial local.\n",
                        codigo);
                return 1;
            }
        }
        fprintf(stderr, "IBGE, município %d: %s\n", codigo,
                ibgeStatusMensagem(status));
        return 0;
    }
    return 1;
}

static int ufAceita(int codigo, int apenasRN, int todoBrasil)
{
    int uf = codigo / 100000;
    return todoBrasil || uf == 24 || (!apenasRN && (uf == 23 || uf == 25));
}

static int guardarCodigo(int codigo, const char *nome, void *contexto)
{
    (void)nome;
    ListaMunicipios *lista = contexto;
    if (lista->total == MAX_MUNICIPIOS)
        return 0;
    lista->cidades[lista->total++].codigo = codigo;
    return 1;
}

int carregarMunicipios(ListaMunicipios *lista, int apenasRN, int todoBrasil,
                       int offline)
{
    static ListaMunicipios cache;
    lista->total = 0;
    if (offline) {
        cache.total = 0;
        if (!lerArquivo(&cache, ARQUIVO_OFFLINE) || cache.total == 0) {
            fprintf(stderr, "Não foi possível carregar %s.\n", ARQUIVO_OFFLINE);
            return 0;
        }
        for (int i = 0; i < cache.total; i++)
            if (ufAceita(cache.cidades[i].codigo, apenasRN, todoBrasil))
                lista->cidades[lista->total++] = cache.cidades[i];
        return lista->total > 0;
    }

    /* A listagem entrega os códigos. A consulta individual traz coordenadas. */
    fputs("Consultando a lista de municípios do IBGE...\n", stderr);
    IbgeStatus status;
    for (int tentativa = 1; ; tentativa++) {
        lista->total = 0;
        status = ibgeListarMunicipios(guardarCodigo, lista);
        if (status != IBGE_ERRO_CONEXAO || tentativa == TENTATIVAS_IBGE)
            break;
        fputs("Falha de conexão ao listar os municípios. Tentando novamente.\n", stderr);
    }
    if (status != IBGE_OK) {
        fprintf(stderr, "Falha ao listar os municípios do IBGE: %s.\n",
                ibgeStatusMensagem(status));
        return 0;
    }
    if (lista->total == 0) {
        fputs("O IBGE retornou uma lista de municípios vazia.\n", stderr);
        return 0;
    }

    cache.total = 0;
    lerArquivo(&cache, ARQUIVO_IBGE);
    FILE *arquivo = fopen(ARQUIVO_IBGE, "a");
    if (!arquivo)
        fprintf(stderr, "Não foi possível abrir %s para salvar as consultas.\n",
                ARQUIVO_IBGE);

    int totalCodigos = lista->total;
    lista->total = 0;
    int novas = 0;
    int sucesso = 1;
    for (int i = 0; i < totalCodigos; i++) {
        int codigo = lista->cidades[i].codigo;
        if (!ufAceita(codigo, apenasRN, todoBrasil))
            continue;

        Municipio cidade = {0};
        int salvo = indiceMunicipio(&cache, codigo);
        if (salvo >= 0) {
            cidade = cache.cidades[salvo];
        } else {
            if (!consultarMunicipio(codigo, &cidade, 0)) {
                sucesso = 0;
                break;
            }
            if (arquivo) {
                fprintf(arquivo, "%d;%s;%.17g;%.17g;%s\n", cidade.codigo,
                        cidade.uf, cidade.latitude, cidade.longitude, cidade.nome);
                fflush(arquivo);
            }
            if (++novas % 100 == 0)
                fprintf(stderr, "%d novas consultas ao IBGE concluídas.\n", novas);
        }
        lista->cidades[lista->total++] = cidade;
    }
    if (arquivo)
        fclose(arquivo);
    return sucesso && lista->total > 0;
}
