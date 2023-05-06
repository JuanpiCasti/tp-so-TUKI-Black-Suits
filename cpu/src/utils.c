#include "utils.h"

void inicializar_registros() {
    AX = malloc(4);
    BX = malloc(4);
    CX = malloc(4);
    DX = malloc(4);
    EBX = malloc(8);
    ECX = malloc(8);
    EAX = malloc(8);
    EDX = malloc(8);
    RAX = malloc(16);
    RBX = malloc(16);
    RCX = malloc(16);
    RDX = malloc(16);
}

void cambiar_contexto() {
    // TODO:

}

as_instruction decode(t_instruccion instruccion) {

    // strcmp con instruccion -> instruccion y devuelve que elemetno del enum as_instruction es.

}

void ejecutar_instrucciones() {
    while(true) {
        // TODO: ejecutar hasta que encuentre un YIELD o un EXIT (break)
        // Un ciclo de ejecucion implica hacer un decode de la instruccion,
        // ejecutar la funcion asociada a esa instruccion,
        // y aumentar el program counter en 1.
    }
}