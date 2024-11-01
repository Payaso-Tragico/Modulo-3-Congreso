//codigo para proyecto de Modulo 3
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CONGRESISTAS 200

struct congreso {
    struct congresista **senadores;
    struct congresista **diputados;
    struct leyABB *raiz;
};

struct leyABB {

    struct ley *Datos;
    struct leyABB *izq, *der;
};

struct ley {
    char *nombre;
    char *tipo;
    int fase;
    struct comision **comisiones;
    struct NodoArticulo *headArticulos;
    int idProyecto;
};

struct NodoArticulo {
    struct articulo *datosArticulo;
    struct NodoArticulo *sig;

};

struct articulo {
    char *nombre;
    char *texto;
    char *cambio;
    int seccion;
};

struct comision {
    char *nombre;
    char *descripcion;
    int totalIntegrantes;
    struct NodoCongresista *headIntegrantes;
};

struct NodoCongresista {
    struct congresista *congresista;
    struct NodoCongresista *sig;
};

struct congresista {
    char *nombre;
    char *rut;
    char *ocupacion;
    char *especializacion;
};




//crear nodo para la lista circular

//como estoy creando un nodo, se asume que es para una comision, pues los congresistas se almacenan en arreglos

struct NodoCongresista *crearNodoCongresista(struct NodoCongresista *head, struct congresista *datos) {

    struct NodoCongresista *nodo = NULL;

