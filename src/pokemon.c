#include "pokemon.h"
#include "tipo.h"
#include "ataque.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define CANT_ATAQUES 3
#define MAX_CARACTERES 20
#define MAX_CARACTERES_LINEA 100
#define SSCANF_LINEA "%20[^;];%20[^;];%20[^;]"

typedef struct ataque ataque_t;

struct pokemon {
	char nombre[MAX_CARACTERES];
	enum TIPO tipo;
	ataque_t ataques[CANT_ATAQUES];
};

struct info_pokemon {
	pokemon_t *pokemones;
	int cant_poke;
};

enum TIPO parsear_tipo(char *tipo)
{
	if (strlen(tipo) > 2)
		return -1;
	switch (*tipo) {
	case 'N':
		return NORMAL;
	case 'F':
		return FUEGO;
	case 'A':
		return AGUA;
	case 'P':
		return PLANTA;
	case 'E':
		return ELECTRICO;
	case 'R':
		return ROCA;
	default:
		return -1;
	}
}

bool es_numero_ui(char *poder)
{
	for (int i = 0; i < strlen(poder) - 1; i++) {
		if (!isdigit(poder[i]))
			return false;
	}
	return true;
}

// Para ordenar la ip utilizo un metodo de ordenamiento por inserción
informacion_pokemon_t *ordenar_ip(informacion_pokemon_t *ip)
{
	int poke_total = ip->cant_poke;
        if(poke_total >= 2){
                int poke_i;
                int poke_j;
                pokemon_t aux_poke;
                for (poke_i = 1; poke_i < poke_total; poke_i++) {
                        aux_poke = ip->pokemones[poke_i];
                        poke_j = poke_i - 1;
                        while (poke_j >= 0 &&
                        strcmp(aux_poke.nombre, ip->pokemones[poke_j].nombre) < 0) {
                                ip->pokemones[poke_j + 1] = ip->pokemones[poke_j];
                                poke_j--;
                        }
                        ip->pokemones[poke_j + 1] = aux_poke;
                }       
        }
	return ip;
}

bool guardar_pokemon_valido(informacion_pokemon_t *info_poke, pokemon_t nuevo_pokemon){
        pokemon_t *aux_pokemones = (pokemon_t *)realloc(info_poke->pokemones,(long unsigned int)(info_poke->cant_poke + 1) * sizeof(pokemon_t));
        if (aux_pokemones == NULL) {
                return false;
        }
        info_poke->pokemones = aux_pokemones;
        info_poke->pokemones[info_poke->cant_poke] = nuevo_pokemon;
        info_poke->cant_poke++;
        return true;
}

bool leer_archivo(FILE *archivo, informacion_pokemon_t *info_poke)
{
        char nombre[MAX_CARACTERES];
        char tipo[MAX_CARACTERES];
        char poder[MAX_CARACTERES];
	char linea[MAX_CARACTERES_LINEA];
        int cant_ataques = 0;
	while (fgets(linea, sizeof(linea), archivo)) {
                pokemon_t nuevo_pokemon;
                int campos_leidos = sscanf(linea, SSCANF_LINEA, nombre, tipo, poder);
                if (campos_leidos == 2 && cant_ataques == 0){
                        strcpy(nuevo_pokemon.nombre, nombre);
                        nuevo_pokemon.tipo = parsear_tipo(tipo);
                        if (nuevo_pokemon.tipo == -1)
                                break; // tipo de pokemon invalido
                } else if ((campos_leidos == 3) && (cant_ataques <= CANT_ATAQUES)) {
                        strcpy(nuevo_pokemon.ataques[cant_ataques - 1].nombre, nombre);
                        nuevo_pokemon.ataques[cant_ataques - 1].tipo = parsear_tipo(tipo);
                        if (nuevo_pokemon.ataques[cant_ataques - 1].tipo == -1)
                                break; // tipo de ataque invalido
                        if (!es_numero_ui(poder))
                                break; // poder de ataque invalido
                        nuevo_pokemon.ataques[cant_ataques - 1].poder = (unsigned int)atoi(poder);
                        if (cant_ataques == CANT_ATAQUES)
                                if(!guardar_pokemon_valido(info_poke, nuevo_pokemon)) break;
                } else {
                        break;
                }
                cant_ataques = (cant_ataques + 1) % (CANT_ATAQUES + 1);
        }
	if (info_poke->cant_poke == 0) {
		return false; // No se puede cargar archivos sin pokemones validos
	}
	return true;
}

informacion_pokemon_t *pokemon_cargar_archivo(const char *path)
{
	if (path == NULL)
		return NULL;
	FILE *archivo;
	archivo = fopen(path, "r");
	if (archivo == NULL)
		return NULL;
	informacion_pokemon_t *info_poke = (informacion_pokemon_t *)calloc(
		1, sizeof(informacion_pokemon_t));
	if (info_poke == NULL){
		fclose(archivo);
		return NULL;
        }
	if (!leer_archivo(archivo, info_poke)) {
		fclose(archivo);
		pokemon_destruir_todo(info_poke);
		return NULL; //Si es un archivo invalido, retorna NULL
	}
	fclose(archivo);
	return ordenar_ip(info_poke);
}

pokemon_t *pokemon_buscar(informacion_pokemon_t *ip, const char *nombre)
{
	if (ip == NULL || nombre == NULL)
		return NULL;
	for (int i = 0; i < ip->cant_poke; i++) {
		if (strcmp(nombre, ip->pokemones[i].nombre) == 0) {
			return &ip->pokemones[i];
		}
	}
	return NULL;
}

int pokemon_cantidad(informacion_pokemon_t *ip)
{
	if (ip == NULL)
		return 0;
	else
		return ip->cant_poke;
}

const char *pokemon_nombre(pokemon_t *pokemon)
{
	if (pokemon == NULL)
		return NULL;
	else
		return pokemon->nombre;
}

enum TIPO pokemon_tipo(pokemon_t *pokemon)
{
	if (pokemon == NULL)
		return NORMAL;
	else if (pokemon->tipo)
		return pokemon->tipo;
	else
		return NORMAL;
}

const struct ataque *pokemon_buscar_ataque(pokemon_t *pokemon,
					   const char *nombre)
{
	if (pokemon == NULL || nombre == NULL)
		return NULL;
	for (int i = 0; i < CANT_ATAQUES; i++) {
		if (strcmp(nombre, pokemon->ataques[i].nombre) == 0)
			return &pokemon->ataques[i];
	}
	return NULL;
}

int con_cada_pokemon(informacion_pokemon_t *ip, void (*f)(pokemon_t *, void *),
		     void *aux)
{
	if (ip == NULL || f == NULL)
		return 0;
	int total_poke = ip->cant_poke;
	int i;
	for (i = 0; i < total_poke; i++) {
		f(&ip->pokemones[i], aux);
	}
	return total_poke;
}

int con_cada_ataque(pokemon_t *pokemon,
		    void (*f)(const struct ataque *, void *), void *aux)
{
	if (pokemon == NULL || f == NULL)
		return 0;
	int i;
	int cant_ataques = 0;
	for (i = 0; i < CANT_ATAQUES; i++) {
		f(&pokemon->ataques[i], aux);
		cant_ataques++;
	}
	return cant_ataques;
}

void pokemon_destruir_todo(informacion_pokemon_t *ip)
{
	if (ip == NULL) {
		return;
	}
	if (ip->pokemones != NULL) {
		free(ip->pokemones);
	}
	free(ip);
}