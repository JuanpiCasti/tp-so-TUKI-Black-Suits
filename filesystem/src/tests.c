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
    // crear_archivo("elpicante");
    // truncar_archivo("elpicante", 128);

    // for (int i = 0; i < BLOCK_COUNT; i++)
    // {
    //   printf("Pos. %d: %d\n", i + 1, bitarray_test_bit(bitmap, i));
    // }
    // crear_archivo("elpapu");
    //     for (int i = 0; i < BLOCK_COUNT; i++)
    // {
    //   printf("Pos. %d: %d\n", i + 1, bitarray_test_bit(bitmap, i));
    // }
    // truncar_archivo("elpapu", 64);
    //     for (int i = 0; i < BLOCK_COUNT; i++)
    // {
    //   printf("Pos. %d: %d\n", i + 1, bitarray_test_bit(bitmap, i));
    // }
    // truncar_archivo("elpicante", 192);
    //     for (int i = 0; i < BLOCK_COUNT; i++)
    // {
    //   printf("Pos. %d: %d\n", i + 1, bitarray_test_bit(bitmap, i));
    // }
    // truncar_archivo("elpapu", 128);
    //     for (int i = 0; i < BLOCK_COUNT; i++)
    // {
    //   printf("Pos. %d: %d\n", i + 1, bitarray_test_bit(bitmap, i));
    // }
    // truncar_archivo("elpicante", 256);
    //     for (int i = 0; i < BLOCK_COUNT; i++)
    // {
    //   printf("Pos. %d: %d\n", i + 1, bitarray_test_bit(bitmap, i));
    // }
    // truncar_archivo("elpapu", 192);
    //     for (int i = 0; i < BLOCK_COUNT; i++)
    // {
    //   printf("Pos. %d: %d\n", i + 1, bitarray_test_bit(bitmap, i));
    // }
    // char* cadena = leer_archivo("elpicante", 0, 4);
    // char* cadena_parseada = imprimir_cadena(cadena, 4);
    // printf("%s\n", cadena_parseada);

    //escribir_archivo("elpicante", 50, 43, "hola papu, como andas? espero que muy bien!");
    //truncar_archivo("elpapu", 128);
    // for (int i = 0; i < BLOCK_COUNT; i++)
    // {
    // printf("Pos. %d: %d\n", i + 1, bitarray_test_bit(bitmap, i));
    // }
    // escribir_archivo("elpicante", 0, 64, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    // escribir_archivo("elpapu", 0, 64, "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
    // escribir_archivo("elpicante", 64, 64, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    // escribir_archivo("elpapu", 64, 64, "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
    // escribir_archivo("elpicante", 128, 64, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    eferrait("elpicante", 50, 43, "hola papu, como andas? espero que muy bien!");
    char* cadena_rescatada = eferrid("elpicante", 50, 43);
    char* cadena_parseada = imprimir_cadena(cadena_rescatada, 43);
    printf("%s\n", cadena_parseada);
}