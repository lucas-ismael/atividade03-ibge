#include <stdio.h>

#include "ibge_api.h"

static int mostrarMunicipio(int codigo, const char *nome, void *contexto) {
    (void)contexto;
    printf("%d: %s\n", codigo, nome);

    /* Um valor diferente de zero solicita que a listagem continue. */
    return 1;
}

int main(void) {
    IbgeStatus status = ibgeListarMunicipios(mostrarMunicipio, NULL);
    if (status != IBGE_OK) {
        fprintf(stderr, "Erro: %s\n", ibgeStatusMensagem(status));
        return 1;
    }

    return 0;
}
