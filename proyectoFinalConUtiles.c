#include <ctype.h>   // toupper()
#include "utiles.h" 

#define MAX_PIZZAS 10
#define MAX_MESAS 10
#define MAX_PEDIDOS 10
#define MAX_NOMBRE 50
#define MAX_CODIGO 10
#define MAX_USUARIO 20
#define MAX_CLAVE 20
#define ARCHIVO_STOCK "stock.dat"   // archivo BINARIO: estado del stock
#define MAX_FECHA 11                // "AAAA-MM-DD" + '\0'

// ======================== ESTRUCTURAS ========================

typedef struct {
    char codigo[MAX_CODIGO];
    char nombre[MAX_NOMBRE];
    int stock;
    float precio;
} Pizza;

typedef struct {
    char estado[20];
    int numero;
} Mesa;

typedef struct {
    char estado[20];
    int numero;
    Pizza pizza;
} Pedido;

typedef struct {
    Pizza pizzas[MAX_PIZZAS];
    Mesa mesas[MAX_MESAS];
    Pedido pedidos[MAX_PEDIDOS];
    float ganancias_totales;
    int cantidad_pizzas;
} Pizzeria;

// Lo que se guarda en el archivo binario stock.dat (un unico registro)
typedef struct {
    Pizza pizzas[MAX_PIZZAS];
    float ganancias_totales;
    int cantidad_pizzas;
    char fecha[MAX_FECHA];      // dia en que se guardo por ultima vez
} RegistroStock;

// ======================== PROTOTIPOS DE FUNCIONES ========================

void inicializarPizzeria(Pizzeria *pizzeria);
void inicializarPizzas(Pizzeria *pizzeria);
int mostrarMenuPrincipal();
void menuGerente(Pizzeria *pizzeria);
void menuEmpleado(Pizzeria *pizzeria);
void menuCliente(Pizzeria *pizzeria);
int login(char perfil[], Pizzeria *pizzeria);
void pedirCredenciales(char usuario[], char clave[], char perfil[]);
void verStockActual(Pizzeria *pizzeria);
void agregarNuevoSabor(Pizzeria *pizzeria);
void verGanancias(Pizzeria *pizzeria);
void verMesas(Pizzeria *pizzeria);
void marcarMesa(Pizzeria *pizzeria);
void marcarPedidoListo(Pizzeria *pizzeria);
void verStockEmpleado(Pizzeria *pizzeria);
void comprarPizza(Pizzeria *pizzeria);
void juegoAdivinarNumero(float *descuento);
void juegoSumaRapida(float *descuento);
void convertirMayusculas(char str[]);

// Archivos
void obtenerFecha(char fecha[], int tam);
void obtenerHora(char hora[], int tam);
void obtenerNombreRegistro(char nombre[], int tam);
int guardarStock(Pizzeria *pizzeria);
int cargarStock(Pizzeria *pizzeria);
void registrarEvento(const char *texto);
void registrarStock(const char *titulo, Pizzeria *pizzeria);
void verRegistroDelDia(void);
///////////////////////////////////////////////////////////////////////////////////

void Grafico_Menu(void);
void Grafico_MostrarLogo(void);
void Grafico_MenuGerente(void);
void Grafico_Usuario(void);
void Graffico_Contrasea(void);
void Grafico_AccesoDenegado(void);
void Grafico_MenuCliente(void);
void Grafico_MenuEmpleado(void);
void Grafico_OpcionIncorrecta(void);
void Grafico_Volviendo(void);

// ======================== FUNCIONES PRINCIPALES ========================

int main() {
    Pizzeria pizzeria;
    int opcion;

    utf8();
    srand((unsigned)time(NULL));

    inicializarPizzeria(&pizzeria);
    if (!cargarStock(&pizzeria)) {
        guardarStock(&pizzeria);          // primera vez: crea stock.dat
    }
    registrarStock("APERTURA", &pizzeria);
    Grafico_Menu();
    pausa();

    do {
        opcion = mostrarMenuPrincipal();

        switch(opcion) {
            case 1:
                if (login("GERENTE", &pizzeria)) {
                    menuGerente(&pizzeria);
                }
                break;
            case 2:
                if (login("EMPLEADO", &pizzeria)) {
                    menuEmpleado(&pizzeria);
                }
                break;
            case 3:
                menuCliente(&pizzeria);
                break;
            case 4:
                registrarStock("CIERRE", &pizzeria);
                guardarStock(&pizzeria);
                printf("Gracias por visitar Pizzeria MASA MAIN\n");
                pausa();
                break;
            default:
                printf("Opcion invalida\n");
                pausa();
        }
    } while (opcion != 4);

    return 0;
}

// ======================== IMPLEMENTACI�N DE FUNCIONES ========================


void inicializarPizzeria(Pizzeria *pizzeria) {
    inicializarPizzas(pizzeria);

    for (int i = 0; i < MAX_MESAS; i++) {
        (*pizzeria).mesas[i].numero = i + 1;
        strcpy((*pizzeria).mesas[i].estado, "LIBRE");
    }

    for (int i = 0; i < MAX_PEDIDOS; i++) {
        (*pizzeria).pedidos[i].numero = i + 1;
        strcpy((*pizzeria).pedidos[i].estado, "LIBRE");
    }

    (*pizzeria).ganancias_totales = 0.0;
    (*pizzeria).cantidad_pizzas = 5;
}

void inicializarPizzas(Pizzeria *pizzeria) {
    // Pizza 1: Muzarella
    strcpy((*pizzeria).pizzas[0].codigo, "PZ1");
    strcpy((*pizzeria).pizzas[0].nombre, "Muzarella");
    (*pizzeria).pizzas[0].precio = 4500;
    (*pizzeria).pizzas[0].stock = 10;

    // Pizza 2: Napolitana
    strcpy((*pizzeria).pizzas[1].codigo, "PZ2");
    strcpy((*pizzeria).pizzas[1].nombre, "Napolitana");
    (*pizzeria).pizzas[1].precio = 5000;
    (*pizzeria).pizzas[1].stock = 10;

    // Pizza 3: Fugazzeta
    strcpy((*pizzeria).pizzas[2].codigo, "PZ3");
    strcpy((*pizzeria).pizzas[2].nombre, "Fugazzeta");
    (*pizzeria).pizzas[2].precio = 4800;
    (*pizzeria).pizzas[2].stock = 10;

    // Pizza 4: Jamon y Morron
    strcpy((*pizzeria).pizzas[3].codigo, "PZ4");
    strcpy((*pizzeria).pizzas[3].nombre, "Jamon y Morron");
    (*pizzeria).pizzas[3].precio = 5200;
    (*pizzeria).pizzas[3].stock = 10;

    // Pizza 5: Calabresa
    strcpy((*pizzeria).pizzas[4].codigo, "PZ5");
    strcpy((*pizzeria).pizzas[4].nombre, "Calabresa");
    (*pizzeria).pizzas[4].precio = 5300;
    (*pizzeria).pizzas[4].stock = 10;

    for (int i = 5; i < MAX_PIZZAS; i++) {
        strcpy((*pizzeria).pizzas[i].codigo, "");
        strcpy((*pizzeria).pizzas[i].nombre, "");
        (*pizzeria).pizzas[i].precio = 0;
        (*pizzeria).pizzas[i].stock = 0;
    }
}

int mostrarMenuPrincipal() {
    int opcion;

    limpiaPantalla();
    //printf("==============================\n");
   // printf("**** PIZZERIA MASA MAIN ****\n");
    //printf("==============================\n");
    //printf("1. Ingresar como GERENTE\n");
    //printf("2. Ingresar como EMPLEADO\n");
    //printf("3. Ingresar como CLIENTE\n");
    //printf("4. Salir\n");
    //printf("==============================\n");
    //printf("Ingrese una opcion (1-4): ");
    Grafico_MostrarLogo();
    opcion = leerEntero("");

    return opcion;
}

int login(char perfil[], Pizzeria *pizzeria) {
    char usuario[MAX_USUARIO];
    char clave[MAX_CLAVE];
    char usuario_correcto[] = "ADMIN";
    char clave_correcta[] = "1234";

    do {
        pedirCredenciales(usuario, clave, perfil);

        if (sonIguales(usuario, "1") || sonIguales(clave, "1")) {
            return 0;
        }

        if (sonIguales(usuario, usuario_correcto) && sonIguales(clave, clave_correcta)) {
            printf("Login exitoso como %s!\n", perfil);
            pausa();
            return 1;
        } else {
            //printf("Usuario o contrasena incorrectos\n");
            Grafico_AccesoDenegado();
            pausa();
        }
    } while (1);
}

void pedirCredenciales(char usuario[], char clave[], char perfil[]) {
    limpiaPantalla();
    Grafico_Usuario();
    //printf("==== LOGIN %s ====\n", perfil);
    //printf("(1 para salir)\n");
    //printf("Usuario: ");
    leerCadena(usuario, MAX_USUARIO, "");
    convertirMayusculas(usuario);

    if (sonIguales(usuario, "ADMIN")) {
         Graffico_Contrasea();
        //printf("Contrasena: ");
        leerCadena(clave, MAX_CLAVE, "");
        convertirMayusculas(clave);
    } else {
        strcpy(clave, "");
    }
}

void menuGerente(Pizzeria *pizzeria) {
    int opcion;

    do {
        limpiaPantalla();
        Grafico_MenuGerente();
        printf("    5. Ver registro de movimientos del dia\n");
        //printf("===== MENU DEL GERENTE =====\n");
        //printf("1. Ver stock actual\n");
        //printf("2. Agregar nuevo sabor\n");
        //printf("3. Ver ganancias totales\n");
        //printf("4. Volver\n");
        //printf("Ingrese una opcion (1-4): ");
        opcion = leerEntero("");

        switch(opcion) {
            case 1:
                verStockActual(pizzeria);
                break;
            case 2:
                agregarNuevoSabor(pizzeria);
                break;
            case 3:
                verGanancias(pizzeria);
                break;
            case 5:
                verRegistroDelDia();
                break;
            case 4:
                //printf("Volviendo...\n");
                Grafico_Volviendo();
                pausa();
                break;
            default:
                printf("Opcion invalida\n");
                pausa();
        }
    } while (opcion != 4);
}

void menuEmpleado(Pizzeria *pizzeria) {
    int opcion;

    do {
        limpiaPantalla();
        Grafico_MenuEmpleado();
        //printf("===== MENU DEL EMPLEADO =====\n");
        //printf("1. Ver mesas disponibles\n");
        //printf("2. Marcar mesa como ocupada/libre\n");
        //printf("3. Marcar pedido como listo\n");
        //printf("4. Ver stock\n");
        //printf("5. Volver\n");
        //printf("Ingrese una opcion (1-5): ");
        opcion = leerEntero("");

        switch(opcion) {
            case 1:
                verMesas(pizzeria);
                break;
            case 2:
                marcarMesa(pizzeria);
                break;
            case 3:
                marcarPedidoListo(pizzeria);
                break;
            case 4:
                verStockEmpleado(pizzeria);
                break;
            case 5:
                printf("Volviendo...\n");
                pausa();
                break;
            default:
                printf("Opcion invalida\n");
                pausa();
        }
    } while (opcion != 5);
}

void menuCliente(Pizzeria *pizzeria) {
    int opcion;

    do {
       limpiaPantalla();
       Grafico_MenuCliente();
        //printf("===== MENU DEL CLIENTE =====\n");
        //printf("1. Comprar pizza\n");
        //printf("2. Volver\n");
        //printf("Ingrese una opcion (1-2): ");
        opcion = leerEntero("");

        switch(opcion) {
            case 1:
                comprarPizza(pizzeria);
                break;
            case 2:
                printf("Volviendo...\n");
                break;
            default:
                Grafico_OpcionIncorrecta();
                //printf("Opcion invalida\n");
                pausa();
        }
    } while (opcion != 2);
}

void verStockActual(Pizzeria *pizzeria) {
    limpiaPantalla();
    printf("===== STOCK ACTUAL =====\n");

    int hayPizzas = 0;
    for (int i = 0; i < MAX_PIZZAS; i++) {
        if (strlen((*pizzeria).pizzas[i].nombre) > 0) {
            printf("%s - %s - $%.2f (Stock: %d)\n",
                   (*pizzeria).pizzas[i].codigo,
                   (*pizzeria).pizzas[i].nombre,
                   (*pizzeria).pizzas[i].precio,
                   (*pizzeria).pizzas[i].stock);
            hayPizzas = 1;
        }
    }

    if (hayPizzas == 0) {
        printf("No hay sabores cargados.\n");
    }

    pausa();
}

void agregarNuevoSabor(Pizzeria *pizzeria) {
    int espacios_disponibles = 0;

    for (int i = 5; i < MAX_PIZZAS; i++) {
        if (strlen((*pizzeria).pizzas[i].nombre) == 0) {
            espacios_disponibles++;
        }
    }

    if (espacios_disponibles == 0) {
        printf("No hay espacio para nuevos sabores.\n");
        pausa();
        return;
    }

    printf("Espacios disponibles: %d\n", espacios_disponibles);

    for (int i = 5; i < MAX_PIZZAS; i++) {
        if (strlen((*pizzeria).pizzas[i].nombre) == 0) {
            leerCadena((*pizzeria).pizzas[i].nombre, MAX_NOMBRE, "Nombre del nuevo sabor: ");

            leerFlotante("Precio: ", &(*pizzeria).pizzas[i].precio);

            (*pizzeria).pizzas[i].stock = pedirEnteroRango("Stock inicial: ", 0, 1000);

            char codigo_temp[10];
            sprintf(codigo_temp, "PZ%d", i + 1);
            strcpy((*pizzeria).pizzas[i].codigo, codigo_temp);

            (*pizzeria).cantidad_pizzas++;

            char evento[200];
            sprintf(evento, "NUEVO SABOR | %s | %s | precio: $%.2f | stock inicial: %d",
                    (*pizzeria).pizzas[i].codigo,
                    (*pizzeria).pizzas[i].nombre,
                    (*pizzeria).pizzas[i].precio,
                    (*pizzeria).pizzas[i].stock);
            registrarEvento(evento);
            guardarStock(pizzeria);
            printf("�Sabor agregado exitosamente!\n");
            pausa();
            return;
        }
    }
}

void verGanancias(Pizzeria *pizzeria) {
    limpiaPantalla();
    printf("===== GANANCIAS ACTUALES =====\n");
    printf("Total generado: $%.2f\n", (*pizzeria).ganancias_totales);
    pausa();
}

void verMesas(Pizzeria *pizzeria) {
    limpiaPantalla();
    printf("===== MESAS DISPONIBLES =====\n");

    for (int i = 0; i < MAX_MESAS; i++) {
        printf("Mesa %d: %s\n",
               (*pizzeria).mesas[i].numero,
               (*pizzeria).mesas[i].estado);
    }

    pausa();
}

void marcarMesa(Pizzeria *pizzeria) {
    int numero_mesa;

    char mensaje[50];
    sprintf(mensaje, "Numero de mesa (1-%d): ", MAX_MESAS);
    numero_mesa = pedirEnteroRango(mensaje, 1, MAX_MESAS);

    int opcion = pedirEnteroRango("1. Ocupada | 2. Libre: ", 1, 2);

    if (opcion == 1) {
        strcpy((*pizzeria).mesas[numero_mesa - 1].estado, "OCUPADA");
    } else {
        strcpy((*pizzeria).mesas[numero_mesa - 1].estado, "LIBRE");
    }

    printf("Mesa %d actualizada.\n", numero_mesa);
    pausa();
}

void marcarPedidoListo(Pizzeria *pizzeria) {
    int numero_pedido;

    char mensaje[50];
    sprintf(mensaje, "Numero de pedido (1-%d): ", MAX_PEDIDOS);
    numero_pedido = pedirEnteroRango(mensaje, 1, MAX_PEDIDOS);

    strcpy((*pizzeria).pedidos[numero_pedido - 1].estado, "LISTO");
    printf("Pedido %d marcado como listo.\n", numero_pedido);
    pausa();
}

void verStockEmpleado(Pizzeria *pizzeria) {
    limpiaPantalla();
    printf("===== STOCK ACTUAL =====\n");

    for (int i = 0; i < MAX_PIZZAS; i++) {
        if (strlen((*pizzeria).pizzas[i].nombre) > 0) {
            printf("%s - %s (Stock: %d)\n",
                   (*pizzeria).pizzas[i].codigo,
                   (*pizzeria).pizzas[i].nombre,
                   (*pizzeria).pizzas[i].stock);
        }
    }

    pausa();
}

