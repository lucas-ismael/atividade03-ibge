# Base local de municípios

A base local reúne 5.571 localidades listadas pela API de municípios do IBGE em 26/11/2026. Os 167 municípios do RN, 184 do CE e 223 da PB somam os 574 vértices da questão 6.

1. Código, nome e UF vieram da [API de Localidades](https://servicodados.ibge.gov.br/api/v1/localidades/municipios).
2. As malhas municipais foram consultadas em lotes, por UF, na API de Malhas v3 do IBGE, com `qualidade=minima` e `intrarregiao=municipio`. [Exemplo do RN](https://servicodados.ibge.gov.br/api/v3/malhas/estados/24?formato=application%2Fvnd.geo%2Bjson&qualidade=minima&intrarregiao=municipio).
3. O ponto de cada município é o centro do retângulo que contém sua malha. Para encontrá-lo, basta tirar a média entre a menor e a maior latitude e repetir o cálculo para a longitude. A biblioteca usa o mesmo método. Os pontos das 19 cidades da tabela da questão 8 coincidiram com os da base.
4. Boa Esperança do Norte (5101837) constava na lista, mas faltava no lote de Mato Grosso, e sua consulta individual apresentou erro. Para preencher seu registro, foi usada a malha oficial disponibilizada em formato GeoJSON no [espelho brasil-geojson](https://github.com/TheWesDias/brasil-geojson/blob/main/README.pt.md).

O CSV usa ponto como separador decimal e ponto e vírgula entre colunas. A questão 8 usa diretamente as coordenadas da tabela do enunciado. Os resultados do modo padrão podem variar conforme a API atualize os limites municipais.
