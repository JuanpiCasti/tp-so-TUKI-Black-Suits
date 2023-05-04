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