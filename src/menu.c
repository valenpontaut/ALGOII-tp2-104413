#include "menu.h"

menu_t *crear_menu(size_t capacidad)
{
	menu_t *menu_nuevo = (menu_t *)hash_crear(capacidad);
	return menu_nuevo;
}

menu_t *insertar_comando(menu_t *menu, const char *clave, void *elemento,
			 void **anterior)
{
	return (menu_t *)hash_insertar((hash_t *)menu, clave, elemento,
				       anterior);
}

bool contiene_comando(menu_t *menu, const char *clave)
{
	return hash_contiene((hash_t *)menu, clave);
}

void *obtener_comando(menu_t *menu, const char *clave)
{
	return hash_obtener((hash_t *)menu, clave);
}

void imprimir_menu(menu_t *menu)
{
	printf("**************************************************\n\n");

	printf("**************************************************\n\n");
}

void menu_destruir(menu_t *menu)
{
	hash_destruir((hash_t *)menu);
}

size_t menu_con_cada_comando(menu_t *menu,
			     bool (*f)(const char *clave, void *valor,
				       void *aux),
			     void *aux)
{
	return hash_con_cada_clave((hash_t *)menu, f, aux);
}