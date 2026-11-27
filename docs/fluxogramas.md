# Fluxogramas da atividade

## Execução do programa

```mermaid
flowchart TD
    A([Início]) --> B{Argumentos válidos?}
    B -- Não --> C[Mostra como usar o programa]
    B -- Sim --> D{Questão 1 ou execução completa?}
    D -- Sim --> E[Calcula a distância entre Pau dos Ferros e Mossoró]
    D -- Não --> F{Alguma questão de 2 a 7 foi selecionada?}
    E --> F
    F -- Sim --> G[Carrega a base local ou consulta a biblioteca do IBGE]
    G --> H[Executa as questões selecionadas de 2 a 7]
    F -- Não --> I{Questão 8 selecionada?}
    H --> I
    I -- Sim --> J[Calcula o circuito de Acari e grava o KML]
    I -- Não --> K([Fim])
    J --> K
    C --> K
```

Sem número, o programa executa todas as questões do enunciado. Se faltar um dado necessário ou ocorrer erro de consulta, a execução é interrompida.

## Busca do circuito de Acari

```mermaid
flowchart TD
    A([Início]) --> B[Calcula as distâncias entre os 19 municípios]
    B --> C[Obtém um circuito inicial pelo vizinho mais próximo]
    C --> D[Começa a busca em Acari]
    D --> E{Visitou os 19 municípios?}
    E -- Sim --> F[Soma a volta a Acari e guarda o circuito se for menor]
    E -- Não --> G{O menor custo possível já supera o melhor circuito?}
    G -- Sim --> H[Volta à chamada anterior]
    G -- Não --> I[Tenta cada município ainda não visitado]
    I --> E
    F --> H
```

Ao voltar de uma tentativa, a busca desmarca o município para poder usá-lo em outra ordem. O melhor circuito encontrado é usado na saída e no mapa.

## Consulta de exemplo em `exemplos/consultar_municipio.c`

```mermaid
flowchart TD
    A([Início]) --> B(argc == 2 e lerCodigo retorna sucesso?)
    B -- Não --> C[Exibe mensagem de uso]
    C --> D([Retorna 1])
    B -- Sim --> E[Chama ibgeBuscarMunicipio]
    E --> F(status == IBGE_OK?)
    F -- Não --> G[Exibe mensagem de erro com ibgeStatusMensagem]
    G --> H([Retorna 1])
    F -- Sim --> I[Exibe código, município, UF, região,<br/>latitude e longitude]
    I --> J([Retorna 0])
    classDef decisao fill:#ffffff,stroke:#3b82f6,stroke-width:1px,stroke-dasharray:2 2,color:#1d4ed8;
    class B,F decisao;
```

Legenda:

- **Argumentos válidos?**: verifica se foi informado exatamente um argumento e se `lerCodigo` conseguiu convertê-lo em um código válido.
- **Status == IBGE_OK?**: verifica se a consulta ao município foi concluída com sucesso.

## `ibgeBuscarMunicipio`

```mermaid
flowchart TD
    A([Início]) --> B(municipio == NULL ou código fora do intervalo válido?)
    B -- Sim --> C([Retorna IBGE_ERRO_ARGUMENTO])
    B -- Não --> D[Limpa a estrutura Municipio]
    D --> E[Monta comando curl da API de localidades]
    E --> F[Abre pipe com popen]
    F --> G(pipe == NULL?)
    G -- Sim --> H([Retorna IBGE_ERRO_CONEXAO])
    G -- Não --> I[Lê resposta e fecha pipe]
    I --> J(codigoSaida != 0 ou lidos == 0?)
    J -- Sim --> H
    J -- Não --> K[Chama extrairMunicipio]
    K --> L(extrairMunicipio retornou sucesso?)
    L -- Não --> M([Retorna IBGE_ERRO_RESPOSTA])
    L -- Sim --> N[Chama buscarLocalizacaoGps]
    N --> O([Retorna status da localização GPS])
    classDef decisao fill:#ffffff,stroke:#3b82f6,stroke-width:1px,stroke-dasharray:2 2,color:#1d4ed8;
    class B,G,J,L decisao;
```

Legenda:

- **municipio == NULL ou código fora do intervalo válido?**: verifica se os argumentos são inválidos.
- **pipe == NULL?**: verifica se não foi possível iniciar o comando `curl`.
- **codigoSaida != 0 ou lidos == 0?**: verifica se o `curl` falhou ou retornou uma resposta vazia.
- **extrairMunicipio retornou sucesso?**: verifica se a resposta contém os dados do município em formato reconhecido.

## `ibgeListarMunicipios`

```mermaid
flowchart TD
    A([Início]) --> B(Callback foi informada?)
    B -- Não --> C([Retorna erro de argumento])
    B -- Sim --> D[Consulta a lista de municípios na API do IBGE]
    D --> E(Consulta realizada?)
    E -- Não --> F([Retorna erro de conexão])
    E -- Sim --> G[Processa cada município recebido]
    G --> H(Próximo município disponível?)
    H -- Sim --> I[Chama a callback com os dados do município]
    I --> J(Continuar listagem?)
    J -- Sim --> G
    J -- Não --> K([Retorna listagem interrompida])
    H -- Não --> L([Retorna sucesso])
    classDef decisao fill:#ffffff,stroke:#3b82f6,stroke-width:1px,stroke-dasharray:2 2,color:#1d4ed8;
    class B,E,H,J decisao;
```

Legenda:

- **Callback foi informada?**: verifica se há uma função para receber os municípios listados.
- **Consulta realizada?**: verifica se foi possível obter a lista na API.
- **Próximo município disponível?**: verifica se ainda há municípios para processar.
- **Continuar listagem?**: a callback determina se a listagem continua ou é interrompida.

## `ibgeStatusMensagem`

```mermaid
flowchart TD
    A([Início]) --> B(Qual status?)
    B -- IBGE_OK --> C[Retorna: sucesso]
    B -- IBGE_ERRO_ARGUMENTO --> D[Retorna: argumento invalido]
    B -- IBGE_ERRO_CONEXAO --> E[Retorna: falha ao consultar a API do IBGE]
    B -- IBGE_ERRO_RESPOSTA --> F[Retorna: resposta da API em formato inesperado]
    B -- IBGE_ERRO_GPS --> G[Retorna: falha ao obter a localizacao GPS pela malha do IBGE]
    B -- IBGE_ERRO_CALLBACK --> H[Retorna: a listagem foi interrompida]
    B -- Outro valor --> I[Retorna: status desconhecido]
    classDef decisao fill:#ffffff,stroke:#3b82f6,stroke-width:1px,stroke-dasharray:2 2,color:#1d4ed8;
    class B decisao;
```

Legenda:

- **Qual status?**: seleciona a mensagem correspondente ao valor recebido em `status`; valores não reconhecidos produzem a mensagem de status desconhecido.
