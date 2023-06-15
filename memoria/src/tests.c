#include "tests.h"

int run_tests()
{
    // CU_initialize_registry();
    // CU_pSuite tests = CU_add_suite("MEMORIA Suite", NULL, NULL);
    // CU_add_test(tests,"Probar Suma", suma_consola);
    // CU_basic_set_mode(CU_BRM_VERBOSE);
    // CU_basic_run_tests();
    // CU_cleanup_registry();
    // return CU_get_error();

    // printf("PUERTO_ESCUCHA_MEMORIA: %u\n", PUERTO_ESCUCHA_MEMORIA);
    // printf("TAM_MEMORIA: %u\n", TAM_MEMORIA);
    // printf("TAM_SEGMENTO_0: %u\n", TAM_SEGMENTO_0);
    // printf("CANT_SEGMENTOS: %u\n", CANT_SEGMENTOS);
    // printf("RETARDO_MEMORIA: %u\n", RETARDO_MEMORIA);
    // printf("RETARDO_COMPACTACION: %u\n", RETARDO_COMPACTACION);
    // printf("ALGORITMO_ASIGNACION: %s\n", t_algo_asig_desc[ALGORITMO_ASIGNACION]);
    //crear_tabla_segmentos();

    char* data1 = "PAPU";
    escribir(150, data1, 4);
    char* data2 = (char *)leer(150, 4);
    char* cadena = imprimir_cadena(data2, 4);
    printf("%s\n", cadena);
    free(cadena);

    uint32_t base;
    crear_segmento(150, &base);
    crear_segmento(100, &base);
    crear_segmento(100, &base);
    print_lista_esp(LISTA_ESPACIOS_LIBRES);
    borrar_segmento(278, 100);
    print_lista_esp(LISTA_ESPACIOS_LIBRES);
    crear_segmento(100, &base);
    print_lista_esp(LISTA_ESPACIOS_LIBRES);
    borrar_segmento(378, 100);
    print_lista_esp(LISTA_ESPACIOS_LIBRES);
    crear_segmento(5000000,&base);
    print_lista_esp(LISTA_ESPACIOS_LIBRES);
    crear_segmento(3718, &base);
    print_lista_esp(LISTA_ESPACIOS_LIBRES);
}