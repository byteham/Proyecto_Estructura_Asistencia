/***********************************************************************
 * Module:  Utilidades.h
 * Author:  Usuario
 * Purpose: Declaration of the class Utilidades
 ***********************************************************************/

#if !defined(__Asistencia_Utilidades_h)
#define __Asistencia_Utilidades_h

#include <string>

// Validaciones, calculos de horario, reloj y lectura por teclado.
// Las funciones "motivo...Invalido" no leen ni imprimen nada: devuelven ""
// si el dato es valido o el texto exacto del error si no lo es.
namespace Utilidades {

// --- Franjas de 2 horas: 1 = 07:00-09:00, 2 = 09:00-11:00, ... 7 = 19:00-21:00 ---
const int FRANJAS = 7;                 // cantidad de franjas por dia
const int HORA_INICIO = 7 * 60;        // las clases empiezan a las 07:00 (420 minutos)
const int DURACION_FRANJA = 120;       // cada franja dura 2 horas (120 minutos)
const int MINUTOS_PUNTUAL = 10;        // ingreso puntual hasta inicio + 10 min
const int MINUTOS_SALIDA = 10;         // la salida se registra en los ultimos 10 min
const int ANIO_MINIMO = 2000;          // menor anio aceptado en una fecha
const int ANIO_MAXIMO = 2100;          // mayor anio aceptado en una fecha
const int MAX_NOMBRE = 40;             // largo maximo de nombres y apellidos
const int MAX_MATERIA = 50;            // largo maximo del nombre de una materia

// La franja n es valida si esta entre 1 y 7.
inline bool franjaValida(int n) { return n >= 1 && n <= FRANJAS; }
// Minuto en que empieza la franja n (franja 1 -> 420 = 07:00).
inline int inicioFranja(int n) { return HORA_INICIO + (n - 1) * DURACION_FRANJA; }
// Minuto en que termina la franja n.
inline int finFranja(int n) { return inicioFranja(n) + DURACION_FRANJA; }
// Ultimo minuto en que el ingreso todavia es PUNTUAL.
inline int limitePuntual(int n) { return inicioFranja(n) + MINUTOS_PUNTUAL; }
// Minuto desde el cual se puede registrar la salida.
inline int inicioSalida(int n) { return finFranja(n) - MINUTOS_SALIDA; }
// true si el minuto cae dentro de la franja n.
inline bool franjaContiene(int n, int minutos) {
   return minutos >= inicioFranja(n) && minutos < finFranja(n);
}
// Franja que corresponde a un minuto del dia (0 si no hay clases a esa hora).
inline int franjaDeMinuto(int minutos) {
   if (minutos < HORA_INICIO || minutos >= HORA_INICIO + FRANJAS * DURACION_FRANJA) return 0;
   return (minutos - HORA_INICIO) / DURACION_FRANJA + 1;
}

std::string dosDigitos(int n);               // 5 -> "05"
std::string formatoHora(int minutos);        // 425 -> "07:05"
std::string textoFranja(int n);              // 1 -> "07:00-09:00"
std::string nombreDia(int dia);              // 0 = Domingo, 1 = Lunes ... 6 = Sabado

// --- Validaciones ---
std::string limpiar(const std::string& texto);       // quita espacios al inicio y al final
bool soloNumeros(const std::string& texto);
std::string motivoCedulaInvalida(const std::string& cedula);
std::string motivoNrcInvalido(const std::string& nrc);
std::string motivoTextoInvalido(const std::string& texto, const std::string& campo, int maximo,
                                const std::string& ejemplo);   // "ejemplo" se muestra en el mensaje de error
std::string motivoHoraInvalida(const std::string& hora);      // formato HH:MM
std::string motivoFechaInvalida(const std::string& fecha);    // formato dd/mm/aaaa
bool validarFecha(const std::string& fecha);
int horaAMinutos(const std::string& hora);           // "07:10" -> 430 (la hora ya debe ser valida)
int diaDeLaSemana(const std::string& fecha);         // 0 = domingo ... 6 = sabado (fecha ya valida)

// --- Reloj ---
void ahora(std::string& fecha, int& minutos, int& diaSemana);

// --- Teclado: todas repiten la pregunta mostrando el error hasta recibir un dato valido ---
std::string leerTexto(const char* mensaje);
int leerOpcion(const char* mensaje, int minimo, int maximo);
std::string leerCedula(const char* mensaje);
std::string leerNrc(const char* mensaje);
std::string leerTextoLetras(const char* mensaje, const std::string& campo, int maximo, const std::string& ejemplo);
int leerHora(const char* mensaje);                                   // devuelve minutos desde la medianoche
std::string leerFecha(const char* mensaje, int& diaSemana);
bool leerConfirmacion(const char* mensaje);                          // s/n
void pausar();

}  // namespace Utilidades

#endif
