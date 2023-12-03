#include "menu.h"
#include <stdlib.h>

struct comando {
	const char *descripcion;
	void (*accion)(void *);
	void *aux;
};

menu_t *crear_menu()
{
	menu_t *menu_nuevo = (menu_t *)hash_crear(20);
	return menu_nuevo;
}

comando_t *crear_comando(const char *descripcion, void (*accion)(void *aux),
			 void *aux)
{
	comando_t *comando_nuevo = (comando_t *)calloc(1, sizeof(comando_t));
	if (!comando_nuevo)
		return NULL;
	comando_nuevo->descripcion = descripcion;
	comando_nuevo->accion = accion;
	comando_nuevo->aux = aux;
	return comando_nuevo;
}

menu_t *insertar_comando(menu_t *menu, const char *clave, void *elemento)
{
	return (menu_t *)hash_insertar((hash_t *)menu, clave, (void *)elemento,
				       NULL);
}

bool contiene_comando(menu_t *menu, const char *clave)
{
	return hash_contiene((hash_t *)menu, clave);
}

comando_t *obtener_comando(menu_t *menu, const char *clave)
{
	return (comando_t *)hash_obtener((hash_t *)menu, clave);
}

bool imprimir_comando(const char *clave, void *valor, void *aux)
{
	if (clave != NULL) {
		printf("%s. %s\n", clave, ((comando_t *)valor)->descripcion);
		return true;
	}
	return false;
}

bool ejecutar_comando(comando_t *comando)
{
	if (comando != NULL) {
		comando->accion(comando->aux);
		return true;
	}
	return false;
}

void destruir_comando(void *comando)
{
	free(comando);
}

void menu_destruir(menu_t *menu)
{
	hash_destruir_todo((hash_t *)menu, destruir_comando);
}

size_t menu_con_cada_comando(menu_t *menu,
			     bool (*f)(const char *clave, void *valor,
				       void *aux),
			     void *aux)
{
	return hash_con_cada_clave((hash_t *)menu, f, aux);
}

void imprimir_menu(menu_t *menu)
{
	printf("\n**************************************************************\n\nMenú del juego:\n\n");
	menu_con_cada_comando(menu, imprimir_comando, NULL);
	printf("\n**************************************************************\n");
}