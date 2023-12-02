#include "juego.h"
#include "ataque.h"
#include "pokemon.h"
#include "tipo.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define CANT_MIN_POKEMONES 4
#define CANT_POKES_JUGADOR 3
#define CANT_ATAQUES_POKE 3

typedef struct ataque ataque_t;

typedef struct {
	int puntos;
	lista_t *pokemones;
	lista_t *ataques_sin_utilizar;
} jugador_t;

struct juego {
	bool finalizado;
	jugador_t *jugador1;
	jugador_t *jugador2;
	informacion_pokemon_t *pokemones;
	lista_t *lista_pokemones;
};

juego_t *juego_crear()
{
	juego_t *juego_nuevo = (juego_t *)calloc(1, sizeof(juego_t));
	return juego_nuevo;
}

JUEGO_ESTADO juego_cargar_pokemon(juego_t *juego, char *archivo)
{
	informacion_pokemon_t *pokemones_cargados =
		pokemon_cargar_archivo(archivo);
	if (!pokemones_cargados)
		return ERROR_GENERAL;
	if (pokemon_cantidad(pokemones_cargados) < CANT_MIN_POKEMONES) {
		pokemon_destruir_todo(pokemones_cargados);
		return POKEMON_INSUFICIENTES;
	}
	juego->pokemones = pokemones_cargados;
	return TODO_OK;
}

void listar_pokemon(pokemon_t *pokemon, void *lista)
{
	lista_insertar(lista, pokemon);
}

lista_t *juego_listar_pokemon(juego_t *juego)
{
	if (!juego || !juego->pokemones)
		return NULL;
	lista_t *lista_pokemones = lista_crear();
	if (!lista_pokemones)
		return NULL;
	juego->lista_pokemones = lista_pokemones;
	int cant_pokemones = pokemon_cantidad(juego->pokemones);
	int cant_leida = con_cada_pokemon(juego->pokemones, listar_pokemon,
					  lista_pokemones);
	if (cant_pokemones != cant_leida) {
		lista_destruir(lista_pokemones);
		return NULL;
	}
	return lista_pokemones;
}

lista_t *insertar_pokemones_jugador(jugador_t *jugador, pokemon_t *pokemon1,
				    pokemon_t *pokemon2, pokemon_t *pokemon3)
{
	lista_t *pokemones = lista_crear();
	if (!pokemones)
		return NULL;
	pokemones = lista_insertar(pokemones, pokemon1);
	pokemones = lista_insertar(pokemones, pokemon2);
	pokemones = lista_insertar(pokemones, pokemon3);
	return pokemones;
}

void insertar_ataque(const ataque_t *ataque, void *lista_ataques)
{
	lista_insertar(lista_ataques, (ataque_t *)ataque);
}

lista_t *insertar_ataques_sin_utilizar(jugador_t *jugador)
{
	lista_t *ataques = lista_crear();
	for (size_t pos = 0; pos < CANT_POKES_JUGADOR; pos++) {
		pokemon_t *pokemon_actual =
			lista_elemento_en_posicion(jugador->pokemones, pos);
		con_cada_ataque(pokemon_actual, insertar_ataque, ataques);
	}
	return ataques;
}

JUEGO_ESTADO juego_seleccionar_pokemon(juego_t *juego, JUGADOR jugador,
				       const char *nombre1, const char *nombre2,
				       const char *nombre3)
{
	if (!juego || !juego->pokemones)
		return ERROR_GENERAL;
	// No acepta pokemones repetidos:
	if (strcmp(nombre1, nombre2) == 0 || strcmp(nombre1, nombre3) == 0 ||
	    strcmp(nombre3, nombre2) == 0)
		return POKEMON_REPETIDO;
	// Verifica que exista el pokemon:
	pokemon_t *pokemon1 = pokemon_buscar(juego->pokemones, nombre1);
	pokemon_t *pokemon2 = pokemon_buscar(juego->pokemones, nombre2);
	pokemon_t *pokemon3 = pokemon_buscar(juego->pokemones, nombre3);
	if (!pokemon1 || !pokemon2 || !pokemon3)
		return POKEMON_INEXISTENTE;

	jugador_t *jugador_actual = (jugador_t *)calloc(1, sizeof(jugador_t));
	// Lista de pokemones seleccionados para el jugador:
	lista_t *pokemones = insertar_pokemones_jugador(
		jugador_actual, pokemon1, pokemon2, pokemon3);
	if (!pokemones)
		return ERROR_GENERAL;
	jugador_actual->pokemones = pokemones;
	// Lista de ataques a utilizar:
	lista_t *ataques = insertar_ataques_sin_utilizar(jugador_actual);
	if (!ataques)
		return ERROR_GENERAL;
	jugador_actual->ataques_sin_utilizar = ataques;

	if (jugador == JUGADOR1) {
		juego->jugador1 = jugador_actual;
	} else {
		juego->jugador2 = jugador_actual;
	}
	return TODO_OK;
}

size_t encontrar_posicion_ataque(lista_t *ataques, const ataque_t *ataque)
{
	for (size_t pos = 0; pos < (CANT_ATAQUES_POKE * CANT_POKES_JUGADOR);
	     pos++) {
		if (lista_elemento_en_posicion(ataques, pos) == ataque)
			return pos;
	}
	return 0;
}

