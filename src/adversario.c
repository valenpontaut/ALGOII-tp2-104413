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
	//lista_t *ataques_sin_utilizar;
	lista_t *pokemones_listado;
	//lista_t *pokemones_seleccionados;
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
	print_pokemon(pokemon1);
	print_pokemon(pokemon2);
	print_pokemon(pokemon3);
	return true;
}

jugada_t adversario_proxima_jugada(adversario_t *adversario)
{
	char *ataque_seleccionado = "Lanzallamas";
	char *pokemon_seleccionado = "Charmander";
	jugada_t j;
	strcpy(j.ataque, ataque_seleccionado);
	strcpy(j.pokemon, pokemon_seleccionado);
	return j;
}

void adversario_informar_jugada(adversario_t *a, jugada_t j)
{
	printf("\n¡Tu oponente ha atacado!\n\n");
	printf("Pokemón utilizado: %s\n", j.pokemon);
	printf("Ataque utilizado: %s\n\n", j.ataque);
}

void adversario_destruir(adversario_t *adversario)
{
	if (adversario) {
		free(adversario);
	}
}
