#include <ctype.h>   // toupper()
#include "utiles.h" // (incluye stdio, stdlib, string, time)

// PROGRAMA QUE SIMULA LA GESTIÓN DE UNA PIZZERIA
// Las pizzas se elaboran en el local con los ingredientes que se compran

#define MAX_PIZZAS 10       // cantidad maxima de sabores que puede haber a la vez
#define MAX_MESAS 10        // cuantas mesas tiene el local
#define MAX_PEDIDOS 10      // cuantos pedidos pueden estar registrados a la vez
#define MAX_USUARIOS 20     // cuantos usuarios (gerentes y empleados) se pueden crear
#define MAX_CLIENTES 50     // cuantos clientes se pueden registrar
#define MAX_NOMBRE 50       // largo de un nombre (pizza o cliente)
#define MAX_CODIGO 10       // largo de un codigo (se genera solo: PZ + numero)
#define MAX_ESTADO 20       // largo del estado de una mesa o de un pedido
#define MAX_USUARIO 20      // largo del nombre de usuario
#define MAX_CLAVE 20        // largo de la contraseña
#define MAX_PERFIL 15       // largo del perfil (GERENTE o EMPLEADO)
#define MAX_TURNO 10        // largo del turno (MANANA, TARDE o NOCHE)
#define MAX_DNI 12          // largo del DNI
#define MAX_INGREDIENTES 20 // cuantos ingredientes se pueden cargar
#define MAX_RECETA 5        // cuantos ingredientes puede llevar una pizza
#define MAX_UNIDAD 10       // largo de la unidad de un ingrediente (g, ml, u)
#define MAX_LINEA 300       // largo de una linea del archivo de datos
#define MAX_EXTRAS 3        // ingredientes extra distintos que se pueden agregar en una compra
#define MAX_UNIDADES_EXTRA 3  // cuantos extras de un mismo ingrediente por pizza
#define MAX_TEXTO_EXTRAS 200  // largo del texto que describe los extras en la factura
#define TIEMPO_LIMITE_SUMA 10 // segundos que tiene el cliente para resolver la suma rapida

#define DESCUENTO_ADIVINAR 0.20  // descuento si se gana el juego de adivinar
#define DESCUENTO_SUMA 0.10      // descuento si se gana la suma rapida

// nombres de los archivos binarios
#define ARCHIVO_USUARIOS "usuarios.dat"
#define ARCHIVO_PIZZAS "pizzas.dat"
#define ARCHIVO_CLIENTES "clientes.dat"
#define ARCHIVO_PEDIDOS "pedidos.dat"
#define ARCHIVO_FACTURAS "facturas.dat"
#define ARCHIVO_COMPRAS "compras.dat"
#define ARCHIVO_INGREDIENTES "ingredientes.dat"
#define ARCHIVO_DATOS "../datos (1).txt"  // archivo de texto desde donde se importan los datos

// colores de la consola (codigos ANSI). RESET vuelve al color normal
#define ROJO     "\033[91m"
#define VERDE    "\033[92m"
#define AMARILLO "\033[93m"
#define MAGENTA  "\033[95m"
#define CIAN     "\033[96m"
#define RESET    "\033[0m"

// ======================== ESTRUCTURAS ========================

typedef struct {
    char nombre[MAX_NOMBRE];
    char unidad[MAX_UNIDAD];  // g, ml, u...
    int stock;                // cantidad disponible, en esa unidad
    float precio_extra;       // lo que cuesta agregarlo como extra a una pizza (0 = no se vende como extra)
    int porcion_extra;        // cuanto se usa del ingrediente por cada extra
} Ingrediente;

typedef struct {
    int indice_ingrediente;   // posicion del ingrediente en el array ingredientes
    int cantidad;             // cuanto lleva UNA pizza de ese ingrediente
} ItemReceta;

typedef struct {
    char nombre[MAX_NOMBRE];  // nombre del ingrediente extra
    int cantidad;             // cuantos extras lleva cada pizza
    float precio;             // precio de cada extra
} Extra;

typedef struct {
    char codigo[MAX_CODIGO];  // PZ1, PZ2, ...
    char nombre[MAX_NOMBRE];  // si esta vacio, ese lugar del array esta libre
    int stock;                // unidades disponibles
    float precio;             // precio de una unidad
    ItemReceta receta[MAX_RECETA];  // ingredientes que lleva la pizza
    int cantidad_items;             // cuantos ingredientes tiene la receta
} Pizza;

typedef struct {
    char estado[MAX_ESTADO];  // LIBRE u OCUPADA
    int numero;
} Mesa;

typedef struct {
    char usuario[MAX_USUARIO]; // nombre de usuario
    char clave[MAX_CLAVE];
    char perfil[MAX_PERFIL];  // GERENTE o EMPLEADO
    char turno[MAX_TURNO];    // MANANA, TARDE o NOCHE
} Usuario;

typedef struct {
    int id;                   // numero de cliente (1, 2, 3...)
    char nombre[MAX_NOMBRE];
    char dni[MAX_DNI];        // es lo que identifica al cliente
} Cliente;

typedef struct {
    char estado[MAX_ESTADO];  // LIBRE, PENDIENTE o LISTO
    int numero;               // numero de pedido, no se repite nunca
    Pizza pizza;              // copia de la pizza que se pidio
    int cantidad;             // unidades pedidas
    int id_cliente;           // a que cliente pertenece
    Extra extras[MAX_EXTRAS]; // ingredientes extra que pidio el cliente
    int cantidad_extras;      // cuantos extras distintos pidio
} Pedido;

typedef struct {
    int numero;               // coincide con el numero de pedido
    time_t fecha;             // cuando se hizo la compra
    int id_cliente;
    char pizza[MAX_NOMBRE];
    int cantidad;
    float precio_unitario;
    char extras[MAX_TEXTO_EXTRAS];  // texto con los extras pedidos (vacio si no hubo)
    float precio_extras;      // lo que suman los extras por cada pizza
    float descuento;          // 0.20 es un 20%
    float total;
    char empleado[MAX_USUARIO];  // empleado de turno que tomo el pedido
    char turno[MAX_TURNO];       // turno de ese empleado
} Factura;

typedef struct {
    int numero;
    time_t fecha;
    char ingrediente[MAX_NOMBRE];
    char unidad[MAX_UNIDAD];
    int cantidad;             // cuanto se compro, en esa unidad
    float costo_total;        // lo que costo la compra
    char gerente[MAX_USUARIO];  // gerente que hizo la compra
} Compra;

typedef struct {
    Pizza pizzas[MAX_PIZZAS];
    Mesa mesas[MAX_MESAS];
    Pedido pedidos[MAX_PEDIDOS];
    Usuario usuarios[MAX_USUARIOS]; // gerentes y empleados
    Cliente clientes[MAX_CLIENTES];
    Ingrediente ingredientes[MAX_INGREDIENTES]; 
    int cantidad_pizzas;      // sabores cargados hasta ahora
    int cantidad_usuarios; // cuantos gerentes y empleados hay
    int cantidad_clientes;
    int cantidad_ingredientes;
    int proximo_pedido;       // numero que va a tener el proximo pedido
    char empleado_turno[MAX_USUARIO];  // empleado que inicio sesion ultimo: es el de turno
    char turno_actual[MAX_TURNO];      // turno de ese empleado
} Pizzeria;

// ======================== PROTOTIPOS DE FUNCIONES ========================

// inicializacion
void inicializarPizzeria(Pizzeria *pizzeria);
void inicializarPizzas(Pizzeria *pizzeria);
void inicializarUsuarios(Pizzeria *pizzeria);
void inicializarIngredientes(Pizzeria *pizzeria);
void cargarPizza(Pizza *pizza, char codigo[], char nombre[], float precio, int stock);
void cargarUsuario(Usuario *usuario, char nombre[], char clave[], char perfil[], char turno[]);

// archivos binarios
int guardarArreglo(char archivo[], void *datos, size_t tamano, int cantidad);
int cargarArreglo(char archivo[], void *datos, size_t tamano, int maximo);
int agregarRegistro(char archivo[], void *registro, size_t tamano);
int contarRegistros(char archivo[], size_t tamano);
void cargarDatos(Pizzeria *pizzeria);
void guardarPizzas(Pizzeria *pizzeria);
void guardarUsuarios(Pizzeria *pizzeria);
void guardarClientes(Pizzeria *pizzeria);
void guardarPedidos(Pizzeria *pizzeria);
void guardarIngredientes(Pizzeria *pizzeria);

// menus (devuelven 1 si se eligio salir del programa y 0 si solo se vuelve)
int mostrarMenuPrincipal();
int menuGerente(Pizzeria *pizzeria, char usuario[]);
int menuEmpleado(Pizzeria *pizzeria);
int menuCliente(Pizzeria *pizzeria);

// login y usuarios
int login(Pizzeria *pizzeria, char perfil[], char usuario[]);
void pedirCredenciales(char usuario[], char clave[]);
int credencialesValidas(Pizzeria *pizzeria, char perfil[], char usuario[], char clave[]);
int buscarUsuario(Pizzeria *pizzeria, char usuario[]);
void iniciarTurno(Pizzeria *pizzeria, char usuario[]);
void crearUsuario(Pizzeria *pizzeria);
void eliminarUsuario(Pizzeria *pizzeria, char usuario_actual[]);
void pedirNombreUsuario(Pizzeria *pizzeria, char usuario[]);
void pedirClaveNueva(char clave[]);
void pedirTurno(char turno[]);
void verUsuarios(Pizzeria *pizzeria);
void mostrarUsuarios(Pizzeria *pizzeria);

// gerente
void verStock(Pizzeria *pizzeria, int mostrarPrecio);
void mostrarStock(Pizzeria *pizzeria, int mostrarPrecio);
void agregarNuevoSabor(Pizzeria *pizzeria);
void eliminarSabor(Pizzeria *pizzeria);
void generarCodigoPizza(Pizzeria *pizzeria, char codigo[]);
void verCompras();
void verGanancias();
float sumarFacturas();
float sumarCompras();
void verFacturas(Pizzeria *pizzeria);

// ingredientes, recetas y elaboracion
void agregarIngrediente(Pizzeria *pizzeria);
void eliminarIngrediente(Pizzeria *pizzeria);
int buscarPizzaQueUsaIngrediente(Pizzeria *pizzeria, int indice_ingrediente);
void corregirRecetas(Pizzeria *pizzeria, int eliminado);
void comprarIngredientes(Pizzeria *pizzeria, char gerente[]);
void registrarCompra(Pizzeria *pizzeria, int indice_ingrediente, int cantidad, float costo_total, char gerente[]);
void mostrarIngredientes(Pizzeria *pizzeria);
int buscarIngredientePorNombre(Pizzeria *pizzeria, char nombre[]);
void cargarReceta(Pizzeria *pizzeria, Pizza *pizza); // pide los ingredientes y cantidades que lleva la pizza
void agregarItemReceta(Pizzeria *pizzeria, Pizza *pizza);
int cantidadElaborable(Pizzeria *pizzeria, int indice_pizza);
void elaborarPizzas(Pizzeria *pizzeria);
void registrarElaboracion(Pizzeria *pizzeria, int indice_pizza, int cantidad);
void descontarIngredientes(Pizzeria *pizzeria, int indice_pizza, int cantidad);

