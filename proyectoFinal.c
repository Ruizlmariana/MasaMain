#include <ctype.h>   // toupper()
#include "utiles.h" // (incluye stdio, stdlib, string, time)

// PROGRAMA QUE SIMULA LA GESTIÓN DE UNA PIZZERIA

#define MAX_PIZZAS 10       // cantidad maxima de sabores que puede haber a la vez
#define PIZZAS_INICIALES 5  // sabores que ya vienen cargados (PZ1 a PZ5)
#define MAX_MESAS 10        // cuantas mesas tiene el local
#define MAX_PEDIDOS 10      // cuantos pedidos pueden estar registrados a la vez
#define MAX_NOMBRE 50       // largo del nombre de una pizza
#define MAX_CODIGO 10       // largo de un codigo (se genera solo: PZ + numero)
#define MAX_ESTADO 20       // largo del estado de una mesa o de un pedido
#define MAX_USUARIO 20      // largo del usuario
#define MAX_CLAVE 20        // largo de la contraseña

// colores de la consola (codigos ANSI). RESET vuelve al color normal
#define ROJO     "\033[91m"
#define VERDE    "\033[92m"
#define AMARILLO "\033[93m"
#define MAGENTA  "\033[95m"
#define CIAN     "\033[96m"
#define RESET    "\033[0m"

// =============== ESTRUCTURAS ========================

typedef struct {
    char codigo[MAX_CODIGO];  // PZ1, PZ2, ...
    char nombre[MAX_NOMBRE];  // si esta vacio, ese lugar del array esta libre
    int stock;                // unidades disponibles
    float precio;             // precio de una unidad
} Pizza;

typedef struct {
    char estado[MAX_ESTADO];  // LIBRE u OCUPADA
    int numero;
} Mesa;

typedef struct {
    char estado[MAX_ESTADO];  // LIBRE, PENDIENTE o LISTO
    int numero;
    Pizza pizza;              // la pizza que se pidio
    int cantidad;             // unidades pedidas
} Pedido;

typedef struct {
    Pizza pizzas[MAX_PIZZAS];
    Mesa mesas[MAX_MESAS];
    Pedido pedidos[MAX_PEDIDOS];
    float ganancias_totales;
    int cantidad_pizzas;      // sabores cargados hasta ahora
} Pizzeria;

// ======================== PROTOTIPOS DE FUNCIONES ========================

// inicializacion
void inicializarPizzeria(Pizzeria *pizzeria);
void inicializarPizzas(Pizzeria *pizzeria);
void cargarPizza(Pizza *pizza, char codigo[], char nombre[], float precio, int stock);

// menus
int mostrarMenuPrincipal();
void menuGerente(Pizzeria *pizzeria);
void menuEmpleado(Pizzeria *pizzeria);
void menuCliente(Pizzeria *pizzeria);

// login
int login(char perfil[]);
void pedirCredenciales(char usuario[], char clave[]);
int credencialesValidas(char perfil[], char usuario[], char clave[]);

// gerente
void verStockActual(Pizzeria *pizzeria);
void mostrarStock(Pizzeria *pizzeria, int mostrarPrecio);
void agregarNuevoSabor(Pizzeria *pizzeria);
void verGanancias(Pizzeria *pizzeria);

// empleado
void verMesas(Pizzeria *pizzeria);
void marcarMesa(Pizzeria *pizzeria);
void marcarPedidoListo(Pizzeria *pizzeria);
int mostrarPedidosPendientes(Pizzeria *pizzeria);
void verStockEmpleado(Pizzeria *pizzeria);

// cliente (compra)
void comprarPizza(Pizzeria *pizzeria);
int mostrarPizzasDisponibles(Pizzeria *pizzeria);
int buscarPizzaPorCodigo(Pizzeria *pizzeria, char codigo[]);
int pedirCantidad(Pizza *pizza);
float jugarPorDescuento();
float calcularPrecioFinal(float precio, int cantidad, float descuento);
int buscarPedidoLibre(Pizzeria *pizzeria);
int registrarPedido(Pizzeria *pizzeria, int indice_pizza, int cantidad);
void registrarVenta(Pizzeria *pizzeria, int indice_pizza, int cantidad, float total);
void mostrarResumenCompra(float total, float descuento, int numero_pedido);

