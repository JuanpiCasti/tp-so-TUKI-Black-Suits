#include "comunicacion.h"
 



void procesar_conexion(void* void_args) {
    t_conexion* args = (t_conexion*) void_args;
    t_log* logger = args->log;
    int cliente_socket = args->socket;
    free(args);

    cod_op cop;
    
    while (cliente_socket != -1) {

        if (recv(cliente_socket, &cop, sizeof(cod_op), 0) != sizeof(cod_op)) {
            log_warning(logger, "Cliente desconectado!");
            break;
        }

        switch (cop) {
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
            default:
                log_error(logger, "Algo anduvo mal en el server de la memoria");
                log_info(logger, "Cop: %d", cop);
                return;
        }
    }

    log_warning(logger, "El cliente se desconecto del server");
    return;
}

int server_escuchar(t_log* logger, int server_socket) {
    int cliente_socket = esperar_cliente(logger, server_socket);

    if (cliente_socket != -1) {
        pthread_t hilo; 
        t_conexion* args = malloc(sizeof(t_conexion));
        args->log = logger;
        args->socket = cliente_socket;
        pthread_create(&hilo, NULL, (void*) procesar_conexion, (void*) args);
        pthread_detach(hilo);
        return 1;
    }
    return 0;
}