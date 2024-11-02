#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_CONGRESISTAS 200
#define MAX_SENADORES 50
#define MAX_DIPUTADOS 100
#define MAX_COMISIONES 10

struct congresista {
    char *nombre;
    char *rut;
    char *ocupacion;
    char *especializacion;
};

struct nodoCongresista {
    struct congresista *datos;
    struct nodoCongresista *sig;
};

struct congreso {
    struct congresista **senadores;
    struct congresista **diputados;
    struct comision **comDiputados;
    struct comision **comSendores;
    struct nodoProyectoLey *raiz;
};

struct nodoProyectoLey {
    struct proyectoLey *datos;
    struct nodoProyectoLey *izq, *der;
};

struct proyectoLey {
    char *nombre;
    char *tipo;
    int idProyecto;
    int urgencia;
    struct nodoArticulo *articulo;
    struct nodoVotacion *votacion;
    struct comision **comision;
    int fase;
};

struct comision {
    struct nodoCongresista *headIntegrantes;
    char *nombre;
    int totalIntegrantes;
    char *descripcion;
};


struct nodoArticulo {
    struct articulo *datos;
    struct nodoArticulo *sig, *ant;
};

struct articulo {
    char *nombre;
    int seccion;
    char *texto;
    char *cambio;
};


struct nodoVotacion {
    struct votacion *datos;
    struct nodoVotacion *sig;
};

struct votacion {
    struct nodoCongresista *favor;
    struct nodoCongresista *contra;
    char *fase;
};

// Function prototype
void menuProyectosLey();

void menuCongresistas();

void menuComisiones();

void funcionSwitch(char opcion, struct congreso *congreso, void (*submenu)(struct congreso *));

//---------------------------------------------------
/*Esta función almacena memoria para todos los datos que debe almacenar "congreso"*/
struct congreso *inicializarCongreso() {
    struct congreso *nuevoCongreso = (struct congreso *) malloc(sizeof(struct congreso));

    if (nuevoCongreso == NULL) {
        return NULL;
    }

    // Inicializa los arreglos para los senadores y diputados
    nuevoCongreso->senadores = (struct congresista **) calloc(MAX_SENADORES, sizeof(struct congresista *));
    nuevoCongreso->diputados = (struct congresista **) calloc(MAX_DIPUTADOS, sizeof(struct congresista *));

    if (nuevoCongreso->senadores == NULL || nuevoCongreso->diputados == NULL) {
        free(nuevoCongreso);
        return NULL;
    }

    // Inicializa los arreglos para las comisiones de senadores y diputados
    nuevoCongreso->comSendores = (struct comision **) calloc(MAX_COMISIONES, sizeof(struct comision *));
    nuevoCongreso->comDiputados = (struct comision **) calloc(MAX_COMISIONES, sizeof(struct comision *));

    if (nuevoCongreso->comSendores == NULL || nuevoCongreso->comDiputados == NULL) {
        free(nuevoCongreso->senadores);
        free(nuevoCongreso->diputados);
        free(nuevoCongreso);
        return NULL;
    }

    // Inicializa la raíz de proyectos de ley
    nuevoCongreso->raiz = NULL;
    return nuevoCongreso;
}

void liberarCongreso(struct congreso *congreso) {
    if (congreso == NULL) {
        return;
    }

    // Libera los arreglos de punteros si fueron asignados
    if (congreso->senadores != NULL) {
        free(congreso->senadores);
    }
    if (congreso->diputados != NULL) {
        free(congreso->diputados);
    }
    if (congreso->comSendores != NULL) {
        free(congreso->comSendores);
    }
    if (congreso->comDiputados != NULL) {
        free(congreso->comDiputados);
    }

    free(congreso);
}

// Función para crear un nuevo nodo para un proyecto de ley
struct nodoProyectoLey *crearNodoProyectoLey(struct proyectoLey *datos) {
    struct nodoProyectoLey *nodo = NULL;

    // Validar que los datos no sean NULL
    if (datos != NULL) {
        nodo = (struct nodoProyectoLey *) malloc(sizeof(struct nodoProyectoLey));

        if (nodo == NULL) {
            printf("Error: No se pudo asignar memoria para el nodo de proyecto de ley.\n");
            return NULL;
        }

        nodo->datos = datos; // Copiar los datos recibidos
        nodo->izq = NULL;
        nodo->der = NULL;
    }

    return nodo;
}

/*Tanto la fase debería ser asignada automáticamente para que siempre sea la primera, seguramente, después de todo...
 * no tiene demasiado sentido que comience desde una fase adelantada
 */
// Función para leer un número entero dentro de un rango
int leerEnteroConLimite(const char *mensaje, int min, int max) {
    int valor;
    char input[10];
    while (1) {
        printf("%s (%d-%d): ", mensaje, min, max);
        scanf("%9s", input);
        valor = atoi(input);
        if (valor >= min && valor <= max) break;
        printf("Error: Valor inválido. Debe estar entre %d y %d.\n", min, max);
    }
    return valor;
}

// Función para crear un proyecto de ley
struct proyectoLey *crearProyectoLey() {
    // Asignar memoria para el proyecto de ley
    struct proyectoLey *nuevoProyecto = (struct proyectoLey *) malloc(sizeof(struct proyectoLey));
    if (!nuevoProyecto) {
        printf("Error: No se pudo asignar memoria para el proyecto de ley.\n");
        return NULL;
    }

    char nombre[100], tipo[50];
    int idProyecto, urgencia, fase;