// juegos
void juegoAdivinarNumero(float *descuento); // elige el número secreto del 1 al 10
void juegoSumaRapida(float *descuento);     // elige los dos números que hay que sumar

// utilitarias
void convertirMayusculas(char str[]);

///////////////////////////////////////////////////////////////////////////////////
//---------------------------------GRAFICOS---------------------------------------

void Grafico_Menu(void);
void Grafico_MostrarLogo(void);
void Grafico_MenuGerente(void);
void Grafico_Usuario(void);
void Grafico_Contrasena(void);
void Grafico_AccesoDenegado(void);
void Grafico_MenuCliente(void);
void Grafico_MenuEmpleado(void);
void Grafico_OpcionIncorrecta(void);
void Grafico_Volviendo(void);

// ======================== FUNCIONES PRINCIPALES ========================

int main() {
    Pizzeria pizzeria;
    int opcion;

    utf8();                       // muestra bien las tildes y la ñ
    activarColoresANSI();         // permite usar colores en la consola de Windows
    srand((unsigned)time(NULL));  // para que los números de los juegos cambien en cada ejecución

    inicializarPizzeria(&pizzeria);
    Grafico_Menu();               // pantalla de inicio
    pausa();

    // menu principal: se repite hasta que se elige salir (4)
    do {
        opcion = mostrarMenuPrincipal();

        switch(opcion) {
            case 1:
                if (login("GERENTE")) {
                    menuGerente(&pizzeria);
                }
                break;
            case 2:
                if (login("EMPLEADO")) {
                    menuEmpleado(&pizzeria);
                }
                break;
            case 3:
                menuCliente(&pizzeria);  // el cliente no necesita login
                break;
            case 4:
                printf(AMARILLO "Gracias por visitar Pizzeria MASA MAIN" RESET "\n");
                break;
            default:
                Grafico_OpcionIncorrecta();
                pausa();
        }
    } while (opcion != 4);

    return 0;
}

// ======================== INICIALIZACION ========================

// deja la pizzeria lista para empezar: pizzas cargadas, mesas y pedidos libres y ganancias en 0
void inicializarPizzeria(Pizzeria *pizzeria) {
    inicializarPizzas(pizzeria);

    // todas las mesas arrancan libres
    for (int i = 0; i < MAX_MESAS; i++) {
        (*pizzeria).mesas[i].numero = i + 1;  // las mesas se numeran desde 1
        strcpy((*pizzeria).mesas[i].estado, "LIBRE");
    }

    // todos los pedidos arrancan libres y sin pizza
    for (int i = 0; i < MAX_PEDIDOS; i++) {
        (*pizzeria).pedidos[i].numero = i + 1;
        strcpy((*pizzeria).pedidos[i].estado, "LIBRE");
        cargarPizza(&(*pizzeria).pedidos[i].pizza, "", "", 0, 0);
        (*pizzeria).pedidos[i].cantidad = 0;
    }

    (*pizzeria).ganancias_totales = 0.0;
    (*pizzeria).cantidad_pizzas = PIZZAS_INICIALES;
}

// carga los 5 sabores iniciales y deja vacios los lugares que sobran
void inicializarPizzas(Pizzeria *pizzeria) {
    cargarPizza(&(*pizzeria).pizzas[0], "PZ1", "Muzarella", 4500, 10);
    cargarPizza(&(*pizzeria).pizzas[1], "PZ2", "Napolitana", 5000, 10);
    cargarPizza(&(*pizzeria).pizzas[2], "PZ3", "Fugazzeta", 4800, 10);
    cargarPizza(&(*pizzeria).pizzas[3], "PZ4", "Jamon y Morron", 5200, 10);
    cargarPizza(&(*pizzeria).pizzas[4], "PZ5", "Calabresa", 5300, 10);

    // un nombre vacio significa que ese lugar esta libre para un sabor nuevo
    for (int i = PIZZAS_INICIALES; i < MAX_PIZZAS; i++) {
        cargarPizza(&(*pizzeria).pizzas[i], "", "", 0, 0);
    }
}