void comprarPizza(Pizzeria *pizzeria) {
    char codigo_buscado[MAX_CODIGO];
    int encontrado = 0;
    char acepta_juego;
    float descuento = 0.0;

    limpiaPantalla();
    printf("=== COMPRA DE PIZZA ===\n");

    for (int i = 0; i < MAX_PIZZAS; i++) {
        if (strlen((*pizzeria).pizzas[i].nombre) > 0 && (*pizzeria).pizzas[i].stock > 0) {
            printf("%s - %s: $%.2f (Stock: %d)\n",
                   (*pizzeria).pizzas[i].codigo,
                   (*pizzeria).pizzas[i].nombre,
                   (*pizzeria).pizzas[i].precio,
                   (*pizzeria).pizzas[i].stock);
        }
    }

    leerCadena(codigo_buscado, MAX_CODIGO, "\nIngrese codigo de la pizza: ");
    convertirMayusculas(codigo_buscado);

    for (int i = 0; i < MAX_PIZZAS; i++) {
        if (sonIguales((*pizzeria).pizzas[i].codigo, codigo_buscado)) {
            encontrado = 1;

            if ((*pizzeria).pizzas[i].stock > 0) {
                int cantidad;
                char mensaje[100];
                sprintf(mensaje, "Cuantas unidades de '%s' desea?: ", (*pizzeria).pizzas[i].nombre);
                cantidad = leerEntero(mensaje);

                if (cantidad <= 0) {
                    printf("Cantidad invalida.\n");
                    pausa();
                    return;
                }

                if (cantidad > (*pizzeria).pizzas[i].stock) {
                    printf("No hay suficiente stock. Stock actual: %d\n", (*pizzeria).pizzas[i].stock);
                    pausa();
                    return;
                }

                char respuesta[10];
                leerCadena(respuesta, sizeof(respuesta), "Jugar para ganar descuento? (S/N): ");
                acepta_juego = toupper(respuesta[0]);

                if (acepta_juego == 'S') {
                    printf("Elija juego:\n");
                    printf("1) Adivinar numero (20%% descuento)\n");
                    printf("2) Suma rapida (10%% descuento)\n");

                    int opcion_juego;
                    opcion_juego = leerEntero("");

                    switch(opcion_juego) {
                        case 1:
                            juegoAdivinarNumero(&descuento);
                            break;
                        case 2:
                            juegoSumaRapida(&descuento);
                            break;
                        default:
                            printf("Opcion invalida. Sin descuento.\n");
                    }
                }

                float precio_bruto = (*pizzeria).pizzas[i].precio * cantidad;
                float precio_final = precio_bruto * (1.0 - descuento);

                (*pizzeria).pizzas[i].stock -= cantidad;

                retardo(0.5);
                printf("\nPedido realizado con exito!\n");
                printf("Total a pagar: $%.2f", precio_final);
                if (descuento > 0) {
                    printf(" (descuento: %.0f%%)", descuento * 100);
                }
                printf("\nSu orden estara lista en breve!\n");

                (*pizzeria).ganancias_totales += precio_final;

                char evento[200];
                sprintf(evento, "VENTA | %s | %s | cant: %d | desc: %.0f%% | total: $%.2f | stock restante: %d",
                        (*pizzeria).pizzas[i].codigo,
                        (*pizzeria).pizzas[i].nombre,
                        cantidad,
                        descuento * 100,
                        precio_final,
                        (*pizzeria).pizzas[i].stock);
                registrarEvento(evento);
                guardarStock(pizzeria);

            } else {
                printf("Sin stock disponible.\n");
            }
            break;
        }
    }

    if (encontrado == 0) {
        printf("Codigo no encontrado.\n");
    }

    pausa();
}

// ======================== JUEGOS MEJORADOS ========================

void juegoAdivinarNumero(float *descuento) {
    int secreto = generarNumAleatorio(1, 10);
    int intento;

    intento = pedirEnteroRango("\n Adivina el numero (1-10): ", 1, 10);

    if (intento == secreto) {
        *descuento = 0.20;
        printf("Correcto! Ganaste 20%% de descuento!\n");
    } else {
        printf("Incorrecto. El numero era %d. No hay descuento.\n", secreto);
    }
}

void juegoSumaRapida(float *descuento) {
    int a = generarNumAleatorio(1, 10);
    int b = generarNumAleatorio(1, 10);
    int resp;

    char mensaje[50];
    sprintf(mensaje, "\nCuanto es %d + %d?: ", a, b);
    resp = leerEntero(mensaje);

    if (resp == a + b) {
        *descuento = 0.10;
        printf("Bien! Ganaste 10%% de descuento!\n");
    } else {
        printf("Mal. La respuesta era %d. No hay descuento.\n", a + b);
    }
}

// ======================== ARCHIVOS (STOCK Y REGISTRO DIARIO) ========================

// Fecha de hoy con formato AAAA-MM-DD
void obtenerFecha(char fecha[], int tam) {
    time_t ahora = time(NULL);
    struct tm *info = localtime(&ahora);
    strftime(fecha, tam, "%Y-%m-%d", info);
}

// Hora actual con formato HH:MM:SS
void obtenerHora(char hora[], int tam) {
    time_t ahora = time(NULL);
    struct tm *info = localtime(&ahora);
    strftime(hora, tam, "%H:%M:%S", info);
}

// Nombre del archivo de texto del dia: ventas_AAAA-MM-DD.txt
void obtenerNombreRegistro(char nombre[], int tam) {
    char fecha[MAX_FECHA];
    obtenerFecha(fecha, sizeof(fecha));
    snprintf(nombre, tam, "ventas_%s.txt", fecha);
}

// ARCHIVO BINARIO: guarda el estado actual del stock en stock.dat
int guardarStock(Pizzeria *pizzeria) {
    RegistroStock datos;
    memset(&datos, 0, sizeof(RegistroStock));

    for (int i = 0; i < MAX_PIZZAS; i++) {
        datos.pizzas[i] = (*pizzeria).pizzas[i];
    }
    datos.cantidad_pizzas = (*pizzeria).cantidad_pizzas;
    datos.ganancias_totales = (*pizzeria).ganancias_totales;
    obtenerFecha(datos.fecha, sizeof(datos.fecha));

    FILE *archivo = fopen(ARCHIVO_STOCK, "wb");
    if (archivo == NULL) {
        printf("No se pudo abrir %s para guardar el stock.\n", ARCHIVO_STOCK);
        return 0;
    }

    size_t escritos = fwrite(&datos, sizeof(RegistroStock), 1, archivo);
    fclose(archivo);

    return escritos == 1;
}

// ARCHIVO BINARIO: recupera el stock guardado. Devuelve 1 si pudo, 0 si no.
int cargarStock(Pizzeria *pizzeria) {
    RegistroStock datos;

    FILE *archivo = fopen(ARCHIVO_STOCK, "rb");
    if (archivo == NULL) {
        return 0;                       // todavia no existe (primera ejecucion)
    }

    size_t leidos = fread(&datos, sizeof(RegistroStock), 1, archivo);
    fclose(archivo);

    if (leidos != 1 || datos.cantidad_pizzas < 0 || datos.cantidad_pizzas > MAX_PIZZAS) {
        return 0;                       // archivo incompleto o danado
    }

    for (int i = 0; i < MAX_PIZZAS; i++) {
        (*pizzeria).pizzas[i] = datos.pizzas[i];
    }
    (*pizzeria).cantidad_pizzas = datos.cantidad_pizzas;
    (*pizzeria).ganancias_totales = datos.ganancias_totales;

    // Si el archivo se guardo otro dia, las ganancias empiezan de cero
    char hoy[MAX_FECHA];
    obtenerFecha(hoy, sizeof(hoy));
    datos.fecha[MAX_FECHA - 1] = '\0';

    if (!sonIguales(datos.fecha, hoy)) {
        char evento[150];
        sprintf(evento, "NUEVO DIA | ganancias del dia anterior (%s): $%.2f",
                datos.fecha, datos.ganancias_totales);
        registrarEvento(evento);
        (*pizzeria).ganancias_totales = 0.0;
    }

    return 1;
}

// ARCHIVO DE TEXTO: agrega una linea con hora al registro del dia
void registrarEvento(const char *texto) {
    char nombre[40];
    char hora[9];

    obtenerNombreRegistro(nombre, sizeof(nombre));
    obtenerHora(hora, sizeof(hora));

    FILE *archivo = fopen(nombre, "a");
    if (archivo == NULL) {
        printf("No se pudo abrir %s.\n", nombre);
        return;
    }

    fprintf(archivo, "[%s] %s\n", hora, texto);
    fclose(archivo);
}

// ARCHIVO DE TEXTO: foto del stock (APERTURA al iniciar, CIERRE al salir)
void registrarStock(const char *titulo, Pizzeria *pizzeria) {
    char nombre[40];
    char hora[9];

    obtenerNombreRegistro(nombre, sizeof(nombre));
    obtenerHora(hora, sizeof(hora));

    FILE *archivo = fopen(nombre, "a");
    if (archivo == NULL) {
        printf("No se pudo abrir %s.\n", nombre);
        return;
    }

    fprintf(archivo, "[%s] ===== %s =====\n", hora, titulo);
    for (int i = 0; i < MAX_PIZZAS; i++) {
        if (strlen((*pizzeria).pizzas[i].nombre) > 0) {
            fprintf(archivo, "    %s - %s: stock %d\n",
                    (*pizzeria).pizzas[i].codigo,
                    (*pizzeria).pizzas[i].nombre,
                    (*pizzeria).pizzas[i].stock);
        }
    }
    fprintf(archivo, "    Ganancias del dia: $%.2f\n", (*pizzeria).ganancias_totales);
    fclose(archivo);
}

// Muestra en pantalla el archivo de texto del dia
void verRegistroDelDia(void) {
    char nombre[40];
    char texto[200];

    limpiaPantalla();
    printf("===== REGISTRO DEL DIA =====\n");

    obtenerNombreRegistro(nombre, sizeof(nombre));
    FILE *archivo = fopen(nombre, "r");

    if (archivo == NULL) {
        printf("Todavia no hay movimientos registrados hoy.\n");
    } else {
        while (fgets(texto, sizeof(texto), archivo) != NULL) {
            printf("%s", texto);
        }
        fclose(archivo);
    }

    pausa();
}

// ======================== FUNCIONES UTILITARIAS ========================

void convertirMayusculas(char str[]) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

