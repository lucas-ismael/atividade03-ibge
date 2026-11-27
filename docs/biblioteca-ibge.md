# Como usar a biblioteca IBGE

Execute os comandos na raiz deste projeto. A implementação já foi compilada em `libibge.so`; o arquivo `ibge_api.c` não é distribuído. A biblioteca consulta a API pública do IBGE e usa o programa curl internamente.

## Requisitos

- GCC ou outro compilador compatível com C11;
- curl disponível no PATH;
- acesso à internet durante as consultas.

No Ubuntu/Debian:

~~~bash
sudo apt update
sudo apt install build-essential curl
~~~

## Estrutura das pastas

Os exemplos usam estes arquivos do projeto:

~~~text
.
├── include/
│   └── ibge_api.h             # interface pública a ser incluída pelos programas
├── lib/
│   └── libibge.so             # implementação compilada da biblioteca
├── exemplos/
│   └── consultar_municipio.c      # programa de exemplo
├── docs/
│   └── biblioteca-ibge.md           # este guia
~~~

A pasta `include/` contém a interface pública. A pasta `lib/` contém o código compilado a ser vinculado ao programa. A pasta `exemplos/` mostra como usar essa interface. O arquivo `ibge_api.c` é interno e não faz parte da distribuição.

## Compilar o exemplo

Compile o exemplo usando o cabeçalho e a biblioteca fornecidos:

~~~bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Iinclude \
  exemplos/consultar_municipio.c -Llib -libge \
  -Wl,-rpath,'$ORIGIN/../lib' -o exemplos/consultar_municipio
~~~

As opções `-Iinclude` e `-Llib` localizam, respectivamente, o cabeçalho e a biblioteca. A opção `-Wl,-rpath,'$ORIGIN/../lib'` faz o executável procurar `libibge.so` na pasta `lib/`, que fica ao lado da pasta `exemplos/`.

## Executar o exemplo

Informe um código IBGE de sete dígitos. Por exemplo, 2304400 corresponde a Fortaleza/CE:

~~~bash
./exemplos/consultar_municipio 2304400
~~~

O programa exibe código, município, UF, região e coordenadas aproximadas. As coordenadas são apenas aproximações calculadas a partir da caixa envolvente da malha municipal e não representam necessariamente a sede da cidade.

## Usar em outro programa

Logo abaixo um exemplo de como usar a biblioteca:

~~~c
#include <stdio.h>

#include "ibge_api.h"

int main() {
    /* Estrutura que receberá os dados do município consultado. */
    Municipio cidade;

    /* Consulta o município cujo código IBGE é 2304400 (Fortaleza/CE). */
    IbgeStatus status = ibgeBuscarMunicipio(2304400, &cidade);

    /* Interrompe o programa e informa o motivo se a consulta falhar. */
    if (status != IBGE_OK) {
        fprintf(stderr, "Erro: %s\n", ibgeStatusMensagem(status));
        return 1;
    }

    /* Exibe o nome e a sigla da UF do município encontrado. */
    printf("%s (%s)\n", cidade.nome, cidade.uf);
    return 0;
}
~~~

Ante de continuar, separe o seu código em pastas:
- src: Pasta dos códigos do cliente, que utilizam a `biblioteca_ibge`;
- build: Local que irá armazenar os programas do cliente;

Com essa estrutura, compile o programa a partir da raiz de `meu_programa/`:

~~~bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Iinclude src/minha_consulta.c \
  -Llib -libge -Wl,-rpath,'$ORIGIN/lib' -o minha_consulta
./minha_consulta
~~~

- -Iinclude localiza ibge_api.h durante a compilação.
- -Llib -libge vincula o programa a lib/libibge.so.
- -Wl,-rpath,'$ORIGIN/lib' registra a localização relativa da biblioteca para a execução.

## Funções disponíveis

- `ibgeBuscarMunicipio(codigo, &municipio)`: consulta um município pelo código IBGE e preenche uma estrutura `Municipio`.
- `ibgeListarMunicipios(callback, contexto)`: percorre os municípios retornados pela API e chama `callback` para cada item. A função deve retornar um valor diferente de zero para continuar a listagem.
- `ibgeStatusMensagem(status)`: retorna uma descrição breve de um `IbgeStatus`.

As funções de consulta retornam `IBGE_OK` em caso de sucesso. Em caso de erro, compare o retorno com as demais constantes `IBGE_ERRO_*` ou exiba a mensagem retornada por `ibgeStatusMensagem`.