// completa todos los datos de una pizza
void cargarPizza(Pizza *pizza, char codigo[], char nombre[], float precio, int stock) {
    strcpy((*pizza).codigo, codigo);
    strcpy((*pizza).nombre, nombre);
    (*pizza).precio = precio;
    (*pizza).stock = stock;
}

// ======================== MENUS ========================

// muestra el logo y devuelve la opcion elegida
int mostrarMenuPrincipal() {
    int opcion;

    limpiaPantalla();
    Grafico_MostrarLogo();
    opcion = leerEntero("");

    return opcion;
}

void menuGerente(Pizzeria *pizzeria) {
    int opcion;

    do {
        limpiaPantalla();
        Grafico_MenuGerente();  // ver stock, agregar sabor, ver ganancias
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
            case 4:
                Grafico_Volviendo();
                pausa();
                break;
            default:
                Grafico_OpcionIncorrecta();
                pausa();
        }
    } while (opcion != 4);
}

void menuEmpleado(Pizzeria *pizzeria) {
    int opcion;

    do {
        limpiaPantalla();
        Grafico_MenuEmpleado();  // ver mesas, marcar mesa, marcar pedido listo, ver stock
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
                Grafico_Volviendo();
                pausa();
                break;
            default:
                Grafico_OpcionIncorrecta();
                pausa();
        }
    } while (opcion != 5);
}

void menuCliente(Pizzeria *pizzeria) {
    int opcion;

    do {
        limpiaPantalla();
        Grafico_MenuCliente();  // comprar pizza o volver
        opcion = leerEntero("");

        switch(opcion) {
            case 1:
                comprarPizza(pizzeria);
                break;
            case 2:
                Grafico_Volviendo();
                pausa();
                break;
            default:
                Grafico_OpcionIncorrecta();
                pausa();
        }
    } while (opcion != 2);
}

// ======================== LOGIN ========================

// pide usuario y clave hasta que sean correctos
// devuelve 1 si el acceso es correcto y 0 si se cancela (escribiendo 1)
int login(char perfil[]) {
    char usuario[MAX_USUARIO];
    char clave[MAX_CLAVE];

    do {
        pedirCredenciales(usuario, clave);

        // con un 1 se cancela y se vuelve al menu principal
        if (sonIguales(usuario, "1") || sonIguales(clave, "1")) {
            return 0;
        }

        if (credencialesValidas(perfil, usuario, clave)) {
            printf(VERDE "Login exitoso como %s!" RESET "\n", perfil);
            pausa();
            return 1;
        }

        Grafico_AccesoDenegado();
        pausa();
    } while (1);
}

// muestra las pantallas de login y guarda lo que se escribe en usuario y clave
void pedirCredenciales(char usuario[], char clave[]) {
    limpiaPantalla();
    Grafico_Usuario();
    leerCadena(usuario, MAX_USUARIO, "");
    convertirMayusculas(usuario);  // el usuario se compara siempre en mayusculas

    if (sonIguales(usuario, "1")) {
        strcpy(clave, "");  // si cancela no hace falta pedir la clave
    } else {
        limpiaPantalla();
        Grafico_Contrasena();
        leerCadena(clave, MAX_CLAVE, "");  // la clave se deja tal cual se escribio
    }
}

// cada perfil tiene su propio usuario y la clave es la misma
int credencialesValidas(char perfil[], char usuario[], char clave[]) {
    char usuario_correcto[MAX_USUARIO];
    char clave_correcta[] = "1234";

    if (sonIguales(perfil, "GERENTE")) {
        strcpy(usuario_correcto, "ADMIN");
    } else {
        strcpy(usuario_correcto, "EMPLEADO");
    }

    return sonIguales(usuario, usuario_correcto) && sonIguales(clave, clave_correcta);
}