// importar datos desde un archivo de texto
void importarDatos(Pizzeria *pizzeria);
int importarIngrediente(Pizzeria *pizzeria, char linea[]);
int importarPizza(Pizzeria *pizzeria, char linea[]);
void importarReceta(Pizzeria *pizzeria, Pizza *pizza, char texto[]);
int buscarPizzaPorNombre(Pizzeria *pizzeria, char nombre[]);

// empleado
void verMesas(Pizzeria *pizzeria);
void marcarMesa(Pizzeria *pizzeria);
void marcarPedidoListo(Pizzeria *pizzeria);
int mostrarPedidosPendientes(Pizzeria *pizzeria);
int buscarPedidoPorNumero(Pizzeria *pizzeria, int numero);

// clientes
int identificarCliente(Pizzeria *pizzeria);
int registrarCliente(Pizzeria *pizzeria, char dni[]);
int buscarClientePorDni(Pizzeria *pizzeria, char dni[]);
int buscarClientePorId(Pizzeria *pizzeria, int id);
int dniValido(char dni[]);
void pedirDni(char dni[]);
void verMisPedidos(Pizzeria *pizzeria);
int mostrarComprasDeCliente(Pizzeria *pizzeria, int id_cliente);

// compra
void comprarPizza(Pizzeria *pizzeria);
int mostrarPizzasDisponibles(Pizzeria *pizzeria);
int pedirPizza(Pizzeria *pizzeria);
int buscarPizzaPorCodigo(Pizzeria *pizzeria, char codigo[]);
int pedirCantidad(Pizza *pizza);
int pedirExtras(Pizzeria *pizzeria, int cantidad_pizzas, Extra extras[]);
int mostrarExtrasDisponibles(Pizzeria *pizzeria, int cantidad_pizzas, Extra extras[], int cantidad_extras);
int extraElegible(Pizzeria *pizzeria, int indice, int cantidad_pizzas, Extra extras[], int cantidad_extras);
int elegirExtra(Pizzeria *pizzeria, int cantidad_pizzas, Extra extras[], int cantidad_extras);
float describirExtras(Extra extras[], int cantidad_extras, char texto[]);
void descontarExtras(Pizzeria *pizzeria, Extra extras[], int cantidad_extras, int cantidad_pizzas);
float jugarPorDescuento();
float calcularPrecioFinal(float precio, int cantidad, float descuento);
int buscarPedidoLibre(Pizzeria *pizzeria);
int registrarPedido(Pizzeria *pizzeria, int indice_pizza, int cantidad, int id_cliente, Extra extras[], int cantidad_extras);
void registrarVenta(Pizzeria *pizzeria, int indice_pizza, int cantidad, Extra extras[], int cantidad_extras);
void guardarCompra(Pizzeria *pizzeria, Factura *factura);

// facturas
Factura crearFactura(Pizzeria *pizzeria, int numero_pedido, int id_cliente, int indice_pizza, int cantidad, float precio_extras, char texto_extras[], float descuento, float total);
void mostrarFactura(Pizzeria *pizzeria, Factura *factura);
void formatearFecha(time_t fecha, char texto[]);

// juegos
void juegoAdivinarNumero(float *descuento); // elige el número secreto del 1 al 10
void juegoSumaRapida(float *descuento);     // elige los dos números que hay que sumar (con tiempo limite)

// utilitarias
void convertirMayusculas(char str[]);
int preguntarSiNo(char mensaje[]);
int textoANumero(char texto[]);
int leerOpcion(char mensaje[]);
int pedirOpcionRango(char mensaje[], int minimo, int maximo);

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
    char usuario[MAX_USUARIO];  // usuario que inicio sesion
    int opcion;
    int salir = 0;              // pasa a 1 cuando se quiere cerrar el programa

    utf8();                       // muestra bien las tildes y la ñ
    activarColoresANSI();         // permite usar colores en la consola de Windows
    srand((unsigned)time(NULL));  // para que los números de los juegos cambien en cada ejecución

    inicializarPizzeria(&pizzeria);  // primero los datos por defecto...
    cargarDatos(&pizzeria);          // ...y si hay archivos guardados, los reemplaza

    Grafico_Menu();               // pantalla de inicio
    pausa();

    // menu principal: se repite hasta que se elige salir
    do {
        opcion = mostrarMenuPrincipal();

        switch(opcion) {
            case 1:
                if (login(&pizzeria, "GERENTE", usuario)) {
                    salir = menuGerente(&pizzeria, usuario);
                }
                break;
            case 2:
                if (login(&pizzeria, "EMPLEADO", usuario)) {
                    iniciarTurno(&pizzeria, usuario);  // desde ahora es el empleado de turno
                    salir = menuEmpleado(&pizzeria);
                }
                break;
            case 3:
                salir = menuCliente(&pizzeria);  // el cliente no necesita login
                break;
            case 4:
                salir = 1;
                break;
            default:
                Grafico_OpcionIncorrecta();
                pausa();
        }
    } while (!salir);

    printf(AMARILLO "Gracias por visitar Pizzeria MASA MAIN" RESET "\n");

    return 0;
}

// ======================== INICIALIZACION ========================

// deja la pizzeria con los datos por defecto: usuarios, mesas y pedidos libres y ninguna pizza
void inicializarPizzeria(Pizzeria *pizzeria) {
    inicializarPizzas(pizzeria);
    inicializarUsuarios(pizzeria);
    inicializarIngredientes(pizzeria);

    // todas las mesas arrancan libres
    for (int i = 0; i < MAX_MESAS; i++) {
        (*pizzeria).mesas[i].numero = i + 1;  // las mesas se numeran desde 1
        strcpy((*pizzeria).mesas[i].estado, "LIBRE");
    }

    // todos los pedidos arrancan libres y sin pizza ni cliente
    for (int i = 0; i < MAX_PEDIDOS; i++) {
        (*pizzeria).pedidos[i].numero = 0;
        strcpy((*pizzeria).pedidos[i].estado, "LIBRE");
        cargarPizza(&(*pizzeria).pedidos[i].pizza, "", "", 0, 0);
        (*pizzeria).pedidos[i].cantidad = 0;
        (*pizzeria).pedidos[i].id_cliente = 0;
        (*pizzeria).pedidos[i].cantidad_extras = 0;
    }

    (*pizzeria).cantidad_pizzas = 0;  // no hay sabores: los carga el gerente
    (*pizzeria).cantidad_clientes = 0;
    (*pizzeria).proximo_pedido = 1;
    strcpy((*pizzeria).empleado_turno, "");  // todavia no hay empleado de turno
    strcpy((*pizzeria).turno_actual, "");
}

// deja todos los lugares de pizzas vacios: los sabores los carga el gerente
void inicializarPizzas(Pizzeria *pizzeria) {
    // un nombre vacio significa que ese lugar esta libre para un sabor nuevo
    for (int i = 0; i < MAX_PIZZAS; i++) {
        cargarPizza(&(*pizzeria).pizzas[i], "", "", 0, 0);
    }
}

// crea un gerente y un empleado por defecto, para que siempre haya quien pueda entrar
void inicializarUsuarios(Pizzeria *pizzeria) {
    cargarUsuario(&(*pizzeria).usuarios[0], "ADMIN", "1234", "GERENTE", "MANANA");
    cargarUsuario(&(*pizzeria).usuarios[1], "EMPLEADO", "1234", "EMPLEADO", "MANANA");
    (*pizzeria).cantidad_usuarios = 2;
}

// deja todos los lugares de ingredientes vacios: los carga el gerente
void inicializarIngredientes(Pizzeria *pizzeria) {
    for (int i = 0; i < MAX_INGREDIENTES; i++) {
        strcpy((*pizzeria).ingredientes[i].nombre, "");
        strcpy((*pizzeria).ingredientes[i].unidad, "");
        (*pizzeria).ingredientes[i].stock = 0;
        (*pizzeria).ingredientes[i].precio_extra = 0;
        (*pizzeria).ingredientes[i].porcion_extra = 0;
    }

    (*pizzeria).cantidad_ingredientes = 0;
}

// completa todos los datos de una pizza
void cargarPizza(Pizza *pizza, char codigo[], char nombre[], float precio, int stock) {
    strcpy((*pizza).codigo, codigo);
    strcpy((*pizza).nombre, nombre);
    (*pizza).precio = precio;
    (*pizza).stock = stock;
    (*pizza).cantidad_items = 0;  // arranca sin receta
}

// completa todos los datos de un usuario
void cargarUsuario(Usuario *usuario, char nombre[], char clave[], char perfil[], char turno[]) {
    strcpy((*usuario).usuario, nombre);
    strcpy((*usuario).clave, clave);
    strcpy((*usuario).perfil, perfil);
    strcpy((*usuario).turno, turno);
}

// ======================== ARCHIVOS BINARIOS ========================

// guarda un arreglo completo: primero la cantidad de elementos y despues los elementos
// "wb" = escribir en binario (si el archivo existe, lo reemplaza)
// devuelve 1 si se pudo guardar y 0 si no
int guardarArreglo(char archivo[], void *datos, size_t tamano, int cantidad) {
    FILE *f = fopen(archivo, "wb");

    if (f == NULL) {
        printf(ROJO "No se pudo guardar el archivo %s" RESET "\n", archivo);
        return 0;
    }

    fwrite(&cantidad, sizeof(int), 1, f);  // direccion de lo que se guarda, tamano de cada elemento, cuantos elementos hay en el arreglo, el archivo

    fwrite(datos, tamano, cantidad, f);
    fclose(f);

    return 1;
}

// lee un arreglo guardado con guardarArreglo y devuelve cuantos elementos leyo
// "rb" = leer en binario. Devuelve -1 si el archivo no existe o esta danado
int cargarArreglo(char archivo[], void *datos, size_t tamano, int maximo) {
    int cantidad;
    FILE *f = fopen(archivo, "rb");

    if (f == NULL) {
        return -1;  // todavia no existe: se usan los datos por defecto
    }

    if (fread(&cantidad, sizeof(int), 1, f) != 1) {
        fclose(f);
        return -1;
    }

    // la cantidad guardada tiene que caber en el arreglo
    if (cantidad < 0 || cantidad > maximo) {
        fclose(f);
        return -1;
    }

    // tienen que haberse leido todos los elementos
    if (fread(datos, tamano, cantidad, f) != (size_t)cantidad) {
        fclose(f);
        return -1;
    }

    fclose(f);
    return cantidad;
}

// agrega un registro al final del archivo, sin borrar lo anterior
// "ab" = agregar al final en binario (si no existe, lo crea)
int agregarRegistro(char archivo[], void *registro, size_t tamano) {
    FILE *f = fopen(archivo, "ab");

    if (f == NULL) {
        printf(ROJO "No se pudo guardar el archivo %s" RESET "\n", archivo);
        return 0;
    }

    fwrite(registro, tamano, 1, f);
    fclose(f);

    return 1;
}

// cuenta cuantos registros tiene un archivo: tamaño total dividido el tamaño de un registro
int contarRegistros(char archivo[], size_t tamano) {
    FILE *f = fopen(archivo, "rb");
    long bytes;

    if (f == NULL) {
        return 0;
    }

    fseek(f, 0, SEEK_END);  // se va al final del archivo
    bytes = ftell(f);       // ftell dice en que posicion esta, o sea cuantos bytes hay
    fclose(f);

    return (int)(bytes / (long)tamano);
}

