# Exercícios: grafos e localização de municípios

## Regras da atividade

- **Quantidade de integrantes:** 3 por grupo.
- **Prazo para entrega:** Dia 03(Alunos da Quinta-feira) ou dia 04(alunos da Sexta-feira).
- **Ordem de apresentação:** No dia 03, todos os grupos fazem a prova simultaneamente. No dia 04, os grupos realizarão a prova de 6 em 6 grupos simultaneamente.
- **Duração de cada "apresentação"**: Cada apresentação terá 30min para responder sobre perguntas do seu códidp;
- **Entrega (60% da Nota):** relatório com os resultados obtidos em cada questão. Obrigatório: Link do repositório acessível para kennedy.lopes@ufersa.edu.br. 
- **Apresentação dos resultados (40% da Nota):** o uso de IA **para esta atividade** não é proibido. Porém, haverá questionamentos sobre as questões e as modificações realizadas no código, respondidos sem auxílio de IA. As perguntas serão aplicadas a todos os grupos simultaneamente.

Use a biblioteca IBGE para obter os dados dos municípios, especialmente os campos `codigo`, `nome`, `latitude` e `longitude` da estrutura `Municipio`. Considere cada cidade um **vértice** de um grafo. Uma rota ou conexão entre duas cidades é uma **aresta**, e sua distância é o **peso** dessa aresta.

## Equações para calcular a distância

Para os cálculos geográficos, use a distância de grande círculo (fórmula de Haversine), considerando o raio da Terra como `6371 km`.

Antes de usar as coordenadas, converta latitude e longitude de graus para radianos:

$$~
\varphi = \text{latitude} \times \frac{\pi}{180}
\qquad
\lambda = \text{longitude} \times \frac{\pi}{180}
$$

Para duas cidades, calcule as diferenças entre as coordenadas:

$$
\Delta\varphi = \varphi_2 - \varphi_1
\qquad
\Delta\lambda = \lambda_2 - \lambda_1
$$

Em seguida, aplique a fórmula de Haversine:

$$
a = \sin^2\left(\frac{\Delta\varphi}{2}\right) +
    \cos(\varphi_1) \times \cos(\varphi_2) \times
    \sin^2\left(\frac{\Delta\lambda}{2}\right)
$$

$$
c = 2 \times \operatorname{atan2}(\sqrt{a}, \sqrt{1-a})
$$

$$
d = R \times c
$$

Onde `d` é a distância em quilômetros e `R = 6371 km` é o raio aproximado da Terra. Em C, as funções `sin`, `cos`, `sqrt` e `atan2` estão disponíveis em `math.h`; compile o programa com a opção `-lm`.

### Exemplo: Natal e Pau dos Ferros (RN)

As coordenadas abaixo foram obtidas pela biblioteca usando os códigos IBGE `2408102` (Natal) e `2409407` (Pau dos Ferros). Elas representam o ponto central aproximado da caixa envolvente de cada município.

| Cidade | Latitude | Longitude |
| --- | ---: | ---: |
| Natal | `-5.801900` | `-35.222700` |
| Pau dos Ferros | `-6.102950` | `-38.158300` |

Convertendo os valores para radianos:

$$
\begin{aligned}
\varphi_1 &= -0.101262258 & \lambda_1 &= -0.614752086 \\
\varphi_2 &= -0.106516572 & \lambda_2 &= -0.665987972
\end{aligned}
$$

Assim, as diferenças são `Δφ = -0.005254314` e `Δλ = -0.051235886`. Aplicando Haversine:

$$
a = 0.000655977
\qquad
c = 0.051229683
$$

$$
d = 6371 \times 0.051229683 \approx 326.38\ \text{km}
$$

Portanto, a distância em linha reta aproximada entre Natal e Pau dos Ferros é **326,38 km**. Essa distância geográfica não corresponde necessariamente à distância por rodovias.

> **Observação:** as funções `sin`, `cos` e `atan2` fazem parte da biblioteca matemática padrão de C e são declaradas no cabeçalho `math.h`. Ao compilar, inclua a opção `-lm`, por exemplo: `gcc programa.c -o programa -lm`.

## 1. Distância entre duas cidades

Calcule a distância geográfica em linha reta entre Pau dos Ferros (`2409407`) e Mossoró (`2408003`). Consulte os dois municípios com `ibgeBuscarMunicipio` e aplique a fórmula de Haversine às coordenadas retornadas.

Exiba o nome, o código IBGE, a latitude e a longitude de cada município. Ao final, apresente a distância em quilômetros com duas casas decimais. Explique que o resultado representa a distância de grande círculo e não a distância de uma rota por rodovias.

## 2. Dez cidades mais próximas de Pau dos Ferros

Considere Pau dos Ferros como cidade de referência. Obtenha todos os municípios do Rio Grande do Norte, da Paraíba e do Ceará, calcule a distância entre Pau dos Ferros e cada um deles e ordene o resultado em ordem crescente de distância.