    //pregunto primero que los datos recibidos no sean null
    if (datos != NULL) {


        nodo = (struct NodoCongresista *) malloc(sizeof(struct NodoCongresista));

        if (nodo == NULL) {
            //si esto ocurre, hay un error al asignar la memoria
            return NULL;
        }

        nodo->congresista = datos; //aqui copio los datos que recibí

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

int comprobarCongresistaEnComision(struct NodoCongresista *head, char *rutBuscado) {
    struct NodoCongresista *rec;

    if (head->sig!= NULL) {
        rec = head->sig; //sig porque es fantasma el primero
        do {
            //en este if pregunto que sea distinto de null solo por el nodo fantasma
            if (rec->congresista != NULL && strcmp(rec->congresista->rut,rutBuscado) == 0) {
                return 1; //se encontró en la lista
            }
            rec = rec->sig;
        }while(rec!=head);

    }
    return 0; //no se encontró en la lista
}


//funcion para recorrer los arreglos, el de diputados o el de senadores correspondientemente
int comprobarCongresistaEnCongreso(struct congresista **arreglo, char *rutBuscado) {
    int i;

    for (i=0;i<MAX_CONGRESISTAS && arreglo[i] != NULL; i++) {
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
void agregarCongresistaEnComision(struct comision *comision,struct congresista *congresista) {
    struct NodoCongresista *nuevoNodo, *fantasma;
    struct NodoCongresista *rec;
    //pregunto que ni uno de los datos sea null
    if(comision != NULL && congresista != NULL) {
        //pregunto que no exista en la comision este integrante
        if (comprobarCongresistaEnComision(comision->headIntegrantes,congresista->rut) == 0) {
            nuevoNodo = crearNodoCongresista(comision->headIntegrantes,congresista);

            //esta parte crea el nodo fantasma si es que aun no se ha hecho
            if (comision->headIntegrantes == NULL) {
                fantasma = (struct NodoCongresista *)malloc(sizeof(struct NodoCongresista ));
                fantasma->sig = fantasma; //se apunta a si mismo
                comision->headIntegrantes = fantasma; //el head apunta al nodo fantasma
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

/*
Funciones para eliminar:
una solo eliminará a un congresista de la comision
la otra eliminará a un congresista del arreglo, por lo tanto se debe eliminar de todas las otras comisiones

igualmente que en funciones anteriores, se puede cambiar el char que recibe por el congresista entero
lo haré int para retornar 0 si hubo un fallo en la eliminacion o 1 si se eliminó bien
*/

int eliminarCongresistaDeComision(struct comision *comision, char *rutQuitado) {
    struct NodoCongresista *rec;  //
    struct NodoCongresista *anterior = NULL; //trabajo con un anterior al ser lista circular

    //puede que la lista esté vacia(no se ha hecho nada) o que esté solo el nodo fantasma
    if (comision->headIntegrantes == NULL || comision->headIntegrantes->sig == comision->headIntegrantes) {
        return 0; //no existe ni un congresista en la comision, por lo tanto se termina el proceso
    }
    anterior = comision->headIntegrantes; //nodo fantasma, por lo tanto el primero
    rec = comision->headIntegrantes->sig; //nodo despues del fantasma, por lo tanto el segundo

    do {
        if (strcmp(rec->congresista->rut,rutQuitado) == 0) {
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
    int i;
    char nombre[100],especializacion[100],rut[13];

    //escaneo los datos nuevos, solo se copiaran si pasan la siguiente parte, en esta parte irían prints
    scanf("%s",nombre); //ingrese nuevo nombre
    scanf("%s",especializacion); //ingrese nueva especializacion
    scanf("%s",rut); //ingrese nuevo rut

    if(strcmp(ocupacion,"senador") == 0) {
        arreglo = congreso->senadores;
    }
    else if(strcmp(ocupacion,"diputado") == 0) {
        arreglo = congreso->diputados;
    }
    else {
        return 0; //la ocupacion puesta no es ni senador ni diputado
    }

    //ahora debo recorrer el arreglo
    for (i=0;arreglo[i]!= NULL && i<MAX_CONGRESISTAS;i++) {
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

struct NodoArticulo *crearNodoArticulo(struct NodoArticulo *head, struct articulo *datos) {

    struct NodoArticulo *nodo = NULL;

    //pregunto primero que los datos recibidos no sean null
    if (datos != NULL) {


        nodo = (struct NodoArticulo *) malloc(sizeof(struct NodoArticulo));

        if (nodo == NULL) {
            //si esto ocurre, hay un error al asignar la memoria
            return NULL;
        }

        nodo->datosArticulo = datos; //aqui copio los datos que recibí

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
int comprobarArticulo(struct NodoArticulo *head,int buscado) {
    struct NodoArticulo *rec;

    //existe la lista
    if (head != NULL) {
        rec = head;

        while (rec->sig != NULL) {
            if(rec->datosArticulo->seccion == buscado) {
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

struct articulo *crearArticulo(struct NodoArticulo *lista) {
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

int agregarArticulo(struct congreso *congreso,struct NodoArticulo **lista) {
    struct NodoArticulo *NuevoArticulo;
    struct NodoArticulo *rec;
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


int eliminarArticulo(struct NodoArticulo **lista,int seccionEliminada) {
    struct NodoArticulo *rec;

    //la lista existe
    if(*lista != NULL) {
        rec = *lista;

        //caso 1: el articulo está en la primera posicion
        if (seccionEliminada == (*lista)->datosArticulo->seccion) {
            *lista = (*lista)->sig;
            return 1; //se ha encontrado y eliminado el articulo
        }

        //caso 2: se encuentra en cualquier posicion de la lista
        while(rec->sig != NULL) {

            if(seccionEliminada == rec->sig->datosArticulo->seccion) {
                rec->sig = rec->sig->sig;
                return 1; //se ha encontrado y eliminado el articulo
            }

            rec =rec->sig;
        }
    }
    return 0; //no se encontró el articulo
}

//return 1: modificado de forma correcta return 0: no se pudo modificar

int modificarArticulo(struct NodoArticulo *articulos, int seccionModificada) {
    struct NodoArticulo *rec;
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
            if (rec->datosArticulo->seccion == seccionModificada) {
                articuloBuscado = rec->datosArticulo; //se copia la info del articulo encontrado

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



int main(void) {
    printf("Hello, World!\n");
    return 0;
}
