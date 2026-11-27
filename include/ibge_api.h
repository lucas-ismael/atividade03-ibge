#ifndef IBGE_API_H
#define IBGE_API_H

#define IBGE_NOME_TAMANHO 128
#define IBGE_SIGLA_UF_TAMANHO 3

/** Dados de um municipio retornados pela API de localidades do IBGE. */
typedef struct {
    int codigo;
    char nome[IBGE_NOME_TAMANHO];
    char microrregiao[IBGE_NOME_TAMANHO];
    char mesorregiao[IBGE_NOME_TAMANHO];
    char uf[IBGE_SIGLA_UF_TAMANHO];
    char regiao[IBGE_NOME_TAMANHO];
    /* Centro da caixa envolvente da malha municipal do IBGE, em WGS 84. */
    double latitude;
    double longitude;
} Municipio;

typedef enum {
    IBGE_OK = 0,
    IBGE_ERRO_ARGUMENTO,
    IBGE_ERRO_CONEXAO,
    IBGE_ERRO_RESPOSTA,
    IBGE_ERRO_GPS,
    IBGE_ERRO_CALLBACK
} IbgeStatus;

/** Funcao chamada para cada municipio recebido pela listagem. */
typedef int (*IbgeMunicipioCallback)(int codigo, const char *nome, void *contexto);

/*
 * Consulta https://servicodados.ibge.gov.br/api/v1/localidades/municipios/{codigo}
 * e preenche `municipio`, incluindo latitude e longitude aproximadas. As
 * coordenadas sao calculadas a partir da caixa envolvente da malha municipal
 * fornecida pelo IBGE. O codigo deve ser o codigo IBGE de sete digitos.
 *
 * A implementacao usa o programa `curl`, que deve estar instalado no sistema.
 */
IbgeStatus ibgeBuscarMunicipio(int codigo, Municipio *municipio);

/**
 * Percorre todos os municipios retornados pela API e chama `callback` para cada
 * um deles. A callback deve retornar valor diferente de zero para continuar.
 */
IbgeStatus ibgeListarMunicipios(IbgeMunicipioCallback callback, void *contexto);

/** Retorna uma descricao curta para um status retornado pelas funcoes do TAD. */
const char *ibgeStatusMensagem(IbgeStatus status);

#endif
