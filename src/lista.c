#include "lista.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct nodo {
	void *elemento;
	struct nodo *siguiente;
} nodo_t;

struct lista {
	nodo_t *nodo_inicio;
	nodo_t *nodo_final;
	size_t cant_elem;
};

struct lista_iterador {
	nodo_t *nodo_actual;
};

lista_t *lista_crear()
{
	lista_t *lista = (lista_t *)calloc(1, sizeof(lista_t));
	return lista;
}

nodo_t *crear_nodo(void *elemento)
{
	nodo_t *nodo_nuevo = (nodo_t *)calloc(1, sizeof(nodo_t));
	if (nodo_nuevo == NULL)
		return NULL;
	nodo_nuevo->elemento = elemento;
	return nodo_nuevo;
}

lista_t *lista_insertar(lista_t *lista, void *elemento)
{
	if (lista == NULL)
		return NULL;
	nodo_t *nodo_nuevo = crear_nodo(elemento);
	if (nodo_nuevo == NULL)
		return NULL;
	if (lista->nodo_inicio == NULL) {
		lista->nodo_inicio = nodo_nuevo;
		lista->nodo_final = nodo_nuevo;
	} else {
		nodo_t *aux = lista->nodo_final;
		aux->siguiente = nodo_nuevo;
		lista->nodo_final = nodo_nuevo;
	}
	lista->cant_elem++;
	return lista;
}

lista_t *lista_insertar_en_posicion(lista_t *lista, void *elemento,
				    size_t posicion)
{
	if (!lista) {
		return NULL;
	}
	if (posicion >= lista->cant_elem || lista_vacia(lista)) {
		lista_insertar(lista, elemento);
	} else {
		nodo_t *nodo_nuevo = crear_nodo(elemento);
		if (nodo_nuevo == NULL)
			return NULL;
		nodo_t *aux = lista->nodo_inicio;
		if (posicion == 0) {
			nodo_nuevo->siguiente = aux;
			lista->nodo_inicio = nodo_nuevo;
		} else {
			size_t i = 0;
			while (aux->siguiente != NULL && i < posicion - 1) {
				aux = aux->siguiente;
				i++;
			}
			nodo_nuevo->siguiente = aux->siguiente;
			aux->siguiente = nodo_nuevo;
		}
		if (nodo_nuevo->siguiente == NULL)
			lista->nodo_final = nodo_nuevo;
		lista->cant_elem++;
	}
	return lista;
}

void *lista_quitar(lista_t *lista)
{
	if (lista == NULL || lista->nodo_inicio == NULL)
		return NULL;
	void *elemento_removido = lista->nodo_final->elemento;
	nodo_t *nodo_removido = lista->nodo_final;
	if (lista->nodo_inicio == lista->nodo_final) {
		// Eliminar en lista con 1 elemento
		lista->nodo_inicio = lista->nodo_final = NULL;
	} else {
		nodo_t *aux = lista->nodo_inicio;
		while (aux->siguiente != lista->nodo_final) {
			aux = aux->siguiente;
		}
		aux->siguiente = NULL;
		lista->nodo_final = aux;
	}
	free(nodo_removido);
	lista->cant_elem--;
	return elemento_removido;
}

void *lista_quitar_de_posicion(lista_t *lista, size_t posicion)
{
	if (lista == NULL || lista->nodo_inicio == NULL)
		return NULL;
	if (posicion == 0 || lista->nodo_inicio == lista->nodo_final) {
		// Eliminar del inicio:
		nodo_t *nodo_removido = lista->nodo_inicio;
		void *elemento_removido = lista->nodo_inicio->elemento;
		lista->nodo_inicio = lista->nodo_inicio->siguiente;
		if (lista->nodo_inicio == NULL)
			lista->nodo_final = NULL;
		free(nodo_removido);
		lista->cant_elem--;
		return elemento_removido;
	} else {
		nodo_t *nodo_anterior = NULL;
		nodo_t *nodo_actual = lista->nodo_inicio;
		size_t i = 0;
		while (nodo_actual->siguiente != NULL && i < posicion) {
			nodo_anterior = nodo_actual;
			nodo_actual = nodo_actual->siguiente;
			i++;
		}
		if (nodo_actual->siguiente == NULL) {
			// Eliminar del final:
			nodo_anterior->siguiente = NULL;
			lista->nodo_final = nodo_anterior;
		} else {
			// Eliminar del medio:
			nodo_anterior->siguiente = nodo_actual->siguiente;
		}
		nodo_t *nodo_removido = nodo_actual;
		void *elemento_removido = nodo_actual->elemento;
		free(nodo_removido);
		lista->cant_elem--;
		return elemento_removido;
	}
}