// ======================== GERENTE ========================

void verStockActual(Pizzeria *pizzeria) {
    limpiaPantalla();
    printf(AMARILLO "===== STOCK ACTUAL =====" RESET "\n");
    mostrarStock(pizzeria, 1);  // el gerente ve tambien el precio
    pausa();
}

// recorre las pizzas y muestra las que tienen nombre
// si mostrarPrecio vale 1 tambien muestra el precio
void mostrarStock(Pizzeria *pizzeria, int mostrarPrecio) {
    int hayPizzas = 0;

    for (int i = 0; i < MAX_PIZZAS; i++) {
        if (strlen((*pizzeria).pizzas[i].nombre) > 0) {
            printf("%s - %s", (*pizzeria).pizzas[i].codigo, (*pizzeria).pizzas[i].nombre);
            if (mostrarPrecio) {
                printf(" - $%.2f", (*pizzeria).pizzas[i].precio);
            }
            printf(" (Stock: %d)\n", (*pizzeria).pizzas[i].stock);
            hayPizzas = 1;
        }
    }

    if (hayPizzas == 0) {
        printf(ROJO "No hay sabores cargados." RESET "\n");
    }
}

// carga un sabor nuevo en el primer lugar libre
void agregarNuevoSabor(Pizzeria *pizzeria) {
    // los sabores se cargan uno atras del otro, asi que el lugar libre es cantidad_pizzas
    int i = (*pizzeria).cantidad_pizzas;

    if (i >= MAX_PIZZAS) {
        printf(ROJO "No hay espacio para nuevos sabores." RESET "\n");
        pausa();
        return;
    }

    printf("Espacios disponibles: %d\n", MAX_PIZZAS - i);

    // el nombre no puede quedar vacio
    do {
        leerCadena((*pizzeria).pizzas[i].nombre, MAX_NOMBRE, "Nombre del nuevo sabor: ");
        if (strlen((*pizzeria).pizzas[i].nombre) == 0) {
            printf(ROJO "El nombre no puede estar vacio." RESET "\n");
        }
    } while (strlen((*pizzeria).pizzas[i].nombre) == 0);

    // el precio tiene que ser mayor a 0
    do {
        leerFlotante("Precio: ", &(*pizzeria).pizzas[i].precio);
        if ((*pizzeria).pizzas[i].precio <= 0) {
            printf(ROJO "El precio debe ser mayor a 0." RESET "\n");
        }
    } while ((*pizzeria).pizzas[i].precio <= 0);

    (*pizzeria).pizzas[i].stock = pedirEnteroRango("Stock inicial: ", 0, 1000);

    // el codigo se arma solo: PZ + posicion (la posicion 5 es PZ6)
    // snprintf es como sprintf pero con un limite de tamaño, asi no se pasa del array
    snprintf((*pizzeria).pizzas[i].codigo, MAX_CODIGO, "PZ%d", i + 1);

    (*pizzeria).cantidad_pizzas++;
    printf(VERDE "Sabor agregado exitosamente!" RESET "\n");
    pausa();
}

void verGanancias(Pizzeria *pizzeria) {
    limpiaPantalla();
    printf(AMARILLO "===== GANANCIAS ACTUALES =====" RESET "\n");
    printf("Total generado: $%.2f\n", (*pizzeria).ganancias_totales);
    pausa();
}

// ======================== EMPLEADO ========================

void verMesas(Pizzeria *pizzeria) {
    limpiaPantalla();
    printf(AMARILLO "===== MESAS DISPONIBLES =====" RESET "\n");

    for (int i = 0; i < MAX_MESAS; i++) {
        // libre en verde y ocupada en rojo
        if (sonIguales((*pizzeria).mesas[i].estado, "LIBRE")) {
            printf("Mesa %d: " VERDE "%s" RESET "\n",
                   (*pizzeria).mesas[i].numero,
                   (*pizzeria).mesas[i].estado);
        } else {
            printf("Mesa %d: " ROJO "%s" RESET "\n",
                   (*pizzeria).mesas[i].numero,
                   (*pizzeria).mesas[i].estado);
        }
    }

    pausa();
}

