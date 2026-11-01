
#ifndef UTILES_H_ // guardas de inclusión, para evitar que el contenido del archivo se incluya más de una vez
#define UTILES_H_
/*
Las guardas de inclusión (también llamadas include guards)
son una técnica en C y C++
que evita que el contenido de un archivo de encabezado (.h)
sea incluido más de una vez durante la compilación.
#ifndef UTILES_H_: pregunta al preprocesador
si no se ha definido el identificador UTILES_H_.

#define UTILES_H_: si no estaba definido, ahora lo define.
Esto marca que ya se incluyó el archivo.

#endif: cierra la condición #ifndef.
*/

#include <stdlib.h> // recordar que los corchetes angulares se utilizan para librerias standard
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <conio.h>
#include <stdbool.h> // Para usar el tipo bool
#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif


// *********************************************************************************
// simplemente hace una linea de asteriscos, para mejorar la presentación en consola
// *********************************************************************************
void linea(int x)
{
    int i;
    for (i = 0; i < x; i++)
        printf("*");
    printf("\n");
}
// *********************************************************************************
// Función para limpiar el búfer de entrada (stdin)
// *********************************************************************************
void limpiarBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}
// *********************************************************************************
// Función para configurar la consola en UTF-8 (Windows)
// *********************************************************************************
void utf8()
{
#ifdef _WIN32
    system("chcp 65001");
    printf("\n");
#endif
}
// *********************************************************************************
// limpia pantalla de forma multiplataforma
// *********************************************************************************
void limpiaPantalla()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
// Función para darle color a la consola (Windows)
void Color()
{
#ifdef _WIN32
    system("color 91");
#endif
}
// *********************************************************************************
// Función para pulsar una tecla para continuar, con mensaje personalizado
// *********************************************************************************
void pausa()
{
#ifdef _WIN32
    printf("\n");
    system("pause");
#else
    printf("Presione ENTER para continuar...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ; // limpiar buffer
    getchar();
    printf("\n"); // esperar ENTER real
#endif
}

// *********************************************************************************
// Funciones para leer enteros de forma segura, con validación de entrada
// *********************************************************************************
int leerEntero(const char *mensaje)
{
    char buffer[100];
    int numero;
    int flag = 1;

    while (flag)
    {
        printf("%s", mensaje);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL)
        {
            if (sscanf(buffer, "%d", &numero) == 1)
            {
                flag = 0;
            }
            else
            {
                printf("Entrada inválida. Intente nuevamente.\n");
            }
        }
    }
    return numero;
}
// *********************************************************************************
// Función para leer un entero dentro de un rango específico
// *********************************************************************************
int pedirEnteroRango(const char *msj, int min, int max)
{
    int n;
    do
    {
        n = leerEntero(msj);
        if (n < min || n > max)
        {
            printf("Fuera de rango [%d - %d]. ", min, max);
        }
    } while (n < min || n > max);
    return n;
}
// *********************************************************************************
// Función para leer un número flotante de forma segura, con validación de entrada
// *********************************************************************************
float leerFlotante(const char *mensaje, float *numero)
{
    char buffer[100];
    int flag = 1;

    while (flag)
    {
        printf("%s", mensaje);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL)
        {
            if (sscanf(buffer, "%f", numero) == 1)
            {
                flag = 0;
            }
            else
            {
                printf("Entrada inválida. Intente nuevamente.\n");
            }
        }
    }
    return *numero;
}
// *********************************************************************************
// genera un numero aleatorio entre valorMin y valorMax (ambos inclusive)
// *********************************************************************************
int generarNumAleatorio(int valorMin, int valorMax)
{
    // srand(time(NULL));
    return (valorMin + (rand() % (valorMax - valorMin + 1)));
}
// *********************************************************************************
// Función para comparar dos cadenas de caracteres (strings) utilizando strcmp
// Devuelve 1 si son iguales, 0 si son diferentes
// *********************************************************************************
int sonIguales(char *texto, char *texto2)
{
    return strcmp(texto, texto2) == 0;
}
// *********************************************************************************
// funcion que invierte dos valores enteros usando punteros
// *********************************************************************************
void swap(int *a, int *b)
{
    int aux = *a;
    *a = *b;
    *b = aux;
}
// *********************************************************************************
// retardo para windows o linux puntos suspensivos para simular carga o espera
// *********************************************************************************

void retardo(float seg)
{
    printf("CARGANDO");
#ifdef _WIN32
    for (int i = 0; i < 3; i++)
    {
        Sleep((DWORD)(seg * 1000));  // Sleep recibe millisegundos en Windows
        printf(".");
    }
#else
    for (int i = 0; i < 3; i++)
    {
        usleep((useconds_t)(seg * 1000000));  // usleep recibe microsegundos en Linux
        printf(".");
    }
#endif
    printf("\n");
}
/*
  Función para leer una cadena de caracteres (string) de forma segura.
  - Utiliza fgets para evitar problemas de desbordamiento (overflow) y para
    consumir el carácter de nueva línea ('\n').
  - Elimina el '\n' del final de la cadena si existe.
*/
void leerCadena(char *destino, int tamano, const char *mensaje)
{
    printf("%s", mensaje);

    // 1. Captura con fgets (incluye \n si cabe)
    if (fgets(destino, tamano, stdin) != NULL)
    {
        // 2. Busca y elimina el carácter de nueva línea ('\n')
        //    Esto es crucial para que la cadena final NO contenga el Enter.
        size_t longitud = strlen(destino);

        // Verifica si el último carácter leído (antes del '\0') es el '\n'
        if (longitud > 0 && destino[longitud - 1] == '\n')
        {
            destino[longitud - 1] = '\0'; // Reemplaza el '\n' por el terminador nulo
        }
        else
        {
            // 3. (Opcional) Limpia el resto del búfer si la entrada fue demasiado larga
            //    Esto maneja el caso de desbordamiento (overflow) donde \n no cupo
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
                ;
        }
    }
    // No se necesita else aquí, pero en aplicaciones reales podrías manejar el error de fgets.
}
// *********************************************************************************
// espera la pulsación de una tecla para continuar, con mensaje personalizado
// *********************************************************************************

#endif /* UTILES_H_ */