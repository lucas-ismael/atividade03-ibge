# Roteiro de estudo · Atividade 03

## Ordem para entender o código

1. Abra `src/main.c` e acompanhe o `main`. O argumento escolhe uma questão. Sem número, o programa executa todas.
2. Leia `distanciaKm` em `municipios.c`. As coordenadas chegam em graus. A função converte para radianos antes de chamar seno e cosseno.
3. Leia `dezMaisProximos`. O par guarda o índice da cidade e sua distância. A inserção direta desloca os pares maiores uma posição para a direita até abrir espaço para o atual.
4. Em `cidadesNoRaio`, acompanhe o `&&`. A cidade só entra na interseção se respeitar os dois limites de 100 km.
5. Em `rotaRN`, desenhe uma fila e um vetor `anterior`. A busca marca a cidade ao colocá-la na fila. Assim cada uma entra uma única vez. Para encontrar a rota, siga os antecessores do destino até a origem e inverta a sequência.
6. Em `dijkstra`, a cada iteração é escolhida a cidade ainda não visitada com menor distância acumulada. A atualização testa se o caminho pelo vértice atual melhora o custo do vizinho. A matriz guarda pesos simétricos e o limite de 30 km seleciona as arestas que podem ser usadas.
7. Em `maisVizinhos`, acompanhe o laço com `j = i + 1`. O par gera uma única aresta, que aumenta o grau dos dois extremos. Um vetor guarda o máximo por UF e uma segunda passagem imprime todos os empatados.
8. Em `src/caixeiro_viajante.c`, acompanhe `caminho`, `visitado` e `melhorCaminho`. A busca escolhe uma próxima cidade, marca, chama a própria função e desmarca ao voltar. Ao completar 19 cidades, soma o trecho de volta até Acari.

## Como explicar a poda da questão 8

Primeiro, a escolha repetida do vizinho mais próximo produz um circuito inicial válido. O custo desse circuito vira o melhor custo conhecido. A busca recursiva tenta outras ordens.

Em um caminho parcial, cada cidade ainda não visitada precisa de duas arestas futuras. A última cidade do caminho e Acari precisam de uma cada. A busca soma os menores pesos que poderiam atender a esses graus e divide por dois, porque cada aresta aparece pelos dois extremos. Isso dá um limite inferior para o custo que falta. Se até esse limite ultrapassar o melhor circuito encontrado, aquele caminho parcial pode ser descartado.

## Perguntas para praticar

| Pergunta | Ponto para explicar |
| --- | --- |
| Por que a BFS pode encontrar uma sequência com menos arestas? | A fila visita primeiro os vértices a um salto da origem, depois os de dois saltos e assim por diante. |
| O Dijkstra da questão 6 minimiza o quê? | A soma dos pesos das arestas. Dois caminhos com a mesma quantidade de cidades podem ter custos diferentes. |
| E se 30 km não conectar as duas cidades? | A segunda execução do Dijkstra usa o maior trecho encontrado no caminho como custo. Ela acha o menor limite de trecho que permite a conexão. |
| Como surgem vizinhos de outra UF? | O laço compara pares do Brasil inteiro. A UF serve para contar a travessia de fronteira e agrupar os maiores graus. |
| Por que guardar o código IBGE? | O código identifica cada município mesmo quando nomes coincidem e seus primeiros dois dígitos indicam a UF. |
| Por que as distâncias mudam em um mapa rodoviário? | O cálculo da atividade segue a superfície da Terra entre dois pontos aproximados. Uma rota rodoviária precisa seguir a rede de vias. |
| Quais laços são quadráticos? | A ordenação por inserção direta tem pior caso quadrático. A matriz, BFS sobre matriz, Dijkstra por varredura e a contagem nacional também usam até um número quadrático de operações no total de vértices. |
| A poda torna o caixeiro polinomial? | A busca ainda pode visitar muitas permutações no pior caso. A poda somente reduz o trabalho para esta entrada. |

Para treinar, rode cada número com `--offline` e explique de onde veio uma linha da saída olhando o código. Faça também a questão 1 à mão com a calculadora usando as coordenadas exibidas.