### Exemplo: consultar todas as cidades

O programa abaixo lista o código e o nome de todos os municípios retornados pela API. A função `mostrarMunicipio` é chamada uma vez para cada item; retornar `1` indica que a listagem deve continuar:

~~~c
#include <stdio.h>

#include "ibge_api.h"

/* Callback executado uma vez para cada município retornado pela API. */
static int mostrarMunicipio(int codigo, const char *nome, void *contexto) {
    /* Este exemplo não precisa de dados adicionais no contexto. */
    (void)contexto;

    /* Exibe o código IBGE e o nome do município atual. */
    printf("%d: %s\n", codigo, nome);

    /* Um valor diferente de zero solicita que a listagem continue. */
    return 1;
}

int main(void) {
    /* Percorre os municípios e chama mostrarMunicipio para cada um deles. */
    IbgeStatus status = ibgeListarMunicipios(mostrarMunicipio, NULL);

    /* Exibe o motivo caso a consulta ou a listagem tenha falhado. */
    if (status != IBGE_OK) {
        fprintf(stderr, "Erro: %s\n", ibgeStatusMensagem(status));
        return 1;
    }

    return 0;
}
~~~

## Dados retornados pela consulta

Quando `ibgeBuscarMunicipio` retorna `IBGE_OK`, ela preenche a estrutura `Municipio` recebida no segundo argumento. Os campos disponíveis são:

| Campo | Tipo | Descrição |
| --- | --- | --- |
| `codigo` | `int` | Código IBGE de sete dígitos do município. |
| `nome` | `char[128]` | Nome do município. |
| `microrregiao` | `char[128]` | Nome da microrregião do município. |
| `mesorregiao` | `char[128]` | Nome da mesorregião do município. |
| `uf` | `char[3]` | Sigla da unidade federativa, como `CE`. |
| `regiao` | `char[128]` | Nome da região brasileira, como `Nordeste`. |
| `latitude` | `double` | Latitude aproximada do centro da caixa envolvente da malha municipal. |
| `longitude` | `double` | Longitude aproximada do centro da caixa envolvente da malha municipal. |

Os campos de texto já são terminados por `\0`. As coordenadas são calculadas a partir da malha GeoJSON disponibilizada pelo IBGE; portanto, representam uma aproximação e não necessariamente a localização da sede do município.

A **caixa envolvente da malha municipal** é o menor retângulo, alinhado aos eixos de latitude e longitude, que cobre todos os pontos do contorno do município. A biblioteca encontra as menores e maiores latitudes e longitudes da malha e usa o ponto central desse retângulo como valor aproximado para `latitude` e `longitude`. Por isso, esse ponto pode ficar distante da sede e, em municípios de formato irregular, pode até não ficar dentro de sua área.

Exemplo: os pontos `●` representam o contorno do município, o retângulo é a caixa envolvente e `×` é o ponto central usado como aproximação.

~~~text
                   longitude mínima       longitude máxima
                           |                   |
                           |                   |
latitude máxima─────────── +-----------●-------+
                           |      ● ●          |
                           |   ●               ●
                           |      ●  x    ●    |
                           |  ●            ●   |
                           ●      ● ●          |
latitude mínima─────────── +------------●------+
~~~

### Exemplo completo

O programa abaixo consulta Fortaleza/CE e exibe todos os campos de `Municipio`:

~~~c
#include <stdio.h>

#include "ibge_api.h"

int main(void) {
    Municipio municipio;
    IbgeStatus status = ibgeBuscarMunicipio(2304400, &municipio);

    if (status != IBGE_OK) {
        fprintf(stderr, "Erro: %s\n", ibgeStatusMensagem(status));
        return 1;
    }

    printf("Código IBGE: %d\n", municipio.codigo);
    printf("Município: %s\n", municipio.nome);
    printf("Microrregião: %s\n", municipio.microrregiao);
    printf("Mesorregião: %s\n", municipio.mesorregiao);
    printf("UF: %s\n", municipio.uf);
    printf("Região: %s\n", municipio.regiao);
    printf("Latitude aproximada: %.6f\n", municipio.latitude);
    printf("Longitude aproximada: %.6f\n", municipio.longitude);
    return 0;
}
~~~

Consulte include/ibge_api.h para a definição completa dos tipos e códigos de erro.
