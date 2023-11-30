#include "juego.h"
#include "lista.h"
#include "tipo.h"
#include <stdbool.h>
#include "pokemon.h"
#include "ataque.h"
#include <stdlib.h>
#include <string.h>

#define CANT_MIN_POKEMONES 4
#define CANT_POKES_JUGADOR 3

typedef struct {
        int puntos;
        lista_t *pokemones;
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
        informacion_pokemon_t *pokemones_cargados = pokemon_cargar_archivo(archivo);
        if(!pokemones_cargados){
                //juego_destruir(juego);
                return ERROR_GENERAL;
        } else if (pokemon_cantidad(pokemones_cargados) < CANT_MIN_POKEMONES){
                //juego_destruir(juego);
                return POKEMON_INSUFICIENTES;
        }
        juego->pokemones = pokemones_cargados;
        return TODO_OK;        
}

void listar_pokemon(pokemon_t *pokemon, void *lista){
        lista_insertar(lista, pokemon);
}

lista_t *juego_listar_pokemon(juego_t *juego)
{
        if(!juego || !juego->pokemones){
                juego_destruir(juego);
                return NULL;
        } 
	lista_t *lista_pokemones = lista_crear();
        int cant_pokemones = pokemon_cantidad(juego->pokemones);
        if(lista_pokemones){
                int cant_leida = con_cada_pokemon(juego->pokemones, listar_pokemon, lista_pokemones);
                if (cant_pokemones != cant_leida){
                        //juego_destruir(juego);
                        return NULL;
                }
                juego->lista_pokemones = lista_pokemones;
                return lista_pokemones;
        }
        return NULL;
}

JUEGO_ESTADO juego_seleccionar_pokemon(juego_t *juego, JUGADOR jugador,
				       const char *nombre1, const char *nombre2,
				       const char *nombre3)
{
        if(!juego || !juego->pokemones) return ERROR_GENERAL;
        if(strcmp(nombre1,nombre2) == 0 || strcmp(nombre1,nombre3) == 0 || strcmp(nombre3,nombre2) == 0) return POKEMON_REPETIDO;
        pokemon_t *pokemon1 = pokemon_buscar(juego->pokemones, nombre1);
        pokemon_t *pokemon2 = pokemon_buscar(juego->pokemones, nombre2);
        pokemon_t *pokemon3 = pokemon_buscar(juego->pokemones, nombre3);
        if (!pokemon1 || !pokemon2 || !pokemon3) return POKEMON_INEXISTENTE;
        jugador_t *jugador_actual = (jugador_t *)calloc(1, sizeof(jugador_t));
        lista_t *pokemones = lista_crear();
        pokemones = lista_insertar(pokemones, pokemon1);
        pokemones = lista_insertar(pokemones, pokemon2);
        pokemones = lista_insertar(pokemones, pokemon3);
        if(!pokemones) return ERROR_GENERAL;
        jugador_actual->pokemones = pokemones;
        if(jugador == JUGADOR1) {
                juego->jugador1 = jugador_actual;
        } else {
                juego->jugador2 = jugador_actual;
        }
	return TODO_OK;
}

/*
typedef enum {
	TODO_OK,
	POKEMON_INSUFICIENTES,
	ERROR_GENERAL,
	POKEMON_INEXISTENTE,
	POKEMON_REPETIDO
} JUEGO_ESTADO;

typedef struct juego juego_t;

typedef enum {
	ATAQUE_ERROR,
	ATAQUE_EFECTIVO,
	ATAQUE_INEFECTIVO,
	ATAQUE_REGULAR
} RESULTADO_ATAQUE;

typedef enum { JUGADOR1, JUGADOR2 } JUGADOR;

typedef struct {
	char pokemon[20];
	char ataque[20];
} jugada_t;

typedef struct {
	RESULTADO_ATAQUE jugador1;
	RESULTADO_ATAQUE jugador2;
} resultado_jugada_t;
*/

int buscar_pokemon(void *pokemon, void *nombre){
        return(strcmp(pokemon_nombre(pokemon),nombre));
}

resultado_jugada_t juego_jugar_turno(juego_t *juego, jugada_t jugada_jugador1,
				     jugada_t jugada_jugador2)
{
	resultado_jugada_t resultado;
	resultado.jugador1 = ATAQUE_ERROR;
	resultado.jugador2 = ATAQUE_ERROR;
        if(!juego) return resultado;
        pokemon_t *pokemon1 = lista_buscar_elemento(juego->jugador1->pokemones, buscar_pokemon, jugada_jugador1.pokemon);
        pokemon_t *pokemon2 = lista_buscar_elemento(juego->jugador2->pokemones, buscar_pokemon, jugada_jugador2.pokemon);
        if(!pokemon1 || !pokemon2) return resultado;
        const struct ataque *ataque1 = pokemon_buscar_ataque(pokemon1, jugada_jugador1.ataque);
        const struct ataque *ataque2 = pokemon_buscar_ataque(pokemon2, jugada_jugador2.ataque);
        if(!ataque1 || !ataque2) return resultado;
        
        return resultado;
}

int juego_obtener_puntaje(juego_t *juego, JUGADOR jugador)
{
        if(!juego) return 0;
	if (jugador == JUGADOR1){
                return juego->jugador1->puntos;
        } else if (jugador == JUGADOR2) {
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
        if (!juego) return;
        pokemon_destruir_todo(juego->pokemones);
        lista_destruir(juego->lista_pokemones);
        if(juego->jugador1){
                lista_destruir(juego->jugador1->pokemones);
                free(juego->jugador1);
        } 
        if(juego->jugador2){
                lista_destruir(juego->jugador2->pokemones);
                free(juego->jugador2);
        }
        free(juego);
}