Exiba os **10 municípios mais próximos**, sem incluir Pau dos Ferros na lista. Para cada resultado, mostre a posição no ranking, o nome, o código IBGE e a distância em quilômetros. Utilize uma estrutura de dados que permita ordenar os pares município–distância.

## 3. Cidades em um raio geográfico

Considere os municípios do Rio Grande do Norte e encontre todos os que estão, simultaneamente, a no máximo `100 km` de Mossoró e a no máximo `100 km` de Pau dos Ferros.

Para cada município, calcule as duas distâncias e inclua-o no resultado somente quando ambas forem menores ou iguais a `100 km`. Exiba o nome do município, a distância até Mossoró e a distância até Pau dos Ferros. Ao final, informe a quantidade de municípios encontrados.

## 5. Existe rota entre duas cidades?

Construa um grafo não direcionado com todos os municípios do Rio Grande do Norte. Crie uma aresta entre duas cidades quando a distância geográfica entre elas for de, no máximo, `20 km`. O peso da aresta deve ser a distância calculada por Haversine.

Depois, verifique se existe uma rota entre Pau dos Ferros e Natal. Use BFS ou DFS para a verificação e, se houver rota, apresente a sequência de municípios visitados. Caso não exista, informe claramente que as conexões de até `20 km` não são suficientes para ligar as duas cidades.

## 6. Menor rota por distância

Construa um grafo não direcionado com todos os municípios do Rio Grande do Norte, Ceará e Paraíba. Crie uma aresta entre duas cidades quando a distância geográfica entre elas for de, no máximo, `30 km`. Inclua conexões entre municípios de estados diferentes sempre que atenderem ao limite.

Utilize o algoritmo de Dijkstra para encontrar a menor rota entre Pau dos Ferros e João Pessoa. Exiba a sequência completa de municípios, as distâncias de cada trecho e a distância total. Se não houver caminho com o limite escolhido, informe isso e discuta qual limite mínimo poderia tornar a rota possível.

## 7. Município com mais vizinhos por estado

Considere todos os municípios brasileiros retornados pela biblioteca. Construa um grafo não direcionado com uma aresta entre duas cidades quando elas estiverem a, no máximo, `15 km` de distância. Não limite as arestas ao mesmo estado: cidades próximas de uma fronteira podem ter vizinhas em estados diferentes.

Para cada estado e para o Distrito Federal, determine o município com a maior quantidade de cidades vizinhas, isto é, o maior grau no grafo. Em caso de empate, mostre todos os municípios empatados. Para cada resultado, exiba a UF, o município, o código IBGE e a quantidade de vizinhos, indicando quantos deles pertencem a outra UF.

## 8. Caixeiro viajante com municípios iniciados por A ou B

### Municípios do RN usados nos exercícios

Use os municípios abaixo, cujos nomes começam com A ou B. As coordenadas foram obtidas com `ibgeBuscarMunicipio` e representam o ponto central aproximado da caixa envolvente de cada município.

| Município | Código IBGE | Latitude | Longitude |
| --- | ---: | ---: | ---: |
| Acari | `2400109` | `-6.423950` | `-36.639350` |
| Assú | `2400208` | `-5.592250` | `-37.005600` |
| Afonso Bezerra | `2400307` | `-5.484150` | `-36.650150` |
| Água Nova | `2400406` | `-6.208850` | `-38.314950` |
| Alexandria | `2400505` | `-6.365650` | `-37.984400` |
| Almino Afonso | `2400604` | `-6.156850` | `-37.748900` |
| Alto do Rodrigues | `2400703` | `-5.342750` | `-36.687700` |
| Angicos | `2400802` | `-5.642400` | `-36.516700` |
| Antônio Martins | `2400901` | `-6.213250` | `-37.920050` |
| Apodi | `2401008` | `-5.628800` | `-37.865950` |
| Areia Branca | `2401107` | `-5.021650` | `-37.004300` |
| Arez | `2401206` | `-6.195250` | `-35.204450` |
| Baía Formosa | `2401404` | `-6.419300` | `-35.047900` |
| Baraúna | `2401453` | `-5.081900` | `-37.627650` |
| Barcelona | `2401503` | `-5.985350` | `-35.933300` |
| Bento Fernandes | `2401602` | `-5.670900` | `-35.786900` |
| Bodó | `2401651` | `-5.954100` | `-36.440700` |
| Bom Jesus | `2401701` | `-6.005900` | `-35.593800` |
| Brejinho | `2401800` | `-6.215850` | `-35.387900` |

Construa um grafo completo com esses `19` municípios. O peso de cada aresta deve ser a distância de Haversine entre os municípios.

Partindo de Acari, determine uma rota de caixeiro viajante de menor distância que visite todos os municípios exatamente uma vez e retorne a Acari. Exiba a ordem de visitação, a distância de cada trecho e a distância total da rota. Como há `19` municípios, use qualquer estratégia.

Mostre o percurso em um mapa. Você pode gerar a visualização em um programa próprio ou traçar manualmente os pontos no Google Maps. Na apresentação, diferencie a distância geográfica calculada pela atividade da distância rodoviária exibida pelo Google Maps.