RESULTADO_ATAQUE calcular_resultado_ataque(const ataque_t *ataque1,
					   pokemon_t *pokemon2,
					   jugador_t *jugador)
{
	// Compara el tipo de ataque del jugador con el tipo de pokemon adversario
	enum TIPO tipo1 = ataque1->tipo;
	enum TIPO tipo2 = pokemon_tipo(pokemon2);
	lista_quitar_de_posicion(
		jugador->ataques_sin_utilizar,
		encontrar_posicion_ataque(jugador->ataques_sin_utilizar,
					  ataque1));
	switch (tipo1) {
	case NORMAL:
		return ATAQUE_REGULAR;
	case FUEGO:
		if (tipo2 == PLANTA) {
			return ATAQUE_EFECTIVO;
		} else if (tipo2 == AGUA) {
			return ATAQUE_INEFECTIVO;
		}
		break;
	case AGUA:
		if (tipo2 == FUEGO) {
			return ATAQUE_EFECTIVO;
		} else if (tipo2 == ELECTRICO) {
			return ATAQUE_INEFECTIVO;
		}
		break;
	case PLANTA:
		if (tipo2 == ROCA) {
			return ATAQUE_EFECTIVO;
		} else if (tipo2 == FUEGO) {
			return ATAQUE_INEFECTIVO;
		}
		break;
	case ELECTRICO:
		if (tipo2 == AGUA) {
			return ATAQUE_EFECTIVO;
		} else if (tipo2 == ROCA) {
			return ATAQUE_INEFECTIVO;
		}
		break;
	case ROCA:
		if (tipo2 == ELECTRICO) {
			return ATAQUE_EFECTIVO;
		} else if (tipo2 == PLANTA) {
			return ATAQUE_INEFECTIVO;
		}
		break;
	default:
		return ATAQUE_ERROR;
	}
	return ATAQUE_REGULAR;
}

int calcular_poder_ataque(RESULTADO_ATAQUE resultado, const ataque_t *ataque)
{
	int poder = (int)ataque->poder;
	switch (resultado) {
	case ATAQUE_EFECTIVO:
		return (poder * 3);
	case ATAQUE_INEFECTIVO:
		return (poder + 1) / 2;
	case ATAQUE_REGULAR:
		return poder;
	default:
		return 0;
	}
}

int buscar_pokemon(void *pokemon, void *nombre)
{
	return (strcmp(pokemon_nombre(pokemon), nombre));
}

int buscar_ataque(void *ataque, void *nombre)
{
	return strcmp(((ataque_t *)ataque)->nombre, nombre);
}

resultado_jugada_t desarrollar_turno(const ataque_t *ataque1,
				     const ataque_t *ataque2,
				     pokemon_t *pokemon1, pokemon_t *pokemon2,
				     juego_t *juego)
{
	resultado_jugada_t resultado;
	// Calculo de resultado:
	resultado.jugador1 =
		calcular_resultado_ataque(ataque1, pokemon2, juego->jugador1);
	resultado.jugador2 =
		calcular_resultado_ataque(ataque2, pokemon1, juego->jugador2);
	// Calculo de puntaje:
	juego->jugador1->puntos +=
		calcular_poder_ataque(resultado.jugador1, ataque1);
	juego->jugador2->puntos +=
		calcular_poder_ataque(resultado.jugador2, ataque2);
	// Controlo finalizacion de juego:
	if (lista_vacia(juego->jugador1->ataques_sin_utilizar) ||
	    lista_vacia(juego->jugador2->ataques_sin_utilizar))
		juego->finalizado = true;
	return resultado;
}

resultado_jugada_t juego_jugar_turno(juego_t *juego, jugada_t jugada_jugador1,
				     jugada_t jugada_jugador2)
{
	resultado_jugada_t resultado;
	resultado.jugador1 = ATAQUE_ERROR;
	resultado.jugador2 = ATAQUE_ERROR;
	if (!juego || juego->finalizado)
		return resultado;
	// No existen los pokemones:
	pokemon_t *pokemon1 = lista_buscar_elemento(juego->jugador1->pokemones,
						    buscar_pokemon,
						    jugada_jugador1.pokemon);
	pokemon_t *pokemon2 = lista_buscar_elemento(juego->jugador2->pokemones,
						    buscar_pokemon,
						    jugada_jugador2.pokemon);
	if (!pokemon1 || !pokemon2)
		return resultado;
	// No existen los ataques:
	const ataque_t *ataque1 =
		pokemon_buscar_ataque(pokemon1, jugada_jugador1.ataque);
	const ataque_t *ataque2 =
		pokemon_buscar_ataque(pokemon2, jugada_jugador2.ataque);
	if (!ataque1 || !ataque2)
		return resultado;
	// Los ataques ya fueron utilizados:
	if (!lista_buscar_elemento(juego->jugador1->ataques_sin_utilizar,
				   buscar_ataque, jugada_jugador1.ataque) ||
	    !lista_buscar_elemento(juego->jugador2->ataques_sin_utilizar,
				   buscar_ataque, jugada_jugador2.ataque))
		return resultado;

	return desarrollar_turno(ataque1, ataque2, pokemon1, pokemon2, juego);
}

int juego_obtener_puntaje(juego_t *juego, JUGADOR jugador)
{
	if (!juego)
		return 0;
	if (jugador == JUGADOR1 && juego->jugador1) {
		return juego->jugador1->puntos;
	} else if (jugador == JUGADOR2 && juego->jugador2) {
		return juego->jugador2->puntos;
	}
	return 0;
}

bool juego_finalizado(juego_t *juego)
{
	return (!juego || juego->finalizado);
}

void juego_destruir(juego_t *juego)
{
	if (!juego)
		return;
	pokemon_destruir_todo(juego->pokemones);
	lista_destruir(juego->lista_pokemones);
	if (juego->jugador1) {
		lista_destruir(juego->jugador1->pokemones);
		lista_destruir(juego->jugador1->ataques_sin_utilizar);
		free(juego->jugador1);
	}
	if (juego->jugador2) {
		lista_destruir(juego->jugador2->pokemones);
		lista_destruir(juego->jugador2->ataques_sin_utilizar);
		free(juego->jugador2);
	}
	free(juego);
}