// cambia el estado de una mesa a ocupada o libre
void marcarMesa(Pizzeria *pizzeria) {
    int numero_mesa;

    char mensaje[50];
    sprintf(mensaje, "Numero de mesa (1-%d): ", MAX_MESAS);
    numero_mesa = pedirEnteroRango(mensaje, 1, MAX_MESAS);

    int opcion = pedirEnteroRango("1. Ocupada | 2. Libre: ", 1, 2);

    // la mesa 1 esta en la posicion 0 del array, por eso el - 1
    if (opcion == 1) {
        strcpy((*pizzeria).mesas[numero_mesa - 1].estado, "OCUPADA");
    } else {
        strcpy((*pizzeria).mesas[numero_mesa - 1].estado, "LIBRE");
    }

    printf(VERDE "Mesa %d actualizada." RESET "\n", numero_mesa);
    pausa();
}

// muestra los pedidos pendientes y deja marcar uno como listo
void marcarPedidoListo(Pizzeria *pizzeria) {
    limpiaPantalla();
    printf(AMARILLO "===== PEDIDOS PENDIENTES =====" RESET "\n");

    // si no hay ninguno pendiente no hay nada para marcar
    if (mostrarPedidosPendientes(pizzeria) == 0) {
        printf(ROJO "No hay pedidos pendientes." RESET "\n");
        pausa();
        return;
    }

    char mensaje[50];
    sprintf(mensaje, "\nNumero de pedido (1-%d): ", MAX_PEDIDOS);
    int numero_pedido = pedirEnteroRango(mensaje, 1, MAX_PEDIDOS);

    // solo se puede marcar como listo un pedido que este pendiente
    if (sonIguales((*pizzeria).pedidos[numero_pedido - 1].estado, "PENDIENTE")) {
        strcpy((*pizzeria).pedidos[numero_pedido - 1].estado, "LISTO");
        printf(VERDE "Pedido %d marcado como listo." RESET "\n", numero_pedido);
    } else {
        printf(ROJO "El pedido %d no esta pendiente." RESET "\n", numero_pedido);
    }

    pausa();
}

// muestra los pedidos en estado PENDIENTE y devuelve cuantos hay
int mostrarPedidosPendientes(Pizzeria *pizzeria) {
    int cantidad = 0;

    for (int i = 0; i < MAX_PEDIDOS; i++) {
        if (sonIguales((*pizzeria).pedidos[i].estado, "PENDIENTE")) {
            printf("Pedido %d: %d x %s\n",
                   (*pizzeria).pedidos[i].numero,
                   (*pizzeria).pedidos[i].cantidad,
                   (*pizzeria).pedidos[i].pizza.nombre);
            cantidad++;
        }
    }

    return cantidad;
}

void verStockEmpleado(Pizzeria *pizzeria) {
    limpiaPantalla();
    printf(AMARILLO "===== STOCK ACTUAL =====" RESET "\n");
    mostrarStock(pizzeria, 0);  // el empleado no ve el precio
    pausa();
}

// ======================== CLIENTE (COMPRA) ========================

