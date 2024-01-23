#ifndef MENU_H_
#define MENU_H_
#include "hash.h"
#include <stdio.h>

typedef struct _menu_t menu_t;
typedef struct comando comando_t;

/**** Se crea el TDA menu reutilizando las funciones del TDA HASH ****/

menu_t *crear_menu();

comando_t *crear_comando(const char *descripcion, void (*accion)(void *),
			 void *aux);

menu_t *insertar_comando(menu_t *menu, const char *clave, void *elemento);

bool contiene_comando(menu_t *menu, const char *clave);

comando_t *obtener_comando(menu_t *menu, const char *clave);

bool ejecutar_comando(comando_t *comando);

void imprimir_menu(menu_t *menu);

void menu_destruir(menu_t *menu);

size_t menu_con_cada_comando(menu_t *menu,
			     bool (*f)(const char *clave, void *valor,
				       void *aux),
			     void *aux);

#endif // MENU_H_