// si existen los archivos, reemplaza los datos por defecto por los guardados
void cargarDatos(Pizzeria *pizzeria) {
    int cantidad;

    cantidad = cargarArreglo(ARCHIVO_PIZZAS, (*pizzeria).pizzas, sizeof(Pizza), MAX_PIZZAS);
    if (cantidad != -1) {
        (*pizzeria).cantidad_pizzas = cantidad;
    }

    cantidad = cargarArreglo(ARCHIVO_USUARIOS, (*pizzeria).usuarios, sizeof(Usuario), MAX_USUARIOS);
    if (cantidad != -1) {
        (*pizzeria).cantidad_usuarios = cantidad;
    }

    cantidad = cargarArreglo(ARCHIVO_CLIENTES, (*pizzeria).clientes, sizeof(Cliente), MAX_CLIENTES);
    if (cantidad != -1) {
        (*pizzeria).cantidad_clientes = cantidad;
    }

    cantidad = cargarArreglo(ARCHIVO_INGREDIENTES, (*pizzeria).ingredientes, sizeof(Ingrediente), MAX_INGREDIENTES);
    if (cantidad != -1) {
        (*pizzeria).cantidad_ingredientes = cantidad;
    }

    // si el archivo de pedidos no existe, quedan los pedidos libres
    cargarArreglo(ARCHIVO_PEDIDOS, (*pizzeria).pedidos, sizeof(Pedido), MAX_PEDIDOS);

    // cada pedido genera una factura, asi que el proximo numero es la cantidad de facturas + 1
    (*pizzeria).proximo_pedido = contarRegistros(ARCHIVO_FACTURAS, sizeof(Factura)) + 1;
}

void guardarPizzas(Pizzeria *pizzeria) {
    guardarArreglo(ARCHIVO_PIZZAS, (*pizzeria).pizzas, sizeof(Pizza), (*pizzeria).cantidad_pizzas);
}

void guardarUsuarios(Pizzeria *pizzeria) {
    guardarArreglo(ARCHIVO_USUARIOS, (*pizzeria).usuarios, sizeof(Usuario), (*pizzeria).cantidad_usuarios);
}

void guardarClientes(Pizzeria *pizzeria) {
    guardarArreglo(ARCHIVO_CLIENTES, (*pizzeria).clientes, sizeof(Cliente), (*pizzeria).cantidad_clientes);
}

void guardarIngredientes(Pizzeria *pizzeria) {
    guardarArreglo(ARCHIVO_INGREDIENTES, (*pizzeria).ingredientes, sizeof(Ingrediente), (*pizzeria).cantidad_ingredientes);
}

// los pedidos se guardan siempre completos (los MAX_PEDIDOS lugares)
void guardarPedidos(Pizzeria *pizzeria) {
    guardarArreglo(ARCHIVO_PEDIDOS, (*pizzeria).pedidos, sizeof(Pedido), MAX_PEDIDOS);
}

// ======================== MENUS ========================

// muestra el logo y devuelve la opcion elegida (se puede escribir con cifras o con letras)
int mostrarMenuPrincipal() {
    int opcion;

    limpiaPantalla();
    Grafico_MostrarLogo();
    opcion = leerOpcion("");

    return opcion;
}

// devuelve 1 si el gerente eligio salir del programa y 0 si solo vuelve al menu principal
int menuGerente(Pizzeria *pizzeria, char usuario[]) {
    int opcion;

    do {
        limpiaPantalla();
        Grafico_MenuGerente();
        opcion = leerOpcion("");

        switch(opcion) {
            case 1:
                verStock(pizzeria, 1);  // el gerente ve tambien el precio
                break;
            case 2:
                agregarNuevoSabor(pizzeria);
                break;
            case 3:
                eliminarSabor(pizzeria);
                break;
            case 4:
                agregarIngrediente(pizzeria);
                break;
            case 5:
                eliminarIngrediente(pizzeria);
                break;
            case 6:
                comprarIngredientes(pizzeria, usuario);  // la compra queda a nombre de este gerente
                break;
            case 7:
                verCompras();
                break;
            case 8:
                verGanancias();
                break;
            case 9:
                verFacturas(pizzeria);
                break;
            case 10:
                crearUsuario(pizzeria);
                break;
            case 11:
                eliminarUsuario(pizzeria, usuario);
                break;
            case 12:
                verUsuarios(pizzeria);
                break;
            case 13:
                importarDatos(pizzeria);
                break;
            case 14:
                Grafico_Volviendo();
                pausa();
                break;
            case 15:
                return 1;  // salir del programa
            default:
                Grafico_OpcionIncorrecta();
                pausa();
        }
    } while (opcion != 14);

    return 0;
}

// devuelve 1 si el empleado eligio salir del programa y 0 si solo vuelve al menu principal
int menuEmpleado(Pizzeria *pizzeria) {
    int opcion;

    do {
        limpiaPantalla();
        Grafico_MenuEmpleado();
        opcion = leerOpcion("");

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
                verStock(pizzeria, 0);  // el empleado no ve el precio
                break;
            case 5:
                elaborarPizzas(pizzeria);
                break;
            case 6:
                Grafico_Volviendo();
                pausa();
                break;
            case 7:
                return 1;  // salir del programa
            default:
                Grafico_OpcionIncorrecta();
                pausa();
        }
    } while (opcion != 6);

    return 0;
}

// devuelve 1 si el cliente eligio salir del programa y 0 si solo vuelve al menu principal
int menuCliente(Pizzeria *pizzeria) {
    int opcion;

    do {
        limpiaPantalla();
        Grafico_MenuCliente();
        opcion = leerOpcion("");

        switch(opcion) {
            case 1:
                comprarPizza(pizzeria);
                break;
            case 2:
                verMisPedidos(pizzeria);
                break;
            case 3:
                Grafico_Volviendo();
                pausa();
                break;
            case 4:
                return 1;  // salir del programa
            default:
                Grafico_OpcionIncorrecta();
                pausa();
        }
    } while (opcion != 3);

    return 0;
}

// ======================== LOGIN Y USUARIOS ========================

