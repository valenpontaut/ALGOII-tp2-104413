#include "src/adversario.h"
#include "src/ataque.h"
#include "src/juego.h"
#include "src/lista.h"
#include "src/menu.h"
#include "src/pokemon.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CARACTERES_MAX 100

typedef struct ataque ataque_t;

/**
 * Este main debe ser modificado para que el usuario pueda jugar el juego. Las
 * instrucciones existentes son solamente a modo ilustrativo del funcionamiento
 * muy alto nivel del juego.
 *
 * Las interacciones deben realizarse por entrada/salida estandar y estar
 * validadas.
 *
 * Se aconseja en todo momento mostrar información relevante para el jugador
 * (por ejemplo, mostrar puntaje actual y movimientos disponibles) para hacer
 * que el juego sea facil de utilizar.
 */

void imprimir_bienvenida()
{
	printf("====================================================================="
	       "===============\n\n¡Bienvenid@!\n\nTe explicamos las reglas de juego "
	       ";) \n\nVas a seleccionar tres pokemones (distintos) y, el tercero de ellos, "
	       "va a ser intercambiado por otro que haya seleccionado tu adversario. "
	       "Cada pokemon tiene 3 ataques posibles para realizar, con sus "
	       "respectivos poderes. Es así como entonces el juego va a durar 9 "
	       "turnos donde cada participante elige en cada turno un ataque de sus "
	       "pokemones que aún no haya utilizado. \nTus puntos se van a ir "
	       "sumando en relación al poder del ataque realizado y el tipo de "
	       "ataque y el pokemon del adversario.\n\n¡Mucha suerte!\n\n");
}

void print_ok(void *empty)
{
	printf(" ");
}

void imprimir_mi_puntaje(void *juego)
{
	printf("\nTu puntaje actual: %d\n",
	       juego_obtener_puntaje(juego, JUGADOR1));
}
void imprimir_puntaje_adversario(void *juego)
{
	printf("\nPuntaje actual de tu adversario: %d\n",
	       juego_obtener_puntaje(juego, JUGADOR2));
}

void imprimir_ataque(const ataque_t *ataque, void *aux)
{
	printf("Ataque: %s\n", ataque->nombre);
}

bool imprimir_pokemon(void *pokemon, void *aux)
{
	printf("\n=> %s\n", pokemon_nombre((pokemon_t *)pokemon));
	con_cada_ataque((pokemon_t *)pokemon, imprimir_ataque, NULL);
	return true;
}

void imprimir_pokemones(void *pokemones)
{
	lista_con_cada_elemento(pokemones, imprimir_pokemon, NULL);
}

JUEGO_ESTADO pedir_archivo(juego_t *juego)
{
	char archivo[CARACTERES_MAX];
	fscanf(stdin, "%100s", archivo);
	JUEGO_ESTADO estado_juego = juego_cargar_pokemon(juego, archivo);
	while (estado_juego != TODO_OK) {
		printf("\nHubo un error al cargar el archivo");
		if (estado_juego == ERROR_GENERAL) {
			printf(". Inténtelo de nuevo.\n");
		} else { // caso estado_juego == POKEMON_INSUFICIENTES
			printf(": El archivo seleccionado contiene pokemones insuficientes para el juego. Inténtelo de nuevo.\n");
		}
		printf("Por favor, para comenzar ingrese un archivo de pokemones válido: ");
		fscanf(stdin, "%100s", archivo);
		estado_juego = juego_cargar_pokemon(juego, archivo);
	}
	printf("\n¡Archivo cargado con éxito!\n\n");
	return estado_juego;
}

int comparar_pokemon(void *pokemon, void *nombre)
{
	return (strcmp(pokemon_nombre(pokemon), (char *)nombre));
}

