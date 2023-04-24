#ifndef UTILS_KERNEL_H
#define UTILS_KERNEL_H

#include <stdio.h>
#include <kernel.h>
#include <commons/collections/list.h>
t_list *deserializar_instrucciones(void *stream, uint32_t tam_instrucciones);

#endif