#include "hash_estructura_privada.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FACTOR_CARGA_MAXIMO 0.7

hash_t *hash_crear(size_t capacidad)
{
	hash_t *hash = calloc(1, sizeof(hash_t));
	if (hash == NULL)
		return NULL;
	hash->capacidad = capacidad >= 3 ? capacidad : 3;
	hash->pares = calloc(hash->capacidad, sizeof(par_t *));
	if (hash->pares == NULL) {
		free(hash);
		return NULL;
	}
	return hash;
}

unsigned long funcion_hash_djb2(const char *clave)
{
	int hash = 5381;
	int c;
	while ((c = *clave++)) {
		hash = ((hash << 5) + hash) + c;
	}
	return (unsigned int)hash;
}

unsigned long calcular_posicion_hash(const char *clave, size_t capacidad)
{
	return (funcion_hash_djb2(clave) % capacidad);
}

par_t *crear_par(const char *clave, void *elemento)
{
	par_t *par_nuevo = calloc(1, sizeof(par_t));
	if (par_nuevo == NULL)
		return NULL;
	char *clave_copiada = malloc(strlen(clave) + 1);
	if (clave_copiada == NULL) {
		free(par_nuevo);
		return NULL;
	}
	strcpy(clave_copiada, clave);
	par_nuevo->clave = clave_copiada;
	par_nuevo->valor = elemento;
	return par_nuevo;
}

hash_t *hash_insertar_par(hash_t *hash, const char *clave, void *elemento,
			  void **anterior)
{
	unsigned long posicion_hash =
		calcular_posicion_hash(clave, hash->capacidad);
	par_t *par_actual = hash->pares[posicion_hash];
	par_t *par_anterior = NULL;
	while (par_actual != NULL) {
		if (strcmp(par_actual->clave, clave) == 0) {
			// Actualiza valor si repite clave
			if (anterior != NULL)
				*anterior = par_actual->valor;
			par_actual->valor = elemento;
			return hash;
		}
		par_anterior = par_actual;
		par_actual = par_actual->siguiente;
	}
	par_t *par_nuevo = crear_par(clave, elemento);
	if (!par_nuevo)
		return NULL;
	if (par_anterior == NULL) {
		// Actualiza el puntero del vector pares si no había guardado ningún par
		// anterior
		hash->pares[posicion_hash] = par_nuevo;
	} else {
		// Agrega al final si hay colisión
		par_anterior->siguiente = par_nuevo;
	}
	hash->cantidad++;
	if (anterior != NULL)
		*anterior = NULL;
	return hash;
}

hash_t *rehash(hash_t *hash, void **anterior)
{
	hash_t *hash_nuevo = hash_crear(hash->capacidad * 2);
	if (!hash_nuevo)
		return NULL;
	par_t *par_actual;
	for (int i = 0; i < hash->capacidad; i++) {
		par_actual = hash->pares[i];
		while (par_actual) {
			par_t *par_siguiente = par_actual->siguiente;
			if (!hash_insertar_par(hash_nuevo, par_actual->clave,
					       par_actual->valor, anterior)) {
				hash_destruir(hash_nuevo);
				return NULL;
			}
			free(par_actual->clave);
			free(par_actual);
			par_actual = par_siguiente;
		}
	}
	free(hash->pares);
	hash->pares = hash_nuevo->pares;
	hash->capacidad = hash_nuevo->capacidad;
	free(hash_nuevo);
	return hash;
}

hash_t *hash_insertar(hash_t *hash, const char *clave, void *elemento,
		      void **anterior)
{
	if (!hash || !clave)
		return NULL;
	if ((double)hash->cantidad / (double)hash->capacidad >
	    FACTOR_CARGA_MAXIMO) {
		hash = rehash(hash, anterior);
		if (!hash)
			return NULL;
	}
	return hash_insertar_par(hash, clave, elemento, anterior);
}

void *hash_quitar(hash_t *hash, const char *clave)
{
	if (!hash || !clave)
		return NULL;
	unsigned long posicion_hash =
		calcular_posicion_hash(clave, hash->capacidad);
	par_t *par_actual = hash->pares[posicion_hash];
	par_t *par_anterior = NULL;
	// Busco el par que coincida con la clave
	while (par_actual) {
		if (strcmp(par_actual->clave, clave) == 0) {
			// Si el par a eliminar es el primero de la lista de colisiones, actualizo
			// el puntero
			if (!par_anterior) {
				hash->pares[posicion_hash] =
					par_actual->siguiente;
			} else {
				par_anterior->siguiente = par_actual->siguiente;
			}
			void *elemento_quitado = par_actual->valor;
			free(par_actual->clave);
			free(par_actual);
			hash->cantidad--;
			return elemento_quitado;
		}
		par_anterior = par_actual;
		par_actual = par_actual->siguiente;
	}
	return NULL;
}

void *hash_buscar(par_t *par_actual, const char *clave)
{
	while (par_actual != NULL) {
		if (strcmp(par_actual->clave, clave) == 0) {
			return par_actual->valor;
		}
		par_actual = par_actual->siguiente;
	}
	return NULL;
}

void *hash_obtener(hash_t *hash, const char *clave)
{
	if (!hash || !clave)
		return NULL;
	unsigned long posicion_hash =
		calcular_posicion_hash(clave, hash->capacidad);
	return hash_buscar(hash->pares[posicion_hash], clave);
}

bool hash_contiene(hash_t *hash, const char *clave)
{
	if (!hash || !clave)
		return false;
	return (hash_obtener(hash, clave) != NULL);
}

size_t hash_cantidad(hash_t *hash)
{
	if (!hash)
		return 0;
	return hash->cantidad;
}

void hash_destruir(hash_t *hash)
{
	hash_destruir_todo(hash, NULL);
}

void hash_destruir_todo(hash_t *hash, void (*destructor)(void *))
{
	if (!hash)
		return;
	for (int i = 0; i < hash->capacidad; i++) {
		par_t *par = hash->pares[i];
		while (par != NULL) {
			par_t *par_liberado = par;
			if (destructor != NULL)
				destructor(par->valor);
			par = par->siguiente;
			free(par_liberado->clave);
			free(par_liberado);
		}
	}
	free(hash->pares);
	free(hash);
}

size_t hash_con_cada_clave(hash_t *hash,
			   bool (*f)(const char *clave, void *valor, void *aux),
			   void *aux)
{
	size_t n = 0;
	if (!hash || !f)
		return n;
	for (int i = 0; i < hash->capacidad; i++) {
		par_t *par_actual = hash->pares[i];
		while (par_actual != NULL) {
			n++;
			if (!f(par_actual->clave, par_actual->valor, aux)) {
				return n;
			}
			par_actual = par_actual->siguiente;
		}
	}
	return n;
}