// recorre todo el proceso de una compra, paso por paso
void comprarPizza(Pizzeria *pizzeria) {
    char codigo_buscado[MAX_CODIGO];

    limpiaPantalla();
    printf(AMARILLO "=== COMPRA DE PIZZA ===" RESET "\n");

    // si todos los pedidos estan ocupados no se puede tomar otro
    if (buscarPedidoLibre(pizzeria) == -1) {
        printf(ROJO "No podemos tomar mas pedidos por el momento." RESET "\n");
        pausa();
        return;
    }

    if (mostrarPizzasDisponibles(pizzeria) == 0) {
        printf(ROJO "No hay pizzas disponibles." RESET "\n");
        pausa();
        return;
    }

    leerCadena(codigo_buscado, MAX_CODIGO, "\nIngrese codigo de la pizza: ");
    convertirMayusculas(codigo_buscado);  // asi pz1 y PZ1 son lo mismo

    int indice = buscarPizzaPorCodigo(pizzeria, codigo_buscado);

    if (indice == -1) {
        printf(ROJO "Codigo no encontrado." RESET "\n");
        pausa();
        return;
    }

    if ((*pizzeria).pizzas[indice].stock == 0) {
        printf(ROJO "Sin stock disponible." RESET "\n");
        pausa();
        return;
    }

    int cantidad = pedirCantidad(&(*pizzeria).pizzas[indice]);
    float descuento = jugarPorDescuento();
    float total = calcularPrecioFinal((*pizzeria).pizzas[indice].precio, cantidad, descuento);

    int numero_pedido = registrarPedido(pizzeria, indice, cantidad);
    registrarVenta(pizzeria, indice, cantidad, total);
    mostrarResumenCompra(total, descuento, numero_pedido);

    pausa();
}

// muestra las pizzas que tienen nombre y stock, y devuelve cuantas mostro
int mostrarPizzasDisponibles(Pizzeria *pizzeria) {
    int cantidad = 0;

    for (int i = 0; i < MAX_PIZZAS; i++) {
        if (strlen((*pizzeria).pizzas[i].nombre) > 0 && (*pizzeria).pizzas[i].stock > 0) {
            printf("%s - %s: $%.2f (Stock: %d)\n",
                   (*pizzeria).pizzas[i].codigo,
                   (*pizzeria).pizzas[i].nombre,
                   (*pizzeria).pizzas[i].precio,
                   (*pizzeria).pizzas[i].stock);
            cantidad++;
        }
    }

    return cantidad;
}

// devuelve la posicion de la pizza con ese codigo, o -1 si no existe
int buscarPizzaPorCodigo(Pizzeria *pizzeria, char codigo[]) {
    // un codigo vacio coincidiria con los lugares libres, asi que se descarta
    if (strlen(codigo) == 0) {
        return -1;
    }

    for (int i = 0; i < MAX_PIZZAS; i++) {
        if (sonIguales((*pizzeria).pizzas[i].codigo, codigo)) {
            return i;
        }
    }

    return -1;
}

// pregunta cuantas unidades quiere, entre 1 y el stock que hay
int pedirCantidad(Pizza *pizza) {
    char mensaje[100];

    sprintf(mensaje, "Cuantas unidades de '%s' desea? (1-%d): ", (*pizza).nombre, (*pizza).stock);
    return pedirEnteroRango(mensaje, 1, (*pizza).stock);
}

// ofrece jugar para ganar descuento y devuelve el descuento ganado (0 si no juega o pierde)
float jugarPorDescuento() {
    float descuento = 0.0;
    char respuesta[10];

    leerCadena(respuesta, sizeof(respuesta), "Jugar para ganar descuento? (S/N): ");

    if (toupper(respuesta[0]) == 'S') {
        printf("Elija juego:\n");
        printf("1) Adivinar numero (20%% descuento)\n");
        printf("2) Suma rapida (10%% descuento)\n");

        int opcion_juego = leerEntero("");

        // los juegos reciben la direccion de descuento para poder modificarlo
        switch(opcion_juego) {
            case 1:
                juegoAdivinarNumero(&descuento);
                break;
            case 2:
                juegoSumaRapida(&descuento);
                break;
            default:
                printf(ROJO "Opcion invalida. Sin descuento." RESET "\n");
        }
    }

    return descuento;
}

// precio por cantidad, menos el descuento (0.20 es un 20%)
float calcularPrecioFinal(float precio, int cantidad, float descuento) {
    float precio_bruto = precio * cantidad;

    return precio_bruto * (1 - descuento);
}

// devuelve la posicion de un pedido que se pueda usar, o -1 si estan todos ocupados
// un pedido LISTO ya se entrego, asi que su lugar se puede volver a usar
int buscarPedidoLibre(Pizzeria *pizzeria) {
    for (int i = 0; i < MAX_PEDIDOS; i++) {
        if (sonIguales((*pizzeria).pedidos[i].estado, "LIBRE") ||
            sonIguales((*pizzeria).pedidos[i].estado, "LISTO")) {
            return i;
        }
    }

    return -1;
}