    // Capturar datos de texto
    printf("Ingrese el nombre del proyecto de ley: ");
    scanf("%99s", nombre);
    printf("Ingrese el tipo del proyecto de ley: ");
    scanf("%49s", tipo);

    // Validación y captura de datos numéricos
    idProyecto = leerEnteroConLimite("Ingrese el ID del proyecto", 10000000, 99999999);
    urgencia = leerEnteroConLimite("Ingrese la urgencia del proyecto", 1, 5);
    fase = leerEnteroConLimite("Ingrese la fase del proyecto", 1, 4);

    // Asignar memoria para campos de texto y verificar
    nuevoProyecto->nombre = (char *) malloc(strlen(nombre) + 1);
    nuevoProyecto->tipo = (char *) malloc(strlen(tipo) + 1);
    if (!nuevoProyecto->nombre || !nuevoProyecto->tipo) {
        printf("Error: No se pudo asignar memoria para los datos del proyecto de ley.\n");
        free(nuevoProyecto->nombre);
        free(nuevoProyecto->tipo);
        free(nuevoProyecto);
        return NULL;
    }

    // Copiar datos y completar estructura
    strcpy(nuevoProyecto->nombre, nombre);
    strcpy(nuevoProyecto->tipo, tipo);
    nuevoProyecto->idProyecto = idProyecto;
    nuevoProyecto->urgencia = urgencia;
    nuevoProyecto->fase = fase;

    // Inicializar punteros a NULL
    nuevoProyecto->articulo = NULL;
    nuevoProyecto->votacion = NULL;
    nuevoProyecto->comision = NULL;

    return nuevoProyecto;
}


// Función para añadir un nodo al árbol binario de búsqueda
void agregarNodoProyectoLey(struct congreso *congreso) {
    struct proyectoLey *datos = crearProyectoLey(); // Crear proyecto de ley
    if (datos == NULL) {
        printf("Error: No se pudo crear el proyecto de ley.\n");
        return;
    }

    struct nodoProyectoLey *nuevoNodo = crearNodoProyectoLey(datos); // crear nodo para el proyecto de ley
    if (nuevoNodo == NULL) {
        printf("Error: No se pudo crear el nodo del proyecto de ley.\n");
        free(datos); // Liberar datos si no se pudo crear el nodo
        return;
    }

    if (congreso->raiz == NULL) {
        // El árbol binario de búsqueda no existe, entonces el nuevo nodo será la raíz
        congreso->raiz = nuevoNodo;
    } else {
        struct nodoProyectoLey *actual = congreso->raiz;
        struct nodoProyectoLey *padre = NULL;

        // Procedimiento estándar para añadir nodos a un árbol binario de búsqueda
        while (actual != NULL) {
            padre = actual;
            if (datos->idProyecto < actual->datos->idProyecto) {
                actual = actual->izq;
            } else {
                actual = actual->der;
            }
        }

        // Añadir el nuevo nodo como hijo del nodo padre adecuado
        if (datos->idProyecto < padre->datos->idProyecto) {
            padre->izq = nuevoNodo;
        } else {
            padre->der = nuevoNodo;
        }
    }
}

// Función para buscar un proyecto de ley por ID
struct proyectoLey *buscarProyectoLeyPorID(struct nodoProyectoLey *raiz, int id) {
    if (raiz == NULL) {
        return NULL;
    }

    if (raiz->datos->idProyecto == id) {
        return raiz->datos;
    } else if (id < raiz->datos->idProyecto) {
        return buscarProyectoLeyPorID(raiz->izq, id); // Buscamos en el subarbol izquierdo si es el ID es menor
    } else {
        return buscarProyectoLeyPorID(raiz->der, id); // Buscamos en el subarbol derecho si es el ID es mayor
    }
}

// Encuentra el nodo mínimo en el árbol (usado para encontrar el sucesor en caso de eliminación)
struct nodoProyectoLey *minValorNodo(struct nodoProyectoLey *nodo) {
    struct nodoProyectoLey *actual = nodo;
    while (actual && actual->izq != NULL) {
        actual = actual->izq;
    }
    return actual;
}

// Función para borrar un nodo en el árbol binario de búsqueda
struct nodoProyectoLey *borrarNodoProyectoLey(struct congreso *congreso, struct nodoProyectoLey *raiz, int id) {
    // Base case
    if (raiz == NULL) return raiz;

    // Si el id a eliminar es más pequeño que el id de la raíz, entonces está en el subárbol izquierdo
    if (id < raiz->datos->idProyecto) {
        raiz->izq = borrarNodoProyectoLey(congreso, raiz->izq, id);
    }
    // Si el id a eliminar es mayor que el id de la raíz, entonces está en el subárbol derecho
    else if (id > raiz->datos->idProyecto) {
        raiz->der = borrarNodoProyectoLey(congreso, raiz->der, id);
    }
    // Si el id es el mismo que el id de la raíz, entonces este es el nodo a eliminar
    else {
        // Nodo con solo un hijo o sin hijos
        if (raiz->izq == NULL) {
            struct nodoProyectoLey *temp = raiz->der;
            free(raiz->datos->nombre);
            free(raiz->datos->tipo);
            free(raiz->datos);
            free(raiz);
            return temp;
        } else if (raiz->der == NULL) {
            struct nodoProyectoLey *temp = raiz->izq;
            free(raiz->datos->nombre);
            free(raiz->datos->tipo);
            free(raiz->datos);
            free(raiz);
            return temp;
        }

        // Nodo con dos hijos: obtener el sucesor en el orden (el más pequeño en el subárbol derecho)
        struct nodoProyectoLey *temp = minValorNodo(raiz->der);

        // Copiar el contenido del sucesor al nodo actual
        raiz->datos->idProyecto = temp->datos->idProyecto;
        strcpy(raiz->datos->nombre, temp->datos->nombre);
        strcpy(raiz->datos->tipo, temp->datos->tipo);
        raiz->datos->urgencia = temp->datos->urgencia;
        raiz->datos->fase = temp->datos->fase;

        // Borrar el sucesor
        raiz->der = borrarNodoProyectoLey(congreso, raiz->der, temp->datos->idProyecto);
    }

