#include "adversario.h"
#include "juego.h"
#include "lista.h"
#include "pokemon.h"
#include "ataque.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define POKEMON_1 0
#define POKEMON_2 1
#define POKEMON_3 2

typedef struct ataque ataque_t;

struct adversario {
	int puntos;
	lista_t *ataques_sin_utilizar;
	lista_t *pokemones_seleccionados;
	lista_t *pokemones_listado;
	pokemon_t *proximo_pokemon;
	ataque_t *proximo_ataque;
};

adversario_t *adversario_crear(lista_t *pokemones_listado)
{
	if (!pokemones_listado)
		return NULL;
	adversario_t *adversario_nuevo =
		(adversario_t *)calloc(1, sizeof(adversario_t));
	if (!adversario_nuevo)
		return NULL;
	adversario_nuevo->pokemones_listado = pokemones_listado;
	lista_t *pokemones_seleccionados = lista_crear();
	if (!pokemones_seleccionados) {
		adversario_destruir(adversario_nuevo);
		return NULL;
	}
	adversario_nuevo->pokemones_seleccionados = pokemones_seleccionados;
	lista_t *ataques_sin_utilizar = lista_crear();
	if (!ataques_sin_utilizar) {
		adversario_destruir(adversario_nuevo);
		return NULL;
	}
	adversario_nuevo->ataques_sin_utilizar = ataques_sin_utilizar;
	return adversario_nuevo;
}

bool adversario_seleccionar_pokemon(adversario_t *adversario, char **nombre1,
				    char **nombre2, char **nombre3)
{
	if (!adversario || !adversario->pokemones_listado)
		return false;
	// Selecciona los primeros 3 pokemones de la lista (asumo que no estan repetidos)
	pokemon_t *pokemon1 = (pokemon_t *)lista_elemento_en_posicion(
		adversario->pokemones_listado, POKEMON_1);
	pokemon_t *pokemon2 = (pokemon_t *)lista_elemento_en_posicion(
		adversario->pokemones_listado, POKEMON_2);
	pokemon_t *pokemon3 = (pokemon_t *)lista_elemento_en_posicion(
		adversario->pokemones_listado, POKEMON_3);
	if (!pokemon1 || !pokemon2 || !pokemon3)
		return false;
	const char *nombre_poke1 = pokemon_nombre(pokemon1);
	const char *nombre_poke2 = pokemon_nombre(pokemon2);
	const char *nombre_poke3 = pokemon_nombre(pokemon3);
	if (!nombre_poke1 || !nombre_poke2 || !nombre_poke3)
		return false;
	strcpy(*nombre1, nombre_poke1);
	strcpy(*nombre2, nombre_poke2);
	strcpy(*nombre3, nombre_poke3);
	return true;
}

void print_ataque(const ataque_t *ataque, void *aux)
{
	printf("Ataque: %s\n", ataque->nombre);
}

bool print_pokemon(void *pokemon)
{
	printf("\n=> %s\n", pokemon_nombre((pokemon_t *)pokemon));
	con_cada_ataque((pokemon_t *)pokemon, print_ataque, NULL);
	return true;
}

int encontrar_pokemon(void *pokemon, void *nombre)
{
	return (strcmp(pokemon_nombre(pokemon), (char *)nombre));
}

void insertar_ataque_sin_utilizar(const ataque_t *ataque, void *lista_ataques)
{
	lista_insertar(lista_ataques, (ataque_t *)ataque);
}

bool insertar_seleccionados(adversario_t *adversario, pokemon_t *pokemon1,
			    pokemon_t *pokemon2, pokemon_t *pokemon3)
{
	adversario->pokemones_seleccionados =
		lista_insertar(adversario->pokemones_seleccionados, pokemon1);
	adversario->pokemones_seleccionados =
		lista_insertar(adversario->pokemones_seleccionados, pokemon2);
	adversario->pokemones_seleccionados =
		lista_insertar(adversario->pokemones_seleccionados, pokemon3);
	if (!adversario->pokemones_seleccionados)
		return false;
	int cant_ataques = 0;
	cant_ataques += con_cada_ataque(pokemon1, insertar_ataque_sin_utilizar,
					adversario->ataques_sin_utilizar);
	cant_ataques += con_cada_ataque(pokemon2, insertar_ataque_sin_utilizar,
					adversario->ataques_sin_utilizar);
	cant_ataques += con_cada_ataque(pokemon3, insertar_ataque_sin_utilizar,
					adversario->ataques_sin_utilizar);
	if (cant_ataques != 9)
		return false;
	return true;
}

bool adversario_pokemon_seleccionado(adversario_t *adversario, char *nombre1,
				     char *nombre2, char *nombre3)
{
	if (!adversario)
		return false;
	pokemon_t *pokemon1 = lista_buscar_elemento(
		adversario->pokemones_listado, encontrar_pokemon, nombre1);
	pokemon_t *pokemon2 = lista_buscar_elemento(
		adversario->pokemones_listado, encontrar_pokemon, nombre2);
	pokemon_t *pokemon3 = lista_buscar_elemento(
		adversario->pokemones_listado, encontrar_pokemon, nombre3);
	if (!pokemon1 || !pokemon2 || !pokemon3)
		return false;
	if (!insertar_seleccionados(adversario, pokemon1, pokemon2, pokemon3))
		return false;
	print_pokemon(pokemon1);
	print_pokemon(pokemon2);
	print_pokemon(pokemon3);
	return true;
}

size_t posicion_ataque(lista_t *ataques, const ataque_t *ataque)
{
	for (size_t pos = 0; pos < 9; pos++) {
		if (lista_elemento_en_posicion(ataques, pos) == ataque)
			return pos;
	}
	return 0;
}

jugada_t adversario_proxima_jugada(adversario_t *adversario)
{
	const char *pokemon_seleccionado =
		pokemon_nombre(adversario->proximo_pokemon);
	const char *ataque_seleccionado = adversario->proximo_ataque->nombre;
	jugada_t j;
	strcpy(j.ataque, ataque_seleccionado);
	strcpy(j.pokemon, pokemon_seleccionado);
	lista_quitar_de_posicion(
		adversario->ataques_sin_utilizar,
		posicion_ataque(adversario->ataques_sin_utilizar,
				adversario->proximo_ataque));
	return j;
}

int buscar_ataque_en_pokemon(void *pokemon, void *ataque)
{
	if (pokemon_buscar_ataque(pokemon,
				  (const char *)((ataque_t *)ataque)->nombre))
		return 0;
	return -1;
}

void adversario_informar_jugada(adversario_t *a, jugada_t j)
{
	a->proximo_ataque =
		lista_elemento_en_posicion(a->ataques_sin_utilizar, 0);
	a->proximo_pokemon = lista_buscar_elemento(a->pokemones_seleccionados,
						   buscar_ataque_en_pokemon,
						   a->proximo_ataque);
}

void adversario_destruir(adversario_t *adversario)
{
	lista_destruir(adversario->pokemones_seleccionados);
	lista_destruir(adversario->ataques_sin_utilizar);
	if (adversario) {
		free(adversario);
	}
}