// guarda el pedido en un lugar libre (comprarPizza ya comprobo que hay) y devuelve su numero
int registrarPedido(Pizzeria *pizzeria, int indice_pizza, int cantidad) {
    int i = buscarPedidoLibre(pizzeria);

    strcpy((*pizzeria).pedidos[i].estado, "PENDIENTE");
    (*pizzeria).pedidos[i].pizza = (*pizzeria).pizzas[indice_pizza];  // copia de la pizza pedida
    (*pizzeria).pedidos[i].cantidad = cantidad;

    return (*pizzeria).pedidos[i].numero;
}

// descuenta las unidades vendidas del stock y suma el total a las ganancias
void registrarVenta(Pizzeria *pizzeria, int indice_pizza, int cantidad, float total) {
    (*pizzeria).pizzas[indice_pizza].stock -= cantidad;
    (*pizzeria).ganancias_totales += total;
}

// muestra el resultado de la compra
void mostrarResumenCompra(float total, float descuento, int numero_pedido) {
    retardo(0.5);
    printf(VERDE "\nPedido realizado con exito!" RESET "\n");
    printf("Total a pagar: " AMARILLO "$%.2f" RESET, total);

    if (descuento > 0) {
        printf(" (descuento: %.0f%%)", descuento * 100);
    }

    printf("\nSu pedido es el numero %d y estara listo en breve!\n", numero_pedido);
}

// ======================== JUEGOS ========================

// el cliente tiene que adivinar un numero del 1 al 10 (gana 20%)
void juegoAdivinarNumero(float *descuento) {
    int secreto = generarNumAleatorio(1, 10);
    int intento;

    intento = pedirEnteroRango("\n Adivina el numero (1-10): ", 1, 10);

    if (intento == secreto) {
        *descuento = 0.20;
        printf(VERDE "Correcto! Ganaste 20%% de descuento!" RESET "\n");
    } else {
        printf(ROJO "Incorrecto. El numero era %d. No hay descuento." RESET "\n", secreto);
    }
}

// el cliente tiene que resolver una suma de dos numeros del 1 al 10 (gana 10%)
void juegoSumaRapida(float *descuento) {
    int a = generarNumAleatorio(1, 10);
    int b = generarNumAleatorio(1, 10);
    int resp;

    char mensaje[50];
    sprintf(mensaje, "\nCuanto es %d + %d?: ", a, b); // sprintf es un printf que guarda el resultado en una variable en lugar de mostrarlo
    resp = leerEntero(mensaje);

    if (resp == a + b) {
        *descuento = 0.10;
        printf(VERDE "Bien! Ganaste 10%% de descuento!" RESET "\n");
    } else {
        printf(ROJO "Mal. La respuesta era %d. No hay descuento." RESET "\n", a + b);
    }
}

// ======================== FUNCIONES UTILITARIAS ========================

// pasa un texto a mayusculas, letra por letra, hasta llegar al '\0'
void convertirMayusculas(char str[]) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

///////////////////////////////////////////////////////////////////////////
void Grafico_Menu(void) {
    printf(AMARILLO);  // color de la pantalla
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
    printf(RESET);
}
void Grafico_MostrarLogo(void) {
    printf(ROJO);  // color de la pantalla
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
    printf(RESET);
}

void Grafico_MenuGerente(void) {
    printf(MAGENTA);  // color de la pantalla
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
    printf(RESET);
}
void Grafico_Usuario(void) {
    printf(CIAN);  // color de la pantalla
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
    printf(RESET);
}