    return raiz;
}

// Función para borrar un proyecto de ley
void borrarProyectoLey(struct congreso *congreso, int id) {
    congreso->raiz = borrarNodoProyectoLey(congreso, congreso->raiz, id);
}


// Función para buscar y mostrar un proyecto de ley por ID en el árbol binario de búsqueda
void buscarYMostrarProyectoLey(struct congreso *congreso, int id) {
    struct proyectoLey *proyecto = buscarProyectoLeyPorID(congreso->raiz, id);
    if (proyecto != NULL) {
        printf("Nombre: %s\n", proyecto->nombre);
        printf("Tipo: %s\n", proyecto->tipo);
        printf("ID Proyecto: %d\n", proyecto->idProyecto);
        printf("Urgencia: %d\n", proyecto->urgencia);
        printf("Fase: %d\n", proyecto->fase);
        // Imprime otros parámetros si es necesario
    } else {
        printf("Proyecto de ley con ID %d no encontrado.\n", id);
    }
}

void modificarProyectoLey(struct congreso *congreso, int idProyecto) {
    struct proyectoLey *proyecto = buscarProyectoLeyPorID(congreso->raiz, idProyecto);
    if (!proyecto) {
        printf("Error: Proyecto de ley no encontrado.\n");
        return;
    }

    int opcion;
    do {
        printf("Seleccione el campo a modificar:\n");
        printf("1. Nombre\n");
        printf("2. Tipo\n");
        printf("3. ID Proyecto\n");
        printf("4. Urgencia\n");
        printf("5. Fase\n");
        printf("6. Salir\n");
        printf("Opción: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1: {
                char nuevoNombre[100];
                printf("Ingrese nuevo nombre: ");
                scanf("%99s", nuevoNombre);
                free(proyecto->nombre);
                proyecto->nombre = (char *) malloc(strlen(nuevoNombre) + 1);
                if (proyecto->nombre) {
                    strcpy(proyecto->nombre, nuevoNombre);
                } else {
                    printf("Error: No se pudo asignar memoria para el nuevo nombre.\n");
                }
                break;
            }
            case 2: {
                char nuevoTipo[50];
                printf("Ingrese nuevo tipo: ");
                scanf("%49s", nuevoTipo);
                free(proyecto->tipo);
                proyecto->tipo = (char *) malloc(strlen(nuevoTipo) + 1);
                if (proyecto->tipo) {
                    strcpy(proyecto->tipo, nuevoTipo);
                } else {
                    printf("Error: No se pudo asignar memoria para el nuevo tipo.\n");
                }
                break;
            }
            case 3: {
                int nuevoID;
                printf("Ingrese nuevo ID de proyecto: ");
                scanf("%d", &nuevoID);
                proyecto->idProyecto = nuevoID;
                break;
            }
            case 4: {
                int nuevaUrgencia;
                printf("Ingrese nueva urgencia (1-5): ");
                scanf("%d", &nuevaUrgencia);
                proyecto->urgencia = nuevaUrgencia;
                break;
            }
            case 5: {
                int nuevaFase;
                printf("Ingrese nueva fase: ");
                scanf("%d", &nuevaFase);
                proyecto->fase = nuevaFase;
                break;
            }
            case 6:
                printf("Saliendo de la modificación del proyecto de ley.\n");
                break;
            default:
                printf("Opción inválida. Intente nuevamente.\n");
        }
    } while (opcion != 6);
}


struct nodoCongresista *crearNodoCongresista(struct nodoCongresista *head, struct congresista *datos) {

    struct nodoCongresista *nodo = NULL;

    //pregunto primero que los datos recibidos no sean null
    if (datos != NULL) {
        nodo = (struct nodoCongresista *) malloc(sizeof(struct nodoCongresista));

        if (nodo == NULL) {
            //si esto ocurre, hay un error al asignar la memoria
            return NULL;
        }

        nodo->datos = datos; //aqui copio los datos que recibí
        //y le asigno un siguiente para luego insertarlo
        nodo->sig = NULL;
    }
    return nodo;
}

/*

comprobar que exista congresista, lo haré de manera que retorne 0 si NO existe el RUT, o que retorne 1 si existe
la idea es que el RUT sea el buscado, por lo tanto las otras funciones que la llamen deben ingresar el RUT
aunque esto puede estar sujeto a cambios si se desea, quizas recibir el nodo entero para comodidad
recordar que es circular con fantasma, por lo tanto tengo que iniciar el head->sig para el rec y usar do while

*/