void *lista_elemento_en_posicion(lista_t *lista, size_t posicion)
{
	if (lista == NULL)
		return NULL;
	size_t i = 0;
	nodo_t *nodo = lista->nodo_inicio;
	while (nodo != NULL && i < posicion) {
		nodo = nodo->siguiente;
		i++;
	}
	if (nodo == NULL)
		return NULL;
	return nodo->elemento;
}

void *lista_buscar_elemento(lista_t *lista, int (*comparador)(void *, void *),
			    void *contexto)
{
	if (lista == NULL || comparador == NULL)
		return NULL;
	nodo_t *nodo = lista->nodo_inicio;
	while (nodo != NULL) {
		if (comparador(nodo->elemento, contexto) == 0)
			return nodo->elemento;
		nodo = nodo->siguiente;
	}
	return NULL;
}

void *lista_primero(lista_t *lista)
{
	if (lista == NULL || lista->nodo_inicio == NULL)
		return NULL;
	return lista->nodo_inicio->elemento;
}

void *lista_ultimo(lista_t *lista)
{
	if (lista == NULL || lista->nodo_inicio == NULL)
		return NULL;
	return lista->nodo_final->elemento;
}

bool lista_vacia(lista_t *lista)
{
	return (lista == NULL || lista->nodo_inicio == NULL);
}

size_t lista_tamanio(lista_t *lista)
{
	if (lista == NULL)
		return 0;
	return lista->cant_elem;
}

void lista_destruir(lista_t *lista)
{
	lista_destruir_todo(lista, NULL);
}

void lista_destruir_todo(lista_t *lista, void (*funcion)(void *))
{
	if (lista == NULL)
		return;
	nodo_t *nodo_actual = lista->nodo_inicio;
	nodo_t *nodo_siguiente;
	while (nodo_actual != NULL) {
		nodo_siguiente = nodo_actual->siguiente;
		if (funcion != NULL)
			funcion(nodo_actual->elemento);
		free(nodo_actual);
		nodo_actual = nodo_siguiente;
	}
	free(lista);
}

lista_iterador_t *lista_iterador_crear(lista_t *lista)
{
	if (lista == NULL)
		return NULL;
	lista_iterador_t *iterador = malloc(sizeof(lista_iterador_t));
	if (iterador == NULL)
		return NULL;
	// Cuando creo el iterador, lo coloco apuntando al primer nodo de la lista
	iterador->nodo_actual = lista->nodo_inicio;
	return iterador;
}

bool lista_iterador_tiene_siguiente(lista_iterador_t *iterador)
{
	if (iterador == NULL || iterador->nodo_actual == NULL)
		return false;
	return (iterador->nodo_actual != NULL);
}

bool lista_iterador_avanzar(lista_iterador_t *iterador)
{
	if (iterador == NULL || iterador->nodo_actual == NULL)
		return false;
	iterador->nodo_actual = iterador->nodo_actual->siguiente;
	return lista_iterador_tiene_siguiente(iterador);
}

void *lista_iterador_elemento_actual(lista_iterador_t *iterador)
{
	if (iterador == NULL || iterador->nodo_actual == NULL)
		return NULL;
	return iterador->nodo_actual->elemento;
}

void lista_iterador_destruir(lista_iterador_t *iterador)
{
	if (iterador == NULL)
		return;
	free(iterador);
}

size_t lista_con_cada_elemento(lista_t *lista, bool (*funcion)(void *, void *),
			       void *contexto)
{
	if (lista == NULL || lista->nodo_inicio == NULL || funcion == NULL)
		return 0;
	nodo_t *nodo = lista->nodo_inicio;
	size_t contador = 1;
	while (funcion(nodo->elemento, contexto) && nodo->siguiente != NULL) {
		contador++;
		nodo = nodo->siguiente;
	}
	return contador;
}