void Grafico_Contrasena(void) {
    printf(CIAN);  // color de la pantalla


    
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
    printf(RESET);
}
void Grafico_AccesoDenegado(void) {
    printf(ROJO);  // color de la pantalla
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
    printf(RESET);
}
void Grafico_MenuCliente(void) {
    printf(VERDE);  // color de la pantalla
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
    printf(RESET);
}
void Grafico_MenuEmpleado(void) {
    printf(CIAN);  // color de la pantalla
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
    printf("                @@      @@@@    @@@   @@@@@   @@-@@. *@@@@-   @@@@   @@+@@     @@*@@@+@@@@:   @@@@@   @@@@@  :@@@@@       #@@@@-  -@@@@@   @@#@@@-@@@@@   @@@@@        @@@@@    @@@@@: +@=  :@#  @@*@@@#   @@@@@    @@@@@@*  *@@@@-       @@@@@       @@  *@+  @@#@@@+  +@+@@+ -@@@@+\n");
    printf("              @@@       @@ @@ :@:@@      .@@  @@+   @@          .@@  @@*       @@.  @@   @@  @@ +-@@ +@:         @@:     @@      @@+   @@  @@   @@   @@  @@   @@:    :@@   @@  @@   .  +@=  :@#  @@   =@@     :@@  @@   @@*      @@     :@@   @@      @@  *@+  @@   @@* +@@   @@=++-@*\n");
    printf("           +@@          @@ .@+@@ @@  *@#-+@@  @@    @@      #@@  @@  @@.       @@   @@   @@ .@@         :@@+   @@@@@:    @@      @@-   @@  @@   @@   @@ .@@   #@=    =@@   @@- @@      +@=  -@#  @@   .@@ *@#  @@  @@   #@* =@@  @@     -@@   @@.     @@  *@+  @@   *@# +@=   @@-\n");
    printf("           @@@@@@@ @@@  @@  @@@  @@   @@+@@@@ @@     @@@@@@  @@+@@@@ @@.       @@   @@   @@  #@@@@@# @@@@@@: @@*#@@@:    .@@@@@@  @@@@@@:  @@   @@   @@  +@@@@@@      #@@@@@#  :@@@@@@  @@@@@@#  @@@@@@@  #@@+@@@@ #@@@@@@* +@@+@@@@     #@@@@@#      @@@ *@+  @@@@@@@  +@=    @@@@@@\n");
    printf("                                                                                                                                                                                                 @@\n");
    printf("                                                                                                                                                                                                 @@\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("            -#@#=       -**     .**                                                                    @@  @@=       @@                                                            @@  =@%%\n");
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
    printf(RESET);
}
void Grafico_OpcionIncorrecta(void) {
    printf(ROJO);  // color de la pantalla
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
    printf(RESET);
}
void Grafico_Volviendo(void) {
    printf(AMARILLO);  // color de la pantalla
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
    printf("                @@@       %%@@      @@@@@@=       @@   @@@       =@@   @@        @@@@@*      @@@  @@@@@         @@@@@  @@.       @@@@@@                                                                                                   \n");
    printf("                 @@#      @@    :@@@     @@@     @@    @@+      @@    @@     @@@     @@@    @@@@@    @@@     @@@    @@@@.    @@@     .@@@                                                                                               \n");
    printf("                  @@     @@     @@        @@@    @@     @@     @@.    @@    @@@       @@#   @@@       @@    @@@       @@.   @@@        @@@                                                                                             \n");
    printf("                  #@@   @@*    @@@        @@@    @@     *@@   @@@     @@    @@@@@@@@@@@@    @@@       @@    @@*       @@.   @@@        @@@                                                                                             \n");
    printf("                   @@@ @@@      @@        @@@    @@      @@@ @@@      @@    @@@             @@@       @@    @@@       @@.   @@@        @@                                                                                              \n");
    printf("                    @@@@@        @@@    @@@#     @@       @@@@@       @@     @@@@     @@    @@@       @@     @@@.  +@@@@.    @@@@    @@@     *@@@    @@@=    @@@     @@@                                                         \n");
    printf("                     .+.            @@@@         ==        .+:        -=         @@@@        +.       ==        @@@    =         @@@#         =+      *=      #.      %%                                                          \n");
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
    printf(RESET);
}