int comprobarCongresistaEnComision(struct nodoCongresista *head, char *rutBuscado) {
    struct nodoCongresista *rec;

    if (head->sig!= NULL) {
        rec = head->sig; //sig porque es fantasma el primero
        do {
            //en este if pregunto que sea distinto de null solo por el nodo fantasma
            if (rec->datos != NULL && strcmp(rec->datos->rut,rutBuscado) == 0) {
                return 1; //se encontró en la lista
            }
            rec = rec->sig;
        }while(rec!=head);
    }
    return 0; //no se encontró en la lista
}

//funcion para recorrer los arreglos, el de diputados o el de senadores correspondientemente
int comprobarCongresistaEnCongreso(struct congresista **arreglo, char *rutBuscado) {

    for (int i = 0;i<MAX_CONGRESISTAS && arreglo[i] != NULL; i++) {
        if (strcmp(arreglo[i]->rut,rutBuscado) == 0) {
            return 1;   //el rut se ha encontrado, por lo tanto no se sigue el proceso
        }
    }
    return 0;           //el rut no se encuentra, se sigue el proceso
}

/*
El crearCongresista va a ser un poco distinto, esta funcion tiene que recibir(scanf) el rut y la ocupacion
al saber la ocupacion eligirá que arreglo recorrer si el de senadores o el de diputados
ahí en ese momento buscará comparando el rut en cada pos del arreglo, si lo encuentra retornará 1
si no es así retornará 0 y dará paso a la copia de datos para ingresarlos en el arreglo

aunque no estoy seguro si la ocupacion fuese el diputado/senador o si es la especializacion

*/

struct congresista *crearCongresista(struct congreso *congreso) {
    struct congresista *nuevoCongresista;                           //esto para guardar los datos del congresista
    char nombre[100],rut[20],ocupacion[20],especializacion[100];    //los datos que se reciben

    struct congresista **arreglo = NULL;                            //este dato decide que arreglo se recorre

    nuevoCongresista = (struct congresista *) malloc(sizeof(struct congresista));

    if(nuevoCongresista == NULL) {
        return NULL;                // error al asignar memoria
    }

    //aqui tomará la decision de que arreglo recorrer
    printf("Ingresa el RUT del congresista y la ocupacion:");
    scanf("%s",rut);
    scanf("%s",ocupacion);

    if (strcmp(ocupacion,"senador") == 0){
        arreglo = congreso->senadores;     //entonces el arreglo recorrido será el de senadores
    }
    else if (strcmp(ocupacion,"diputado") == 0){
        arreglo = congreso->diputados;      //entonces el arreglo recorrido será el de diputados
    }
    else {
        return NULL;         //significa que el que inserta los datos decidió poner algo que no sea ni senador ni diputado
    }

    if (comprobarCongresistaEnCongreso(arreglo,rut) == 1) {
        return NULL; // el congresista ya existe, sin importar si es diputado o senador, anulando la creacion del congresista
    }

    //se escanean los ultimos datos
    scanf("%s",especializacion);
    scanf("%s",nombre);


    //asignacion de memoria
    nuevoCongresista->nombre = (char *)malloc(sizeof(char) * strlen(nombre) + 1);
    nuevoCongresista->rut = (char *)malloc(sizeof(char) * strlen(rut) + 1);
    nuevoCongresista->ocupacion = (char *)malloc(sizeof(char) * strlen(ocupacion) + 1);
    nuevoCongresista->especializacion = (char *)malloc(sizeof(char) * strlen(especializacion) + 1);

    if (nuevoCongresista->nombre == NULL || nuevoCongresista->rut == NULL || nuevoCongresista->ocupacion == NULL || nuevoCongresista->especializacion == NULL) {
        return NULL; //alguno de los valores tuvo un error de asignacion de memoria
    }

    //parte de copiar datos
    strcpy(nuevoCongresista->nombre,nombre);
    strcpy(nuevoCongresista->rut,rut);
    strcpy(nuevoCongresista->ocupacion,ocupacion);
    strcpy(nuevoCongresista->especializacion,especializacion);

    return nuevoCongresista;
}

/*
primero hago la funcion para insertar el nuevo congresista en el arreglo correspondiente

agregue un MAX_CONGRESISTAS = 200 por que son arreglos separados
al ver la historia de chile te das cuenta que nunca han habido mas de 150 diputados
y tampoco han habido mas de 50 senadores en periodos de mas de 200 años
por lo tanto dudo que en otros 100 años se superen los 200
era esta solución o configurar el struct congreso para agregar un plibre de cada arreglo

*/

//este agrega un NUEVO congresista al arreglo correspondiente
int agregarCongresistaEnArreglo(struct congreso *congreso) {
    int i=0;
    struct congresista *nuevoCongresista = crearCongresista(congreso); //se crea el congresista para insertarlo
    struct congresista **arreglo = NULL; //decidirá a que arreglo pertenece

    //pregunto que el congresista sea valido
    if (nuevoCongresista != NULL) {
        if (strcmp(nuevoCongresista->ocupacion,"senador") == 0) {
            arreglo = congreso->senadores; //el congresista es un senador
        }
        else if (strcmp(nuevoCongresista->ocupacion,"diputado") == 0) {
            arreglo = congreso->diputados; //el congresista es diputado
        }
        else {
            //NOTA!!!! Recordemos que los participantes externos PUEDEN ocurrir,
            //por lo que seguramente deberíamos seguir contruyendo esto por aquí cuando toque implementar esa función
            return 0; // de alguna manera el congresista no es ni senador ni diputado
        }

        //se recorre el arreglo elegido validando que se haya elegido uno
        if (arreglo != NULL) {
            while (i < MAX_CONGRESISTAS && arreglo[i]!= NULL) {
                i++; //se añade a la ultima posicion
            }
            if (i<MAX_CONGRESISTAS) {
                arreglo[i] = nuevoCongresista; //añade el nuevo congresista
                return 1; //se logra asignar sin problema
            }
        }
    }
    return 0; //el nuevo congresista era null
}

