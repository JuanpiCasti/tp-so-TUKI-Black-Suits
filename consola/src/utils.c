#include "utils.h"

FILE *leer_archivo(char *nombre_archivo, t_log *logger)
{
    FILE *archivo_instrucciones = fopen(nombre_archivo, "r");
    if (archivo_instrucciones == NULL)
    {
        log_error(logger, "Error al abrir el archivo de instrucciones\n");
        exit(-1);
    }

    return archivo_instrucciones;
}

t_list *parsear_archivo(char *nombre_archivo, t_log *logger)
{
    FILE *archivo_a_parsear = leer_archivo(nombre_archivo, logger);
    t_list *instrucciones = list_create();
    char *linea = malloc(sizeof(t_instruccion));

    t_instruccion *instruccion = malloc(sizeof(t_instruccion));

    while (fgets(linea, sizeof(t_instruccion), archivo_a_parsear))
    {
        if (sscanf(linea, "%s %s %s %s", instruccion->instruccion, instruccion->arg1, instruccion->arg2, instruccion->arg3) == 4)
        {
            // Instrucción con 3 parámetros
        }
        else if (sscanf(linea, "%s %s %s", instruccion->instruccion, instruccion->arg1, instruccion->arg2) == 3)
        {
            strcpy(instruccion->arg3, "");
        }
        else if (sscanf(linea, "%s %s", instruccion->instruccion, instruccion->arg1) == 2)
        {
            strcpy(instruccion->arg2, "");
            strcpy(instruccion->arg3, "");
        }
        else if (sscanf(linea, "%s", instruccion->instruccion) == 1)
        {
            strcpy(instruccion->arg1, "");
            strcpy(instruccion->arg2, "");
            strcpy(instruccion->arg3, "");
        }

        list_add(instrucciones, (void *)instruccion);
        instruccion = malloc(sizeof(t_instruccion));
    }

    fclose(archivo_a_parsear);

    return instrucciones;
}

void instruccion_destruir(void *instruccion)
{
    free((t_instruccion *)instruccion);
}

void *serializar_paquete_instrucciones(t_list *instrucciones, int cant_instrucciones, uint32_t tam_buffer_instrucciones, uint32_t size_paquete)
{
    int cop = PAQUETE_INSTRUCCIONES;
    void *stream = malloc(size_paquete);

    memcpy(stream, &cop, sizeof(cod_op));

    void *buffer_instrucciones = serializar_instrucciones(instrucciones, cant_instrucciones, tam_buffer_instrucciones);

    memcpy(stream + sizeof(cod_op), buffer_instrucciones, sizeof(uint32_t) + tam_buffer_instrucciones);

    free(buffer_instrucciones);
    return stream;
}

int enviar_instrucciones(t_log *logger, char *ip, char *puerto, char *archivo_instrucciones)
{
    int socket_kernel = conectar_servidor(logger, ip, puerto, "Kernel");
    if (socket_kernel == -1)
    {
        log_error(logger, "No se pudo conectar a Kernel");
        return;
    }
    t_list *instrucciones = list_create();
    instrucciones = parsear_archivo(archivo_instrucciones, logger);

    log_info(logger, "Enviando instrucciones a Kernel...");

    int cant_instrucciones = list_size(instrucciones);
    uint32_t tam_buffer_instrucciones = cant_instrucciones * sizeof(t_instruccion);
    uint32_t size_paquete = tam_buffer_instrucciones + sizeof(uint32_t) + sizeof(cod_op);

    void *stream_instrucciones = serializar_paquete_instrucciones(instrucciones, cant_instrucciones, tam_buffer_instrucciones, size_paquete);

    int result = send(socket_kernel, stream_instrucciones, size_paquete, NULL);
    if (result == -1)
    {
        log_error(logger, "No se pudo enviar el paquete de instrucciones");
    }

    free(stream_instrucciones);

    list_destroy_and_destroy_elements(instrucciones, instruccion_destruir);
    return socket_kernel;
}

cod_op_kernel recibir_resultado(int socket_kernel) {
    cod_op_kernel resultado;
    recv(socket_kernel, &resultado, sizeof(cod_op_kernel), NULL);
    return resultado;
}