///////////////////////////////////////////////////////////////////////////
void Grafico_Menu(void) {
    printf("                                                                                       QXZZ Z Z Z Z Z Z Z Z Z Z Z ZZ Z    Z Z Z ZZ   Z Z Z Z Z Z Z Z Z Z Z Z Z Z Z ZZZZZZZZZZZZXZZZZZZZZZZZZZZZ Z Z Z Z Z Z Z Z ZZ     ZZ ZZ Z Z ZZ   ZZZZ Z Z Z    Z ZZ Z Z  Z Z Z Z  Z Z Z Z    Z ZZ  ZZ  ZZZZZ  Z Z Z Z Z Z Z Z Z Z  Z Z Z Z    Z Z  Z Z ZZZZZS                 \n");
    printf("                                                                                     VX                                                                                                                            AAA                                                                                                                            ZV               \n");
    printf("                                                                                    XZ                                                                                                                                                                                                                                                             XZ              \n");
    printf("                                                                                    XX  Z A                                                                                                                                                                                                                                                   A Z  XX              \n");
    printf("                                                                                    ZXZ   A                                                                                                                                                                                                                                                   A  Z XZ              \n");
    printf("                                                                                    XX  Z A                                                                                                                                                                                                                                                   A  Z XZ              \n");
    printf("                                                                                    XX    A                                                                                                                                                                                                                                                   A  Z XZ              \n");
    printf("                                                                                    ZXZ   A                                                                                                          AAAAAAAA    AA       A                                                                                                                   A  Z XZ              \n");
    printf("                                                                                    ZX  Z A                                                                                                   AAE    AA          ACAA     AZ    AA     AAAAAE                                                                                                 A    XZ              \n");
    printf("                                                                                    XX    A                                                                                           AA      AA     AA          AA AA    A     AA     AA    AAA                                                                                              A  Z XZ              \n");
    printf("                                                                                    XXZ   A                                                                                     AA     AA     AV     AAAAAAA     AA  AAA  A     AA    AAV     OAA       AAAA                                                                                  A  Z XZ              \n");
    printf("                                                                                    ZX  Z A                                                                            AAA      AA      AAZ  AA      AA          AA   ZAA A    EAA    AA       AA    AAA   QAA                                                                                A  Z XZ              \n");
    printf("                                                                                    XX    A                                                                       A     AAAAA    AA      OAA AA      AA          AA     AAA    AAJ    AA      AAJ   AA       AA                                                                               A  Z XZ              \n");
    printf("                                                                                    XX    A                                                                   AAAA       A  AAAA  AH       AAA       HAAAAAAAC           AA    AA    AA      AAA   AAA       JAA     AAAAX                                                                    A  Z XZ              \n");
    printf("                                                                                    XX    A                                                                 AA           AA    AAAAA        AA                                        XAAAAAAA     HAA       AA    LAS   AAA                                                                  A  Z XZ              \n");
    printf("                                                                                    XX    A                                                           AS     AA   AAA     AA     ZAAA                                                               AA      AAH    SAA     H                                                                  A  Z XZ              \n");
    printf("                                                                                    ZXZ   A                                                           AAO     AAAA        AA                                                                          AAAAAAA        AAA                                                                      A  Z XZ              \n");
    printf("                                                                                    XX    A                                                            OAA     AH                                                                                                A     AAA                                                                    A  Z XZ              \n");
    printf("                                                                                    XX    A                                                  AAAAAA     XAJ    JAZ EAAA                                                                                          AAE    AA                                                                    A  Z XZ              \n");
    printf("                                                                                    XX Z  A                                               AAAC    A      AAZ    AAA                                                                                                AAAAAH              VAAA                                                   A  Z XX              \n");
    printf("                                                                                    XX Z  A                                                AA   AAAAAA    AAA                                                                                                                        AAAQAA                                                   A  Z XX              \n");
    printf("                                                                                    XX    A                                                 AAAAA    AAX                  A  A AAAAAAAAQAA          AAAA      AA                               X  Z          A                    AAA   AA                                                    A  Z XX              \n");
    printf("                                                                                    XX Z  A                                                  AAE     AA                A                         A  AAAAAAAAAA  A        XAA   SAAA Q  OEA   AAAAAAAAAAAAAAA    L              AAA  AAA AA                                                    A  Z XX              \n");
    printf("                                                                                    ZX    A                                                   ZAA AAAA             A      ZXZ                   Q     AAAAA      A AAA AECA  A V  A  AA     A               AAACA                     AAAH                                                    A  Z XX              \n");
    printf("                                                                                    XX    A                                                     AAA              A      Z    VQJHAAAAAAAAAAA   AA  AAAAAAAAAAA     ZA A  ZL AAA A  A    AA  E  ZZ             AA    V                  AA                                                     A  Z XX              \n");
    printf("                                                                                    XXZ Z A                                                                      QSAAAASQJEAAAAAAAAAAAAAAAAAC AAAQAAAACCAAAAAAAAAA O   JAQA AAAAAAAAAA AXXLJA AAAAAAAEJJHJOS   AA     Z                AA                                                     A  Z XX              \n");
    printf("                                                                                    ZX    A                                                                      LXJJJEHAAAAAALV     AA    E ZAAAAAEEQOLCEO    HAAA EAAAAAAAAAAHHAAAAAAAAAO  AAAAAAAAAAAAAAAAL AA  AHO X                                                                      A  Z XX              \n");
    printf("                                                                                    XX    A                                                                       A ZQAAOZ     AZ    AAA   AASAOZV         XAA       LQX                 AE  A   AA   ZVAAAAAAAAAAEALLSA                                                                      A  Z XX              \n");
    printf("                                                                                    XX  Z A                                                                          ALAH   AAA   A      A  AEA    AA       AAAA     SA   AAC      AA H  A   X  O              JAEAAOE                                                                        A  Z XX              \n");
    printf("                                                                                    XXZ   A                                                                          C EALQ AAE A          A AZH  AC  AA AXA    AA    A O      Z  VA   VAA   AS         AAAA AA A JA                                                                          A  Z XX              \n");
    printf("                                                                                    ZX    A                                                                           HXAA     AAAEAAAQ AA   AJAAA    AAA AA     A        A LA Z AAA   AA    AA  AA  Z         QAAJQ                                                                          A  Z XX              \n");
    printf("                                                                                    XX  Z A                                                                              HJAAA A  L   A A A  AAE AJ    A   AAOJAAA    QC AAA       HAJ VA A AAA   OA   AAAA   AAA A                                                                           A  Z XX              \n");
    printf("                                                                                    XX    A                                                                             AAA     A    AA AV    AXO OAAAA      ASC A    AAV     OAO  VQO A    AAAHAA  HX   OAA AAQ   S                                                                          A  Z XX              \n");
    printf("                                                                                    XX  Z A                                                                             A EAQS SAAAAAA V LAAAA  ZA    A    AAA            AA       A   A     AL  A   VZ      AA CAJ                                                                           A  Z XX              \n");
    printf("                                                                                    XX    A                                                                              QVAJ      X    C       OLA   AAAAAAAA            AAAA   AAA  L          AA     AAA  AA   A                                                                           A  Z XX              \n");
    printf("                                                                                    XX  Z A                                                                               ESA  A     ZZ   AQ     JJA     AA            A   AAAAAAAA   A          EAAAAAAAA   AQA                                                                              A  Z XX              \n");
    printf("                                                                                    XX    A                                                                                ZAXAAAA ZZZZXZ         ZAL        E         ALAA  AACA    A                      OAL                                                                               A  Z XX              \n");
    printf("                                                                                    XX  Z A                                                                                ASA JA        Z        AJA    AAA   X        ZAA         AJ                     JAVO                                                                               A  Z XX              \n");
    printf("                                                                                    XX    A                                                                                 EJA SX J AAA          AAAAZ  AAA   AH                   HA         O Z        AALA                                                                                A  Z XX              \n");
    printf("                                                                                    XX    A                                                                                  AEA JA AAAAAAA       HAAAAX      AAJ       C    QAAA VLA         A  Z ZAAAA ZAHV                                                                                 A  Z XX              \n");
    printf("                                                                                    XX Z  A                                                                                 AXAHAX    AA  AA     LEAAAAAA   AAA         AH   AAHAA AA               AAA  AAAA                                                                                 A  Z XX              \n");
    printf("                                                                                    XX    A                                                                                   AAEAS   AHA AA     XAAAAAA   AAEA JA       A    AAA XAA          SAAA     AAAAA                                                                                 A  Z XX              \n");
    printf("                                                                                    XX    A                                                                                AAAAHAAAX  ASA  A    AOAAA AAC   AAA A      AOA  HZ    AJAAA       AALAA  Z LAAA AA                                                                                A    XX              \n");
    printf("                                                                                    XX  Z A                                                                                AAAVOAAAE  AVAA A    QHA AJA AOAE OA A      AAA      AAA AAA       AA L    VAAAO AA                                                                                A  Z XX              \n");
    printf("                                                                                    XX    A                                                                               AA  O LAAAE AZJA A    ZA  XJA AAAA OALAHL   AA HAAAA AEAQ A AA      AA  L  VAAALX  AA                                                                               A  Z XX              \n");
    printf("                                                                                    XX  Z A                                                                               AA  Q  SAAC AAL  A   ALAC AAA  AA  SAJ AA    A  AZA   AA  J A       A      AAAA ZA AA                                                                               A  Z XX              \n");
    printf("                                                                                    XX    A                                                                               AA  Z   QAALZX  AA   CHAC SAA  VAA OAA A     A  AQ  OAA    AAS     JA  AA EAAA   A AA                                                                               A  Z XX              \n");
    printf("                                                                                    XX  Z A                                                                               AAA  X     AO   AA   ALAA  EA OZOA QAAJLAA   A   A XAAZ    Q AX       Z  LAAL    AA AA                                                                              A  Z XX              \n");
    printf("                                                                                    XX    A                                                                               AA  AAAAAAAAAH  O    AEAC  QASQVXAAAAEA AA  AAA  A VA      A AA     SA  XCAL    AA  AA                                                                              A  Z XX              \n");
    printf("                                                                                    XX  Z A                                                                              VVA  AAA    AAA EL    AAAA  HAA    AA EASH  S A    AA   AA  A   V         AA   ACA    LS                                                                             A  Z XX              \n");
    printf("                                                                                    XX Z  A                                                                               AC  AAA    AEA        SX   AAA    AA   V   OXS    AA   AA    QZE         AA   AHA                                                                                   A  Z XX              \n");
    printf("                                                                                    ZX    A                                                                                   ACE    AE               AA    AA              CA   AA                CA   ALA                                                                                   A  Z XX              \n");
    printf("                                                                                    ZX    A                                                                                   AOA    AO              AA     AC              HA   AE                AA    JA                                                                                   A  Z XX              \n");
    printf("                                                                                    ZXZ Z A                                                                                   AH     AH              AA     AA              EA   AA                AAA  AAAA                                                                                  A  Z XX              \n");
    printf("                                                                                    XX    A       AAAA                                                                       ACA     AO              AA     AA              CA   AAV                AA  AX A                                                                    AAQ           A  Z XX              \n");
    printf("                                                                                    XX    A       A   AAA                                                                   AAA     AAAEAA         AA O    AC AA          HOAAA  AAVAA        A A AACA   A A                                                                       AAE        A  Z XX              \n");
    printf("                                                                                    XX  Z A      AA     AA    A                                                           AL  SE   AZJEEHS       A    AC  QHAA    H         AAZV   AAQ        A     QHA  EA    A                                                       AA            AA       A  Z XX              \n");
    printf("                                                                                    XX    A     AAA    AA    AA    AAA                                                    AAAAA                   AAA                    A         AAAAA                                                                              AA              AA      A  Z XX              \n");
    printf("                                                                                    XX  Z A     AH AAAAA    AA        AAAAAZ                                                                                                                                                                              C           AC              AA      A  Z XX              \n");
    printf("                                                                                    XX    A    AA          ZAJ          AAA    AAE                                                                                                                                                                        AA          AA              EAX     A  Z XX              \n");
    printf("                                                                                    ZX Z  A                AA        AAAA         AAAAAA                                                                                                                                                         SAAA     JAQ         AA              AA      A  Z XX              \n");
    printf("                                                                                    XX Z  A               AA      ZAAA              AAA    AAAAAA                                                                                                                                           AA    AAAAAC   AA          AA            AA       A  Z XX              \n");
    printf("                                                                                    XX    A                     AAA              AAAS      AA         AAAV                                                                                                                         AAA      VAA    A   AAA  AZ          AAL                   A  Z XX              \n");
    printf("                                                                                    XX    A                     QAAAAL         AAA        AA          AA   OAAL                                                                                                           AA      AAXAA      AA    AA     AAAA            LAAA                A  Z XX              \n");
    printf("                                                                                    XX  Z A                          SAAA   XEAA          AAAAAAA    AA      AA    AA       AA                                                                                  AAAC     AAAE     AA  AA     AAA    AE      AA                                A  Z XX              \n");
    printf("                                                                                    XX    A                                 JAAAAA       AAX         AA      AA    AA      AAAA                                                     AAAAAA       AAA             ACAA   AA AA     AV   AAO    AA    AA                                        A    XX              \n");
    printf("                                                                                    XX    A                                        AA    AA          AAAAAAAA     AAL     AA AA                       AAAS     VAA       AAA      AA     Q      AAVAA            AA QAAAA  AA    AAAAA   AA   AA                                              A  Z XX              \n");
    printf("                                                                                    ZX Z  A                                               AAAAAAA   AAX     AA    AA     AA   AA                      AACAC   AAAA      AA AA     HAA           AA  AA           AA   AA   AAZ   AA       C                                                   A  Z XX              \n");
    printf("                                                                                    XX Z  A                                                         AA     XAA    AA    AAAAAAAA                      AA  AA AA AA     QA   AJ      AAAAAAA    AA    AA           AA        AA                                                                A  Z XX              \n");
    printf("                                                                                    XX    A                                                                       AA   AA      AA                     AA   AAA  AA     AA HLAAA          OAA   AAAA SSAA          AA                                                                          A  Z XX              \n");
    printf("                                                                                    XX    A                                                                                    AA                     AA        AA    AAX    XA   VA     AA   AA       AA                                                                                     A  Z XX              \n");
    printf("                                                                                    XXZ Z A                                                                                                           AA        AA   AA       AA    AAAAA                                                                                                     A  Z XX              \n");
    printf("                                                                                    ZX    A                                                                                                                                                                                                                                                   A  Z XX              \n");
    printf("                                                                                    SZZ                                                                                                                                                                                                                                                            XX              \n");
    printf("                                                                                    VZ                                                                                                                                                                                                                                                              Z              \n");
    printf("                                                                                    JJOLLJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJLLJLLLLLLLLLLLLJLLLLJLLLLLLJJLJJLLLLJJLJJLJJLJJLJJLJJLJJLJJLJLLLJJLJJLJJLJJLJJLJJLJJLJJLJJLJJLJJLJJLJJLJLLJJJHJJJJJJJLJJJJJJJJJJJJJLJJJJJJLLLJJJJJJJJJJJJJJJJJJJJJJJHL              \n");
    printf("                                                                                    VVXVZXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXZZVVVVVVVVVVVVVVXXXXXXXVVVXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXVXXXXVXXXSX              \n");
    printf("                                                                                    SVXVXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXZVVVVVVVXZXVVVVVVVVVVVVVXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXVXXXXXVSV              \n");
    printf("                                                                                    VVXVXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXVXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXVXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXVVVVXVXSV              \n");
    printf("                                                                                    SVXVXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXVVXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXVXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXSV              \n");
    printf("                                                                                    VVXVXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXZVVVVVXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXVVXXXXXXXXXXXXXXXXXXXXXXXXXXXXVVVVXSV              \n");
    printf("                                                                                    SVXVXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXZXVVVVVVVVVXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXSX              \n");
    printf("                                                                                    SSXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXZXVVXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXVXSV              \n");
    printf("                                                                                    VVXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXVVVVVVVVVVVXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXVXXXXXXXXXXXXXVXVV              \n");
    printf("                                                                                    VVXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXVXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXVXXXXXXXXXXXXXXXVV              \n");
    printf("                                                                                    VSXXXVXXXXXXXXXXXXXXVXVXXXVXVXXVXVXVXVXVXVXVXVXVXVXVXVXVXVXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXVVVVVVVVVVVVVVVVVVXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXVVVVXVXSILxxxxxxxxxxx              \n");
    printf("                                                                                    LQXXZZXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXZZZXXXXXXXXXXXXXZZZZZZZZZZZZXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXZZZXZXXXXXXXXXXZXVXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXVXSL              \n");
    printf("                                                                                    ZXZXXXZXXXZXZXXXXZXZXZXZXZXZXZXZXZXZZXXXZZXZZZXZZZXZZXXXXXXXXXXXXXXXZZXXXXXXXXXZZXXXXXXZXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXZZXXXXZXZXZXZXZXXZXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXVXXVJXXXX              \n");
    printf("                                                                                   \n");
    printf("                                                                                   \n");
//    printf("                                                                                    _______  _______  _______  _______ _________ _______  _        _______          _______  _       _________ _______  _______          _______  _______  _______  _______          _______  _______  _       __________________ _                 _______  _______              \n");
//    printf("                                                                                   (  ____ )(  ____ )(  ____ \(  ____ \\__   __/(  ___  )( (    /|(  ____ \        (  ____ \( (    /|\__   __/(  ____ \(  ____ )        (  ____ )(  ___  )(  ____ )(  ___  )        (  ____ \(  ___  )( (    /|\__   __/\__   __/( (    /||\     /|(  ___  )(  ____ )             \n");
//    printf("                                                                                   | (    )|| (    )|| (    \/| (    \/   ) (   | (   ) ||  \  ( || (    \/        | (    \/|  \  ( |   ) (   | (    \/| (    )|        | (    )|| (   ) || (    )|| (   ) |        | (    \/| (   ) ||  \  ( |   ) (      ) (   |  \  ( || )   ( || (   ) || (    )|             \n");
//    printf("                                                                                   | (____)|| (____)|| (__    | (_____    | |   | |   | ||   \ | || (__            | (__    |   \ | |   | |   | (__    | (____)|        | (____)|| (___) || (____)|| (___) |        | |      | |   | ||   \ | |   | |      | |   |   \ | || |   | || (___) || (____)|             \n");
//    printf("                                                                                   |  _____)|     __)|  __)   (_____  )   | |   | |   | || (\ \) ||  __)           |  __)   | (\ \) |   | |   |  __)   |     __)        |  _____)|  ___  ||     __)|  ___  |        | |      | |   | || (\ \) |   | |      | |   | (\ \) || |   | ||  ___  ||     __)             \n");
//    printf("                                                                                   | (      | (\ (   | (            ) |   | |   | |   | || | \   || (              | (      | | \   |   | |   | (      | (\ (           | (      | (   ) || (\ (   | (   ) |        | |      | |   | || | \   |   | |      | |   | | \   || |   | || (   ) || (\ (                \n");
//    printf("                                                                                   | )      | ) \ \__| (____/\/\____) |___) (___| (___) || )  \  || (____/\        | (____/\| )  \  |   | |   | (____/\| ) \ \__        | )      | )   ( || ) \ \__| )   ( |        | (____/\| (___) || )  \  |   | |   ___) (___| )  \  || (___) || )   ( || ) \ \__ _    _    _ \n");
//    printf("                                                                                   |/       |/   \__/(_______/\_______)\_______/(_______)|/    )_)(_______/        (_______/|/    )_)   )_(   (_______/|/   \__/        |/       |/     \||/   \__/|/     \|        (_______/(_______)|/    )_)   )_(   \_______/|/    )_)(_______)|/     \||/   \__/(_)  (_)  (_)\n");
//    printf("                                                                                                                                                                                                                                                                               \n");
}
void Grafico_MostrarLogo(void) {
    printf(" \n");
    printf("                                                                                                                               .=+:                                                    :==:                                                                                                     \n");
    printf("                                                                                     @@@@@@@@@@+  @@@@        @@@@@@@@@@    @@@@@@@@@@=   @@@@   @@@@@@@@@@@         @@@@@@@@@@@.  .@@@@@@@@@@    @@@@                                                                                          \n");
    printf("                                                                                     @@@@         @@@@        @@@@        .@@@@    #@@@:  @@@@   @@@@    @@@@        @@@@   :@@@@ .@@@@    @@@@:  @@@@                                                                                          \n");
    printf("                                                                                     @@@@@@@@@   @@@@         @@@@@@@@@@  *@@@*        :  @@@@   @@@@@@@@@@@         @@@@@@@@@@@: @@@@.     -@@@  @@@@                                                                                          \n");
    printf("                                                                                     @@@@         @@@@        @@@@-----:  =@@@@  @@@@@@=  @@@@   @@@@@@@@@#          @@@@@@@@@@    @@@=    +@@@*  @@@@                                                                                          \n");
    printf("                                                                                     @@@@@@@@@@*  @@@@@@@@@@  @@@@@@@@@@@  @@@@@  @@@@@=  @@@@   @@@@   @@@@         @@@@  @@@@@   @@@@@##@@@@@   @@@@@@@@@@                                                                                    \n");
    printf("                                                                                     @@@@@@@@@@   @@@@@@@@@@  @@@@@@@@@@@   -@@@@@@@=@@=  @@@@   @@@@   @@@@@        @@@@   @@@@@   :@@@@@@@@.    @@@@@@@@@@                                                                                    \n");
    printf(" \n");
    printf("                                                             #@@@@@@@@@@                                                                                                                                              -*@@@   :@@@.                                                            \n");
    printf("                                              .+@@@@@@@@@@   @@@@@@@@@@@   #@@@@                                                                                                                          .*+   #@@@@@@@@@@:  #@@@@@   @@@@   @*                                               \n");
    printf("                                         -@   *@@@@@+ @@@@*  @@@@-         @@@@@@   @@@@                                                                                                                 @@@@   -@@@@         @@@@@@@  @@@@  @@@@@@@@:                                         \n");
    printf("                                    .@@@@@@#   @@@@  -@@@@.  @@@@@@@@@@   -@@@@@@@ #@@@.  @@@@@.                                                                                                         +@@@@   @@@@@@@@@@   @@@@@@@@#@@@*     :@@@@@@@@*                                     \n");
    printf("                                 @@@@@@@.      @@@@@@@@@@.   @@@@-        @@@@#@@@@@@@@    :@@@@@@@.                                                                                        .@@@@         @@@@   @@@@.        @@@+ @@@@@@@:     @@@@+ .@@   @@.                                \n");
    printf("                                 .@@@@ :@@@@+  .@@@@  @@@@@  @@@@@@@@@@@ =@@@= #@@@@@@      .@@@@@@@@#                                                                                       #@@@@        .@@@@  @@@@@@@@@@@.-@@@   @@@@@@      @@@@       @@@@@@+                             \n");
    printf("                          -@@@-   :@@@@@@@-     @@@@.  *@@@@.*@@@@@@@@@@ @@@@   *@@@@@     :@@@@   :#   @@                                                                           +@@@@     @@@@      . @@@@. =@@@@@@@@@#..*@@    =@@@@     @@@@      =@@@@:@@@@@@                          \n");
    printf("                       =@@@@@@@-   *@@@@  =@@@@=:@@@.                            =@@@=     @@@@.      #@@@@@.                                                                      @@@@@@@@@   @@@@  #@@@@  @@@=                              @@@@#      @@@@@@    @@                          \n");
    printf("                      @@@@    @@@@  #@@@@@@@@+                                            @@@@.      @@@@@@@@@@                                                                  :@@@:      .@  @@@@@@@@*                                        -     @@@@@-@@@@@                             \n");
    printf("                     @@@@  :@@@@@@@: @@@@                                                  @@      *@@@@@@  *@@@@                                                                @@@@      @@@   @@@:                                                 =@@@@@=   @=                             \n");
    printf("                     @@@@   =- @@@@@+                                                             @@@@@+@@@@:  +                                                                  @@@@#    @@@+                                                          :@@@@@#                               \n");
    printf("                      @@@@@@*@@@@=                                                                @@@@@   @@@                                                                      @@@@@@@@@@.                                                               @@@-                              \n");
    printf("                       -@@@@@@@@                                                                    -@@@@*                                                                           @@@@@                                                                                                     \n");
    printf("                          .:                                  :+#@@#+:                                 @@@@                                                                                                              :+#@@#*-                                                              \n");
    printf("                                                          @@@@@@@@@@@@@@@@.                                                                                                                                          @@@@@@@@@@@@@@@@:                                                         \n");
    printf("                                                       #@@@@@@@@@@@@@@@@@@@@@                                                                    ==:.                                                             *@@@@@@@@@@@@@@@@@@@@@                                                       \n");
    printf("                                                     #@@@@@@@@@@@@@@@@@@@@@@@@@                                                     @@@@         @@@@@@@@@@@                                                    +@@@@@@@@@@@@@@@@@@@@@@@@@                                                     \n");
    printf("                                                    @@@@@@@@@@@@@@@@@@@@@@@@@@@@                                        =@@@@@@@@   @@@@         @@@@    :-.     =@@@@+                                        @@@@@@@@@@@@@@@@@@@@@@@@@@@@                                                    \n");
    printf("                                                   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                     @@@@@@=@@@@@  @@@@.       +@@@@@@@@@.     @@@@@@@                                       @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                                   \n");
    printf("                                                  #@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                             @@@@    @@@@  -@@@@  +@@@*       @@@@*#@@@@    -@@@@:@@@       @@                             =@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                                  \n");
    printf("                                                  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                           .@@@@@@-  *@@@@@@@@*   .@@@@@@@@@@ @@@@@#+=:    @@@@+ :@@@     @@@@@@@-                         @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@.                                                 \n");
    printf("                                                  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@.                     =@@    @@@@@@*  @@@@=        @@@@@@@@@@+@@@@@@@@@@@+@@@@@@@@@@@.   @@@@ :@@@@:                       @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@-                                                 \n");
    printf("                                                  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                   -@@@@@@@. @@ @@@@@ :@@@@                               +@@  .*@@@@-  @@@@+   @@@@                       @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@:                                                 \n");
    printf("                                                  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                    =@@@@@@@@@@@ @@@@@  @@-                                      -@@@+:@@@@:    @@@@                       @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                                  \n");
    printf("                                                  :@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@=               @@@   .@@@@-@@@@@@ #@@                                                 @@@@@@. @@@@@:   @@@@@                @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@+                                                  \n");
    printf("                                                   +@@@@@@@@@@@@@@@@@@@@@@@@@@@@+             .@@@@.     @@@@..@@@@                                                        +@@@@@@@@  @@@@@@@@@@*             .@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                                   \n");
    printf("                                                     @@@@@@@@@@@@@@@@@@@@@@@@@@:             @@@@  =@@@   @@@@=                          -@@@@@@@@@@@=                               @@@@@    @@@@              @@@@@@@@@@@@@@@@@@@@@@@@@@+                                                    \n");
    printf("                                                      #@@@@@@@@@@@@@@@@@@@@@@@             @@@@@#*@@@#  @@-@@                         @@@@@@@@@@@@@@@@@@@                           :@@@:     =@@@-              +@@@@@@@@@@@@@@@@@@@@@@@                                                      \n");
    printf("                                                        :@@@@@@@@@@@@@@@@@@-                 @@@@@@#  @@@@#                         @@@@@@@@@@@@@@@@@@@@@@@:                         @@@@    @@@@@                 .@@@@@@@@@@@@@@@@@@+                                                        \n");
    printf("                                                            *@@@@@@@@@@#                      .@@@@@@@@@#                          @@@@@@@@@@@@@@@@@@@@@@@@@@                          @@@@@@@@@@                      +@@@@@@@@@@#                                                            \n");
    printf("                                                           =@@@@@@@@@@@@=                       .@@@@@+                          @@@@@@@@@@@@@@@@@@@@@@@@@@@@@                          -@@@@@                       -@@@@@@@@@@@@+                                                           \n");
    printf("                                                       @@@@@@@@@@@@@@@@@@@@@@                     :@#                            @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                                   @@@@@@@@@@@@@@@@@@@@@@                                                       \n");
    printf("                                                   .@@@@@@@@@@@@@@@@@@@@@@@@@@@@:                                              .@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@+                                               @@@@@@@@@@@@@@@@@@@@@@@@@@@@-                                                   \n");
    printf("                                                 *@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                            *@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                            =@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                                 \n");
    printf("                                               *@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                           @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                          =@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                               \n");
    printf("                                             -@@@@@@@@@@@@@@@@@@@@@. @@@@@@@@@@@@@@@@@*                                        *@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                        :@@@@@@@@@@@@@@@@@      =@@@@@@@@@@@@@@@@#                                             \n");
    printf("                                            @@@@@@@@@@@@@@@@@@#.     #@@@@@@@@@@@@@@@@@@                                        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                        @@@@@@@@@@@@@@@@            @@@@@@@@@@@@@@@@                                            \n");
    printf("                                           @@@@@@@@@@@@@@@@@:        #@@@@@@@@@@@@@@@@@@@:                                      -@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#                                       @@@@@@@@@@@@@@@@=    @@@@     @@@@@@@@@@@@@@@@-                                          \n");
    printf("                                         .@@@@@@@@@@@@@@@@@@@@@@:    #@@@@@@@@@@@@@@@@@@@@-                                      -@@@@@@@@@@@@@@@@@@@@@@@@@@@*                                       @@@@@@@@@@@@@@@@@@@@@@@@@@    +@@@@@@@@@@@@@@@@@+                                         \n");
    printf("                                        .@@@@@@@@@@@@@@@@@@@@@@@:    #@@@@@@@@@@@@@@@@@@@@@-                                       @@@@@@@@@@@@@@@@@@@@@@@@@.                                       @@@@@@@@@@@@@@@@@@@@@@@@      @@@@@@@@@@@@@@@@@@@@+                                        \n");
    printf("                                        @@@@@@@@@@@@@@@@@@@@@@@@:    #@@@@@@@@@@@@@@@@@@@@@@.                                        @@@@@@@@@@@@@@@@@@@@@-                                        @@@@@@@@@@@@@@@@@@@@@@@@@@@#     @@@@@@@@@@@@@@@@@@@.                                       \n");
    printf("                                       @@@@@@@@@@@@@@@@@@@@@@@@@:    #@@@@@@@@@@@@@@@@@@@@@@@                                          =@@@@@@@@@@@@@@@*                                          @@@@@@@@@@@@@@@@@@@@....@@@@@-    @@@@@@@@@@@@@@@@@@@@                                       \n");
    printf("                                      -@@@@@@@@@@@@@@@@@@@@@:            @@@@@@@@@@@@@@@@@@@@*                                              .#@@@@:                                              :@@@@@@@@@@@@@@@@@@@@              @@@@@@@@@@@@@@@@@@@@*                                      \n");
    printf("                                      @@@@@@@@@@@@@@@@@@@@@@:            @@@@@@@@@@@@@@@@@@@@@                                        :@@@@@@@@@@@@@@@@@:                                        @@@@@@@@@@@@@@@@@@@@@@@          @@@@@@@@@@@@@@@@@@@@@@@                                      \n");
    printf("                                     .@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@:                                    @@@@@@@@@@@@@@@@@@@@@@@@@.                                    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@+                                     \n");
    printf("                                     #@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#                                 @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                 *@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                     \n");
    printf("                                     @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                               @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@.                              @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                     \n");
    printf("                                     @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                             @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                             @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                     \n");
    printf("                                     @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                           +@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                           @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                     \n");
    printf("                                      -@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@:                           @@@@@@@@@@@@@@@@@@@+     :@@@@@@@@@@@@@@@@@@@                           :@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@-                                      \n");
    printf("                                            *@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@=                                @@@@@@@@@@@@@@@@@@           =@@@@@@@@@@@@@@@@@-                               *@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@+                                            \n");
    printf("                                                           .::--------:.                                               @@@@@@@@@@@@@@@@@@    +@@@     #@@@@@@@@@@@@@@@@@=                                             .::--------:..                                                           \n");
    printf("                                                                                                                      @@@@@@@@@@@@@@@@@@@####@@@@      @@@@@@@@@@@@@@@@@@:                                                                                                                     \n");
    printf("                                                                                                                     @@@@@@@@@@@@@@@@@@@@@@@@@@      @@@@@@@@@@@@@@@@@@@@@                                                                                                                     \n");
    printf("                                                                                                                    *@@@@@@@@@@@@@@@@@@@@@@@+      @@@@@@@@@@@@@@@@@@@@@@@@                                                                                                                    \n");
    printf("                                                                                                                    @@@@@@@@@@@@@@@@@@@@@@.     *@@@@@@@@@@@@@@@@@@@@@@@@@@.                                                                                                                   \n");
    printf("                                                                                                                    @@@@@@@@@@@@@@@@@@@@@             -@@@@@@@@@@@@@@@@@@@@                                                                                                                    \n");
    printf("                                                                                                                   @@@@@@@@@@@@@@@@@@@@@#             -@@@@@@@@@@@@@@@@@@@@@                                                                                                                   \n");
    printf("                                                                                                                   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@-                                                                                                                  \n");
    printf("                                                                                                                  .@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@+                                                                                                                  \n");
    printf("                                                                                                                  :@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*                                                                                                                  \n");
    printf("                                                                                                                   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                                                                                                   \n");
    printf("                                                                                                                      @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                                                                                                       \n");
    printf("                                                                                                                             .=+ @@@@@@@@@@@@@@@@@@@@@@@@@@@@ #=-                                                                                                                              \n");
    printf(" \n");
}