/*
esta funcion agrega un ya EXISTENTE congresista a la lista de comisiones
voy a darle la comision, pues para acceder a la lista simplemente hago comision->headintegrantes
esto puede cambiar por int para los print
*/
/*
void agregarCongresistaEnComision(struct comision *comision,struct congresista *congresista) {
    struct nodoCongresista *nuevoNodo = NULL, *fantasma = NULL; // AÑADÍ LA INICIALIZACIÓN DE LOS PUNTEROS EN NULL PARA ESTAS 3 VARIABLES
    struct nodoCongresista *rec = NULL;
    //pregunto que ni uno de los datos sea null
    if(comision != NULL && congresista != NULL) {
        //pregunto que no exista en la comision este integrante
        if (comprobarCongresistaEnComision(comision->headIntegrantes,congresista->rut) == 0) {
            nuevoNodo = crearNodoCongresista(comision->headIntegrantes, congresista);

            //ESTA PARTE NO TENDRÍA QUE IR AL REVÉS? ABAJO SE REVISA SI EL NODO FANTASMA EXISTE, LO QUE QUERRÍA DECIR QUE
            //LA COMISIÓN NO HA SIDO CREADA Y/O ESTÁ VACÍA, SIN EMBARGO, ARRIBA ESTAMOS INTENTANDO RECORRER ESA COMISION
            //CUANDO REVISAMOS SI ES QUE YA EXISTE EL CONGRESISTA EN DICHA COMISION

            //esta parte crea el nodo fantasma si es que aun no se ha hecho
            if (comision->headIntegrantes == NULL) {
                fantasma = (struct nodoCongresista *)malloc(sizeof(struct nodoCongresista ));
                fantasma->sig = fantasma; //se apunta a si mismo
                comision->headIntegrantes = fantasma; //el head apunta al nodo fantasma

                //SI NO ESTOY LEYENDO ESTO MAL... EN EL CASO DE QUE NO ESTÁ HECHA COMISIÓN, Y/O ESTÁ VACÍA PORQUE EL NODO FANTASMA NO ESTÁ CREADO,
                //CUANDO INTENTEMOS AGREGAR UN CONGRESISTA A LA COMISIÓN, SÓLO SE VA A CREAR EL NODO FANTASMA, PERO NO VA A AGREGAR EL INTEGRANTE
            }
            //aqui comienza el caso en el que ya exista el nodo fantasma, agregar a la ultima posicion
            else {
                rec = comision->headIntegrantes;

                while (rec->sig != comision->headIntegrantes){
                    rec = rec->sig;
                }
                rec->sig = nuevoNodo;
                nuevoNodo->sig = comision->headIntegrantes;
            }
        }
    }
}

*/

//LA NUEVA PROPUESTA PARA LA FUNCIÓN
void agregarCongresistaEnComision(struct comision *comision, struct congresista *congresista) {
    struct nodoCongresista *nuevoNodo = NULL, *fantasma = NULL;
    struct nodoCongresista *rec = NULL;

    //PREGUNTO QUE NI UNO DE LOS DOS SEA NULL
    if (comision != NULL && congresista != NULL) {
        //VERIFICO SI EL NODO FANTASMA YA EXISTE, Y SI NO, CREO UNO
        if (comision->headIntegrantes == NULL) {
            fantasma = (struct nodoCongresista *)malloc(sizeof(struct nodoCongresista));
            if (fantasma == NULL) {
                return;
            }
            fantasma->sig = fantasma;
            comision->headIntegrantes = fantasma;
        }

        //VERIFICO QUE EL CONGRESISTA NO ESTÉ EN LA COMISIÓN
        if (comprobarCongresistaEnComision(comision->headIntegrantes, congresista->rut) == 0) {
            //CREO Y ENLAZO EL NUEVO NODO
            nuevoNodo = crearNodoCongresista(comision->headIntegrantes, congresista);
            if (nuevoNodo == NULL) {
                return;
            }

            rec = comision->headIntegrantes;
            while (rec->sig != comision->headIntegrantes) {
                rec = rec->sig;
            }
            rec->sig = nuevoNodo;
            nuevoNodo->sig = comision->headIntegrantes;
        }
    }
}





/*
Funciones para eliminar:
una solo eliminará a un congresista de la comision
la otra eliminará a un congresista del arreglo, por lo tanto se debe eliminar de todas las otras comisiones

igualmente que en funciones anteriores, se puede cambiar el char que recibe por el congresista entero
lo haré int para retornar 0 si hubo un fallo en la eliminacion o 1 si se eliminó bien
*/

