<div align="right">
<img width="32px" src="img/algo2.svg">
</div>

# TP2

## Repositorio de Valentina Llanos Pontaut - 104413 - vllanos@fi.uba.ar

- Para compilar y ejecutar el juego:
```bash
make
```

- Para compilar y ejecutar las pruebas de la cátedra:
```bash
bash .algo2/test.sh
```
---
##  Funcionamiento

El presente trabajo práctico consistió en integrar los conceptos abordados a lo largo de la cursada de Algoritmos y Programación II del segundo cuatrimiestre de 2023. El desarrollo del mismo consistió en elegir distintos TDA's (que fuimos desarrollando en trabajos prácticos anteriores), utilizando un óptimo uso de memoria y cuidando la complejidad computacional de las diferentes funciones. Teniendo en cuenta todo esto, se desarrolló para este trabajo un juego que simula ser PvP donde cada jugador se enfrenta al otro en una batalla de pokemones. 

### Funcionalidad del juego:
- Se carga un archivo con los pokemones disponibles y sus respectivos ataques (ver ejemplo en `ejemplos/correcto.txt`)
- Cada jugador elige tres pokemones de esa lista sin repetir
- El tercer pokemon cargado se destina a formar parte de los pokemones del oponente
- Comienza el juego:
  - Dura 9 turnos si es que no se decide abandonar el juego antes (opción disponible en el menú)
  - Se elige pokemon y ataque en cada turno
  - Los puntos se acreditan según el poder de ataque y el tipo del pokemon atacado y el tipo del ataque elegido
- Finaliza el juego cuando todos los ataques disponibles fueron utilizados o el jugador elija terminarlo

### Demostración del juego:
- Bienvenida con instrucciones:

<div align="center">
        <img width="50%" src="img/juego/bienvenida.png">
</div>

- Subir archivo: Valida que exista y que contenga mínimo 4 pokemones diferentes. Sino pide reintentar cargarlo.

<div align="center">
        <img width="50%" src="img/juego/archivo.png">
</div>

- Elegir pokemon: Valida que exista y que no se repitan entre sí. Sino pide reintentar cargarlos.

<div align="center">
        <img width="40%" src="img/juego/elegir_pokemon.png">
</div>

- A continuación se imprime la distribución final de pokemones entre ambos jugadores y comienza el juego

<div align="center">
        <img width="40%" src="img/juego/mostrar_pokemones_inicio.png">
</div>

- Se elige el pokemon y ataque para jugar el primer turno. Este no debe estar repetido. Por consola se muestra el resultado del primer turno junto al ataque del adversario.

<div align="center">
        <img width="40%" src="img/juego/primer_turno.png">
</div>

- Se puede repetir el ataque si es que este se encuentra dos veces en tu lista de pokemones.
<div align="center">
        <img width="40%" src="img/juego/pkm_repetido_valido.png">
        <img width="40%" src="img/juego/pkm_repetido_invalido.png">
</div>

- Los ataques pueden tener distintos resultados: ATAQUE EFECTIVO, ATAQUE INEFECTIVO, ATAQUE REGULAR. Y esto determina la potencia de poder de ataque que luego sumará puntos al jugador
<div align="center">
        <img width="40%" src="img/juego/resultado_1.png">
        <img width="40%" src="img/juego/resultado_2.png">
</div>

- Finalizados los 9 turnos se anuncia si el jugador ganó, perdió o empató con su rival

<div align="center">
        <img width="30%" src="img/juego/ganador.png">
</div>

- El juego cuenta con un menú que se muestra cada vez que finaliza el turno:
<div align="center">
        <img width="40%" src="img/juego/menu_op2.png">
        <img width="40%" src="img/juego/menu_op6y7.png">
</div>
<div align="center">
        <img width="40%" src="img/juego/menu_op3.png">
        <img width="40%" src="img/juego/menu_op4.png">
</div>

- La opción 7 finaliza el juego por mas que no se hayan jugado los 9 turnos y la 1 continúa al siguiente turno
<div align="center">
        <img width="40%" src="img/juego/menu_op1.png">
</div>
<div align="center">
        <img width="40%" src="img/juego/menu_op7.png">
</div>


---

## Estructuras imprementadas

### TDA Juego

Para empezar, se implementó un TDA Juego que reutiliza las implementaciones del TDA Lista y del TDA Pokemones desarroladas en trabajos prácticos anteriores (TP1 y TP TDA Lista). Recordando estas estructuras, a continuacióon su diagramas de alocación de memoria, sumado al que quedó para el TDA Juego:
- TDA Lista:
<div align="center">
        <img width="55%" src="img/memory_allocation/tda_lista.png">
</div>

- TDA Pokemon:
<div align="center">
        <img width="55%" src="img/memory_allocation/tda_pokemon.jpg">
</div>

- TDA Juego:
<div align="center">
        <img width="55%" src="img/memory_allocation/tda_juego.png">
</div>

La complejidad de sus operaciones se detalla en la siguiente tabla:

<div align="center">
        <img width="70%" src="img/complejidad/complejidad_juego.png">
</div>

### TDA Adversario

Para el desarrollo del juego se precisó crear un TDA Adversario para simular lo mejor posible una interacción entre dos jugadores. Este selecciona los pokemones que va a utilizar en el juego contra el jugador, toma decisiones de ataque y simula como sería jugar con otro participante del que no sabríamos "como piensa". A continuación muestro el diagrama de alocación de memoria del adversario. 

<div align="center">
        <img width="55%" src="img/memory_allocation/tda_adversario.png">
</div>

**NOTA:** No desarrollo las estructuras lista_t, pokemon_t, ataque_t porque fueron mostradas en diagramas anteriores

La complejidad de sus operaciones se detalla en la siguiente tabla:

<div align="center">
        <img width="70%" src="img/complejidad/complejidad_adversario.png">
</div>

### TDA Menú

Se implementó un TDA Menú para hacer más rápida la búsqueda de comandos a la hora de interactuar con el menú del juego. Para ello se reutilizaron las implementaciones del TDA Hash desarrolladas en TP anteriores a éste. Los valores de las claves esta vez fueron estructuras comando_t donde se guardaron la descripción del comando, la función que ejecuta ese comando y el elemento que precisa la función del comando para funcionar correctamente. Recordando lo que fue el uso de memoria dinámica del TDA Hash, acontinuación su diagrama de alocación de memoria junto al modificado para el TDA Menú:

<div align="center">
        <img width="45%" src="img/memory_allocation/tda_hash.jpg">
        <img width="45%" src="img/memory_allocation/tda_menu.png">
</div>

La complejidad de sus operaciones se detalla en la siguiente tabla:

<div align="center">
        <img width="70%" src="img/complejidad/complejidad_menu.png">
</div>