void Grafico_MenuGerente(void) {
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                    @@@@        @@@@   +@@@@@@@   @@@#     @@    @@      .@.        @@@@@@@@      @@@@@@@@          @@@@@@@@    @@@@@@@@   @@@@@@@     @@@@@@@@   @@@      @@  @@@@@@@@@@  @@@@@@@@                                              \n");
    printf("                                                                    @@ @@      *@ @@   +@         @@ @@    @@    @@      .@.        @@      @@    @@              @@            @@         @@    @@-   @@         @@@@     @@      @@      @@                                                    \n");
    printf("                                                                    @@ :@=     @  @@   +@         @@  @@   @@    @@      .@.        @@       @@   @@             @@             @@         @@    @@    @@         @@ @@.   @@      @@      @@                                                    \n");
    printf("                                                                    @@  @@    @@  @@   +@@@@@@.   @@   @@  @@    @@      .@.        @@       @@   @@@@@@@        @@    @@@@@    @@@@@@@    @@@@@@      @@@@@@@    @@  #@@  @@      @@      @@@@@@@                                               \n");
    printf("                                                                    @@   @@  @@   @@   +@         @@    @@ @@    @@      .@.        @@       @@   @@             @@        @    @@         @@   +@+    @@         @@    @@ @@      @@      @@                                                    \n");
    printf("                                                                    @@    @@@@    @@   +@         @@     @@@@    @@      @@         @@      @@    @@              @@       @    @@         @@    @@    @@         @@     @@@@      @@      @@                                                    \n");
    printf("                                                                    @@     @@     @@   +@@@@@@@   @@      @@@     *@@@@@@@          @@@@@@@@      @@@@@@@@          @@@@@@@@    @@@@@@@@   @@     @@   @@@@@@@@   @@      @@@      @@      @@@@@@@@                                              \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                      @@          @@     @                       @                 *@                      @=                  @                                                                                                                             \n");
    printf("                       @           @.   @   @@@.  @ @@     @@@+ @@@@  @@@@    @@@@ *@  @+     @@@@   +@@@ @@@@: @   #@  @@@@   @                                                                                                                             \n");
    printf("                       @            @  @# +@   @  @.      *@     @   @    @  @     *@@#          @+ @@     @*   @   #@      @  @                                                                                                                             \n");
    printf("                       @            -@ @  #@      @          @@  @   @    @  @     *@ @      @*  @+ @@     @*   @   #@ *@   @  @                                                                                                                             \n");
    printf("                    .@@@@@  @+       @@    #@@@@  @       @@@@   @@@  @@@@    @@@@ *@  @@     @@@++  @@@@   @@:  @@@ @  @@@ @  @                                                                                                                             \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                    @@:@@            @@                                                                                                    @@                                                                                                          \n");
    printf("                        @*          #  @    @@@@@  @@@# @@@@#  @@@@@@ @@@@   @=@@     @@@@@  @    @   @@@@  @    @  @@@@      @@@@  @@@@@  @@@@@@   @@@@   @:@@                                                                                                  \n");
    printf("                       @@           @  .@  @*  @   @   @@   @  @   @      @  @        @   @  @    @  @    @ :@  @  @    @     @@        @  @@   @  @    @  @                                                                                                  \n");
    printf("                     @@            @@@@ @@ @ :.    @   @@      @ -   +@++ @  @        @   @  @@   @  @ +++   @@@.  @    @        @+ @++ @  @@   @  @    @  @                                                                                                  \n");
    printf("                    @@@@@@  @.    @=     @  @ .@@  @    .@@@@  @ .=@  @@@ @  @        @   @   @@@ @   @@@@:   @@    @@@@      @@@@  +@@=@  @ @@@    @@@@   @                                                                                                  \n");
    printf("                                           @@  @@              @= @@                                                                                                                                                                         \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                    @@:@@         @@     @                                                            @                                                                                                                                    \n");
    printf("                        @          @=   @  @@@@@  @ @@     @@@@@ @@@@@   @@@@@  #@@@@   @@@@@   @@@@  @  @@@@@  @@@@                                                                                                                  \n");
    printf("                     @@@@           @  @= #@   @  @       @@  @+     @=  @   @      @   @   @  @      @      @  @@                                                                                                                  \n");
    printf("                        -@          .@ @  +@      @       @ ::   @++ @=  @   @  @++ @   @   @  @      @  @++ @     @                                                                                                                  \n");
    printf("                    *@@@@   @.       @@     @@@@  @       =@ :@@  @@@@-  @   @   @@@@   @   @   @@@@  @  @@@:@  @@@@                                                                                                                  \n");
    printf("                                                          @@  @@                                                                                                                                                                         \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                       @@         *@    .@         @                                                                                                                                                                                                   \n");
    printf("                     @@ @          @#   @  @@=@@   @ +@   @  @@ @@  @-@@                                                                                                                                                                                 \n");
    printf("                    @   @           @  @. -@    @  @  @=  @  @@@@@  @                                                                                                                                                                                      \n");
    printf("                    ++++@+           @-@   @   -@  @   @ @  =@      @                                                                                                                                                                                      \n");
    printf("                        @   @        :@     @@@    @    @     @@@=  @                                                                                                                                                                                      \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
}
void Grafico_Usuario(void) {
    printf("                                                                                                                                                                                                                             AAAAA                                                                                                                                        \n");
    printf("                                                                                                                                                                                                                      AAAAAAAAAAAAAAAAAAAA                                                                                                                                 \n");
    printf("                                                                                                                                                                                                                  AAAAAA                  AAAA                                                                                                                             \n");
    printf("                                                                                                                                                                                                               AAAAA                         AAAA                                                                                                                         \n");
    printf("                                                                                                                                                                                                             AAAA                               AAA                                                                                                                       \n");
    printf("                                                                                                                                                                                                           AAA                                    AAA                                                                                                                     \n");
    printf("                                                                                                                                                                                                          AAA                                       AA                                                                           aa      aa                                   \n");
    printf("                                                                                                                                                                                                        AAA                                          AA                                                                         aaaa    aaaa                                   \n");
    printf("                                                                                                                                                                                                       AAA                                            AA                                                                        aaaa    aaaa         a                  \n");
    printf("                                                                                                                                                                                                       AAA           AAAAAA          AAAAAA            AA                                                         aa            aaaa    aaaa       aaa                     \n");
    printf("                                                                                                                                                                                                      AAA            AAAAAA          AAAAAA             AA                                                        aaaa         aaaaa    aaaa     aaaa                       \n");
    printf("                                                                                                                                                                                                     AAA             AAAAAA          AAAAAA             AA                                                          aaaa       aaaaa   aaaaa    aaaa                         \n");
    printf("                                                                                                                                                                                                     AAA             AAAAAA          AAAAAA              AA                                                            aaaa    aaaaa  aaaaaa   aaaa                          \n");
    printf("                                                                                                                                                                                                     AAA             AAAAAA          AAAAAA              AA                                                              aaaa  aaaaa  aaaaa   aaaaa                              \n");
    printf("                                                                                                                                                                                                     AAA                                                 AA                                                               aaaaaaaaaaaaaaaaaaaaaaa                                             \n");
    printf("                                                                                                                                                                                                     AAA        AAA                          AAAA        AA                                                    aaaa       aaaaaaaaaaaaaaaaaaaaaaa                                           \n");
    printf("                                                                                                                                                                                                     AAA         A                            AA        AA                                                      aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa                                             \n");
    printf("                                                                                                                                                                                                      AAA         A                           A         AA                                                        aaaaaaaaaaaaaaaaaaaaaaaaaaaa                                             \n");
    printf("                                                                                                                                                                                                      AAA         AA                         A          AA                                                             aaaaaaaaaaaaaaaaaaaaaa                                             \n");
    printf("                                                                                                                                                                                                       AAA          A                      AA         AAA                                                                        aaaaaaaa                                         \n");
    printf("                                                                                                                                                                                                        AAA           AA                 AA          AAA                                                                         aaaaaaaaa                                         \n");
    printf("                                                                                                                                                                                                         AAAA            AAAAAAAAAAAAAA             AAA                                                                          aaaaaaaaa                                         \n");
    printf("                                                                                                                                                                                                          AAAA                                     AAA                                                                           aaaaaaaaa                                        \n");
    printf("                                                                                                                                                                                                            AAAA                                 AAA                                                                             aaaaaaaaaa                                         \n");
    printf("                                                                                                                                                                                                             AAAAA                             AAA                                                                               aaaaaaaaaaa                                        \n");
    printf("                                                                                                                                                                                                                AAAAA                      AAAAA                                                                                 aaaaaaaaaaa                                         \n");
    printf("                                                                                                                                                                                                                   AAAAAAAAAA      AAAAAAAAAA                                                                                   aaaaaaaaaaaa                                          \n");
    printf("                                                                                                                                                                                                                       AAAAAAAAAAAAAAAAAA                                                                                       aaaaaaaaaaa                                          \n");
    printf("                                                                                                                                                                                                                                                                                                                                aaaaaaaaaaaa                            \n");
    printf("                                                                                                                                                                                                                                                                                                                                aaaaaaaaaaaa                                                                                  \n");
    printf("                                                                                                                                                                                                       AAAAAAAAAAAAAAAAA               AAAAAAAAAAAAAAAAAA                                                                       aaaaaaaaaaaa                                          \n");
    printf("                                                                                                                                                                                                  AAAAAAAAAAAAAAAAAAAAAAA             AAAAAAAAAAAAAAAAAAAAAAAA                                                                  aaaaaaaaaaaa                                          \n");
    printf("                                                                                                                                                                                              AAAAAAAAAAAAAAAAAAAAAAAAAAAAA          AAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                              aaaaaaaaaaaa                                          \n");
    printf("                                                                                                                                                                                          AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA        AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAaaaaa                                                      aaaaaaaaaaaa                                         \n");
    printf("                                                                                                                                                                                       AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAaaaaaaaaaaaaaaaaaaaa                                   aaaaaaaaaaaaaa                                        \n");
    printf("                                                                                                                                                                                     AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAaaaaaaaaaaaaaaaaaa                             aaaaaaaaaaaaaaaaaaa                                           \n");
    printf("                                                                                                                                                                                   AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA       AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa                                                                                             \n");
    printf("                                                                                                                                                                                 AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA       AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa                                                                                          \n");
    printf("                                                                                                                                                                               AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA        AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa                                                                                         \n");
    printf("                                                                                                                                                                              AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA        AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa                                                                                       \n");
    printf("                                                                                                                                                                            AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA        AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                      \n");
    printf("                                                                                                                                                                           AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA        AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                                     \n");
    printf("                                                                                                                                                                          AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA        AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                                    \n");
    printf("                                                                                                                                                                         AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA          AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                                   \n");
    printf("                                                                                                                                                                        AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA          AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                                  \n");
    printf("                                                                                                                                                                       AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA           AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
    printf("                                                                                                                                                                       AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA           AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                                 \n");
    printf("                                                                                                                                                                      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA           AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                                \n");
    printf("                                                                                                                                                                     AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA           AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                               \n");
    printf("                                                                                                                                                                     AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA           AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                              \n");
    printf("                                                                                                                                                                    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA            AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                              \n");
    printf("                                                                                                                                                                    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA            AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                              \n");
    printf("                                                                                                                                                                    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA            AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                              \n");
    printf("                                                                                                                                                                    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA            AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                              \n");
    printf("                                                                                                                                                                    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA            AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                              \n");
    printf("                                                                                                                                                                    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA             AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                              \n");
    printf("                                                                                                                                                                    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA             AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                              \n");
    printf("                                                                                                                                                                    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA          AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                              \n");
    printf("                                                                                                                                                                    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA        AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                              \n");
    printf("                                                                                                                                                                    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA     AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                              \n");
    printf("                                                                                                                                                                    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA  AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                              \n");
    printf("                                                                                                                                                                    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                              \n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("                                                                                                                                            AAAA   AAAA   AAA     AAAAAAA    AAAAAAAAA    AAAAAAA    AAAAAAA    AAAAAAA       AAAA  AAAA    AAAAAAA    AAAAA AAAA     AAAAAA     AAAAAAAAA    AAAA    AAAAAAAA           A                                                                                                  \n");
    printf("                                                                                                                                            AAAA   AAAAA  AAA   AAAAA AAAA   AAAAA AAAA   AAA       AAAA AAAA   AAA           AAAA  AAAA   AAAA AAAA   AAAAA AAAA    AAAAAAAA    AAAA  AAAA   AAAA   AAAA  AAAA        AAAAA                                                                                               \n");
    printf("                                                                                                                                            AAAA   AAAAAA AAA   AAAAA        AAAAA AAAA   AAAAA     AAAAAA     AAAAA          AAAA  AAAA   AAAAAAA     AAAAA AAAA    AAAAAAAA    AAAA  AAAA   AAAA   AAAA  AAAA         AAA                                                                                                 \n");
    printf("                                                                                                                                            AAAA   AAAAAAAAAA   AAAAA AAAA   AAAAAAAAA    AAAAA        AAAAAA   AAAAA         AAAA  AAAA     AAAAAAA   AAAAA AAAA    AAAA AAA    AAAAAAAAA    AAAA   AAAA  AAAA                                                                                                           \n");
    printf("                                                                                                                                            AAAA   AAAA AAAAA   AAAAA AAAA   AAAAA AAAA   AAA       AAA  AAAA   AAA           AAAA  AAAA   AAAA AAAA   AAAAA AAAA   AAAAAAAAAA   AAAA  AAAA   AAAA   AAAA  AAAA        AAAAA                                                                                                 \n");
    printf("                                                                                                                                            AAAA   AAAA  AAAA    AAAAAAAAA   AAAAA AAAA   AAAAAAA   AAAAAAAAA   AAAAAAA        AAAAAAAAA   AAAAAAAAA    AAAAAAAAA   AAAA  AAAA   AAAA  AAAA   AAAA    AAAAAAAAA         AAA                                                                                                 \n");
    printf("                                                                                                                                                                                                                                                                         A                         \n");
    printf("\n");
    printf("\n");
    printf("\n");
}

void Graffico_Contrasea(void) {


    printf("");
    
    printf("");
    printf("                                                                                                                                                                                                                             AAAAA                                                                                                                                        \n");
    printf("                                                                                                                                                                                                                      AAAAAAAAAAAAAAAAAAAA                                                                                                                                 \n");
    printf("                                                                                                                                                                                                                  AAAAAA                  AAAA                                                                                                                             \n");
    printf("                                                                                                                                                                                                               AAAAA                         AAAA                                                                                                                         \n");
    printf("                                                                                                                                                                                                             AAAA                               AAA                                                                                                                       \n");
    printf("                                                                                                                                                                                                           AAA                                    AAA                                                                                                                     \n");
    printf("                                                                                                                                                                                                          AAA                                       AA                                                                                                                    \n");
    printf("                                                                                                                                                                                                        AAA                                          AA                                                                                                                   \n");
    printf("                                                                                                                                                                                                       AAA                                            AA                                                                                                                  \n");
    printf("                                                                                                                                                                                                       AAA           AAAAAA          AAAAAA            AA                                                                                                                 \n");
    printf("                                                                                                                                                                                                      AAA            AAAAAA          AAAAAA             AA                                                                                                                \n");
    printf("                                                                                                                                                                                                     AAA             AAAAAA          AAAAAA             AA                                                                                                                \n");
    printf("                                                                                                                                                                                                     AAA             AAAAAA          AAAAAA              AA                                                                                                               \n");
    printf("                                                                                                                                                                                                     AAA             AAAAAA          AAAAAA              AA                                                                                                               \n");
    printf("                                                                                                                                                                                                     AAA                                                 AA                                                                                                               \n");
    printf("                                                                                                                                                                                                     AAA        AAA                          AAAA        AA                                                                                                               \n");
    printf("                                                                                                                                                                                                     AAA         A                            AA        AA                                                                                                                \n");
    printf("                                                                                                                                                                                                      AAA         A                           A         AA                                                                                                                \n");
    printf("                                                                                                                                                                                                      AAA         AA                         A          AA                                                                                                                \n");
    printf("                                                                                                                                                                                                       AAA          A                      AA         AAA                                                                                                                 \n");
    printf("                                                                                                                                                                                                        AAA           AA                 AA          AAA                                                                                                                  \n");
    printf("                                                                                                                                                                                                         AAAA            AAAAAAAAAAAAAA             AAA                                                                                                                   \n");
    printf("                                                                                                                                                                                                          AAAA                                     AAA                                                                                                                    \n");
    printf("                                                                                                                                                                                                            AAAA                                 AAA                                                                                                                      \n");
    printf("                                                                                                                                                                                                             AAAAA                             AAA                                                                                                                       \n");
    printf("                                                                                                                                                                                                                AAAAA                      AAAAA                                                                                                                          \n");
    printf("                                                                                                                                                                                                                   AAAAAAAAAA      AAAAAAAAAA                                                                                                                             \n");
    printf("                                                                                                                                                                                                                       AAAAAAAAAAAAAAAAAA\n");
    printf("\n");
    printf("\n");
    printf("                                                                                                                                                                                                       AAAAAAAAAAAAAAAAA               AAAAAAAAAAAAAAAAAA                                                                                                                 \n");
    printf("                                                                                                                                                                                                  AAAAAAAAAAAAAAAAAAAAAAA             AAAAAAAAAAAAAAAAAAAAAAAA                                                                                                            \n");
    printf("                                                                                                                                                                                              AAAAAAAAAAAAAAAAAAAAAAAAAAAAA          AAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                                                        \n");
    printf("                                                                                                                                                                                          AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA        AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                                                     \n");
    printf("                                                                                                                                                                                       AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                                                 \n");
    printf("                                                                                                                                                                                     AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                                               \n");
    printf("                                                                                                                                                                                   AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA       AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                                             \n");
    printf("                                                                                                                                                                                 AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA       AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                                           \n");
    printf("                                                                                                                                                                               AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA        AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                                         \n");
    printf("                                                                                                                                                                              AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA        AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                                       \n");
    printf("                                                                                                                                                                            AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA        AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                                      \n");
    printf("                                                                                                                                                                           AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA        AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                                     \n");
    printf("                                                                                                                                                                          AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA        AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                                    \n");
    printf("                                                                                                                                                                         AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA          AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                                   \n");
    printf("                                                                                                                                                                        AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA          AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                                  \n");
    printf("                                                                                                                                                                       AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA           AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
    printf("                                                                                                                                                                       AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA           AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                                 \n");
    printf("                                                                                                                                                                      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA           AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                                \n");
    printf("                                                                                                                                                                     AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA           AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                               \n");
    printf("                                                                                                                                                                     AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA           AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                               \n");
    printf("                                                                                                                                                                    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA            AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                              \n");
    printf("                                                                                                                                                                    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA            AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                              \n");
    printf("                                                                                                                                                                    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA            AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                              \n");
    printf("                                                                                                                                                                    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA            AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                              \n");
    printf("                                                                                                                                                                    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA            AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                              \n");
    printf("                                                                                                                                                                    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA             AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                              \n");
    printf("                                                                                                                                                                    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA             AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                              \n");
    printf("                                                                                                                                                                    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA          AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                              \n");
    printf("                                                                                                                                                                    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA        AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                              \n");
    printf("                                                                                                                                                                    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA     AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                              \n");
    printf("                                                                                                                                                                    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA  AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                              \n");
    printf("                                                                                                                                                                    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                                                                              \n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("                                                                                                                                                                                                                                                         AAAAAA                                                                       \n");
    printf("                                                                                                                                         AAAA   AAAAA  AAA    AAAAAAAAA   AAAAAAAAAA   AAAAAAA   AAAAAAAA    AAAAAAA        AAAAAAAA     AAAAAAAA    AAAA   AAA  AAAAAAAAAA  AAAAAAAAA     AAAAAAA     AAAAAAAA    AAAAAAA   AAAA   AAA    AAAAAAA                                                                        \n");
    printf("                                                                                                                                         AAAA   AAAAAA AAA   AAAAA AAAA   AAAAA AAAA   AAAA      AAAA  AA    AAAA          AAAA  AAAA   AAAA  AAAA   AAAAA  AAA     AAAA     AAAAA AAAA    AAAAAAAA    AAAA  AA    AAAA      AAAAA  AAA    AAAAAAA          AAA                                                           \n");
    printf("                                                                                                                                         AAAA   AAAAAAAAAA   AAAAA        AAAAA AAAA   AAAAAA    AAAAAAAA    AAAAAA        AAAA         AAAA  AAAA   AAAAAAAAAA     AAAA     AAAAA AAA     AAA AAAA    AAAAAAAA    AAAAAA    AAAAAA AAA    AAA AAAA                                                                       \n");
    printf("                                                                                                                                         AAAA   AAAAAAAAAA   AAAAAAAAAA   AAAAAAAAAA   AAAAA       AAAAAAA   AAAA          AAAA         AAAA  AAAA   AAAAAAAAAA     AAAA     AAAAAAAAA     AAA  AAA      AAAAAAA   AAAA      AAAAAAAAAA    AAA AAAA                                                                       \n");
    printf("                                                                                                                                         AAAA   AAAA AAAAA   AAAAA AAAA   AAAAA AAAA   AAAA      AAAA AAAA   AAAA          AAAA  AAAA   AAAA  AAAA   AAA AAAAAA     AAAA     AAAAA AAAA   AAAAAAAAAA   AAAA AAAA   AAAA      AAA AAAAAA   AAAAAAAAAA        AAA                                                           \n");
    printf("                                                                                                                                         AAAA   AAAA  AAAA    AAAAAAAAA   AAAAA AAAA   AAAAAAA   AAAAAAAA    AAAAAAA        AAAAAAAA     AAAAAAAA    AAA   AAAA     AAAA     AAAAA AAAA   AAAA  AAAA    AAAAAAA    AAAAAAA   AAA   AAAA   AAAA  AAAA\n");
}
void Grafico_AccesoDenegado(void) {
    printf("                                                                                                                                                                                                                                                                                                                                                             \n");
    printf("                                                                                                                                                                                                                                                                                                                                                             \n");
    printf("                                                                                                                                                                                                                                                                                                                                                             \n");
    printf("                                                                                                                                                                                                                                                                                                                                                             \n");
    printf("                                                                                                                                                                                                                                                                                                                                                                                                        \n");
    printf("                                                                                                                                                                                                  AAAAAAAAAAAAAAAAAAAAAA                                                                                                                                     \n");
    printf("                                                                                                                                                                                          AAAAAAAAI11111111111111111111IAAAAAAAA                                                                                                                             \n");
    printf("                                                                                                                                                                                    AAAAAAI1111RIIIIIIIIIIIIIIIIIIIIIIIIIIR1111IAAAAAA                                                                                                                       \n");
    printf("                                                                                                                                                                                AAAAI111RIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIR111IAAAA                                                                                                                   \n");
    printf("                                                                                                                                                                            AAAAI11RIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIR11IAAAA                                                                                                               \n");
    printf("                                                                                                                                                                         AAAI11RIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIR11IAAA                                                                                                            \n");
    printf("                                                                                                                                                                      AAAI1WNIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIINW1IAAA                                                                                                         \n");
    printf("                                                                                                                                                                    AAN1WNIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIINW1NAA                                                                                v                      \n");
    printf("                                                                            v                                                       v                            AAAI1RIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIR1IAAA                                                                                                    \n");
    printf("                                                                                                                                                               AAN1RIIIIIIIIIIIIEIIIIIIIIIIIIIIIIR1111111WNAAAANW1111111RIIIIIIIIIIIIIIIIIIIIIIIIIIIIIR1NAA                                                                                                  \n");
    printf("                                                                                                                                                             AAN1RIIIIIIIIIIIIIIIIIIIIIIIIIR111IAAAAAAAAAAAIvvWAAAAAAAAAAAI111RIIIIIIIIIIIIIIIIIIIIIIIIIR1NAA                                                                                                \n");
    printf("                                                                                                                                                            AN1RIIIIIIIIIIIIIIIIIIIIIIIR16IAAAAA                          AAAAAI11RIIIIIIIIIIIIIIIIIIIIIIIR1IA                                                                                               \n");
    printf("                                                                                                                                                          AANWIIIIIIIIIIIIIIINIIIIINW1NAAAA                                    AAAAI1RIIIIIIIIIIIIIIIIIIIIIIRNAA                                                                                             \n");
    printf("                                                                                                                                                        AAR1WIINIINIINIININNNIIIIW1NAAA                                            AAAI1RIIIIIIIIIIIIIIIIIIIIR1NAA                                                                                           \n");
    printf("                                                                                                                                                       AA6WIIIIIIIIIIIIIIIIIIIN1RRAA                                                  AAR1WNIIIIIIIIIIIIIIIIIIIR1AA                                                                                          \n");
    printf("                                                                                                                                                      AR1WNNNNNNNNNNNNNNNNNNR1NAAA                                                      AAANIIIIIIIIIIIIIIIIIIIIR1NA                                                                                         \n");
    printf("                                                                                                                                                     ANWNIIINIINIINININININWRAA                                                       AAA6WNIIIINIIIIIIIIIIIIIIIIIRIA                                                                                        \n");
    printf("                                                                                                                                                   AARWNNNNNNNNNNNNNNNNNNW6RA                                                       AARW1WNNIINIIIIIIIIIINIIIIIIIIIRNAA                                                                                      \n");
    printf("                                                                                                                                                  AA61NNNNNNNNNNNNNNNNNN1RAA                          v                          AAAN1WNNIIIIIIIIININNIINIIIIIIIINNIW6AA                                                                                     \n");
    printf("                                                                                                                                                 EA61NNNNNNNNNNNNNNNNN16NA                                                    AAAR1WNINNIINNNNNNNNNIIIIIIINNINNINIIIIR6AA                                                                                   \n");
    printf("                                                                                                                                                 AR1NNNNNNNNNNNNNNNNN16AA                                                   AAR61WNNNNNNNNNIINNINIINNNNNNNNIIIIIIININNWRA                                                                                   \n");
    printf("                                                                                                                                                AR1NNNNNNNNNNNNNNNNN16AA                                                 AAIR61NNNINNNNINNNNNNNNNNNNIININIINNNNNNNNIIIIWRA                                                                                  \n");
    printf("                                                                                                                                               AA6NNNNNNNNNNNNNNNNN10AA                                               AAAR61NNNNNNNNNNNNNNNNINNNINNNNNNNNNNIINNINIINNNNN1AA                                                                                 \n");
    printf("                                                                                                                                               A01NNNNNNNNNNNNNNNN10AA                                             AAAR61WNNNNNNNNNNNNNNNNNNNNNNNNNN1111NNNNNNNNNNNIININW6A                                                                                 \n");
    printf("                                                                                                                                              AR1NNNNNNNNNNNNNNNNN6AA                                            AARR61NNNNNNNNNNNNNNNNNNNNNNNNNN11RRAAWNNNNINNINNNNNNNNNWRA                                                                                \n");
    printf("                                                                                                                                              A6RRRRRRRRRRRRRRRRR1RA                                          AAAR61NNNNNNNNNNNNNNNNNNNNNNNNNNW16RAAR AR1NNNNNNNNNNINNINNN6AW                                                                               \n");
    printf("                                                                                                                                             AE1NNNNNNNNNNNNNNNNR6AA                                       AAAR61RNNNNNNRNNNNNNRNNNNNNNNNNNR16RAAA    AA6RNNNNNNNNNNNNNNNN1IA                                                                               \n");
    printf("                                                                                                                                             Ad666166166166166166WA                                      AAR061RRRRRNRRNNNNNNNNNNNNNNNNNR16REAA        AR1NNNNNNNNNNNNNNNNR6A                                                                               \n");
    printf("                                                                                                                                            AAWAAAAAAAAAAAAAAAAAIAA                                   AARR06611RNNNNNNNNNRRRRRRNRNNNNN160RAA           AA6NNNNNNNNNNNNNNNNN6AA                                                                              \n");
    printf("                                                                                                                                            AA1IIIIIIIIIIIIIIIIN6A                                 AAAIREAAAAAW66166166111RNNNNNNNRW16RAAA              A0RNNNNNNNNNNNNNNNN6AA                                                                              \n");
    printf("                                                                                                                                            A6RIIIIIIIIIIIIIIIIWRA                              AAAN11RIIIIIIEAAAAAAAAAAAR661661660RAAA                 A11NNNNNNNNNNNNNNNN10A                                                                              \n");
    printf("                                                                                                                                            A6IIIIIIIIIIIIIIIII1AA                            AAIR1RIIIIIIIIIIIIIIIIIIIIEAAAAAAEAAAA                    AA6NNNNNNNNNRNNNNNNN0A                                                                              \n");
    printf("                                                                                                                                            A1IIIIIIIIIIIIIIIII6AA                         AAAI1RIIIIIIIIIIIIIIIIIIIIIIIIIIR11NAA                       AA0RRNNNNNNNNNNNNNNN0A                                                                              \n");
    printf("                                                                                                                                            A6IIIIIIIIIIIIIIIII1AA                      AAAN1WNIIIIIIIIIIIIIIIIIIIIIIIIIIR1NAAA                         AA0NNNRRRRNRNNNNNNNN0A                                                                              \n");
    printf("                                                                                                                                            A6NIIIIIIIIIIIIIIII1AA                    AAR11RIIIIIIIIIIIIIIIIIIIIIIIIIIR1IAAA                            AA0661WNNNNNNRRRNNNR0A                                                                              \n");
    printf("                                                                                                                                            ARWIIIIIIIIIIIIIIIIR6A                 AAAN1RIIIIIIIIIIIIIIIIIIIIIIIIIIR16NAA                               A1AAAA666111RNNNNNR11A                                                                              \n");
    printf("                                                                                                                                            AA1IIIIIIIIINIIIIIII6A              AAAN1RIIIIIIIIIIIIIINIIIIIIIIIIINW1RAAA                                 A1IIIAAAAAA161611RN6AA                                                                              \n");
    printf("                                                                                                                                             A6IIIIIIIIIIIIIIIII1AA          AAAR11RIIIIIIIIIIIIIIIIIIIIIIIIIIW1NAAA                                   AA1IIIIIIIIAAAAAA161dAW                                                                              \n");
    printf("                                                                                                                                             A6WIIIIIIIIIIIIIIIIR6A        AANW1WIIIIIIIIIIIIIIIIIIIIIIIIIIW1REAA                                      A6RIIIIIIIIIIIIIEAAA1A                                                                               \n");
    printf("                                                                                                                                             AA1IIIIIIIIINNNNNNNN1AA    AAAN1WIIIINININNIIIIIIIIIIIIIIIIR16RAAr                                       AA1IIIIIIIIIIIIIIIII1AA                                                                               \n");
    printf("                                                                                                                                              A6WNNNNNNNIIIIIIIIIW6A AAAR1WNNINNNIIIIIINNNNNIIIIINININW1RAAA                                          A6RIIIIIIIIIIIIIIIIR6A                                                                                \n");
    printf("                                                                                                                                              AA1IIIIIIIIIIIINIIIIWEAR66WIIIIIIIIININIIIIIIINNNNNIIW1NAAA                                            AERIIIIIIIIIIIIIIIII1AA                                                                                \n");
    printf("                                                                                                                                               A1WNNNNNNNNNNNNNNNNNWWWNNNIIIIIIIIIIIINIINIIIIIIIW16RAA                                              AAWIIIIIIIIIIIIIIIIIR1A                                                                                \n");
    printf("                                                                                                                                               AA6WNINNINNINIININNNNNNIINNNNNNNNNNNNNNNNNNNNNN11RAAA                                               AA1NNIIIIIIIIIIIIIIIR6AA                                                                                \n");
    printf("                                                                                                                                                AA1NIIINIIINIIINIIIIIINIIIINININNINNINIIINNW1RAAA                                                 AA1RIIIIIINIIIIIIIIII1AA                                                                                  \n");
    printf("                                                                                                                                                 ARWNNNNNNNNNNNNNNNNNNNNNNNNIININNNNNNNNW1RNAA                                                   ANWNIIIIIIIIIIIIIIIIIRNA                                                                                   \n");
    printf("                                                                                                                                                  AN1NNNNNNNNNNNNNNNNNNNNNNNNNNNIIIIIW16RAAE                                                   AANRIIIIIIIIIIIIIIIIINRIA                                                                                    \n");
    printf("                                                                                                                                                   ARWNNNNNNNNNNNNNNNNNNNNNNNNNNNNN16RAAA                                                     AI1WINIIIIIIIINIIIIIIIWIA                                                                                     \n");
    printf("                                                                                                                                                    AR1RNNNNNNNNNNNNNNNNNNNNNNNNW1RAAA                                                      AANWNIINIINNIIIIIIIIIIN1NA                                                                                      \n");
    printf("                                                                                                                                                     AA6WNNINININININININNINNNIW6AA                                                       AAR1WIIIIINIINIINNIIIIIW1AA                                                                                       \n");
    printf("                                                                                                                                                      AAR1NNNNNNNNNNNNNNNNNNNNNWAAA                                                    AAAR1WIIINIIINIIINIINIINNRRAA                                                                                        \n");
    printf("                                                                                                                                                        AR61NNNNNNNNNNNNNNNNNNNW16RAA                                                AANR1RNNNNNNIINNIIIIIINIIW1RA                                                                                           \n");
    printf("                                                                                                                                                         AAR1NNNNNNNNNNNNNNNNNNNNN16REAAA                             v          mAAEN1WIINININNNNNNIIINIIIIIWRAA                               v                                                            \n");
    printf("                                                                                                                                                          NAR61NNNNNNNNNNNNNNNNNNNNNN11WRAAAA                                AAAANR11INNNNNNNIINNINNINNNIINW1RAA                                                                                             \n");
    printf("                                                                                                                                                            AAR61NNNNNNNNNNNNNNNNNNNNNNNW166RIAAAAA                    IAAAAANW61WNNNNNNIIININNNIINNINNNNW1RAA                                                                                               \n");
    printf("                                                                                                                                                              AAR61NNNNNNNNNNNNNNNNNNNNNNNNNN1666RNAAAAAAAAAAAAAAAAAAAAIR661WNNNNNNNNNNNNNNNININNNNININW1RAA                                                                                                 \n");
    printf("                                                                                                                                                                AAR61NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN16666666666666661WNNNNNNNINNNNNNNNNNNNNNNNNIINNNNW1RAA               v                                                                                  \n");
    printf("                                                                                                                                                                  AANR61NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNINNNNNNNNNNNNNNNNNW1RIAA                                                                                                     \n");
    printf("                                                                                                                                                                     AAR61WNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNINNNNNNNNNNNR16RAA                                                                                                        \n");
    printf("                                                                                                                                                                       AAAR061NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNINNNNNW16RAAA                                                                                                          \n");
    printf("                                                                                                                                                                          AAARR61NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNW1RRAAA                                                                                                             \n");
    printf("                                                           v                                                                                                                 AAAIR066WNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNW166REAAA                                                                                                                \n");
    printf("                                                                                                                                                                                 AAAANR0061RNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN1666RIAAAA                                                                                                                    \n");
    printf("                                                                                                                                                                                      AAAAAER0000661RNNNNNNNNNNNNNNNNR1166666REAAAAA                                                                                                                         \n");
    printf("                                                                                                                                                                                            AAAAAAAAAAAIRW000000WREAAAAAAAAAAA                                                                                                                               \n");
    printf("                                                                                                                                                                                                       0AAAAAAAAAA0                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                                                                                                                             \n");
    printf("                                                                                                                                                                                                                                                                                                                                                             \n");
    printf("                                                                                                         AAAA        dAAAARAAAAA    AAAAAWAAAAA   AAAAAAAAAA    AAAA  AAA      AAAA  AAAA             AAAAAAAAAA      AAAAAAAAAA    AAAA      AA    AAAAAAAAAA      AAAAA AAAAA       AAAA        AAAAAAAAAAA       AAAA  AAAA                               \n");
    printf("                                                                                                        AA AA       AA             AA             AA           AA             AA        AAA           AA       AAA    AA            AA AA     AA    AA            AAA                 AA AA       AA        AA     AA        AA                              \n");
    printf("                                                                                                       AA   AA     AAA            AAA             AA           AAA           AA          AA           AA        AAA   AA            AA  AA    AA    AA            AA                 AA  AAA      AA         AA   AA          AA                             \n");
    printf("                                                                                                       AA    AA    AA             AA              AAAAAAAAAA     AAAAAAA     AA          AA           AA         AA   AAAAAAAAAA    AA   AA   AA    AAAAAAAAAA   AAA                AA    AA      AA         AA   AA          AA                             \n");
    printf("                                                                                                      AA     AAr   AA             AA              AA                   AAA   AA          AA           AA        1AA   AA            AA    AA  AA    AA           AAA         AA    AA      AA     AA         AA   AA          AA                             \n");
    printf("                                                                                                     AAAAAAAAAAA    AA             AA             AA                    AA   AAA         AA           AA        AA    AA            AA     AA AA    AA            AA         AA    AAAAAAAAAAA    AA        AAA   AAA        AAA                             \n");
    printf("                                                                                                    AA0        AA    AAN      AA    AAi      AA   AA           A       AAA    AAA      AAA            AA      AAA     AA            AA      AAAA    AA             AAA       AA   AA        AAE   AA      AAA      AAA      AAA                              \n");
    printf("                                                                                                    AA          AA     AAAAAAA        AAAAAAA     AAAAAAAAAA    AAAAAAAA         AAAAAAm              AAAAAAAA        AAAAAAAAAAA   AA       AAA    AAAAAAAAAAA      AAAAAAAA    AA          AA   AAAAAAAA            AAAAAA                                 \n");
    printf("                                                                                                                                                                                                                                                                                                                                                             \n");
    printf("                                                                                                                                                                                                                                                                                                                                           \n");
    printf("                                                                                                                                                                                                                                                                                                                                                                                                        \n");
    printf("                                                                                                                                                                                                                                                                                                                                                             \n");
    printf("                                                                                                                                                                                                                                                                                                                                                             \n");
    printf("                                                                                                                                                                                                                                                                                                                                                             \n");
    printf("                                                                                                                                                                                                                                                                                                                                                             \n");
    printf("                                                                                                                                                                                                                                                                                                                                                             \n");
}
void Grafico_MenuCliente(void) {
    printf("                                                                                                                                                                                                                                                                                                \n");
    printf("                                                                                                                                                                                                                                                                                                \n");
    printf("                                                                                                                                                                                                                                                                                                \n");
    printf("                                                                                                                                                                                                                                                                                                \n");
    printf("                                                                                                                                                                                                                                                                                                \n");
    printf("                        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  @@@@      @@@@   @@@@@@@   @@@:     @@   @@@     @@@       @@@@@@*      .@@@@@@@  +@@               @@@@@   #@@       @@@   @@@@@@@@  @@@@     @@@ +@@@@@@@@@  @@@@@@@  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                        \n");
    printf("                    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@      @@@@     @@@@@   @@@====   @@@@@    @@   @@@     @@@       @@@==@@@@@   .@@=====  +@@             @@@* =@@  #@@       @@@   @@@=====  @@@@@    @@@ .***@@@***  @@@====      @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                    \n");
    printf("                @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@          @@ @@    @@@@@   @@@       @@ @@@   @@   @@@     @@@       @@@     @@@  .@@       +@@            @@@        #@@       @@@   @@@       @@@@@@   @@@     @@@     @@@              @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                \n");
    printf("             -@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@            @@@ @@  @@ @@@   @@@@@@@   @@  @@@  @@   @@@     @@@       @@@     @@@  .@@@@@@   +@@           .@@         #@@       @@@   @@@@@@@   @@@ @@@  @@@     @@@     @@@@@@@            @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@=             \n");
    printf("                 *@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#        @@@ @@ @@# @@@   @@@       @@   @@@ @@   @@@     @@@       @@@     @@@  .@@       +@@            @@#        #@@       @@@   @@@       @@@  -@@ @@@     @@@     @@@            #@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*                 \n");
    printf("                     *@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@+    @@@  @@@@  @@@   @@@       @@    @@@@@    @@@   @@@        @@@   @@@@   .@@       +@@            @@@@    @  #@@       @@@   @@@       @@@    @@@@@     @@@     @@@        +@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*                     \n");
    printf("                                                             @@@  @@@   @@@   @@@@@@@   @@     @@@@     @@@@@@-         @@@@@@@      .@@@@@@@  +@@@@@@@         @@@@@@@  #@@@@@@@  @@@   @@@@@@@@  @@@     @@@@     @@@     @@@@@@@                                                              \n");
    printf("                                                                                                                                                                                                                                                                                                \n");
    printf("                                                                                                                                                                                                                                                                                                \n");
    printf("                                                                                                                                                                                                                                                                                                \n");
    printf("                                                                                                                                                                                                                        .=:                                                                     \n");
    printf("                                                                                                                                                                                                                     :@@@@@@@@@@@@*                                                             \n");
    printf("                                                                                                                                                                                                                     @@@@@@@@@@@@@@@@@@.                                                        \n");
    printf("                                                                                                                                                                                                                    @@@@@@@       #@@@@@@@@                                                     \n");
    printf("                                                                                                                                                                                                                   @@@@@@@            @@@@@@@@                                                  \n");
    printf("                                                                                                                                                                                                                  .@@@@@@                #@@@@@@.                                               \n");
    printf("                          +@@@           @@@@@@@.                                                                                    @@@                                                                          @@@@@@@                   @@@@@@#                                             \n");
    printf("                        @@@@@@          @@@          @@@-     @@ .@@=  :@@*    @@  @@@    @@  =@@   @@@@     @@  @@       @@  @@=    @@@  @@@@@@@ @@@@@@@@   .@@@@@                                              @@@@@@@@@@@@@@               #@@@@@@                                           \n");
    printf("                           @@@         @@@         @@@ *@@@   @@@+.@@@@*.@@@   @@@@ @@@@  @@@@@@.  @#  @@@   @@@@@@       @@@* @@@   @@@  ----@@@ :---*@@:   @  :@@@                                            *@@@@@@  @@@@@@@@@*             .@@@@@@                                       \n");
    printf("                           @@@         @@@        @@@    @@   @@    @@    @@   @@@   @@@  @@@       @@@@@@   @@@          @@.   @@@  @@@    =@@      @@@      @@@@@@                                            @@@@@@       @@@@@@@@*             @@@@@#                                       \n");
    printf("                           @@@         @@@        @@@    @@   @@    @@    @@   @@@   @@@  @@@     @@@   @@   @@@          @@+   @@@  @@@   @@@      @@#    @@@   @@@                                           @@@@@@@@@@@@@    @@@@@@@@            *@@@@@                                      \n");
    printf("                           @@@    @@@   @@@@@@@@   @@@@@@@@   @@    @@    @@   @@@@@@@@   @@@     @@@@@@@@   @@@          @@@@@@@@   @@@  @@@@@@@ @@@@@@@@ @@@@@@@@@                                          @@@@@@@@@@@@@@@@@@@  @@@@@@@:           @@@@@@                                    \n");
    printf("                                   .                                           @@@                                        @@                                                                                 .@@@@@@#@@@@@@@@@@@@@@@ @@@@@@@=           @@@@#:                                  \n");
    printf("                                                                               @@@                                        @@                                                                                 @@@@@@@@@@@@@@@@@@@@@@#   @@@@@@@.          @@@@@@                                \n");
    printf("                                                                               ##*                                        ##                                                                                @@@@@@@@@@@@@@@@@@@@@@@      @@@@@@@           @@@@@                               \n");
    printf("                                                                                                                                                                                                           *@@@@@@@@@@@@+   @@@@@-    #@@@@@@@@@@@          @@@@#                              \n");
    printf("                                                                                                                                                                                                           @@@@@@ :@@@@@@            @@@@@@@@@@@@#@          @@@@@:                             \n");
    printf("                                                                                                                                                                                                          @@@@@@* =@@@@@@@@@@@@@   @@@@@+     @@@@ ##         @@@@                              \n");
    printf("                                                                                                                                                                                                          @@@@@@  @@@@@@@@@@@@   @@@@@#        @@@ ##         @@@@#                            \n");
    printf("                                                                                                                                                                                                         @@@@@@@@@@@@@@@@@@@@@@ .@@@          @@@@#@         @@@@#                           \n");
    printf("                                                                                                                                                                                                        @@@@@@@@@@@@ @@@@@@@@@@*  @@@###          @@@@#@        .@@@@*                          \n");
    printf("                                                                                                                                                                                                        @@@@@@      @@@@@@@@@@    @@@            @@@@@@##        @@@@@                          \n");
    printf("                          @@@@@@@       @@@     @@@             @@                                                                                                                                    *@@@@@@     @@@@@@@@@@@@     @@@@@@@@@* *@@@@@@@@@#*       -@@@@                          \n");
    printf("                          @*   @@@       @@     @@              @@                                                                                                                                    @@@@@@ #@@@@@@@@@@@@@          @@@@@@@@@@@@@=  @@@@@      .@@@@@                          \n");
    printf("                               @@-       @@@   @@@   @@@@@@@@   @@  @@@   @@@  @@@@@@@    @@@@@@                                                                                                     @@@@@@@@@@@@@@@@@@@@    @@@@@@@@@:@@@@@@@-     *@@@@@@@@@@@@@@@                           \n");
    printf("                             @@@          @@# @@@   @@@    @@   @@   @@@  @@   @@@   @@@  @@-                                                                                                        @@@@@@@@@@@@   @@@#@@ #@@@@@@@@@@@@             #@@@@@@@@@@@@@                             \n");
    printf("                           @@@             @@ @@    @@@    @@   @@    @@ @@@   @@@@@@@@@  @@                                                                                                        @@@@@@ @@@@@@  @@@@@@@@@@@@  @@@@@@@      .@@@@@@@@@@@@@#                                 \n");
    printf("                         @@@@@@@@@  @@*    @@@@@    @@@    @@   @@    .@@@@    @@@        @@                                                                                                       @@@@@@@@@@@@@@@@@@@@@@ @@@@@@   .@@@@@ @@@@@@@@@@@@@@                                        \n");
    printf("                         @@@@@@@@@  @@      @@@      @@@@@@@@   @@     #@@      @@@@@@    @@                                                                                                       @@@@@@@@@@@@@@@@@@@#   @@@@@@@@=@@@@@@@@@@@@@@@                                              \n");
    printf("                                                                                                                                                                                                 +@@@@@@@@@@@@@@@@@        @@@@@@@@@@@@@@@@@                                                    \n");
    printf("                                                                                                                                                                                                 @@@@@@@@@@@@@@          @@@@@@@@@@@@@+                                                         \n");
    printf("                                                                                                                                                                                                @@@@@@@@@@@@@@     =@@@@@@@@@@@@#                                                                \n");
    printf("                                                                                                                                                                                               #@@@@@@  @@    @@@@@@@@@@@@@                                                                     \n");
    printf("                                                                                                                                                                                               @@@@@@#  #@@@@@@@@@@@@                                                                            \n");
    printf("                                                                                                                                                                                              .@@@@@@@@@@@@@@@@:                                                                                \n");
    printf("                                                                                                                                                                                               #@@@@@@@@@*                                                                                      \n");
    printf("                                                                                                                                                                                                  =+                                                                                            \n");
    printf("                          @@                                                                                                                                         @@@       @@-                                               \n");
    printf("                          @@                                                                                                                                                                             \n");
    printf("                          @@   @@@@@@@@@   @@@@@@@@  @@ @@@   @@@@@@   @@@@@@@   @@@@@@@       @@@    @@   @@@@@@@@@   @@@@@@@         @@@@@@@   @@@@@@@@    @@@@@@  -@@   +@@@@@@@   @@@@@@@@.                            \n");
    printf("                          @@   @@@   -@@  @@@   @@   @@@     @@    @@  @@@.     @@@   @@@      @@@    @@   @@@   @@@        @@@       @@+   @@@  @@@   @@@  @@@      -@@  :@@    @@@  @@@   @@@                            \n");
    printf("                          @@   @@@    @@   @@@@@@@   @@@    #@@@@@@@@    @@@@@  @@@@@@@@@      @@@    @@   @@@   @@@   @@@@+@@@       @@    @@@  @@@   *@@  @@+      -@@  @@@    @@@  @@@   @@@                            \n");
    printf("                          @@   @@@    @@   @@        @@@     @@@            @@  @@@            -@@.  @@@   @@@   @@@  @@@   @@@       @@@   @@@  @@@   @@@  @@@      -@@   @@@  #@@   @@@   @@@                            \n");
    printf("                          @@   @@@    @@   @@@@@@@@  @@@      :@@@@@@  @@@@@@     @@@@@@        @@@@@@@@   @@@   @@@   @@@@@@@         @@@@@     @@@@@@@*    @@@@@@  =@@    #@@@@@    @@@   @@@                            \n");
    printf("                                          @@     @@                                                                                              @@@                                                    \n");
    printf("                                          -@@@@@@@                                                                                               @@@                                                    \n");
    printf("                                                                                                                                                                                                                                                                                                \n");
}
void Grafico_MenuEmpleado(void) {
    printf("                     @@@@@@@    @@@@@@@ @@@@@@@@@@@@@ @@@@@     *@@@@ #@@@@     +@@@@.      @@@@@@@@@@@-    @@@@@@@@@@@@: @@@@             -@@@@@@@@@@@@#@@@@@@@:    @@@@@@ #@@@@@@@@@@@  *@@@@        @@@@@@@@@@@@@     @@@@@@     @@@@@@@@@@@=     .@@@@@@@@@@\n");
    printf("                     @@@@@@@-  #@@@@@@@ @@@@@@@@@@@@@ @@@@@@@   *@@@@ #@@@@     +@@@@.      @@@@@@@@@@@@@+  @@@@@@@@@@@@: @@@@             -@@@@@@@@@@@@#@@@@@@@@   @@@@@@@ #@@@@@@@@@@@@ *@@@@        @@@@@@@@@@@@@    @@@@@@@@    @@@@@@@@@@@@@*  @@@@@@@@@@@@@@\n");
    printf("                     @@@@@@@@  @@@@@@@@ @@@@@         @@@@@@@@= *@@@@ #@@@@     +@@@@.      @@@@@    @@@@@  @@@@          @@@@             -@@@@-        @@@@@@@@- @@@@@@@@ #@@@@   -@@@@+*@@@@        @@@@@            @@@*@@@@*   @@@@@    #@@@@-@@@@@      @@@@@\n");
    printf("                     @@@@-@@@+#@@#@@@@@ @@@@@@@@@@@@  @@@@@@@@@@@@@@@ #@@@@     +@@@@.      @@@@@    :@@@@* @@@@@@@@@@@   @@@@             -@@@@@@@@@@@- @@@@ @@@@ @@@=@@@@ #@@@@@@@@@@@@ *@@@@        @@@@@@@@@@@@   =@@@@  @@@@:  @@@@@     @@@@@@@@@@      @@@@@\n");
    printf("                     @@@@ @@@@@@@ @@@@@ @@@@@******+  @@@@:-@@@@@@@@@ #@@@@     +@@@@       @@@@@    =@@@@- @@@@#******   @@@@             -@@@@@******  @@@@@-@@@@@@*+@@@@ #@@@@@@@@@@@  *@@@@        @@@@@#*****+   @@@@.  #@@@@  @@@@@    :@@@@#@@@@@      @@@@@\n");
    printf("                     @@@@. @@@@@+ @@@@@ @@@@@........ @@@@:  *@@@@@@@ .@@@@@.   @@@@@       @@@@@..=@@@@@@  @@@@:.......  @@@@@@@@@@@#     -@@@@*........@@@@@ @@@@@@ *@@@@ #@@@@         *@@@@@@@@@@  @@@@@@@@@@@:. @@@@@@@@@@@@@@ @@@@@..-@@@@@@ -@@@@@.  -@@@@@.\n");
    printf("                     @@@@. @@@@@  @@@@@ @@@@@@@@@@@@@ @@@@:    @@@@@@   @@@@@@@@@@@         @@@@@@@@@@@@-   @@@@@@@@@@@@+ @@@@@@@@@@@@     -@@@@@@@@@@@@@@@@@@ .@@@@- *@@@@ #@@@@         *@@@@@@@@@@@ @@@@@@@@@@@@@@@@@@     .@@@@#@@@@@@@@@@@@=    @@@@@@@@@@@@\n");
    printf("                                                                             +**:                                                                                                                                                                         =**-\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("             #@*      @@     @@                                                                           @@  @@                                      @@#  @@       @@-\n");
    printf("           @@@@*      @@*    @*                                                                           @@                                               @@       @@-\n");
    printf("             @@*       @@.  @@  @@@-#@@  @@@++    .@@@-@@@*-@@:  @@=-@@= @@@@@+ .@@-+@@  @@=-@=     +@@-=@@@  @@  @@=-@+  @@@-*@@  .@@=-@@*  @@@-*@@   @=  @@@-#@@  @@-  @@@-#@@ =@@-@@\n");
    printf("             @@*        @@ @@  .@@@@     @@       .@@  =@@  *@* @@@@@@@  +@@@#   .*@@@@  +@@@#      @@    @@  @@  +@@@#   @@   -@  @@    @@  @@   @@   @=  @@   #@* @@- .@@@@@@@  @@@@:\n");
    printf("             @@*    @   #@@@=   @@   @   @@       .@@  =@   *@* +@@      +.  @@  @:  @@  #   @@.    @@*  #@@  @@  +.  @@. @@   @@- *@@  -@@  @@   @@   @=  @@   @@: @@-  @@   @   @  :@@\n");
    printf("           @@@@@@@ .@:   @@@     .@@@=   @@       .@@  -@#  +@+   +@@@.   @@@@   +@@- @@  @@@@       :@@@:@@  @@   @@@@   @@:@@@     #@@@    @@   @@  #@-  @ :@@     @@   :@@@=   -@@@:\n");
    printf("                                                                                                                          @@\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("           @@@@@@@      @@@:    @@@                                                                                                                                                                                     *@*                           @@  *@+  @@\n");
    printf("                @@      @@@@    @@@   @@@@@   @@-@@. *@@@@-   @@@@   @@+@@     @@*@@@+@@@@:   @@@@@   @@@@@  :@@@@@       #@@@@-  -@@@@%   @@#@@@-@@@@    @@@@@        @@@@@    @@@@@: +@=  :@#  @@*@@@#   @@@@@    @@@@@@*  *@@@@-       @@@@@       @@  *@+  @@#@@@+  +@+@@+ -@@@@+\n");
    printf("              @@@       @@ @@ :@:@@      .@@  @@+   @@          .@@  @@*       @@.  @@   @@  @@ +-@@ +@:         @@:     @@      @@+   @@  @@   @@   @@  @@   @@:    :@@   @@  @@   .  +@=  :@#  @@   =@@     :@@  @@   @@*      @@     :@@   @@      @@  *@+  @@   @@* +@@   @@=++-@*\n");
    printf("           +@@          @@ .@+@@ @@  *@#-+@@  @@    @@      #@@  @@  @@.       @@   @@   @@ .@@         :@@+   @@@@@:    @@      @@-   @@  @@   @@   @@ .@@   #@=    =@@   @@- @@      +@=  -@#  @@   .@@ *@#  @@  @@   #@* =@@  @@     -@@   @@.     @@  *@+  @@   *@# +@=   @@-\n");
    printf("           @@@@@@@ @@@  @@  @@@  @@   @@+@@@@ @@     @@@@@@  @@+@@@@ @@.       @@   @@   @@  #@@@@@# @@@@@@: @@*#@@@:    .@@@@@@  @@@@@@:  @@   @@   @@  +@@@@@@      #@@@@@#  :@@@@@@  @@@@@@#  @@@@@@@  #@@+@@@@ #@@@@@@* +@@+@@@@     #@@@@@#      @@@ *@+  @@@@@@@  +@=    @@@@@@\n");
    printf("                                                                                                                                                                                                 @@\n");
    printf("                                                                                                                                                                                                 @@\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("            -#@#=       -**     .**                                                                    @@  @@=       @@                                                            @@  =@%\n");
    printf("           +@  .@@      @@@@    @@@-                                                                   @@            @@                                                            @@              .@@\n");
    printf("               @@-      @@+@=  @@@@- .@@@@@@  *@@@@  @@@@@@ =@@@@@@   @@@@     @@@@@@@+  @@@@@@=  @@@@@@@  #@:  @@@@@@@  :@@@@@@      =@@@@@@  @@@@@@-  @@@@@@@@@@@@  @@@@@@@      @@  :@* -@@@@@ @@@@@@ .@@@@@@:\n");
    printf("             :-+@@      @@ @@ =@ @@-   *@@@@  *@-   @@        *#@@@   @+       @@:   @@ #@@@@@@@ @@.   @@  #@:  @.   @@  @@    @@     @@      @@    @@  @@   @@  .@@ =@@   #@+     @@  :@*  @@@#:  .@@   @@    @@\n");
    printf("           @@   @@:     @@  @@@# @@-  @-  @@  *@:   @@-  .   @.  @@   @=       @@#  :@@ :@@   :  #@#   @@  #@: +@@  .@@  @@.  =@@     @@.  #= *@*   @@  @@   @@  .@@  @@   @@      @@  :@*  .  :@@ .@@   @@=  -@@\n");
    printf("            @@@@@   @@  @@   @@  @@-  @@@@-@@ *@:     @@@@   @@@#+@@  @=       @@+@@@#    @@@@@   :@@@-@@  #@:  :@@@=@@   :@@@@        =@@@@    @@@@=   @@   @@  .@@   @@@@@       @@@ :@*  @@@@@   *@@@  .@@@@:\n");
    printf("                                                                               @@.\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("                                                                                   ..\n");
    printf("              @@@*     @@    :@@                           =@+                     @@    \n");
    printf("             @@=@*     -@@   @@    @@@@   @@+@@     @@@@# @@@@@@  +@@@@.    @@@@=  @@  @@.\n");
    printf("           =@* =@*      @@+  @+  @@   @@  @@       @@      #@+   @@   #@@ #@@    -  @@+@@\n");
    printf("          :@@@@@@@@      @@ @@   @@.....  @@         *@@@  #@+   @@   :@@ @@-      @@@@+\n");
    printf("               =@*  @@@  -@@@    @@@@@@@  @@       @@@@@@  =@@@@ +@@@@@@   @@@@@@. @@ -@@\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("            ******      *=     +*           @@\n");
    printf("           -@-          @@*   =@@           @@\n");
    printf("           +@:@@@.       @@   @@   @@@@@@=  @@  @@:  :@  @@@@@@  @@@@@\n");
    printf("                @@=      -@@ @@-   @+   @@  @@   @@  @@  @@@@@@@- @@.\n");
    printf("           -@    @=  .     @@@    *@    @@  @@   .@@@@   @@.      @@\n");   
    printf("            @@@@@-  @@+    @@@     :@@@@@   +@@   =@@:    #@@@@#  @@\n");
    printf("\n");
}
void Grafico_OpcionIncorrecta(void) {
    printf("    																								                                                                                                                                                                                                                                                                                            \n");
    printf("    																								                                                                                                                                                                                                                                                                                               \n");
    printf("    																								                                                                                                                                                                                                                                                                                               \n");
    printf("    																								                                                                                                                                                                                                                                                                                               \n");
    printf("    																								                                                                                                                                                                                                                                                                                               \n");
    printf("    																								                                                                                                                                                                                                                                                                                               \n");
    printf("    																								                                                                                                                                                                                                                                                                                               \n");
    printf("    																								                                                                                                                                                                                                                                                                                               \n");
    printf("    																								                                                                                                                                                                                                                                                                                               \n");
    printf("    																								                                                                                                                                                                                                                                                                                               \n");
    printf("    																								                                                                                                                                                                                                                                                                                               \n");
    printf("    																								                                                                                                                                                                                                                                                                                               \n");
    printf("    																								                                                                                                                                                                                                                                                                                               \n");
    printf("    																								                                                                                                                                                                                                                                                                                               \n");
    printf("    																								                                                                                                                                                                                                                                                                                               \n");
    printf("    																								                                                                                                                                                                                                                                                                                               \n");
    printf("    																								                                                                                                                                                                                                                                                                                               \n");
    printf("    																								                                                                                                                                                                                                                                                                                               \n");
    printf("    																								                                                                                                                                                      RRRRRRRRRRR                                                                                                                              \n");
    printf("    																								                                                                                                                                                   RRRRRRRRRRRRRRRR                                                                                                                            \n");
    printf("    																								                                                                                                                                                RRRRRRRRRRRRRRRRRNRN                                                                                                                           \n");
    printf("    																								                                                                                                  RRRRr                                       RRRRRRRRRRRRRRRRIAAAAAW                                                                                                                          \n");
    printf("    																								                                                                                                RRRRRRRRRRRRR                              6RRRRRRRRRRRRRRRIAAAAAA                                                                                                                             \n");
    printf("    																								                                                                                               dRRRRRRRRRRRRRRRRv                        RRRRRRRRRRRRRRRRAEAAAA                                                                                                                                \n");
    printf("    																								                                                                                               RRRRRRRRRRRRRRRRRRRRv                   RRRRRRRRRRRRRRRNEAAAA                                                                                                                                   \n");
    printf("    																								                                                                                               RRRRRRRRRRRRRRRRRRRRRRR               RRRRRRRRRRRRRRRIEAAAA                                                                                                                                     \n");
    printf("    																								                                                                                               0RRRRRRRRRRRRRRRRRRRRRRRRi          RRRRRRRRRRRRRRREEAAA0                                                                                                                                       \n");
    printf("    																								                                                                                                RRRRRRRRRRRRRRRRRRRRRRRRRRR      RRRRRRRRRRRRRRREEAAA                                                                                                                                          \n");
    printf("    																								                                                                                                 RRRRRRRRRRRRRRRRRRRRRRRRRRRR  RRRRRRRRRRRRRRRIEAAAv                                                                                                                                           \n");
    printf("    																								                                                                                                  RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRIIAEAm                                                                                                                                             \n");
    printf("    																								                                                                                                     ARRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRNEEEAR                                                                                                                                               \n");
    printf("    																								                                                                                                        ERRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRREEEEE                                                                                                                                                 \n");
    printf("    																								                                                                                                          WNRRRRRRRRRRRRRRRRRRRRRRRRRRREIEEA                                                                                                                                                   \n");
    printf("    																								                                                                                                             ERRRRRRRRRRRRRRRRRRRRRRRIIIEA                                                                                                                                                     \n");
    printf("    																								                                                                                                                ERRRRRRRRRRRRRRRRRRRRRRRRI1                                                                                                                                                   \n");
    printf("    																								                                                                                                                  NRRRRRRRRRRRRRRRRRRRRRRRRR1                                                                                                                                                  \n");
    printf("    																								                                                                                                                 RRRRRRRRRRRRRRRRRRRRRRRRRRRRR6                                                                                                                                                \n");
    printf("    																								                                                                                                               RRRRRRRRRRRRRRRRRRRRRRRRRRRRRWRRRi                                                                                                                                              \n");
    printf("    																								                                                                                                             RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR6RRR                                                                                                                                             \n");
    printf("    																								                                                                                                           rRRRRRRRRRRRRRRRIEANRRRRRRRRRRRRRRRR1WRRN                                                                                                                                           \n");
    printf("    																								                                                                                                          RRRRRRRRRRRRRRRREAAA0 ARRRRRRRRRRRRRRRRRRRRN                                                                                                                                         \n");
    printf("    																								                                                                                                        RRRRRRRRRRRRRRRRIIAAA     ERRRRRRRRRRRRRRRRRRRRm                                                                                                                                       \n");
    printf("                                                                                                      																								     RRRRRRRRRRRRRRRREAAAd        6RRRRRRRRRRRRRRRRRRRN                                                                                                                                      \n");
    printf("                                                                                                      																								   1RRRRRRRRRRRRRRRIIAAA            iNRRRRRRRRRRRRRRRRRR6                                                                                                                                    \n");
    printf("    																								                                                                                                    RRRRRRRRRRRRRRRREEAA1               iRRRRRRRRRRRRRRRRRRN                                                                                                                                   \n");
    printf("    																								                                                                                                   RRRRRRRRRRRRRRRRIAAA                   WRRRRRRRRRRRRRRRRRN                                                                                                                                  \n");
    printf("    																								                                                                                                   RRRRRRRRRRRRRRIIAAA                      IRRRRRRRRRRRRRRRRN                                                                                                                                 \n");
    printf("    																								                                                                                                  1RRRRRRRRRRRRREEAAI                         ERRRRRRRRRRRRRRRN                                                                                                                                \n");
    printf("    																								                                                                                                   RRRRRRRRRRRRIEEAv                            IRRRRRRRRRRRNRRN                                                                                                                               \n");
    printf("    																								                                                                                                   RRRRRRRRRRRIEEA                               vNRRRRRRRRRRRNNR                                                                                                                              \n");
    printf("    																								                                                                                                     RRRRRRRRIEEA                                  IRRRRRRRRRNRNI                                                                                                                              \n");
    printf("    																								                                                                                                      NRRRRRIEEA                                     IRRRRRRRRNNI                                                                                                                              \n");
    printf("    																								                                                                                                        6RRIEEA                                       rRRRRRRNNE                                                                                                                               \n");
    printf("    																								                                                                                                           v                                            ERRRIE                                                                                                                                 \n");
    printf("    																								                                                                                                                                                                                                                                                                                               \n");
    printf("    																								                                                                                                                                                                                                                                                                                               \n");
    printf("    																								                                                                                                                                                                                                                                                                                               \n");
    printf("    																								                                                                                                                                                                                                                                                                                               \n");
    printf("    																								                                                                                                                                                                                                                                                                                               \n");
    printf("    																								                                                                                                                                                                                                                                                                                               \n");
    printf("    																								                                                                                                                                                                                                                                                                                               \n");
    printf("    																								                                                                                       AA                                                                                                                                                                                                      \n");
    printf("    																								                                                                                     Ai                                                                                                                                                                                                        \n");
    printf("    																								                                         AAAAAAr     AAAAAA       AAAAAA   AA     mAAAAAA      AAv     AA        AA    AAr     AA     rAAAAAE     AAAAAAE     AAAAAAR     AAAAAA     AAAAAAA6    mAAAAAI AAAAAAAAAAv    dAA                                                                    \n");
    printf("    																								                                       AA      AA    AR    AA   AA      d  AA    AA      AA    AvAW    AA        AA    AvAR    AA    AA         AAv     AAW   AA    AA   vAm    AA   AA         AA           1A         AvAA                                                                   \n");
    printf("    																								                                      AA        AA   AR    AA  AA          AA   AA        AE   Av AE   AA        AA    Av AA   AA   AA         dAv       AA   AA    AA   vAm    AA   AA        AA            1A        AA vAm                                                                  \n");
    printf("    																								                                      AA        AA   AR  vAA   AA          AA   AA        AA   Av  AA  AA        AA    Av  AA  AA   AA         AA        IA   AAAAAAA    vAAAAAA     AAAAAAA   AA            1A       EA   EA                                                                  \n");
    printf("    																								                                      AA        AA   AR        AA          AA   AA        AA   Av   AA AA        AA    Av   AA AA   AA         EA        AA   AA   AA    vAm  rAA    AA        AA            1A       AR6616AA                                                                 \n");
    printf("    																								                                      IA6      AA    AR        iAA         AA   AA       AA    Av    AAAA        AA    Av    AAAA   IAi         AA       AE   AA    AA   vAm   vAW   AA        EAv           1A      AA      AI                                                                \n");
    printf("    																								                                        AAAAAAAA     AR          AAAAAAAR  AA    rAAAAAAAA     Av     AAA        AA    Av     AAA     AAAAAAAv   AAAAAAAA     AA     AA  vAm    IAv  AAAAAAAN    AAAAAAA     1A     AA       EAv                                                               \n");
    printf("    																								                                                                                                                                                                                                                                                                                               \n");
    printf("    																								                                                                                                                                                                                                                                                                                               \n");
    printf("    																								                                                                                                                                                                                                                                                                                               \n");
    printf("    																								                                                                                                                                                                                                                                                                                               \n");
    printf("    																								                                                                                                                                                                                                                                                                                               \n");
    printf("    																								                                                                                                                                                                                                                                                                                               \n");
    printf("    																								                                                                                                                                                                                                                                                                                               \n");
    printf("    																								                                                                                                                                                                                                                                                                                               \n");
    printf("    																								                                                                                                                                                                                                                                                                                               \n");
    printf("                                                                                                      																								                                                                                                                                                                                             \n");
    printf("                                                                                                      																								                                                                                                                                                                                             \n");
    printf("                                                                                                      																								                                                                                                                                                                                             \n");
    printf("                                                                                                      																								                                                                                                                                                                                             \n");
    printf("                                                                                                      																								                                                                                                                                                                                             \n");
    printf("                                                                                                      																								                                                                                                                                                                                             \n");
    printf("                                                                                                      																								                                                                                                                                                                                             \n");
    printf("                                                                                                      																								                                                                                                                                                                                             \n");
    printf("                                                                                                      																								                                                                                                                                                                                             \n");
    printf("                                                                                                      																								                                                                                                                                                                                             \n");
    printf("                                                                                                      																								                                                                                                                                                                                             \n");
    printf("                                                                                                      																								                                                                                                                                                                                             \n");
    printf("                                                                                                      																								                                                                                                                                                                                             \n");
    printf("                                                                                                      																								                                                                                                                                                                                             \n");
}
void Grafico_Volviendo(void) {
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                 ::                                                                                                                                                                                                    \n");
    printf("              @@@           @@@                  @@                   @@@                                             @@.                                                                                                                  \n");
    printf("               @@@         @@@                   @@                                                                   @@.                                                                                                                  \n");
    printf("                @@@       %@@      @@@@@@=       @@   @@@       =@@   @@        @@@@@*      @@@  @@@@@         @@@@@  @@.       @@@@@@                                                                                                   \n");
    printf("                 @@#      @@    :@@@     @@@     @@    @@+      @@    @@     @@@     @@@    @@@@@    @@@     @@@    @@@@.    @@@     .@@@                                                                                               \n");
    printf("                  @@     @@     @@        @@@    @@     @@     @@.    @@    @@@       @@#   @@@       @@    @@@       @@.   @@@        @@@                                                                                             \n");
    printf("                  #@@   @@*    @@@        @@@    @@     *@@   @@@     @@    @@@@@@@@@@@@    @@@       @@    @@*       @@.   @@@        @@@                                                                                             \n");
    printf("                   @@@ @@@      @@        @@@    @@      @@@ @@@      @@    @@@             @@@       @@    @@@       @@.   @@@        @@                                                                                              \n");
    printf("                    @@@@@        @@@    @@@#     @@       @@@@@       @@     @@@@     @@    @@@       @@     @@@.  +@@@@.    @@@@    @@@     *@@@    @@@=    @@@     @@@                                                         \n");
    printf("                     .+.            @@@@         ==        .+:        -=         @@@@        +.       ==        @@@    =         @@@#         =+      *=      #.      %                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
    printf("                                                                                                                                                                                                                                                          \n");
}
