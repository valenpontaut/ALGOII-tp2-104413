#include "hash.h"

typedef struct par {
	char *clave;
	void *valor;
	struct par *siguiente;
} par_t;

struct hash {
	par_t **pares;
	size_t capacidad;
	size_t cantidad;
};