int eliminarCongresistaDeComision(struct comision *comision, char *rutQuitado) {
    struct nodoCongresista *rec = NULL;  // AGREGADOA AHORA
    struct nodoCongresista *anterior = NULL; //trabajo con un anterior al ser lista circular

    //puede que la lista esté vacia(no se ha hecho nada) o que esté solo el nodo fantasma
    if (comision->headIntegrantes == NULL || comision->headIntegrantes->sig == comision->headIntegrantes) {
        return 0; //no existe ni un congresista en la comision, por lo tanto se termina el proceso
    }
    anterior = comision->headIntegrantes; //nodo fantasma, por lo tanto el primero
    rec = comision->headIntegrantes->sig; //nodo despues del fantasma, por lo tanto el segundo

    do {
        if (strcmp(rec->datos->rut,rutQuitado) == 0) {
            anterior->sig = rec->sig;
            return 1; //se encuentra y se desvincula de la lista
        }
        anterior = rec;
        rec = rec->sig;

    }while(rec != comision->headIntegrantes);

    return 0; //no se encontró en la lista
}

//int eliminarCongresistaDeCongreso()

/*
Funcion para modificar a un congresista
primero tengo que elegir el arreglo en el que está o simplemente recorrer ambos arreglos
en este caso no incluí la modificacion de la ocupación
ya que al cambiar la ocupacion es mejor eliminarlo del arreglo y agregarlo al otro simplemente
*/

int modificarCongresista(struct congreso *congreso,char *ocupacion, char *rutBuscado) {
    struct congresista **arreglo = NULL;
    char nombre[100],especializacion[100],rut[13];

    //escaneo los datos nuevos, solo se copiaran si pasan la siguiente parte, en esta parte irían prints
    printf("Ingresa el nombre del congresista:\n");
    scanf("%s",nombre); //ingrese nuevo nombre
    printf("ingresa la especialización del congresista:\n");
    scanf("%s",especializacion); //ingrese nueva especializacion
    printf("ingresa el rut del congresista:\n");
    scanf("%s",rut); //ingrese nuevo rut

    if(strcmp(ocupacion,"senador") == 0) {
        arreglo = congreso->senadores;
    }
    else if(strcmp(ocupacion,"diputado") == 0) {
        arreglo = congreso->diputados;
    }
    else {
        //NUEVAMENTE, AQUÍ PROBABLEMENTE TENGAMOS QUE VER LO DE LAS COMISIONES MIXTAS
        return 0; //la ocupacion puesta no es ni senador ni diputado
    }

    //ahora debo recorrer el arreglo
    for (int i = 0 ; arreglo[i]!= NULL && i<MAX_CONGRESISTAS ; i++) {
        if (strcmp(arreglo[i]->rut,rutBuscado) == 0) {
            //se encontró el rut, comienza la modificacion

            //asignacion de memoria
            arreglo[i]->rut = (char *)malloc(sizeof(char)*strlen(rut) + 1);
            arreglo[i]->nombre = (char *) malloc(sizeof(char)*strlen(nombre) + 1);
            arreglo[i]->especializacion = (char *) malloc(sizeof(char)*strlen(especializacion) + 1);

            strcpy(arreglo[i]->rut,rut);
            strcpy(arreglo[i]->nombre,nombre);
            strcpy(arreglo[i]->especializacion,especializacion);

            return 1; //se asigna correctamente los nuevos valores
        }
    }
    return 0; //hubo problemas en la creacion de datos
}
/* Funciones para los articulos */

//crear nodo para la lista doble

struct nodoArticulo *crearNodoArticulo(struct nodoArticulo *head, struct articulo *datos) {

    struct nodoArticulo *nodo = NULL;

    //pregunto primero que los datos recibidos no sean null
    if (datos != NULL) {


        nodo = (struct nodoArticulo *) malloc(sizeof(struct nodoArticulo));

        if (nodo == NULL) {
            //si esto ocurre, hay un error al asignar la memoria
            return NULL;
        }

        nodo->datos = datos; //aqui copio los datos que recibí

        //y le asigno un siguiente para luego insertarlo
        nodo->sig = NULL;
    }
    return nodo;
}

/*

comprobar que exista articulo, lo haré de manera que retorne 0 si NO existe el articulo, o que retorne 1 si existe
la idea es que la seccion sea el buscado, por lo tanto las otras funciones que la llamen deben ingresar la seccion
aunque esto puede estar sujeto a cambios si se desea, quizas recibir el nodo entero para comodidad

*/
int comprobarArticulo(struct nodoArticulo *head,int buscado) {
    struct nodoArticulo *rec;

    //existe la lista
    if (head != NULL) {
        rec = head;

        while (rec->sig != NULL) {
            if(rec->datos->seccion == buscado) {
                return 1; //se encuentra en la lista, por lo tanto no se inserta
            }
            rec = rec->sig;
        }
    }
    return 0; //no se encuentra en la lista, por lo que se pasa a la insercion
}



/*
Agregar el nuevo articulo, llamará a 2 funciones, la de crear el nodo y la de crear el nuevo articulo
la idea es que primero se creen los datos del articulo y luego se cree el nodo, la funcion crear nodo articulo
recibirá los datos de la función crear articulo

PD: todos los prints serán eliminados posterior a la creacion del main
*/

struct articulo *crearArticulo(struct nodoArticulo *lista) {
    struct articulo *NuevoArticulo;
    char *nombre,*texto,*cambio;
    int seccion;
    NuevoArticulo = (struct articulo *)malloc(sizeof(struct articulo));
    if (NuevoArticulo == NULL) {
        return NULL; //significa que hubo un error en la asignacion de memoria
    }

    printf("Ingresa el numero de seccion del articulo: ");
    scanf("%d", &seccion); //aqui recibo primero el numero de seccion para utilizar el comprobar que no se repita

    if (comprobarArticulo(lista,seccion) == 1) {
        return NULL; // en este caso el articulo ya existe
    }

