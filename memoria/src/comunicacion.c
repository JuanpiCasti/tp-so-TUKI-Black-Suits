#include "comunicacion.h"

void devolver_tabla_inicial(int socket) {
    uint32_t size = sizeof(t_ent_ts) * CANT_SEGMENTOS + sizeof(uint32_t);
    void* buffer = malloc(size);

    memcpy(buffer, &CANT_SEGMENTOS, sizeof(uint32_t));

    void* tabla = crear_tabla_segmentos();

    memcpy(buffer + sizeof(uint32_t), tabla, sizeof(t_ent_ts) * CANT_SEGMENTOS);

    send(socket, buffer, size, NULL);

    free(buffer);
    free(tabla);

}

void devolver_resultado_creacion(cod_op_kernel resultado, int socket, uint32_t base) {
    int tam_buffer = sizeof(cod_op_kernel);
    if(resultado == MEMORIA_SEGMENTO_CREADO) {
        tam_buffer += sizeof(uint32_t);
    }
    void* buffer = malloc(tam_buffer);

    int despl = 0;

    memcpy(buffer, &resultado, sizeof(cod_op_kernel));
    despl += sizeof(cod_op_kernel);

    if(resultado == MEMORIA_SEGMENTO_CREADO) {
        memcpy(buffer + despl, &base, sizeof(uint32_t));
    }

    send(socket, buffer, tam_buffer, NULL);
    free(buffer);
}

