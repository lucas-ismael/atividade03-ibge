# Validação da atividade

A conferência foi feita com a base local de municípios. Para reproduzir todos os resultados, rode na raiz do projeto.

```sh
make
./build/atividade --offline
```

## Distâncias e municípios

- Questão 1. A distância de grande círculo entre Pau dos Ferros e Mossoró foi de 135,86 km, como no relatório.
- Questão 2. A base carregou 5.571 registros. A lista retornou dez municípios, começando por Francisco Dantas, a 9,38 km de Pau dos Ferros.
- Questão 3. O programa encontrou oito municípios do RN a até 100 km de Pau dos Ferros e de Mossoró.

## Rotas e vizinhança

- Questão 5. Com ligações de até 20 km, a busca em largura não encontrou rota entre Pau dos Ferros e Natal.
- Questão 6. Com ligações de até 30 km, Dijkstra encontrou uma rota de 18 trechos até João Pessoa, somando 391,06 km. O total confere com o relatório.
- Questão 7. As 380 linhas da saída, cobrindo as 27 UFs, coincidiram com o [CSV da entrega](../resultados/questao-7-vizinhos.csv). A comparação incluiu os empates e a contagem de vizinhos de outra UF.