// pide usuario y clave hasta que sean correctos y deja el nombre en usuario
// devuelve 1 si el acceso es correcto y 0 si se cancela (escribiendo 1)
int login(Pizzeria *pizzeria, char perfil[], char usuario[]) {
    char clave[MAX_CLAVE];

    do {
        pedirCredenciales(usuario, clave);

        // con un 1 se cancela y se vuelve al menu principal
        if (sonIguales(usuario, "1") || sonIguales(clave, "1")) {
            return 0;
        }

        if (credencialesValidas(pizzeria, perfil, usuario, clave)) {
            int i = buscarUsuario(pizzeria, usuario);
            printf(VERDE "Login exitoso como %s: %s (turno %s)" RESET "\n",
                   perfil, (*pizzeria).usuarios[i].usuario, (*pizzeria).usuarios[i].turno);
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

// el usuario tiene que existir, ser del perfil que se pide y tener esa clave
int credencialesValidas(Pizzeria *pizzeria, char perfil[], char usuario[], char clave[]) {
    int i = buscarUsuario(pizzeria, usuario);

    if (i == -1) {
        return 0;  // no existe ese usuario
    }

    return sonIguales((*pizzeria).usuarios[i].perfil, perfil) &&
           sonIguales((*pizzeria).usuarios[i].clave, clave);
}

// devuelve la posicion del usuario, o -1 si no existe
int buscarUsuario(Pizzeria *pizzeria, char usuario[]) {
    for (int i = 0; i < (*pizzeria).cantidad_usuarios; i++) {
        if (sonIguales((*pizzeria).usuarios[i].usuario, usuario)) {
            return i;
        }
    }

    return -1;
}

// el empleado que inicia sesion queda como empleado de turno hasta que otro inicie sesion
void iniciarTurno(Pizzeria *pizzeria, char usuario[]) {
    int i = buscarUsuario(pizzeria, usuario);

    strcpy((*pizzeria).empleado_turno, usuario);
    strcpy((*pizzeria).turno_actual, (*pizzeria).usuarios[i].turno);
}

// el gerente da de alta un usuario nuevo (gerente o empleado) con su turno
void crearUsuario(Pizzeria *pizzeria) {
    int i = (*pizzeria).cantidad_usuarios;  // el usuario nuevo va en el primer lugar libre

    limpiaPantalla();
    printf(AMARILLO "===== CREAR USUARIO =====" RESET "\n");

    if (i >= MAX_USUARIOS) {
        printf(ROJO "No hay espacio para mas usuarios." RESET "\n");
        pausa();
        return;
    }

    pedirNombreUsuario(pizzeria, (*pizzeria).usuarios[i].usuario);
    pedirClaveNueva((*pizzeria).usuarios[i].clave);

    int perfil = pedirOpcionRango("Perfil (1. Gerente | 2. Empleado): ", 1, 2);
    if (perfil == 1) {
        strcpy((*pizzeria).usuarios[i].perfil, "GERENTE");
    } else {
        strcpy((*pizzeria).usuarios[i].perfil, "EMPLEADO");
    }

    pedirTurno((*pizzeria).usuarios[i].turno);

    (*pizzeria).cantidad_usuarios++;
    guardarUsuarios(pizzeria);

    printf(VERDE "Usuario %s creado exitosamente!" RESET "\n", (*pizzeria).usuarios[i].usuario);
    pausa();
}

// pide un nombre de usuario hasta que no este vacio, no sea "1" y no este repetido
void pedirNombreUsuario(Pizzeria *pizzeria, char usuario[]) {
    int valido;

    do {
        valido = 0;
        leerCadena(usuario, MAX_USUARIO, "Nombre de usuario: ");
        convertirMayusculas(usuario);

        if (strlen(usuario) == 0 || sonIguales(usuario, "1")) {
            printf(ROJO "Nombre no valido." RESET "\n");  // el 1 se usa para cancelar el login
        } else if (buscarUsuario(pizzeria, usuario) != -1) {
            printf(ROJO "Ese usuario ya existe." RESET "\n");
        } else {
            valido = 1;
        }
    } while (!valido);
}

// pide una clave hasta que no este vacia ni sea "1" (el 1 cancela el login)
void pedirClaveNueva(char clave[]) {
    do {
        leerCadena(clave, MAX_CLAVE, "Clave: ");

        if (strlen(clave) == 0 || sonIguales(clave, "1")) {
            printf(ROJO "Clave no valida." RESET "\n");
        }
    } while (strlen(clave) == 0 || sonIguales(clave, "1"));
}

// pregunta el turno y lo guarda como texto
void pedirTurno(char turno[]) {
    int opcion = pedirOpcionRango("Turno (1. Manana | 2. Tarde | 3. Noche): ", 1, 3);

    if (opcion == 1) {
        strcpy(turno, "MANANA");
    } else if (opcion == 2) {
        strcpy(turno, "TARDE");
    } else {
        strcpy(turno, "NOCHE");
    }
}

// pantalla con la lista de usuarios
void verUsuarios(Pizzeria *pizzeria) {
    limpiaPantalla();
    printf(AMARILLO "===== USUARIOS =====" RESET "\n");
    mostrarUsuarios(pizzeria);
    pausa();
}

// lista los usuarios (sin mostrar las claves)
void mostrarUsuarios(Pizzeria *pizzeria) {
    for (int i = 0; i < (*pizzeria).cantidad_usuarios; i++) {
        printf("%s - %s - turno %s\n",
               (*pizzeria).usuarios[i].usuario,
               (*pizzeria).usuarios[i].perfil,
               (*pizzeria).usuarios[i].turno);
    }
}

// el gerente elimina un usuario (no puede eliminar el suyo)
void eliminarUsuario(Pizzeria *pizzeria, char usuario_actual[]) {
    char nombre[MAX_USUARIO];

    limpiaPantalla();
    printf(AMARILLO "===== ELIMINAR USUARIO =====" RESET "\n");
    mostrarUsuarios(pizzeria);

    leerCadena(nombre, MAX_USUARIO, "\nUsuario a eliminar: ");
    convertirMayusculas(nombre);

    int i = buscarUsuario(pizzeria, nombre);

    if (i == -1) {
        printf(ROJO "Ese usuario no existe." RESET "\n");
    } else if (sonIguales(nombre, usuario_actual)) {
        printf(ROJO "No puede eliminar su propio usuario." RESET "\n");
    } else if (!preguntarSiNo("Confirma que quiere eliminarlo? (S/N): ")) {
        printf("Operacion cancelada.\n");
    } else {
        // se corre cada usuario un lugar hacia atras para tapar el hueco
        for (int k = i; k < (*pizzeria).cantidad_usuarios - 1; k++) {
            (*pizzeria).usuarios[k] = (*pizzeria).usuarios[k + 1];
        }
        (*pizzeria).cantidad_usuarios--;

        // si era el empleado de turno, el turno queda sin empleado
        if (sonIguales((*pizzeria).empleado_turno, nombre)) {
            strcpy((*pizzeria).empleado_turno, "");
            strcpy((*pizzeria).turno_actual, "");
        }

        guardarUsuarios(pizzeria);
        printf(VERDE "Usuario %s eliminado." RESET "\n", nombre);
    }

    pausa();
}

// ======================== GERENTE ========================

// pantalla de stock: pizzas (con precio para el gerente y sin precio para el empleado) e ingredientes
void verStock(Pizzeria *pizzeria, int mostrarPrecio) {
    limpiaPantalla();
    printf(AMARILLO "===== STOCK DE PIZZAS =====" RESET "\n");
    mostrarStock(pizzeria, mostrarPrecio);

    printf(AMARILLO "\n===== STOCK DE INGREDIENTES =====" RESET "\n");
    mostrarIngredientes(pizzeria);
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

    cargarReceta(pizzeria, &(*pizzeria).pizzas[i]);  // ingredientes que lleva cada pizza

    generarCodigoPizza(pizzeria, (*pizzeria).pizzas[i].codigo);  // el codigo se arma solo

    (*pizzeria).cantidad_pizzas++;
    guardarPizzas(pizzeria);
    printf(VERDE "Sabor agregado exitosamente!" RESET "\n");
    pausa();
}

// arma un codigo nuevo: PZ + (el numero mas alto que ya existe + 1)
// asi no se repite aunque se hayan eliminado sabores
void generarCodigoPizza(Pizzeria *pizzeria, char codigo[]) {
    int mayor = 0;

    for (int i = 0; i < (*pizzeria).cantidad_pizzas; i++) {
        int numero = atoi((*pizzeria).pizzas[i].codigo + 2);  // saltea las dos letras PZ

        if (numero > mayor) {
            mayor = numero;
        }
    }

    snprintf(codigo, MAX_CODIGO, "PZ%d", mayor + 1);
}

// el gerente elimina un sabor
void eliminarSabor(Pizzeria *pizzeria) {
    limpiaPantalla();
    printf(AMARILLO "===== ELIMINAR SABOR =====" RESET "\n");

    if ((*pizzeria).cantidad_pizzas == 0) {
        printf(ROJO "No hay sabores cargados." RESET "\n");
        pausa();
        return;
    }

    mostrarStock(pizzeria, 1);

    int i = pedirPizza(pizzeria);

    if (i == -1) {
        printf(ROJO "Codigo no encontrado." RESET "\n");
    } else {
        printf("Se va a eliminar %s.\n", (*pizzeria).pizzas[i].nombre);

        if (!preguntarSiNo("Confirma que quiere eliminarlo? (S/N): ")) {
            printf("Operacion cancelada.\n");
        } else {
            // se corre cada sabor un lugar hacia atras y el ultimo lugar queda vacio
            for (int k = i; k < (*pizzeria).cantidad_pizzas - 1; k++) {
                (*pizzeria).pizzas[k] = (*pizzeria).pizzas[k + 1];
            }
            cargarPizza(&(*pizzeria).pizzas[(*pizzeria).cantidad_pizzas - 1], "", "", 0, 0);
            (*pizzeria).cantidad_pizzas--;

            guardarPizzas(pizzeria);
            printf(VERDE "Sabor eliminado." RESET "\n");
        }
    }

    pausa();
}

// lee el archivo de compras de principio a fin y las muestra
void verCompras() {
    Compra compra;
    char fecha[20];
    int hay = 0;
    FILE *f = fopen(ARCHIVO_COMPRAS, "rb");

    limpiaPantalla();
    printf(AMARILLO "===== COMPRAS DE INGREDIENTES =====" RESET "\n");

    if (f != NULL) {
        // fread devuelve 1 mientras pueda leer un registro entero
        while (fread(&compra, sizeof(Compra), 1, f) == 1) {
            formatearFecha(compra.fecha, fecha);
            printf("N %d - %s - %d %s de %s - $%.2f - gerente: %s\n",
                   compra.numero, fecha, compra.cantidad, compra.unidad,
                   compra.ingrediente, compra.costo_total, compra.gerente);
            hay = 1;
        }
        fclose(f);
    }

    if (hay == 0) {
        printf(ROJO "Todavia no hay compras." RESET "\n");
    }

    pausa();
}

// ganancia neta = lo facturado en ventas menos lo gastado en compras de ingredientes
void verGanancias() {
    float facturado = sumarFacturas();
    float gastado = sumarCompras();
    float neta = facturado - gastado;

    limpiaPantalla();
    printf(AMARILLO "===== GANANCIAS =====" RESET "\n");
    printf("Total facturado en ventas:   $%.2f\n", facturado);
    printf("Gastado en compras:          $%.2f\n", gastado);

    if (neta >= 0) {
        printf("Ganancia neta:               " VERDE "$%.2f" RESET "\n", neta);
    } else {
        printf("Ganancia neta:               " ROJO "$%.2f" RESET "\n", neta);
    }

    pausa();
}

// suma el total de todas las facturas guardadas
float sumarFacturas() {
    Factura factura;
    float total = 0;
    FILE *f = fopen(ARCHIVO_FACTURAS, "rb");

    if (f == NULL) {
        return 0;
    }

    while (fread(&factura, sizeof(Factura), 1, f) == 1) {
        total += factura.total;
    }

    fclose(f);
    return total;
}

// suma el costo de todas las compras guardadas
float sumarCompras() {
    Compra compra;
    float total = 0;
    FILE *f = fopen(ARCHIVO_COMPRAS, "rb");

    if (f == NULL) {
        return 0;
    }

    while (fread(&compra, sizeof(Compra), 1, f) == 1) {
        total += compra.costo_total;
    }

    fclose(f);
    return total;
}

// lee el archivo de facturas de principio a fin y las muestra
void verFacturas(Pizzeria *pizzeria) {
    Factura factura;
    int hay = 0;
    FILE *f = fopen(ARCHIVO_FACTURAS, "rb");

    limpiaPantalla();
    printf(AMARILLO "===== FACTURAS =====" RESET "\n");

    if (f != NULL) {
        while (fread(&factura, sizeof(Factura), 1, f) == 1) {
            mostrarFactura(pizzeria, &factura);
            hay = 1;
        }
        fclose(f);
    }

    if (hay == 0) {
        printf(ROJO "Todavia no hay facturas." RESET "\n");
    }

    pausa();
}

// ======================== INGREDIENTES Y ELABORACION ========================

// da de alta un ingrediente nuevo con su unidad, su stock inicial y su precio como extra
void agregarIngrediente(Pizzeria *pizzeria) {
    int i = (*pizzeria).cantidad_ingredientes;  // el nuevo va en el primer lugar libre
    char nombre[MAX_NOMBRE];
    float precio_extra = -1;
    int valido;

    limpiaPantalla();
    printf(AMARILLO "===== AGREGAR INGREDIENTE =====" RESET "\n");

    if (i >= MAX_INGREDIENTES) {
        printf(ROJO "No hay espacio para mas ingredientes." RESET "\n");
        pausa();
        return;
    }

    // el nombre no puede estar vacio ni repetido
    do {
        valido = 0;
        leerCadena(nombre, MAX_NOMBRE, "Nombre del ingrediente: ");

        if (strlen(nombre) == 0) {
            printf(ROJO "El nombre no puede estar vacio." RESET "\n");
        } else if (buscarIngredientePorNombre(pizzeria, nombre) != -1) {
            printf(ROJO "Ese ingrediente ya existe." RESET "\n");
        } else {
            valido = 1;
        }
    } while (!valido);

    strcpy((*pizzeria).ingredientes[i].nombre, nombre);

    // la unidad tampoco puede quedar vacia
    do {
        leerCadena((*pizzeria).ingredientes[i].unidad, MAX_UNIDAD, "Unidad (g, ml, u): ");
        if (strlen((*pizzeria).ingredientes[i].unidad) == 0) {
            printf(ROJO "La unidad no puede estar vacia." RESET "\n");
        }
    } while (strlen((*pizzeria).ingredientes[i].unidad) == 0);

    (*pizzeria).ingredientes[i].stock = pedirEnteroRango("Stock inicial: ", 0, 100000);

    // el precio como extra no puede ser negativo (0 significa que no se vende como extra)
    do {
        leerFlotante("Precio como ingrediente extra (0 si no se vende como extra): ", &precio_extra);
        if (precio_extra < 0) {
            printf(ROJO "El precio no puede ser negativo." RESET "\n");
        }
    } while (precio_extra < 0);

    (*pizzeria).ingredientes[i].precio_extra = precio_extra;

    if (precio_extra > 0) {
        (*pizzeria).ingredientes[i].porcion_extra = pedirEnteroRango("Cuanto se usa por cada extra (en la misma unidad): ", 1, 10000);
    } else {
        (*pizzeria).ingredientes[i].porcion_extra = 0;
    }

    (*pizzeria).cantidad_ingredientes++;
    guardarIngredientes(pizzeria);

    printf(VERDE "Ingrediente agregado exitosamente!" RESET "\n");
    pausa();
}

// el gerente compra ingredientes: suma el stock y deja registrado lo que costo
void comprarIngredientes(Pizzeria *pizzeria, char gerente[]) {
    float costo_total = 0;

    limpiaPantalla();
    printf(AMARILLO "===== COMPRA DE INGREDIENTES =====" RESET "\n");

    if ((*pizzeria).cantidad_ingredientes == 0) {
        printf(ROJO "Todavia no hay ingredientes cargados." RESET "\n");
        pausa();
        return;
    }

    mostrarIngredientes(pizzeria);

    // el ingrediente 1 esta en la posicion 0, por eso el - 1
    int i = pedirOpcionRango("Numero de ingrediente: ", 1, (*pizzeria).cantidad_ingredientes) - 1;
    int cantidad = pedirEnteroRango("Cantidad comprada: ", 1, 100000);

    // el costo tiene que ser mayor a 0
    do {
        leerFlotante("Costo total de la compra: ", &costo_total);
        if (costo_total <= 0) {
            printf(ROJO "El costo debe ser mayor a 0." RESET "\n");
        }
    } while (costo_total <= 0);

    registrarCompra(pizzeria, i, cantidad, costo_total, gerente);
    pausa();
}

// suma lo comprado al stock del ingrediente y guarda el registro de la compra
void registrarCompra(Pizzeria *pizzeria, int indice_ingrediente, int cantidad, float costo_total, char gerente[]) {
    Compra compra;

    compra.numero = contarRegistros(ARCHIVO_COMPRAS, sizeof(Compra)) + 1;
    compra.fecha = time(NULL);
    strcpy(compra.ingrediente, (*pizzeria).ingredientes[indice_ingrediente].nombre);
    strcpy(compra.unidad, (*pizzeria).ingredientes[indice_ingrediente].unidad);
    compra.cantidad = cantidad;
    compra.costo_total = costo_total;
    strcpy(compra.gerente, gerente);  // quien hizo la compra

    (*pizzeria).ingredientes[indice_ingrediente].stock += cantidad;

    guardarIngredientes(pizzeria);
    agregarRegistro(ARCHIVO_COMPRAS, &compra, sizeof(Compra));

    printf(VERDE "Compra registrada. Nuevo stock de %s: %d %s" RESET "\n",
           compra.ingrediente, (*pizzeria).ingredientes[indice_ingrediente].stock, compra.unidad);
}

// lista los ingredientes numerados (el numero es posicion + 1)
void mostrarIngredientes(Pizzeria *pizzeria) {
    if ((*pizzeria).cantidad_ingredientes == 0) {
        printf(ROJO "No hay ingredientes cargados." RESET "\n");
        return;
    }

    for (int i = 0; i < (*pizzeria).cantidad_ingredientes; i++) {
        printf("%d. %s - %d %s\n",
               i + 1,
               (*pizzeria).ingredientes[i].nombre,
               (*pizzeria).ingredientes[i].stock,
               (*pizzeria).ingredientes[i].unidad);
    }
}

// devuelve la posicion del ingrediente con ese nombre, o -1 si no existe
int buscarIngredientePorNombre(Pizzeria *pizzeria, char nombre[]) {
    for (int i = 0; i < (*pizzeria).cantidad_ingredientes; i++) {
        if (sonIguales((*pizzeria).ingredientes[i].nombre, nombre)) {
            return i;
        }
    }

    return -1;
}

// el gerente elimina un ingrediente (no se puede si lo usa la receta de alguna pizza)
void eliminarIngrediente(Pizzeria *pizzeria) {
    limpiaPantalla();
    printf(AMARILLO "===== ELIMINAR INGREDIENTE =====" RESET "\n");

    if ((*pizzeria).cantidad_ingredientes == 0) {
        printf(ROJO "No hay ingredientes cargados." RESET "\n");
        pausa();
        return;
    }

    mostrarIngredientes(pizzeria);

    int i = pedirOpcionRango("Numero de ingrediente a eliminar: ", 1, (*pizzeria).cantidad_ingredientes) - 1;
    int p = buscarPizzaQueUsaIngrediente(pizzeria, i);

    if (p != -1) {
        printf(ROJO "No se puede eliminar: lo usa la receta de %s." RESET "\n", (*pizzeria).pizzas[p].nombre);
    } else if (!preguntarSiNo("Confirma que quiere eliminarlo? (S/N): ")) {
        printf("Operacion cancelada.\n");
    } else {
        int ultimo = (*pizzeria).cantidad_ingredientes - 1;

        // se corre cada ingrediente un lugar hacia atras y el ultimo lugar queda vacio
        for (int k = i; k < ultimo; k++) {
            (*pizzeria).ingredientes[k] = (*pizzeria).ingredientes[k + 1];
        } 
        strcpy((*pizzeria).ingredientes[ultimo].nombre, "");
        strcpy((*pizzeria).ingredientes[ultimo].unidad, "");
        (*pizzeria).ingredientes[ultimo].stock = 0;
        (*pizzeria).ingredientes[ultimo].precio_extra = 0;
        (*pizzeria).ingredientes[ultimo].porcion_extra = 0;
        (*pizzeria).cantidad_ingredientes--;

        corregirRecetas(pizzeria, i);  // las recetas guardan posiciones, que acaban de moverse

        guardarIngredientes(pizzeria);
        guardarPizzas(pizzeria);
        printf(VERDE "Ingrediente eliminado." RESET "\n");
    }

    pausa();
}

// devuelve la posicion de una pizza cuya receta use ese ingrediente, o -1 si ninguna lo usa
int buscarPizzaQueUsaIngrediente(Pizzeria *pizzeria, int indice_ingrediente) {
    for (int p = 0; p < (*pizzeria).cantidad_pizzas; p++) {
        for (int k = 0; k < (*pizzeria).pizzas[p].cantidad_items; k++) {
            if ((*pizzeria).pizzas[p].receta[k].indice_ingrediente == indice_ingrediente) {
                return p;
            }
        }
    }

    return -1;
}

// despues de eliminar un ingrediente, los que estaban mas adelante bajan una posicion
// y las recetas que los usan tienen que apuntar a la posicion nueva
void corregirRecetas(Pizzeria *pizzeria, int eliminado) {
    for (int p = 0; p < (*pizzeria).cantidad_pizzas; p++) {
        for (int k = 0; k < (*pizzeria).pizzas[p].cantidad_items; k++) {
            if ((*pizzeria).pizzas[p].receta[k].indice_ingrediente > eliminado) {
                (*pizzeria).pizzas[p].receta[k].indice_ingrediente--;
            }
        }
    }
}

// arma la receta de una pizza: el gerente va eligiendo ingredientes y cantidades
void cargarReceta(Pizzeria *pizzeria, Pizza *pizza) {
    (*pizza).cantidad_items = 0;

    if ((*pizzeria).cantidad_ingredientes == 0) {
        printf("No hay ingredientes cargados: el sabor queda sin receta.\n");
        return;
    }

    while ((*pizza).cantidad_items < MAX_RECETA &&
           preguntarSiNo("Agregar un ingrediente a la receta? (S/N): ")) {
        agregarItemReceta(pizzeria, pizza);
    }
}

// agrega un ingrediente y su cantidad por pizza a la receta
void agregarItemReceta(Pizzeria *pizzeria, Pizza *pizza) {
    mostrarIngredientes(pizzeria);

    int ingrediente = pedirOpcionRango("Numero de ingrediente: ", 1, (*pizzeria).cantidad_ingredientes) - 1;

    // un ingrediente no puede estar dos veces en la misma receta
    for (int k = 0; k < (*pizza).cantidad_items; k++) {
        if ((*pizza).receta[k].indice_ingrediente == ingrediente) {
            printf(ROJO "Ese ingrediente ya esta en la receta." RESET "\n");
            return;
        }
    }

    int cantidad = pedirEnteroRango("Cantidad que lleva cada pizza: ", 1, 10000);

    (*pizza).receta[(*pizza).cantidad_items].indice_ingrediente = ingrediente;
    (*pizza).receta[(*pizza).cantidad_items].cantidad = cantidad;
    (*pizza).cantidad_items++;
}

// devuelve cuantas pizzas de ese sabor se pueden elaborar con los ingredientes que hay
// (el ingrediente que menos alcanza es el que limita). Sin receta no se puede elaborar: devuelve 0
int cantidadElaborable(Pizzeria *pizzeria, int indice_pizza) {
    if ((*pizzeria).pizzas[indice_pizza].cantidad_items == 0) {
        return 0;
    }

    int maximo = -1;  // -1 significa que todavia no se calculo ninguno

    for (int k = 0; k < (*pizzeria).pizzas[indice_pizza].cantidad_items; k++) {
        int ingrediente = (*pizzeria).pizzas[indice_pizza].receta[k].indice_ingrediente;
        int por_pizza = (*pizzeria).pizzas[indice_pizza].receta[k].cantidad;
        int alcanzan = (*pizzeria).ingredientes[ingrediente].stock / por_pizza;

        if (maximo == -1 || alcanzan < maximo) {
            maximo = alcanzan;
        }
    }

    return maximo;
}

// el empleado elabora pizzas: se gastan ingredientes y se suman pizzas al stock
void elaborarPizzas(Pizzeria *pizzeria) {
    limpiaPantalla();
    printf(AMARILLO "===== ELABORAR PIZZAS =====" RESET "\n");

    if ((*pizzeria).cantidad_pizzas == 0) {
        printf(ROJO "Todavia no hay sabores cargados." RESET "\n");
        pausa();
        return;
    }

    mostrarStock(pizzeria, 0);
    printf("\nIngredientes disponibles:\n");
    mostrarIngredientes(pizzeria);

    int indice = pedirPizza(pizzeria);

    if (indice == -1) {
        printf(ROJO "Codigo no encontrado." RESET "\n");
        pausa();
        return;
    }

    if ((*pizzeria).pizzas[indice].cantidad_items == 0) {
        printf(ROJO "Esa pizza no tiene receta cargada." RESET "\n");
        pausa();
        return;
    }

    int maximo = cantidadElaborable(pizzeria, indice);

    if (maximo == 0) {
        printf(ROJO "Faltan ingredientes para elaborar esta pizza." RESET "\n");
        pausa();
        return;
    }

    char mensaje[100];
    sprintf(mensaje, "Cuantas pizzas elabora? (1-%d): ", maximo);
    int cantidad = pedirEnteroRango(mensaje, 1, maximo);

    registrarElaboracion(pizzeria, indice, cantidad);
    pausa();
}

// descuenta los ingredientes usados, suma las pizzas al stock y guarda los cambios
void registrarElaboracion(Pizzeria *pizzeria, int indice_pizza, int cantidad) {
    descontarIngredientes(pizzeria, indice_pizza, cantidad);
    (*pizzeria).pizzas[indice_pizza].stock += cantidad;

    guardarIngredientes(pizzeria);
    guardarPizzas(pizzeria);

    printf(VERDE "Se elaboraron %d pizzas. Nuevo stock de %s: %d" RESET "\n",
           cantidad, (*pizzeria).pizzas[indice_pizza].nombre, (*pizzeria).pizzas[indice_pizza].stock);
}

// resta de cada ingrediente lo que se uso para las pizzas elaboradas
void descontarIngredientes(Pizzeria *pizzeria, int indice_pizza, int cantidad) {
    for (int k = 0; k < (*pizzeria).pizzas[indice_pizza].cantidad_items; k++) {
        int ingrediente = (*pizzeria).pizzas[indice_pizza].receta[k].indice_ingrediente;

        (*pizzeria).ingredientes[ingrediente].stock -= (*pizzeria).pizzas[indice_pizza].receta[k].cantidad * cantidad;
    }
}

// ======================== IMPORTAR DATOS ========================

// lee el archivo de texto datos.txt y carga ingredientes y pizzas
// formato de cada linea:
//   INGREDIENTE;nombre;unidad;stock
//   PIZZA;nombre;precio;stock;ingrediente:cantidad,ingrediente:cantidad
// los ingredientes tienen que estar antes que las pizzas que los usan
void importarDatos(Pizzeria *pizzeria) {
    char linea[MAX_LINEA];
    int ingredientes = 0;
    int pizzas = 0;
    FILE *f = fopen(ARCHIVO_DATOS, "r");  // "r" = leer texto

    limpiaPantalla();
    printf(AMARILLO "===== IMPORTAR DATOS =====" RESET "\n");

    if (f == NULL) {
        printf(ROJO "No se encontro el archivo %s en la carpeta del programa." RESET "\n", ARCHIVO_DATOS);
        pausa();
        return;
    }

    // fgets lee una linea por vuelta hasta que se terminan
    while (fgets(linea, sizeof(linea), f) != NULL) {
        linea[strcspn(linea, "\r\n")] = '\0';  // saca el salto de linea del final

        char *texto = linea;
        if (strncmp(linea, "\xEF\xBB\xBF", 3) == 0) {
            texto = linea + 3;  // algunos editores agregan 3 bytes invisibles al principio
        }

        if (strncmp(texto, "INGREDIENTE;", 12) == 0) {
            ingredientes += importarIngrediente(pizzeria, texto);
        } else if (strncmp(texto, "PIZZA;", 6) == 0) {
            pizzas += importarPizza(pizzeria, texto);
        }
        // las demas lineas (vacias o comentarios) se ignoran
    }

    fclose(f);

    guardarIngredientes(pizzeria);
    guardarPizzas(pizzeria);

    printf(VERDE "Importacion terminada: %d ingredientes y %d pizzas." RESET "\n", ingredientes, pizzas);
    pausa();
}

// carga un ingrediente desde una linea del archivo. Devuelve 1 si lo agrego y 0 si no
// la linea puede traer o no el precio y la porcion del extra: nombre;unidad;stock[;precio_extra;porcion_extra]
int importarIngrediente(Pizzeria *pizzeria, char linea[]) {
    int i = (*pizzeria).cantidad_ingredientes;
    char nombre[MAX_NOMBRE];
    char unidad[MAX_UNIDAD];
    float precio_extra = 0;
    int porcion_extra = 0;
    int stock;

    // %[^;] lee texto hasta el proximo punto y coma (el numero es el maximo de caracteres)
    // sscanf devuelve cuantos datos leyo: 3 sin extra y 5 con extra
    int leidos = sscanf(linea, "INGREDIENTE;%49[^;];%9[^;];%d;%f;%d", nombre, unidad, &stock, &precio_extra, &porcion_extra);

    if ((leidos != 3 && leidos != 5) || stock < 0 || precio_extra < 0 || (precio_extra > 0 && porcion_extra <= 0)) {
        printf(ROJO "Linea invalida: %s" RESET "\n", linea);
        return 0;
    }

    if (i >= MAX_INGREDIENTES) {
        printf(ROJO "No hay espacio para el ingrediente %s." RESET "\n", nombre);
        return 0;
    }

    if (buscarIngredientePorNombre(pizzeria, nombre) != -1) {
        printf(ROJO "El ingrediente %s ya existe: se omite." RESET "\n", nombre);
        return 0;
    }

    strcpy((*pizzeria).ingredientes[i].nombre, nombre);
    strcpy((*pizzeria).ingredientes[i].unidad, unidad);
    (*pizzeria).ingredientes[i].stock = stock;
    (*pizzeria).ingredientes[i].precio_extra = precio_extra;
    (*pizzeria).ingredientes[i].porcion_extra = porcion_extra;
    (*pizzeria).cantidad_ingredientes++;

    return 1;
}

// carga una pizza (con su receta) desde una linea del archivo. Devuelve 1 si la agrego y 0 si no
int importarPizza(Pizzeria *pizzeria, char linea[]) {
    int i = (*pizzeria).cantidad_pizzas;
    char nombre[MAX_NOMBRE];
    char receta[MAX_LINEA] = "";
    float precio;
    int stock;

    // sscanf devuelve cuantos datos leyo: 3 si no tiene receta y 4 si la tiene
    int leidos = sscanf(linea, "PIZZA;%49[^;];%f;%d;%299[^\n]", nombre, &precio, &stock, receta);

    if (leidos < 3 || precio <= 0 || stock < 0) {
        printf(ROJO "Linea invalida: %s" RESET "\n", linea);
        return 0;
    }

    if (i >= MAX_PIZZAS) {
        printf(ROJO "No hay espacio para la pizza %s." RESET "\n", nombre);
        return 0;
    }

    if (buscarPizzaPorNombre(pizzeria, nombre) != -1) {
        printf(ROJO "La pizza %s ya existe: se omite." RESET "\n", nombre);
        return 0;
    }

    strcpy((*pizzeria).pizzas[i].nombre, nombre);
    (*pizzeria).pizzas[i].precio = precio;
    (*pizzeria).pizzas[i].stock = stock;
    generarCodigoPizza(pizzeria, (*pizzeria).pizzas[i].codigo);
    (*pizzeria).pizzas[i].cantidad_items = 0;

    if (leidos == 4) {
        importarReceta(pizzeria, &(*pizzeria).pizzas[i], receta);
    }

    (*pizzeria).cantidad_pizzas++;
    return 1;
}

// separa el texto de la receta (ingrediente:cantidad,ingrediente:cantidad) y la carga
void importarReceta(Pizzeria *pizzeria, Pizza *pizza, char texto[]) {
    // strtok corta el texto en partes separadas por coma, una por vuelta
    char *item = strtok(texto, ",");

    while (item != NULL && (*pizza).cantidad_items < MAX_RECETA) {
        char nombre[MAX_NOMBRE];
        int cantidad;

        if (sscanf(item, "%49[^:]:%d", nombre, &cantidad) == 2 && cantidad > 0) {
            int ingrediente = buscarIngredientePorNombre(pizzeria, nombre);

            if (ingrediente != -1) {
                (*pizza).receta[(*pizza).cantidad_items].indice_ingrediente = ingrediente;
                (*pizza).receta[(*pizza).cantidad_items].cantidad = cantidad;
                (*pizza).cantidad_items++;
            } else {
                printf(ROJO "Ingrediente %s no encontrado: se omite en la receta de %s." RESET "\n", nombre, (*pizza).nombre);
            }
        }

        item = strtok(NULL, ",");
    }
}

// devuelve la posicion de la pizza con ese nombre, o -1 si no existe
int buscarPizzaPorNombre(Pizzeria *pizzeria, char nombre[]) {
    for (int i = 0; i < (*pizzeria).cantidad_pizzas; i++) {
        if (sonIguales((*pizzeria).pizzas[i].nombre, nombre)) {
            return i;
        }
    }

    return -1;
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

    int opcion = pedirOpcionRango("1. Ocupada | 2. Libre: ", 1, 2);

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

    int numero_pedido = leerEntero("\nNumero de pedido: ");
    int i = buscarPedidoPorNumero(pizzeria, numero_pedido);

    // solo se puede marcar como listo un pedido que exista y este pendiente
    if (i != -1 && sonIguales((*pizzeria).pedidos[i].estado, "PENDIENTE")) {
        strcpy((*pizzeria).pedidos[i].estado, "LISTO");
        guardarPedidos(pizzeria);
        printf(VERDE "Pedido %d marcado como listo." RESET "\n", numero_pedido);
    } else {
        printf(ROJO "El pedido %d no esta pendiente." RESET "\n", numero_pedido);
    }

    pausa();
}

// muestra los pedidos en estado PENDIENTE con su cliente y sus extras, y devuelve cuantos hay
int mostrarPedidosPendientes(Pizzeria *pizzeria) {
    int cantidad = 0;

    for (int i = 0; i < MAX_PEDIDOS; i++) {
        if (sonIguales((*pizzeria).pedidos[i].estado, "PENDIENTE")) {
            int c = buscarClientePorId(pizzeria, (*pizzeria).pedidos[i].id_cliente);
            printf("Pedido %d: %d x %s - cliente: %s",
                   (*pizzeria).pedidos[i].numero,
                   (*pizzeria).pedidos[i].cantidad,
                   (*pizzeria).pedidos[i].pizza.nombre,
                   (c != -1) ? (*pizzeria).clientes[c].nombre : "-");

            // si pidio ingredientes extra, se muestran al final de la linea
            if ((*pizzeria).pedidos[i].cantidad_extras > 0) {
                printf(" - extras:");
                for (int k = 0; k < (*pizzeria).pedidos[i].cantidad_extras; k++) {
                    printf(" %s x%d", (*pizzeria).pedidos[i].extras[k].nombre, (*pizzeria).pedidos[i].extras[k].cantidad);
                }
            }
            printf("\n");
            cantidad++;
        }
    }

    return cantidad;
}

// devuelve la posicion del pedido con ese numero, o -1 si no esta
int buscarPedidoPorNumero(Pizzeria *pizzeria, int numero) {
    // el numero 0 es el de los lugares libres, no es un pedido real
    if (numero <= 0) {
        return -1;
    }

    for (int i = 0; i < MAX_PEDIDOS; i++) {
        if ((*pizzeria).pedidos[i].numero == numero) {
            return i;
        }
    }

    return -1;
}

// ======================== CLIENTES ========================

// pide el DNI y devuelve el id del cliente (lo registra si es nuevo), o -1 si no se pudo
int identificarCliente(Pizzeria *pizzeria) {
    char dni[MAX_DNI];

    pedirDni(dni);

    int i = buscarClientePorDni(pizzeria, dni);

    if (i != -1) {
        printf("Bienvenido de nuevo, %s!\n", (*pizzeria).clientes[i].nombre);
        return (*pizzeria).clientes[i].id;
    }

    return registrarCliente(pizzeria, dni);  // no lo conocemos: se registra
}

// pide el nombre y guarda un cliente nuevo. Devuelve su id, o -1 si no hay espacio
int registrarCliente(Pizzeria *pizzeria, char dni[]) {
    int i = (*pizzeria).cantidad_clientes;

    if (i >= MAX_CLIENTES) {
        printf(ROJO "No hay espacio para registrar mas clientes." RESET "\n");
        return -1;
    }

    // el nombre no puede quedar vacio
    do {
        leerCadena((*pizzeria).clientes[i].nombre, MAX_NOMBRE, "Es su primera compra. Ingrese su nombre: ");
        if (strlen((*pizzeria).clientes[i].nombre) == 0) {
            printf(ROJO "El nombre no puede estar vacio." RESET "\n");
        }
    } while (strlen((*pizzeria).clientes[i].nombre) == 0);

    strcpy((*pizzeria).clientes[i].dni, dni);
    (*pizzeria).clientes[i].id = i + 1;  // los id se numeran desde 1

    (*pizzeria).cantidad_clientes++;
    guardarClientes(pizzeria);

    return (*pizzeria).clientes[i].id;
}

// devuelve la posicion del cliente con ese DNI, o -1 si no esta registrado
int buscarClientePorDni(Pizzeria *pizzeria, char dni[]) {
    for (int i = 0; i < (*pizzeria).cantidad_clientes; i++) {
        if (sonIguales((*pizzeria).clientes[i].dni, dni)) {
            return i;
        }
    }

    return -1;
}

// devuelve la posicion del cliente con ese id, o -1 si no existe
int buscarClientePorId(Pizzeria *pizzeria, int id) {
    for (int i = 0; i < (*pizzeria).cantidad_clientes; i++) {
        if ((*pizzeria).clientes[i].id == id) {
            return i;
        }
    }

    return -1;
}

// un DNI valido tiene 7 u 8 caracteres y todos son numeros
int dniValido(char dni[]) {
    int largo = strlen(dni);

    if (largo < 7 || largo > 8) {
        return 0;
    }

    for (int i = 0; i < largo; i++) {
        if (!isdigit((unsigned char)dni[i])) {
            return 0;
        }
    }

    return 1;
}

// pide el DNI hasta que sea valido
void pedirDni(char dni[]) {
    do {
        leerCadena(dni, MAX_DNI, "Ingrese su DNI (solo numeros): ");

        if (!dniValido(dni)) {
            printf(ROJO "DNI invalido: tienen que ser 7 u 8 numeros." RESET "\n");
        }
    } while (!dniValido(dni));
}

// el cliente consulta sus compras con su DNI
void verMisPedidos(Pizzeria *pizzeria) {
    char dni[MAX_DNI];

    limpiaPantalla();
    printf(AMARILLO "===== MIS PEDIDOS =====" RESET "\n");
    pedirDni(dni);

    int i = buscarClientePorDni(pizzeria, dni);

    if (i == -1) {
        printf(ROJO "No hay compras registradas con ese DNI." RESET "\n");
        pausa();
        return;
    }

    printf("Cliente: %s\n\n", (*pizzeria).clientes[i].nombre);

    if (mostrarComprasDeCliente(pizzeria, (*pizzeria).clientes[i].id) == 0) {
        printf("Todavia no tiene compras.\n");
    }

    pausa();
}

// recorre las facturas y muestra las de un cliente con el estado de su pedido
// devuelve cuantas encontro
int mostrarComprasDeCliente(Pizzeria *pizzeria, int id_cliente) {
    Factura factura;
    char fecha[20];
    int cantidad = 0;
    FILE *f = fopen(ARCHIVO_FACTURAS, "rb");

    if (f == NULL) {
        return 0;
    }

    while (fread(&factura, sizeof(Factura), 1, f) == 1) {
        if (factura.id_cliente == id_cliente) {
            // si el pedido ya no esta en el array, hace tiempo que se entrego
            int p = buscarPedidoPorNumero(pizzeria, factura.numero);
            char *estado = (p != -1) ? (*pizzeria).pedidos[p].estado : "ENTREGADO";

            formatearFecha(factura.fecha, fecha);
            printf("Pedido %d - %s - %d x %s - $%.2f - %s\n",
                   factura.numero, fecha, factura.cantidad, factura.pizza, factura.total, estado);
            cantidad++;
        }
    }

    fclose(f);
    return cantidad;
}

// ======================== CLIENTE (COMPRA) ========================

// recorre todo el proceso de una compra, paso por paso
void comprarPizza(Pizzeria *pizzeria) {
    limpiaPantalla();
    printf(AMARILLO "=== COMPRA DE PIZZA ===" RESET "\n");

    // si todos los pedidos estan ocupados no se puede tomar otro
    if (buscarPedidoLibre(pizzeria) == -1) {
        printf(ROJO "No podemos tomar mas pedidos por el momento." RESET "\n");
        pausa();
        return;
    }

    // primero se muestran las pizzas: si no hay ninguna, no tiene sentido seguir
    if (mostrarPizzasDisponibles(pizzeria) == 0) {
        printf(ROJO "No hay pizzas disponibles." RESET "\n");
        pausa();
        return;
    }

    // el cliente se identifica antes de comprar
    int id_cliente = identificarCliente(pizzeria);

    if (id_cliente == -1) {
        pausa();
        return;
    }

    int indice = pedirPizza(pizzeria);

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

    // ingredientes extra que el cliente quiera agregarle a cada pizza
    Extra extras[MAX_EXTRAS];
    char texto_extras[MAX_TEXTO_EXTRAS];
    int cantidad_extras = pedirExtras(pizzeria, cantidad, extras);
    float precio_extras = describirExtras(extras, cantidad_extras, texto_extras);

    float descuento = jugarPorDescuento();
    float total = calcularPrecioFinal((*pizzeria).pizzas[indice].precio + precio_extras, cantidad, descuento);

    // se registra el pedido (para el empleado), la factura y se descuenta el stock
    int numero_pedido = registrarPedido(pizzeria, indice, cantidad, id_cliente, extras, cantidad_extras);
    Factura factura = crearFactura(pizzeria, numero_pedido, id_cliente, indice, cantidad, precio_extras, texto_extras, descuento, total);
    registrarVenta(pizzeria, indice, cantidad, extras, cantidad_extras);
    guardarCompra(pizzeria, &factura);

    retardo(0.5);
    printf(VERDE "\nPedido realizado con exito!" RESET "\n");
    mostrarFactura(pizzeria, &factura);
    printf("Su pedido es el numero %d y estara listo en breve!\n", numero_pedido);

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

// pide un codigo y devuelve la posicion de esa pizza, o -1 si no existe
int pedirPizza(Pizzeria *pizzeria) {
    char codigo[MAX_CODIGO];

    leerCadena(codigo, MAX_CODIGO, "\nIngrese codigo de la pizza: ");
    convertirMayusculas(codigo);  // asi pz1 y PZ1 son lo mismo

    return buscarPizzaPorCodigo(pizzeria, codigo);
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

// pregunta cuantas pizzas quiere pedir, entre 1 y el stock que hay
int pedirCantidad(Pizza *pizza) {
    char mensaje[100];

    sprintf(mensaje, "Cuantas pizzas de %s quiere pedir? (de 1 a %d): ", (*pizza).nombre, (*pizza).stock);
    return pedirEnteroRango(mensaje, 1, (*pizza).stock);
}

// deja elegir ingredientes extra para las pizzas. Devuelve cuantos extras distintos se eligieron
int pedirExtras(Pizzeria *pizzeria, int cantidad_pizzas, Extra extras[]) {
    int cantidad_extras = 0;

    while (cantidad_extras < MAX_EXTRAS) {
        // si no queda ningun extra para ofrecer, no se pregunta nada
        if (mostrarExtrasDisponibles(pizzeria, cantidad_pizzas, extras, cantidad_extras) == 0) {
            break;
        }

        if (!preguntarSiNo("Desea agregar un ingrediente extra a sus pizzas? (S/N): ")) {
            break;
        }

        cantidad_extras += elegirExtra(pizzeria, cantidad_pizzas, extras, cantidad_extras);
    }

    return cantidad_extras;
}

// muestra los ingredientes que se pueden elegir como extra y devuelve cuantos son
int mostrarExtrasDisponibles(Pizzeria *pizzeria, int cantidad_pizzas, Extra extras[], int cantidad_extras) {
    int cantidad = 0;

    for (int i = 0; i < (*pizzeria).cantidad_ingredientes; i++) {
        if (extraElegible(pizzeria, i, cantidad_pizzas, extras, cantidad_extras)) {
            if (cantidad == 0) {
                printf("\nIngredientes extra disponibles (precio por pizza):\n");
            }
            printf("%d. %s: +$%.2f\n", i + 1, (*pizzeria).ingredientes[i].nombre, (*pizzeria).ingredientes[i].precio_extra);
            cantidad++;
        }
    }

    return cantidad;
}

// un ingrediente se puede elegir como extra si tiene precio de extra, alcanza para todas
// las pizzas y todavia no fue elegido
int extraElegible(Pizzeria *pizzeria, int indice, int cantidad_pizzas, Extra extras[], int cantidad_extras) {
    if ((*pizzeria).ingredientes[indice].precio_extra <= 0) {
        return 0;
    }

    if ((*pizzeria).ingredientes[indice].stock < (*pizzeria).ingredientes[indice].porcion_extra * cantidad_pizzas) {
        return 0;
    }

    for (int k = 0; k < cantidad_extras; k++) {
        if (sonIguales(extras[k].nombre, (*pizzeria).ingredientes[indice].nombre)) {
            return 0;
        }
    }

    return 1;
}

// el cliente elige un extra y cuantos quiere en cada pizza. Devuelve 1 si lo agrego y 0 si no
int elegirExtra(Pizzeria *pizzeria, int cantidad_pizzas, Extra extras[], int cantidad_extras) {
    char mensaje[100];

    int i = pedirOpcionRango("Numero de ingrediente: ", 1, (*pizzeria).cantidad_ingredientes) - 1;

    if (!extraElegible(pizzeria, i, cantidad_pizzas, extras, cantidad_extras)) {
        printf(ROJO "Ese ingrediente no se puede elegir como extra." RESET "\n");
        return 0;
    }

    // lo maximo es lo que alcanza el stock para todas las pizzas (hasta MAX_UNIDADES_EXTRA)
    int maximo = (*pizzeria).ingredientes[i].stock / ((*pizzeria).ingredientes[i].porcion_extra * cantidad_pizzas);

    if (maximo > MAX_UNIDADES_EXTRA) {
        maximo = MAX_UNIDADES_EXTRA;
    }

    sprintf(mensaje, "Cuantos extras de %s por pizza? (1-%d): ", (*pizzeria).ingredientes[i].nombre, maximo);

    strcpy(extras[cantidad_extras].nombre, (*pizzeria).ingredientes[i].nombre);
    extras[cantidad_extras].cantidad = pedirEnteroRango(mensaje, 1, maximo);
    extras[cantidad_extras].precio = (*pizzeria).ingredientes[i].precio_extra;

    return 1;
}

// arma el texto con los extras (para la factura) y devuelve cuanto suman por cada pizza
float describirExtras(Extra extras[], int cantidad_extras, char texto[]) {
    float precio = 0;
    char parte[MAX_NOMBRE + 20];

    strcpy(texto, "");

    for (int k = 0; k < cantidad_extras; k++) {
        precio += extras[k].precio * extras[k].cantidad;

        sprintf(parte, "%s%s x%d", (k > 0) ? ", " : "", extras[k].nombre, extras[k].cantidad);
        strcat(texto, parte);  // strcat agrega un texto al final de otro
    }

    return precio;
}

// resta del stock de cada ingrediente lo que se uso en los extras de todas las pizzas
void descontarExtras(Pizzeria *pizzeria, Extra extras[], int cantidad_extras, int cantidad_pizzas) {
    for (int k = 0; k < cantidad_extras; k++) {
        int i = buscarIngredientePorNombre(pizzeria, extras[k].nombre);

        (*pizzeria).ingredientes[i].stock -= (*pizzeria).ingredientes[i].porcion_extra * extras[k].cantidad * cantidad_pizzas;
    }
}

// ofrece jugar para ganar descuento y devuelve el descuento ganado (0 si no juega o pierde)
float jugarPorDescuento() {
    float descuento = 0.0;
    char respuesta[10];

    leerCadena(respuesta, sizeof(respuesta), "Jugar para ganar descuento? (S/N): ");

    if (toupper(respuesta[0]) == 'S') {
        printf("Elija juego:\n");
        printf("1) Adivinar numero (%.0f%% descuento)\n", DESCUENTO_ADIVINAR * 100);
        printf("2) Suma rapida (%.0f%% descuento)\n", DESCUENTO_SUMA * 100);

        int opcion_juego = leerOpcion("");

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
int registrarPedido(Pizzeria *pizzeria, int indice_pizza, int cantidad, int id_cliente, Extra extras[], int cantidad_extras) {
    int i = buscarPedidoLibre(pizzeria);

    (*pizzeria).pedidos[i].numero = (*pizzeria).proximo_pedido;  // el numero no se repite nunca
    (*pizzeria).proximo_pedido++;
    strcpy((*pizzeria).pedidos[i].estado, "PENDIENTE");
    (*pizzeria).pedidos[i].pizza = (*pizzeria).pizzas[indice_pizza];  // copia de la pizza pedida
    (*pizzeria).pedidos[i].cantidad = cantidad;
    (*pizzeria).pedidos[i].id_cliente = id_cliente;

    // se copian los extras al pedido para que el empleado los vea
    for (int k = 0; k < cantidad_extras; k++) {
        (*pizzeria).pedidos[i].extras[k] = extras[k];
    }
    (*pizzeria).pedidos[i].cantidad_extras = cantidad_extras;

    return (*pizzeria).pedidos[i].numero;
}

// descuenta del stock las pizzas vendidas y los ingredientes usados en los extras
void registrarVenta(Pizzeria *pizzeria, int indice_pizza, int cantidad, Extra extras[], int cantidad_extras) {
    (*pizzeria).pizzas[indice_pizza].stock -= cantidad;
    descontarExtras(pizzeria, extras, cantidad_extras, cantidad);
}

// deja guardado todo lo que cambio con la compra: stock, ingredientes, pedidos y la factura
void guardarCompra(Pizzeria *pizzeria, Factura *factura) {
    guardarPizzas(pizzeria);
    guardarIngredientes(pizzeria);
    guardarPedidos(pizzeria);
    agregarRegistro(ARCHIVO_FACTURAS, factura, sizeof(Factura));
}

// ======================== FACTURAS ========================

// arma una factura con los datos de la compra
// (en C una funcion puede devolver un struct entero)
Factura crearFactura(Pizzeria *pizzeria, int numero_pedido, int id_cliente, int indice_pizza, int cantidad, float precio_extras, char texto_extras[], float descuento, float total) {
    Factura factura;

    factura.numero = numero_pedido;  // la factura lleva el mismo numero que el pedido
    factura.fecha = time(NULL);      // fecha y hora actuales
    factura.id_cliente = id_cliente;
    strcpy(factura.pizza, (*pizzeria).pizzas[indice_pizza].nombre);
    factura.cantidad = cantidad;
    factura.precio_unitario = (*pizzeria).pizzas[indice_pizza].precio;
    strcpy(factura.extras, texto_extras);
    factura.precio_extras = precio_extras;
    factura.descuento = descuento;
    factura.total = total;

    // el empleado de turno es el que tomo el pedido
    if (strlen((*pizzeria).empleado_turno) > 0) {
        strcpy(factura.empleado, (*pizzeria).empleado_turno);
        strcpy(factura.turno, (*pizzeria).turno_actual);
    } else {
        strcpy(factura.empleado, "SIN ASIGNAR");
        strcpy(factura.turno, "-");
    }

    return factura;
}

// muestra una factura con formato
void mostrarFactura(Pizzeria *pizzeria, Factura *factura) {
    char fecha[20];
    int c = buscarClientePorId(pizzeria, (*factura).id_cliente);

    formatearFecha((*factura).fecha, fecha);

    printf("\n------------ FACTURA Nro %d ------------\n", (*factura).numero);
    printf("Fecha:   %s\n", fecha);
    if (c != -1) {
        printf("Cliente: %s (DNI %s)\n", (*pizzeria).clientes[c].nombre, (*pizzeria).clientes[c].dni);
    }
    printf("Atendido por: %s (turno %s)\n", (*factura).empleado, (*factura).turno);
    printf("Detalle: %d x %s ($%.2f c/u)\n", (*factura).cantidad, (*factura).pizza, (*factura).precio_unitario);
    if (strlen((*factura).extras) > 0) {
        printf("Extras en cada pizza: %s (+$%.2f c/u)\n", (*factura).extras, (*factura).precio_extras);
    }
    if ((*factura).descuento > 0) {
        printf("Descuento: %.0f%%\n", (*factura).descuento * 100);
    }
    printf("TOTAL:   " AMARILLO "$%.2f" RESET "\n", (*factura).total);
    printf("----------------------------------------\n");
}

// convierte una fecha en un texto con formato dia/mes/año hora:minutos
// el texto tiene que tener lugar para 20 caracteres
void formatearFecha(time_t fecha, char texto[]) {
    struct tm *t = localtime(&fecha);

    strftime(texto, 20, "%d/%m/%Y %H:%M", t);
}

// ======================== JUEGOS ========================

// el cliente tiene que adivinar un numero del 1 al 10
void juegoAdivinarNumero(float *descuento) {
    int secreto = generarNumAleatorio(1, 10);
    int intento;

    intento = pedirEnteroRango("\n Adivina el numero (1-10): ", 1, 10);

    if (intento == secreto) {
        *descuento = DESCUENTO_ADIVINAR;
        printf(VERDE "Correcto! Ganaste %.0f%% de descuento!" RESET "\n", DESCUENTO_ADIVINAR * 100);
    } else {
        printf(ROJO "Incorrecto. El numero era %d. No hay descuento." RESET "\n", secreto);
    }
}

// el cliente tiene que resolver una suma de dos numeros del 1 al 10 antes de que se acabe el tiempo
void juegoSumaRapida(float *descuento) {
    int a = generarNumAleatorio(1, 10);
    int b = generarNumAleatorio(1, 10);
    int resp;
    time_t inicio;
    time_t fin;

    char mensaje[50];
    sprintf(mensaje, "\nCuanto es %d + %d?: ", a, b); // sprintf es un printf que guarda el resultado en una variable en lugar de mostrarlo

    printf("Tiene %d segundos para responder!\n", TIEMPO_LIMITE_SUMA);

    inicio = time(NULL);       // hora en que se mostro la suma
    resp = leerEntero(mensaje);
    fin = time(NULL);          // hora en que respondio

    // difftime calcula los segundos que pasaron entre las dos horas
    if (difftime(fin, inicio) > TIEMPO_LIMITE_SUMA) {
        printf(ROJO "Se acabo el tiempo. No hay descuento." RESET "\n");
    } else if (resp == a + b) {
        *descuento = DESCUENTO_SUMA;
        printf(VERDE "Bien! Ganaste %.0f%% de descuento!" RESET "\n", DESCUENTO_SUMA * 100);
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

// hace una pregunta de si o no: devuelve 1 si la respuesta empieza con S (o s)
int preguntarSiNo(char mensaje[]) {
    char respuesta[10];

    leerCadena(respuesta, sizeof(respuesta), mensaje);

    return toupper(respuesta[0]) == 'S';
}

// convierte lo que escribio el usuario en un numero: acepta cifras ("3") o letras ("tres")
// devuelve -1 si no se entiende
int textoANumero(char texto[]) {
    char *palabras[] = {"CERO", "UNO", "DOS", "TRES", "CUATRO", "CINCO", "SEIS", "SIETE", "OCHO",
                        "NUEVE", "DIEZ", "ONCE", "DOCE", "TRECE", "CATORCE", "QUINCE"};
    char copia[MAX_NOMBRE];
    char palabra[MAX_NOMBRE];
    char resto;
    int numero;

    strncpy(copia, texto, MAX_NOMBRE - 1);
    copia[MAX_NOMBRE - 1] = '\0';
    convertirMayusculas(copia);  // asi tres, TRES y Tres son lo mismo

    // tiene que haber una sola palabra (sscanf devuelve cuantas leyo)
    if (sscanf(copia, "%49s %c", palabra, &resto) != 1) {
        return -1;
    }

    // si escribio el numero con cifras
    if (sscanf(palabra, "%d%c", &numero, &resto) == 1) {
        return numero;
    }

    // si escribio el numero con letras: se busca en la lista (la posicion es el numero)
    for (int i = 0; i < 16; i++) {
        if (sonIguales(palabra, palabras[i])) {
            return i;
        }
    }

    return -1;
}

// lee una opcion y la devuelve como numero (-1 si no se entiende)
int leerOpcion(char mensaje[]) {
    char texto[MAX_NOMBRE] = "";

    leerCadena(texto, MAX_NOMBRE, mensaje);

    return textoANumero(texto);
}

// pide una opcion entre minimo y maximo (con cifras o con letras) hasta que sea valida
int pedirOpcionRango(char mensaje[], int minimo, int maximo) {
    int n;

    do {
        n = leerOpcion(mensaje);

        if (n < minimo || n > maximo) {
            printf(ROJO "Opcion invalida. Elija entre %d y %d (con numeros o letras). " RESET, minimo, maximo);
        }
    } while (n < minimo || n > maximo);

    return n;
}

// ======================== MENUS DE TEXTO ========================

void Grafico_MenuGerente(void) {
    printf(MAGENTA);
    printf("\n");
    printf("  ==========================================\n");
    printf("                MENU DEL GERENTE\n");
    printf("  ==========================================\n");
    printf("    1.  Ver stock (pizzas e ingredientes)\n");
    printf("    2.  Agregar nuevo sabor\n");
    printf("    3.  Eliminar sabor\n");
    printf("    4.  Agregar ingrediente\n");
    printf("    5.  Eliminar ingrediente\n");
    printf("    6.  Comprar ingredientes\n");
    printf("    7.  Ver compras\n");
    printf("    8.  Ver ganancias\n");
    printf("    9.  Ver facturas\n");
    printf("    10. Crear usuario\n");
    printf("    11. Eliminar usuario\n");
    printf("    12. Ver usuarios\n");
    printf("    13. Importar datos (datos.txt)\n");
    printf("    14. Volver\n");
    printf("    15. Salir\n");
    printf("  ==========================================\n");
    printf("    Elija una opcion (numero o letras): ");
    printf(RESET);
}

void Grafico_MenuCliente(void) {
    printf(VERDE);
    printf("\n");
    printf("  ==========================================\n");
    printf("                MENU DEL CLIENTE\n");
    printf("  ==========================================\n");
    printf("    1. Comprar pizza\n");
    printf("    2. Ver mis pedidos\n");
    printf("    3. Volver\n");
    printf("    4. Salir\n");
    printf("  ==========================================\n");
    printf("    Elija una opcion (numero o letras): ");
    printf(RESET);
}

void Grafico_MenuEmpleado(void) {
    printf(CIAN);
    printf("\n");
    printf("  ==========================================\n");
    printf("               MENU DEL EMPLEADO\n");
    printf("  ==========================================\n");
    printf("    1. Ver mesas\n");
    printf("    2. Marcar mesa ocupada o libre\n");
    printf("    3. Marcar pedido listo\n");
    printf("    4. Ver stock (pizzas e ingredientes)\n");
    printf("    5. Elaborar pizzas\n");
    printf("    6. Volver\n");
    printf("    7. Salir\n");
    printf("  ==========================================\n");
    printf("    Elija una opcion (numero o letras): ");
    printf(RESET);
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
    printf("                @@@       @@@      @@@@@@=       @@   @@@       =@@   @@        @@@@@*      @@@  @@@@@         @@@@@  @@.       @@@@@@                                                                                                   \n");
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