    //ya comprobada la existencia del articulo, se reciben los datos y se copia el numero de seccion

    NuevoArticulo->seccion = seccion;

    scanf("%s",&nombre);
    scanf("%s",&texto);
    scanf("%s",&cambio);

    NuevoArticulo->nombre = (char *)malloc(sizeof(char) * strlen(nombre) + 1);
    NuevoArticulo->texto = (char *)malloc(sizeof(char) * strlen(texto) + 1);
    NuevoArticulo->cambio = (char *)malloc(sizeof(char) * strlen(cambio) + 1);

    if (NuevoArticulo->nombre == NULL || NuevoArticulo->texto == NULL || NuevoArticulo->cambio == NULL) {
        return NULL; //en alguno de los procesos hubo un error de asignacion de memoria
    }

    strcpy(NuevoArticulo->nombre,nombre);
    strcpy(NuevoArticulo->texto,texto);
    strcpy(NuevoArticulo->cambio,cambio);


    return NuevoArticulo;

}

int agregarArticulo(struct congreso *congreso,struct nodoArticulo **lista) {
    struct nodoArticulo *NuevoArticulo;
    struct nodoArticulo *rec;
    struct articulo *datos;

    datos = crearArticulo(*lista);
    NuevoArticulo = crearNodoArticulo(*lista,datos);

    //pregunto que no hayan errores al crear el nodo
    if (NuevoArticulo != NULL) {
        //a partir de aqui pregunto los tipos de caso, cuando no hay nodos, cuando hay un nodo, etc

        if (*lista == NULL) {
            *lista = NuevoArticulo; //la lista estaba vacia, se agrega sin problemas
        }
        //la lista no está vacia
        else {
            rec = *lista;
            //llego a la ultima posicion de la lista
            while (rec->sig != NULL) {
                rec = rec->sig;
            }
            rec->sig = NuevoArticulo; //llego al final y lo agrego
            return 1; //agregado correctamente
        }

    }
    return 0; //no se cumple los requisitos, no se agrega
}

/*
El eliminar articulo tambien será int para que en la consola sea mas facil poner algo como: articulo eliminado, o error
si devuelve 1, ha sido un exito la eliminacion, si retorna 0, no se encontró en la lista
la funcion recibe la lista de articulos, la idea es que se seleccione la ley y a partir de ahi se elimine el articulo
*/


int eliminarArticulo(struct nodoArticulo **lista,int seccionEliminada) {
    struct nodoArticulo *rec;

    //la lista existe
    if(*lista != NULL) {
        rec = *lista;

        //caso 1: el articulo está en la primera posicion
        if (seccionEliminada == (*lista)->datos->seccion) {
            *lista = (*lista)->sig;
            return 1; //se ha encontrado y eliminado el articulo
        }

        //caso 2: se encuentra en cualquier posicion de la lista
        while(rec->sig != NULL) {

            if(seccionEliminada == rec->sig->datos->seccion) {
                rec->sig = rec->sig->sig;
                return 1; //se ha encontrado y eliminado el articulo
            }

            rec =rec->sig;
        }
    }
    return 0; //no se encontró el articulo
}

//return 1: modificado de forma correcta return 0: no se pudo modificar

int modificarArticulo(struct nodoArticulo *articulos, int seccionModificada) {
    struct nodoArticulo *rec;
    struct articulo *articuloBuscado = NULL;
    char nombre[100],texto[256],cambio[100];

    //se escanean los datos nuevos
    scanf("%s",nombre);
    scanf("%s",texto);
    scanf("%s",cambio);
    //que la lista no sea null
    if (articulos != NULL) {
        rec = articulos;

        //recorro y busco el especifico
        while (rec != NULL) {
            //si se encuentra el que se modifica
            if (rec->datos->seccion == seccionModificada) {
                articuloBuscado = rec->datos; //se copia la info del articulo encontrado

                //ahora almaceno memoria
                articuloBuscado->nombre = (char *)malloc(sizeof(char) * strlen(nombre) + 1);
                articuloBuscado->texto = (char *)malloc(sizeof(char) * strlen(texto) + 1);
                articuloBuscado->cambio = (char *)malloc(sizeof(char) * strlen(cambio) + 1);

                //copio los datos
                strcpy(articuloBuscado->nombre,nombre);
                strcpy(articuloBuscado->texto,texto);
                strcpy(articuloBuscado->cambio,cambio);

                return 1; //modificado correctamente
            }
            rec = rec->sig;
        }

    }
    return 0; //no se logra modificar
}

void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

char leerOpcion() {
    char opcion;
    scanf("%c", &opcion);
    limpiarBuffer();  // Limpia el buffer para evitar múltiples entradas
    return (opcion >= 'a' && opcion <= 'z') ? opcion - ('a' - 'A') : opcion;  // Convierte a mayúscula si es necesario
}

void funcionSwitch(char opcion, struct congreso *congreso, void (*submenu)(struct congreso *)) {
    switch (opcion) {
        case 'A':
            printf("Seleccionaste la opcion A. Accediendo al menu...\n");
            submenu(congreso);
            break;
        case 'B':
            printf("Seleccionaste la opcion B. Accediendo al menu...\n");
            submenu(congreso);
            break;
        case 'C':
            printf("Seleccionaste la opcion C. Accediendo al menu...\n");
            submenu(congreso);
            break;
        case 'D':
            printf("Seleccionaste la opcion D. Terminando el programa.\n");
            break;
        default:
            printf("Opcion invalida, por favor intente otra vez.\n");
            break;
    }
}

