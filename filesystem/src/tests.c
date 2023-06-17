#include "tests.h"

int run_tests()
{
    // CU_initialize_registry();
    // CU_pSuite tests = CU_add_suite("FILESYSTEM Suite", NULL, NULL);
    // // CU_add_test(tests,"Probar Suma", suma_consola);
    // CU_basic_set_mode(CU_BRM_VERBOSE);
    // CU_basic_run_tests();
    // CU_cleanup_registry();
    // return CU_get_error();
    // Prueba de archivo de bloques
    //crear_archivo("elpicante");
    truncar_archivo("elpicante", 128);

    for (int i = 0; i < BLOCK_COUNT; i++)
    {
        printf("Pos. %d: %d\n", i + 1, bitarray_test_bit(bitmap, i));
    }

    for (int i = 0; i < 3; i++)
    {
        uint32_t puntero;
        memcpy(&puntero, blocks_buffer + BLOCK_SIZE + sizeof(uint32_t) * i, sizeof(uint32_t));
        printf("Puntero %d: %d\n", i + 1, puntero);
    }
}