JUEGO_ESTADO pedir_pokemones(juego_t *juego, char *nombre_jugador1,
			     char *nombre_jugador2, char *nombre_jugador3)
{
	printf("\nNombre del pokemon 1: ");
	fscanf(stdin, "%255s", nombre_jugador1);
	printf("Nombre del pokemon 2: ");
	fscanf(stdin, "%255s", nombre_jugador2);
	printf("Nombre del pokemon 3: ");
	fscanf(stdin, "%255s", nombre_jugador3);
	JUEGO_ESTADO juego_estado =
		juego_seleccionar_pokemon(juego, JUGADOR1, nombre_jugador1,
					  nombre_jugador2, nombre_jugador3);
	while (juego_estado != TODO_OK) {
		if (juego_estado == POKEMON_INEXISTENTE) {
			printf("\nAlgun/os de los pokemones elegidos son inexistentes. Intente ingresar tres nombres de pokemones diferentes.\n");
		} else if (juego_estado == POKEMON_REPETIDO) {
			printf("\nNo se pueden repetir pokemones. Intente ingresar tres nombres de pokemones diferentes.\n");
		} else {
			printf("\nOcurrió un error inesperado. Intentelo nuevamente.\n");
		}
		printf("\nNombre del pokemon 1: ");
		fscanf(stdin, "%255s", nombre_jugador1);
		printf("Nombre del pokemon 2: ");
		fscanf(stdin, "%255s", nombre_jugador2);
		printf("Nombre del pokemon 3: ");
		fscanf(stdin, "%255s", nombre_jugador3);
		juego_estado = juego_seleccionar_pokemon(juego, JUGADOR1,
							 nombre_jugador1,
							 nombre_jugador2,
							 nombre_jugador3);
	}
	return juego_estado;
}

menu_t *menu_crear(juego_t *juego, lista_t *pokemones,
		   lista_t *pokemones_jugador, lista_t *pokemones_adversario)
{
	menu_t *menu = crear_menu();
	menu = insertar_comando(menu, "1",
				crear_comando("Jugar próximo turno", print_ok,
					      NULL));
	menu = insertar_comando(
		menu, "2",
		crear_comando("Ver listado de todos los pokemones",
			      imprimir_pokemones, pokemones));
	menu = insertar_comando(menu, "3",
				crear_comando("Ver listado de mis pokemones",
					      imprimir_pokemones,
					      pokemones_jugador));
	menu = insertar_comando(
		menu, "4",
		crear_comando("Ver listado de los pokemones adversarios",
			      imprimir_pokemones, pokemones_adversario));
	menu = insertar_comando(menu, "5",
				crear_comando("Ver mi puntaje actual",
					      imprimir_mi_puntaje, juego));
	menu = insertar_comando(
		menu, "6",
		crear_comando("Ver puntaje actual de mi adversario",
			      imprimir_puntaje_adversario, juego));
	menu = insertar_comando(
		menu, "7", crear_comando("Abandonar juego", print_ok, NULL));
	return menu;
}

void destruir_todo(juego_t *juego, menu_t *menu, lista_t *pokemones_jugador,
		   lista_t *pokemones_adversario, adversario_t *adversario)
{
	if (juego) {
		juego_destruir(juego);
	}
	if (menu) {
		menu_destruir(menu);
	}
	lista_destruir(pokemones_jugador);
	lista_destruir(pokemones_adversario);
	adversario_destruir(adversario);
}

jugada_t jugador_pedir_nombre_y_ataque()
{
	jugada_t jugada;
	printf("Nombre del pokemon: ");
	fscanf(stdin, "%20s", jugada.pokemon);
	printf("Nombre de ataque: ");
	fscanf(stdin, "%20s", jugada.ataque);
	return jugada;
}