void menuProyectosLey(struct congreso *congreso) {
    char opcion[2];
    int id;
    while (1) {
        printf("Menu Proyectos de Ley.\n"
            "Opcion A: Agregar nuevo Proyecto de Ley\n"
            "Opcion B: Borrar Proyecto de Ley\n"
            "Opcion C: Buscar Proyecto de Ley\n"
            "Opcion D: Modificar Proyecto de Ley\n"
            "Opcion E: Listar Proyectos de Ley\n"
            "Opcion F: Volver al menu principal\n");

        scanf("%1s", opcion);
        getchar(); // Clear any extra characters from the buffer
        opcion[0] = (opcion[0] >= 'a' && opcion[0] <= 'z') ? opcion[0] - ('a' - 'A') : opcion[0];

        switch (opcion[0]) {
            case 'A':
                printf("Funcion: Agregar nuevo Proyecto de Ley\n");
                agregarNodoProyectoLey(congreso);
                break;
            case 'B':
                printf("Funcion: Borrar Proyecto de Ley\n");
                id = leerEnteroConLimite("ingrese la ID del Proyecto que quiere borrar",10000000, 99999999);
                borrarProyectoLey(congreso, id);
                break;
            case 'C':
                printf("Funcion: Buscar Proyecto de Ley\n");
                id = leerEnteroConLimite("ingrese la ID del Proyecto que quiere buscar",10000000, 99999999);
                buscarYMostrarProyectoLey(congreso, id);
                break;
            case 'D':
                printf("Funcion: Modificar Proyecto de Ley\n");
                id = leerEnteroConLimite("ingrese la ID del Proyecto que quiere modificar",10000000, 99999999);
                modificarProyectoLey(congreso, id);
                break;
            case 'E':
                printf("Funcion: Listar Proyectos de Ley\n");
                break;
            case 'F':
                return;
            default:
                printf("Opcion invalida, por favor intente otra vez.\n");
                break;
        }
    }
}

void menuCongresistas(struct congreso *congreso) {
    char opcion[2];
    while (1) {
        printf("Menu Congresistas.\n"
            "Opcion A: Agregar Congresista\n"
            "Opcion B: Borrar Congresista\n"
            "Opcion C: Buscar Congresista\n"
            "Opcion D: Modificar Congresista\n"
            "Opcion E: Listar Congresistas\n"
            "Opcion F: Volver al menu principal\n");

        scanf("%1s", opcion);
        getchar(); // Clear any extra characters from the buffer
        opcion[0] = (opcion[0] >= 'a' && opcion[0] <= 'z') ? opcion[0] - ('a' - 'A') : opcion[0];

        switch (opcion[0]) {
            case 'A':
                printf("Funcion: Agregar Congresista\n");
                break;
            case 'B':
                printf("Funcion: Borrar Congresista\n");
                break;
            case 'C':
                printf("Funcion: Buscar Congresista\n");
                break;
            case 'D':
                printf("Funcion: Modificar Congresista\n");
                break;
            case 'E':
                printf("Funcion: Listar Congresistas\n");
                break;
            case 'F':
                return;
            default:
                printf("Opcion invalida, por favor intente otra vez.\n");
                break;
        }
    }
}

void menuComisiones(struct congreso *congreso) {
    char opcion[2];
    while (1) {
        printf("Menu Comisiones.\n"
            "Opcion A: Agregar Comision\n"
            "Opcion B: Borrar Comision\n"
            "Opcion C: Buscar Comision\n"
            "Opcion D: Modificar Comision\n"
            "Opcion E: Listar Comisiones\n"
            "Opcion F: Volver al menu principal\n");

        scanf("%1s", opcion);
        getchar(); // Clear any extra characters from the buffer
        opcion[0] = (opcion[0] >= 'a' && opcion[0] <= 'z') ? opcion[0] - ('a' - 'A') : opcion[0];

        switch (opcion[0]) {
            case 'A':
                printf("Funcion: Agregar Comision\n");
                break;
            case 'B':
                printf("Funcion: Borrar Comision\n");
                break;
            case 'C':
                printf("Funcion: Buscar Comision\n");
                break;
            case 'D':
                printf("Funcion: Modificar Comision\n");
                break;
            case 'E':
                printf("Funcion: Listar Comisiones\n");
                break;
            case 'F':
                return;
            default:
                printf("Opcion invalida, por favor intente otra vez.\n");
                break;
        }
    }
}

int main(void) {
    int flag = 1;
    char opcion;

    struct congreso *congreso = NULL;
    congreso = inicializarCongreso();

    while (flag == 1) {
        printf("Opcion A: Proyectos de Ley.\n"
            "Opcion B: Congresistas.\n"
            "Opcion C: Comisiones.\n"
            "Opcion D: Salir.\n\n");

        opcion = leerOpcion(); // Lee solo un carácter y limpia el buffer

        switch (opcion) {
            case 'A':
                funcionSwitch(opcion, congreso, menuProyectosLey);
                break;
            case 'B':
                funcionSwitch(opcion, congreso, menuCongresistas);
                break;
            case 'C':
                funcionSwitch(opcion, congreso, menuComisiones);
                break;
            case 'D':
                flag = 0;
                break;
            default:
                printf("Opcion invalida, por favor intente otra vez.\n");
                break;
        }
    }

    liberarCongreso(congreso);
    return 0;
}
