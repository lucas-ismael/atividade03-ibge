# Atividade 03 — Grafos e localização de municípios

Trabalho em C de Laboratório de Algoritmos e Estruturas de Dados II.

**Integrantes:** José Lucas Ismael Sarmento, Kauan Nascimento Oliveira e Erick Vinicius Lopes de Oliveira.

## Entrega

- [Relatório com os resultados das sete questões](resultados/relatorio-atividade-03.pdf)
- [Municípios com mais vizinhos por UF e empates da questão 7 (CSV)](resultados/questao-7-vizinhos.csv)
- [Mapa do circuito com saída e chegada em Acari, questão 8 (KML)](resultados/rota-acari.kml)

## Como rodar

Na raiz do projeto, compile com GCC e Make no Linux x86-64. A biblioteca do IBGE fornecida pelo professor está em `lib/`.

```sh
make
./build/atividade --offline
```

O modo `--offline` usa o [CSV do projeto](dados/README.md) e reproduz os resultados do relatório. Para rodar uma questão por vez, passe o número. Por exemplo, `./build/atividade --offline 1` calcula a distância entre Pau dos Ferros e Mossoró. A numeração segue o enunciado, que salta da questão 3 para a 5.

Sem `--offline`, o programa consulta o IBGE pela biblioteca e precisa de internet e `curl`. A primeira execução completa pode levar horas. As coordenadas obtidas ficam em `build/consultas-ibge.csv` para as próximas execuções. A questão 8 usa a tabela do enunciado em ambos os modos e grava o mapa em `build/rota-acari.kml`.

## Onde está o código

| Arquivo | O que faz |
| --- | --- |
| `src/main.c` | Seleciona a questão e resolve as questões 1 a 3. |
| `src/municipios.c` | Carrega os municípios e calcula a distância por Haversine. |
| `src/grafo.c` | Faz as buscas de rota e conta os vizinhos de cada município. |
| `src/caixeiro_viajante.c` | Calcula o circuito de Acari e exporta o mapa. |

Em `include/` ficam os cabeçalhos. A pasta `exemplos/` tem dois programas curtos para testar a consulta e a listagem de municípios.

## Material de apoio

- [Enunciado](docs/enunciado.md)
- [Guia da biblioteca IBGE](docs/biblioteca-ibge.md)
- [Fluxogramas](docs/fluxogramas.md)
- [Roteiro de estudo](docs/estudo.md)
- [Validação dos resultados offline](docs/validacao.md)
- [Repositório da biblioteca do professor](https://github.com/kennedyufersa/biblioteca_ibge)