int main(int argc, char *argv[])
{
	juego_t *juego = juego_crear();
	if (!juego) {
		printf("Hubo un error al inicializar el juego. Disculpe las molestias\n");
		return 0;
	};
	imprimir_bienvenida();
	JUEGO_ESTADO juego_estado = TODO_OK;
	// Carga de archivo de pokemones
	printf("Por favor, para comenzar ingrese un archivo de pokemones válido: ");
	juego_estado = pedir_archivo(juego);
	while (juego_estado != TODO_OK) {
		printf("\nOcurrió un error inesperado. Intentelo nuevamente.\n");
		juego_estado = pedir_archivo(juego);
	}
	lista_t *pokemones = juego_listar_pokemon(juego);
	if (!pokemones) {
		destruir_todo(juego, NULL, NULL, NULL, NULL);
		return 0;
	}
	lista_t *pokemones_jugador = lista_crear();
	if (!pokemones_jugador) {
		destruir_todo(juego, NULL, NULL, NULL, NULL);
		return 0;
	}
	lista_t *pokemones_adversario = lista_crear();
	if (!pokemones_adversario) {
		destruir_todo(juego, NULL, pokemones_jugador, NULL, NULL);
		return 0;
	}
	menu_t *menu = menu_crear(juego, pokemones, pokemones_jugador,
				  pokemones_adversario);
	if (!menu) {
		printf("Hubo un error al crear el menú de opciones. Disculpe las molestias\n");
		destruir_todo(juego, NULL, pokemones_jugador,
			      pokemones_adversario, NULL);
		return 0;
	};
	printf("Lista de pokemones cargados: \n");
	imprimir_pokemones(pokemones);

	// El jugador elige los pokemones
	printf("\nA continuación elija tres pokemones. Los primeros dos serán tuyos y el tercero será de tu adversario: \n");
	char nombre_jugador1[CARACTERES_MAX];
	char nombre_jugador2[CARACTERES_MAX];
	char nombre_jugador3[CARACTERES_MAX];
	juego_estado = pedir_pokemones(juego, nombre_jugador1, nombre_jugador2,
				       nombre_jugador3);
	printf("\n¡Pokemones elegidos exitosamente!\n");

	// Creacion del adversario
	adversario_t *adversario = adversario_crear(pokemones);
	if (!adversario) {
		printf("Hubo un problema configurando tu adversario. Disculpe las molestias\n");
		destruir_todo(juego, menu, pokemones_jugador,
			      pokemones_adversario, NULL);
		return 0;
	}
	char nombre_adversario1[CARACTERES_MAX];
	char nombre_adversario2[CARACTERES_MAX];
	char nombre_adversario3[CARACTERES_MAX];
	char *p_nombre_adversario1 = nombre_adversario1;
	char *p_nombre_adversario2 = nombre_adversario2;
	char *p_nombre_adversario3 = nombre_adversario3;

	if (!adversario_seleccionar_pokemon(adversario, &p_nombre_adversario1,
					    &p_nombre_adversario2,
					    &p_nombre_adversario3)) {
		printf("Hubo un problema configurando tu adversario. Disculpe las molestias\n");
		destruir_todo(juego, menu, pokemones_jugador,
			      pokemones_adversario, adversario);
		return 0;
	}
	juego_seleccionar_pokemon(juego, JUGADOR2, nombre_adversario1,
				  nombre_adversario2, nombre_adversario3);
	// Inserto los pokemones que van a jugar cada jugador en sus respectivas listas
	pokemon_t *pokemon_jugador1 = lista_buscar_elemento(
		pokemones, comparar_pokemon, nombre_jugador1);
	pokemon_t *pokemon_jugador2 = lista_buscar_elemento(
		pokemones, comparar_pokemon, nombre_jugador2);
	pokemon_t *pokemon_jugador3 = lista_buscar_elemento(
		pokemones, comparar_pokemon, nombre_adversario3);
	pokemon_t *pokemon_adversario1 = lista_buscar_elemento(
		pokemones, comparar_pokemon, nombre_adversario1);
	pokemon_t *pokemon_adversario2 = lista_buscar_elemento(
		pokemones, comparar_pokemon, nombre_adversario2);
	pokemon_t *pokemon_adversario3 = lista_buscar_elemento(
		pokemones, comparar_pokemon, nombre_jugador3);
	lista_insertar(pokemones_jugador, pokemon_jugador1);
	lista_insertar(pokemones_jugador, pokemon_jugador2);
	lista_insertar(pokemones_jugador, pokemon_jugador3);
	lista_insertar(pokemones_adversario, pokemon_adversario1);
	lista_insertar(pokemones_adversario, pokemon_adversario2);
	lista_insertar(pokemones_adversario, pokemon_adversario3);

	printf("\nFinalmente la distribución de pokemones quedó de la siguiente manera:\n");

	printf("\n************************ TUS POKEMONES ***********************\n");
	imprimir_pokemones(pokemones_jugador);
	printf("\n***************** POKEMONES DE TU ADVERSARIO *****************\n");
	if (!adversario_pokemon_seleccionado(adversario, nombre_adversario1,
					     nombre_adversario2,
					     nombre_jugador3)) {
		destruir_todo(juego, menu, pokemones_jugador,
			      pokemones_adversario, adversario);
		return 0;
	}

	printf("\n¡Comienza el juego!\n");
	resultado_jugada_t resultado_ronda;
	while (!juego_finalizado(juego)) {
		printf("\n--------------------------------------------------------------\n");
		// Jugada del jugador
		printf("Seleccione el pokemon y ataque que quiera usar para este turno (recuerde que no puede repetir ataque)\n");
		jugada_t jugada_jugador1 = jugador_pedir_nombre_y_ataque();
		// Jugada del adversario
		jugada_t jugada_jugador2 =
			adversario_proxima_jugada(adversario);
		// Se juega la partida
		resultado_ronda = juego_jugar_turno(juego, jugada_jugador1,
						    jugada_jugador2);
		while (resultado_ronda.jugador1 == ATAQUE_ERROR) {
			printf("\nHubo un error seleccionando su jugada. Inténtelo de nuevo. \n");
			jugada_jugador1 = jugador_pedir_nombre_y_ataque();
			resultado_ronda = juego_jugar_turno(
				juego, jugada_jugador1, jugada_jugador2);
		}
		printf("\n//////////////////////////////////////////////////////////////\n=============== INFORMACIÓN PARA EL ADVERSARIO ===============\n");
		adversario_informar_jugada(adversario, jugada_jugador1);
		printf("==============================================================\n//////////////////////////////////////////////////////////////\n");

		printf("\n//////////////////////////////////////////////////////////////\n================= INFORMACIÓN PARA EL JUGADOR ================\n");
		adversario_informar_jugada(adversario, jugada_jugador2);
		printf("==============================================================\n//////////////////////////////////////////////////////////////\n");

		printf("\n=> RESULTADO DE LA PARTIDA\n");
		printf("\n# JUGADOR 1:\n");
		printf("Resultado de ataque: %s\n",
		       (resultado_ronda.jugador1 == ATAQUE_EFECTIVO ?
				"ATAQUE EFECTIVO" :
				((resultado_ronda.jugador1 ==
				  ATAQUE_INEFECTIVO) ?
					 "ATAQUE INEFECTIVO" :
					 "ATAQUE REGULAR")));
		printf("Puntos acumulados totales: %d\n",
		       juego_obtener_puntaje(juego, JUGADOR1));
		printf("\n# JUGADOR 2:\n");
		printf("Resultado de ataque: %s\n",
		       (resultado_ronda.jugador2 == ATAQUE_EFECTIVO ?
				"ATAQUE EFECTIVO" :
				((resultado_ronda.jugador2 ==
				  ATAQUE_INEFECTIVO) ?
					 "ATAQUE INEFECTIVO" :
					 "ATAQUE REGULAR")));
		printf("Puntos acumulados totales: %d\n",
		       juego_obtener_puntaje(juego, JUGADOR2));

		// Imprimo el menú de comandos
		imprimir_menu(menu);
		char menu_seleccion[CARACTERES_MAX];
		printf("\nInserte opción del menú: ");
		fscanf(stdin, "%5s", menu_seleccion);
		while (strcmp(menu_seleccion, "1") != 0 &&
		       strcmp(menu_seleccion, "7") != 0) {
			if (contiene_comando(menu, menu_seleccion)) {
				if (!ejecutar_comando(obtener_comando(
					    menu, menu_seleccion))) {
					printf("Ocurrió un error en la ejecución de la opcón seleccionada. Inténtelo de nuevo.");
				}
			} else {
				printf("Opción inválida. Inténtelo de nuevo.\n");
			}
			imprimir_menu(menu);
			printf("\nInserte opción del menú: ");
			fscanf(stdin, "%5s", menu_seleccion);
		}
		if (strcmp(menu_seleccion, "7") == 0)
			break;
	}
	printf("\n¡Gracias por participar!\n\n");
	destruir_todo(juego, menu, pokemones_jugador, pokemones_adversario,
		      adversario);
}