void devolver_nuevas_bases(int cliente_socket) {
    uint32_t size = (sizeof(uint32_t) * 3) * list_size(LISTA_GLOBAL_SEGMENTOS) + sizeof(uint32_t);
    void* buffer = malloc(size);
    int desplazamiento = 0;
    
    memcpy(buffer, &size, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    t_segmento* segmento;
    // for each segmento in LISTA_GLOBAL_SEGMENTOS copy its pid, id and base
    for (int i = 0; i < list_size(LISTA_GLOBAL_SEGMENTOS); i++)
    {   
        segmento = list_get(LISTA_GLOBAL_SEGMENTOS, i);
        memcpy(buffer + desplazamiento, &segmento->pid, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
        memcpy(buffer + desplazamiento, &segmento->id, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
        memcpy(buffer + desplazamiento, &segmento->base, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
    }

    send(cliente_socket, buffer, size, NULL);
    free(buffer);
}

void procesar_conexion(void *void_args)
{
    t_conexion *args = (t_conexion *)void_args;
    t_log *logger = args->log;
    int cliente_socket = args->socket;
    free(args);

    cod_op cop;

    while (cliente_socket != -1)
    {
        if (recv(cliente_socket, &cop, sizeof(cod_op), 0) != sizeof(cod_op))
        {
            log_warning(logger, "Cliente desconectado!");
            break;
        }

        switch (cop)
        {
        case HANDSHAKE_CPU:
        case HANDSHAKE_FILESYSTEM:
        case HANDSHAKE_KERNEL:
            aceptar_handshake(logger, cliente_socket, cop);
            break;
        // Errores
        case HANDSHAKE_CONSOLA:
        case HANDSHAKE_MEMORIA:
            rechazar_handshake(logger, cliente_socket);
            break;
        case CREATE_SEGTABLE:
            pthread_mutex_lock(&mutex_memoria);
            uint32_t pid;
            recv(cliente_socket, &pid, sizeof(uint32_t), NULL);
            devolver_tabla_inicial(cliente_socket);
            log_info(logger_memoria, "Creacion de Proceso PID: %d", pid);
            pthread_mutex_unlock(&mutex_memoria);
            break;
        case MEMORIA_CREATE_SEGMENT:
            pthread_mutex_lock(&mutex_memoria);
            uint32_t pid_create_segment;
            uint32_t id_seg;
            uint32_t tam_seg;
            recv(cliente_socket, &pid_create_segment, sizeof(uint32_t), NULL);
            recv(cliente_socket, &id_seg, sizeof(uint32_t), NULL);
            recv(cliente_socket, &tam_seg, sizeof(uint32_t), NULL);

            uint32_t n_base;
            cod_op_kernel resultado = crear_segmento(tam_seg, &n_base);

            if (resultado == MEMORIA_SEGMENTO_CREADO)
            {
                t_segmento* n_seg = malloc(sizeof(t_segmento));
                n_seg->pid = pid_create_segment;
                n_seg->id = id_seg;
                n_seg->base = n_base;
                n_seg->limite = tam_seg;
                list_add_sorted(LISTA_GLOBAL_SEGMENTOS, n_seg, comparador_base_segmento);
                log_info(logger_memoria, "PID: %d - Crear Segmento: %d - Base: %d - TAMAÑO: %d", pid_create_segment, id_seg, n_base, tam_seg);
            }
            
            // print_lista_segmentos();
            // print_lista_esp(LISTA_ESPACIOS_LIBRES);

            devolver_resultado_creacion(resultado, cliente_socket, n_base);
            pthread_mutex_unlock(&mutex_memoria);
            break;
        case MEMORIA_FREE_SEGMENT:
            pthread_mutex_lock(&mutex_memoria);
            uint32_t pid_free_segment;
            uint32_t free_seg_id;
            uint32_t base;
            uint32_t tam;

            recv(cliente_socket, &pid_free_segment, sizeof(uint32_t), NULL);
            recv(cliente_socket, &free_seg_id, sizeof(uint32_t), NULL);
            recv(cliente_socket, &base, sizeof(uint32_t), NULL);
            recv(cliente_socket, &tam, sizeof(uint32_t), NULL);

            borrar_segmento(base, tam);
            log_info(logger_memoria, "PID: %d - Eliminar Segmento: %d - Base: %d - TAMAÑO: %d", pid_free_segment, free_seg_id, base, tam);
            print_lista_esp(LISTA_ESPACIOS_LIBRES); //
            pthread_mutex_unlock(&mutex_memoria);
            break;
        case MEMORIA_MOV_OUT:
            pthread_mutex_lock(&mutex_memoria);
            uint32_t pid_mov_out;
            uint32_t dir_fisica;
            uint32_t tam_escrito;
            recv(cliente_socket, &pid_mov_out, sizeof(uint32_t), NULL);
            recv(cliente_socket, &dir_fisica, sizeof(uint32_t), NULL);
            recv(cliente_socket, &tam_escrito, sizeof(uint32_t), NULL);
            char* valor = malloc(tam_escrito);
            recv(cliente_socket, valor, tam_escrito, NULL);
            
            // Para probar
            // char* cadena = imprimir_cadena(valor, tam_escrito);
            // printf("Valor recibido: %s\n", cadena);

            escribir(dir_fisica, valor, tam_escrito);
            log_info(logger_memoria, "PID: %d - Acción: ESCRIBIR - Dirección física: %d - Tamaño: %d - Origen: CPU", pid_mov_out, dir_fisica, tam_escrito);
            sleep(RETARDO_MEMORIA/1000);
            free(valor);
            uint32_t mov_out_ok = 1;
            send(cliente_socket, &mov_out_ok, sizeof(uint32_t), NULL);
            //char* cosita = leer(dir_fisica, tam_escrito);
            pthread_mutex_unlock(&mutex_memoria);
            break;
        case MEMORIA_MOV_IN:
            pthread_mutex_lock(&mutex_memoria);
            uint32_t pid_mov_in;
            uint32_t dir_fisica_in;
            uint32_t tam_a_leer;
            recv(cliente_socket, &pid_mov_in, sizeof(uint32_t), NULL);
            recv(cliente_socket, &dir_fisica_in, sizeof(uint32_t), NULL);
            recv(cliente_socket, &tam_a_leer, sizeof(uint32_t), NULL);
            char* valor_in = leer(dir_fisica_in, tam_a_leer);
            send(cliente_socket, valor_in, tam_a_leer, NULL);
            free(valor_in);
            log_info(logger_memoria, "PID: %d - Acción: LEER - Dirección física: %d - Tamaño: %d - Origen: CPU", pid_mov_in, dir_fisica_in, tam_a_leer);
            sleep(RETARDO_MEMORIA/1000);
            pthread_mutex_unlock(&mutex_memoria);
            break;

        case COMPACTAR:
            pthread_mutex_lock(&mutex_memoria);
            compactar();
            for(int i = 0; i < list_size(LISTA_GLOBAL_SEGMENTOS); i++)
            {
                t_segmento* segmento = list_get(LISTA_GLOBAL_SEGMENTOS, i);
                log_info(logger_memoria, "PID: %d - Segmento: %d - Base: %d - Tamaño: %d", segmento->pid, segmento->id, segmento->base, segmento->limite);
            }
            devolver_nuevas_bases(cliente_socket);
            print_lista_esp(LISTA_ESPACIOS_LIBRES);
            pthread_mutex_unlock(&mutex_memoria);
            break;
        
        case LEER_ARCHIVO:
            pthread_mutex_lock(&mutex_memoria);
            uint32_t pid_leer_archivo;
            uint32_t dir_fisica_leer_archivo;
            uint32_t tam_a_leer_archivo;
            recv(cliente_socket, &pid_leer_archivo, sizeof(uint32_t), NULL);
            recv(cliente_socket, &dir_fisica_leer_archivo, sizeof(uint32_t), NULL);
            recv(cliente_socket, &tam_a_leer_archivo, sizeof(uint32_t), NULL);
            char* valor_leer_archivo = malloc(tam_a_leer_archivo);
            recv(cliente_socket, valor_leer_archivo, tam_a_leer_archivo, NULL);
            escribir(dir_fisica_leer_archivo, valor_leer_archivo, tam_a_leer_archivo);
            free(valor_leer_archivo);
            log_info(logger_memoria, "PID: %d - Accion: ESCRIBIR - Dirección física: %d - Tamaño: %d - Origen: FS", pid_leer_archivo, dir_fisica_leer_archivo, tam_a_leer_archivo);
            sleep(RETARDO_MEMORIA/1000);
            uint32_t escritura_ok = 0;
            send(cliente_socket, &escritura_ok, sizeof(uint32_t), NULL);
            pthread_mutex_unlock(&mutex_memoria);
            break;

        case ESCRIBIR_ARCHIVO:
            pthread_mutex_lock(&mutex_memoria);
            uint32_t pid_escribir_archivo;
            uint32_t dir_fisica_escribir_archivo;
            uint32_t tam_a_escribir_archivo;
            recv(cliente_socket, &pid_escribir_archivo, sizeof(uint32_t), NULL);
            recv(cliente_socket, &dir_fisica_escribir_archivo, sizeof(uint32_t), NULL);
            recv(cliente_socket, &tam_a_escribir_archivo, sizeof(uint32_t), NULL);
            char* valor_escribir_archivo = leer(dir_fisica_escribir_archivo, tam_a_escribir_archivo);
            log_info(logger_memoria, "PID: %d - Accion: LEER - Dirección física: %d - Tamaño: %d - Origen: FS", pid_escribir_archivo, dir_fisica_escribir_archivo, tam_a_escribir_archivo);
            sleep(RETARDO_MEMORIA/1000);
            send(cliente_socket, valor_escribir_archivo, tam_a_escribir_archivo, NULL);
            free(valor_escribir_archivo);
            pthread_mutex_unlock(&mutex_memoria);
            break;
        default:
            log_error(logger, "Algo anduvo mal en el server Memoria");
            log_info(logger, "Cop: %d", cop);
            return;
        
        }
    }

    log_warning(logger, "El cliente se desconectó del server");
    return;
}