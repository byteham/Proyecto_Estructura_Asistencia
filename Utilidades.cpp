/***********************************************************************
 * Module:  Utilidades.cpp
 * Author:  Usuario
 * Purpose: Implementation of the class Utilidades
 ***********************************************************************/

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS      // evita advertencias de localtime en Visual Studio
#endif
#include "Utilidades.h"
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <iostream>

namespace Utilidades {

// Funciones auxiliares privadas de este archivo (no se declaran en el .h).
namespace {

// true si el caracter es un digito '0'..'9'.
bool esDigito(char c) { return c >= '0' && c <= '9'; }

// true si el caracter es una letra A-Z o a-z (sin tildes ni enie).
bool esLetra(char c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); }

// Cantidad de dias que tiene un mes (febrero depende del anio bisiesto).
int diasDelMes(int mes, int anio) {
   if (mes == 2) {
      bool bisiesto = (anio % 4 == 0 && anio % 100 != 0) || anio % 400 == 0;
      return bisiesto ? 29 : 28;
   }
   if (mes == 4 || mes == 6 || mes == 9 || mes == 11) return 30;
   return 31;
}

}  // namespace

// ------------------------------------------------------------------ horario

////////////////////////////////////////////////////////////////////////
// Name:       Utilidades::dosDigitos(n)
// Purpose:    Implementation of Utilidades::dosDigitos()
// Parameters:
// - n
// Return:     std::string
////////////////////////////////////////////////////////////////////////
// Escribe un numero con al menos dos digitos: 5 -> "05", 12 -> "12".
std::string dosDigitos(int n) {
   return n < 10 ? "0" + std::to_string(n) : std::to_string(n);
}

////////////////////////////////////////////////////////////////////////
// Name:       Utilidades::formatoHora(minutos)
// Purpose:    Implementation of Utilidades::formatoHora()
// Parameters:
// - minutos
// Return:     std::string
////////////////////////////////////////////////////////////////////////
// Convierte minutos desde la medianoche a "HH:MM": 425 -> "07:05".
std::string formatoHora(int minutos) {
   return dosDigitos(minutos / 60) + ":" + dosDigitos(minutos % 60);
}

////////////////////////////////////////////////////////////////////////
// Name:       Utilidades::textoFranja(n)
// Purpose:    Implementation of Utilidades::textoFranja()
// Parameters:
// - n
// Return:     std::string
////////////////////////////////////////////////////////////////////////
// Texto de una franja: 1 -> "07:00-09:00" (inicio y fin calculados).
std::string textoFranja(int n) {
   return formatoHora(inicioFranja(n)) + "-" + formatoHora(finFranja(n));
}

////////////////////////////////////////////////////////////////////////
// Name:       Utilidades::nombreDia(dia)
// Purpose:    Implementation of Utilidades::nombreDia()
// Parameters:
// - dia
// Return:     std::string
////////////////////////////////////////////////////////////////////////
// Nombre del dia de la semana (0 = Domingo ... 6 = Sabado).
std::string nombreDia(int dia) {
   switch (dia) {
      case 0: return "Domingo";
      case 1: return "Lunes";
      case 2: return "Martes";
      case 3: return "Miercoles";
      case 4: return "Jueves";
      case 5: return "Viernes";
      case 6: return "Sabado";
      default: return "Dia invalido";
   }
}

// ------------------------------------------------------------------ validaciones

////////////////////////////////////////////////////////////////////////
// Name:       Utilidades::limpiar(texto)
// Purpose:    Implementation of Utilidades::limpiar()
// Parameters:
// - texto
// Return:     std::string
////////////////////////////////////////////////////////////////////////
// Quita los espacios, tabuladores y saltos de linea del inicio y del final.
std::string limpiar(const std::string& texto) {
   std::size_t inicio = texto.find_first_not_of(" \t\r\n");       // primer caracter util
   if (inicio == std::string::npos) return "";                    // solo habia espacios
   return texto.substr(inicio, texto.find_last_not_of(" \t\r\n") - inicio + 1);
}

////////////////////////////////////////////////////////////////////////
// Name:       Utilidades::soloNumeros(texto)
// Purpose:    Implementation of Utilidades::soloNumeros()
// Parameters:
// - texto
// Return:     bool
////////////////////////////////////////////////////////////////////////
// true si el texto no esta vacio y todos sus caracteres son digitos.
bool soloNumeros(const std::string& texto) {
   if (texto.empty()) return false;
   for (char c : texto)
      if (!esDigito(c)) return false;
   return true;
}

////////////////////////////////////////////////////////////////////////
// Name:       Utilidades::motivoCedulaInvalida(cedula)
// Purpose:    Implementation of Utilidades::motivoCedulaInvalida()
// Parameters:
// - cedula
// Return:     std::string
////////////////////////////////////////////////////////////////////////
// Valida una cedula ecuatoriana. Devuelve "" si es correcta o el motivo exacto del error.
std::string motivoCedulaInvalida(const std::string& cedula) {
   if (cedula.empty())
      return "No ingreso nada. Ingrese una cedula de 10 digitos, por ejemplo 1710034065.";
   if (!soloNumeros(cedula))
      return "La cedula solo debe tener numeros. Ingrese 10 digitos sin letras, espacios ni simbolos, por ejemplo 1710034065.";
   if (cedula.size() != 10)
      return "La cedula debe tener exactamente 10 digitos y usted ingreso " + std::to_string(cedula.size()) +
             ". Ingrese 10 digitos, por ejemplo 1710034065.";

   // Los dos primeros digitos son la provincia (01 a 24, o 30 para ecuatorianos en el exterior).
   int provincia = std::stoi(cedula.substr(0, 2));
   if (provincia < 1 || (provincia > 24 && provincia != 30))
      return "Los dos primeros digitos de la cedula (provincia) deben estar entre 01 y 24, o ser 30. Usted ingreso " +
             cedula.substr(0, 2) + ".";
   // El tercer digito de una persona natural es 0 a 5.
   int tercerDigito = std::stoi(cedula.substr(2, 1));
   if (tercerDigito > 5)
      return "El tercer digito de la cedula debe estar entre 0 y 5. Usted ingreso " + cedula.substr(2, 1) + ".";

   // Algoritmo modulo 10: los digitos en posicion par (0,2,4...) se multiplican por 2
   // (si el resultado pasa de 9 se resta 9) y luego se suman los 9 primeros digitos.
   int suma = 0;
   int posicion = 0;
   for (char c : cedula.substr(0, 9)) {
      int d = c - '0';                    // convierte el caracter en numero
      if (posicion % 2 == 0) {
         d *= 2;
         if (d > 9) d -= 9;
      }
      suma += d;
      ++posicion;
   }
   // El digito verificador es lo que falta para llegar a la siguiente decena.
   int esperado = (10 - suma % 10) % 10;
   int verificador = std::stoi(cedula.substr(9, 1));
   if (esperado != verificador)
      return "La cedula no es valida: con esos 9 primeros digitos el ultimo digito deberia ser " +
             std::to_string(esperado) + " y usted ingreso " + std::to_string(verificador) + ". Revise la cedula.";
   return "";
}

////////////////////////////////////////////////////////////////////////
// Name:       Utilidades::motivoNrcInvalido(nrc)
// Purpose:    Implementation of Utilidades::motivoNrcInvalido()
// Parameters:
// - nrc
// Return:     std::string
////////////////////////////////////////////////////////////////////////
// Un NRC son solo numeros, de 4 a 6 digitos. Devuelve "" si es valido.
std::string motivoNrcInvalido(const std::string& nrc) {
   if (nrc.empty()) return "No ingreso nada. Ingrese un NRC de 4 a 6 digitos, por ejemplo 14872.";
   if (!soloNumeros(nrc))
      return "El NRC solo debe tener numeros. Ingrese de 4 a 6 digitos sin letras, espacios ni simbolos, por ejemplo 14872.";
   if (nrc.size() < 4 || nrc.size() > 6)
      return "El NRC debe tener entre 4 y 6 digitos y usted ingreso " + std::to_string(nrc.size()) +
             ". Ingrese por ejemplo 14872.";
   return "";
}

////////////////////////////////////////////////////////////////////////
// Name:       Utilidades::motivoTextoInvalido(texto, campo, maximo, ejemplo)
// Purpose:    Implementation of Utilidades::motivoTextoInvalido()
// Parameters:
// - texto
// - campo
// - maximo
// - ejemplo
// Return:     std::string
////////////////////////////////////////////////////////////////////////
// Texto formado solo por letras A-Z y espacios simples entre palabras.
// "campo" es el nombre que se muestra en el mensaje (Nombres, Apellidos, Materia) y
// "ejemplo" es un valor correcto que se muestra para guiar al usuario.
std::string motivoTextoInvalido(const std::string& texto, const std::string& campo, int maximo,
                                const std::string& ejemplo) {
   // Este texto final se repite en los mensajes para que el usuario sepa que debe ingresar.
   const std::string indicacion = " Ingrese solo letras, por ejemplo " + ejemplo + ".";

   if (texto.empty()) return "El campo " + campo + " no puede estar vacio." + indicacion;
   if (static_cast<int>(texto.size()) > maximo)
      return "El campo " + campo + " no puede tener mas de " + std::to_string(maximo) + " caracteres y usted ingreso " +
             std::to_string(texto.size()) + "." + indicacion;
   if (texto.front() == ' ' || texto.back() == ' ')
      return "El campo " + campo + " no puede empezar ni terminar con espacios." + indicacion;

   bool espacioAnterior = false;       // recuerda si el caracter anterior fue un espacio
   for (char c : texto) {
      if (c == ' ') {
         if (espacioAnterior)
            return "El campo " + campo + " no puede tener dos espacios seguidos. Separe las palabras con un solo espacio.";
         espacioAnterior = true;
      } else if (esDigito(c)) {
         return "El campo " + campo + " no puede contener numeros (encontro el '" + std::string(1, c) + "')." + indicacion;
      } else if (static_cast<unsigned char>(c) > 127) {
         // Las tildes y la enie ocupan caracteres fuera del rango ASCII (0 a 127).
         return "El campo " + campo + " no admite tildes ni la letra enie. Escriba la letra sin tilde y la enie como n" +
                indicacion;
      } else if (!esLetra(c)) {
         return "El campo " + campo + " no puede contener simbolos (encontro el '" + std::string(1, c) + "')." + indicacion;
      } else {
         espacioAnterior = false;
      }
   }
   return "";
}

////////////////////////////////////////////////////////////////////////
// Name:       Utilidades::motivoHoraInvalida(hora)
// Purpose:    Implementation of Utilidades::motivoHoraInvalida()
// Parameters:
// - hora
// Return:     std::string
////////////////////////////////////////////////////////////////////////
// Valida una hora con formato HH:MM de 24 horas. Devuelve "" si es correcta.
std::string motivoHoraInvalida(const std::string& hora) {
   if (hora.empty()) return "No ingreso nada. Ingrese la hora como HH:MM de 24 horas, por ejemplo 07:05.";
   if (hora.size() != 5 || hora.substr(2, 1) != ":")
      return "Formato incorrecto. Ingrese la hora como HH:MM con dos digitos en cada parte y dos puntos en medio, "
             "por ejemplo 07:05 (no 7:5).";
   std::string horas = hora.substr(0, 2);       // los dos primeros caracteres
   std::string minutos = hora.substr(3, 2);     // los dos ultimos (despues de los dos puntos)
   if (!soloNumeros(horas) || !soloNumeros(minutos))
      return "La hora solo debe tener numeros separados por dos puntos. Ingrese por ejemplo 07:05.";
   if (std::stoi(horas) > 23)
      return "Las horas deben estar entre 00 y 23 y usted ingreso " + horas + ". Ingrese por ejemplo 07:05.";
   if (std::stoi(minutos) > 59)
      return "Los minutos deben estar entre 00 y 59 y usted ingreso " + minutos + ". Ingrese por ejemplo 07:05.";
   return "";
}

////////////////////////////////////////////////////////////////////////
// Name:       Utilidades::horaAMinutos(hora)
// Purpose:    Implementation of Utilidades::horaAMinutos()
// Parameters:
// - hora
// Return:     int
////////////////////////////////////////////////////////////////////////
// Convierte "HH:MM" a minutos desde la medianoche ("07:10" -> 430).
// Se debe llamar solo con una hora que ya paso motivoHoraInvalida.
int horaAMinutos(const std::string& hora) {
   return std::stoi(hora.substr(0, 2)) * 60 + std::stoi(hora.substr(3, 2));
}

////////////////////////////////////////////////////////////////////////
// Name:       Utilidades::motivoFechaInvalida(fecha)
// Purpose:    Implementation of Utilidades::motivoFechaInvalida()
// Parameters:
// - fecha
// Return:     std::string
////////////////////////////////////////////////////////////////////////
// Valida una fecha dd/mm/aaaa: formato, mes, anio y que el dia exista en ese mes.
std::string motivoFechaInvalida(const std::string& fecha) {
   if (fecha.empty()) return "No ingreso nada. Ingrese la fecha como dd/mm/aaaa, por ejemplo 05/11/2026.";
   if (fecha.size() != 10 || fecha.substr(2, 1) != "/" || fecha.substr(5, 1) != "/")
      return "Formato incorrecto. Ingrese la fecha como dd/mm/aaaa con dos digitos de dia, dos de mes, "
             "cuatro de anio y barras, por ejemplo 05/11/2026.";
   std::string textoDia = fecha.substr(0, 2);
   std::string textoMes = fecha.substr(3, 2);
   std::string textoAnio = fecha.substr(6, 4);
   if (!soloNumeros(textoDia) || !soloNumeros(textoMes) || !soloNumeros(textoAnio))
      return "La fecha solo debe tener numeros separados por barras. Ingrese por ejemplo 05/11/2026.";

   int dia = std::stoi(textoDia);
   int mes = std::stoi(textoMes);
   int anio = std::stoi(textoAnio);
   if (mes < 1 || mes > 12)
      return "El mes debe estar entre 01 y 12 y usted ingreso " + textoMes + ". Ingrese por ejemplo 05/11/2026.";
   if (anio < ANIO_MINIMO || anio > ANIO_MAXIMO)
      return "El anio debe estar entre " + std::to_string(ANIO_MINIMO) + " y " + std::to_string(ANIO_MAXIMO) +
             " y usted ingreso " + textoAnio + ".";
   int maximoDias = diasDelMes(mes, anio);
   if (dia < 1 || dia > maximoDias)
      return "El mes " + textoMes + " del " + textoAnio + " solo tiene " + std::to_string(maximoDias) +
             " dias. Ingrese un dia entre 01 y " + std::to_string(maximoDias) + ".";
   return "";
}

////////////////////////////////////////////////////////////////////////
// Name:       Utilidades::validarFecha(fecha)
// Purpose:    Implementation of Utilidades::validarFecha()
// Parameters:
// - fecha
// Return:     bool
////////////////////////////////////////////////////////////////////////
// true si la fecha es valida (no hay motivo de error).
bool validarFecha(const std::string& fecha) { return motivoFechaInvalida(fecha).empty(); }

////////////////////////////////////////////////////////////////////////
// Name:       Utilidades::diaDeLaSemana(fecha)
// Purpose:    Implementation of Utilidades::diaDeLaSemana()
// Parameters:
// - fecha
// Return:     int
////////////////////////////////////////////////////////////////////////
// Dia de la semana de una fecha ya validada (0 = domingo ... 6 = sabado).
// Usa mktime, que calcula el dia de la semana a partir de dia, mes y anio.
int diaDeLaSemana(const std::string& fecha) {
   std::tm f = {};                                            // estructura de fecha, en ceros
   f.tm_mday = std::stoi(fecha.substr(0, 2));                 // dia del mes
   f.tm_mon = std::stoi(fecha.substr(3, 2)) - 1;              // mes (en tm va de 0 a 11)
   f.tm_year = std::stoi(fecha.substr(6, 4)) - 1900;          // anio (en tm cuenta desde 1900)
   f.tm_hour = 12;                                            // mediodia, para evitar problemas de horario
   f.tm_isdst = -1;
   if (std::mktime(&f) == -1) return -1;
   return f.tm_wday;                                          // mktime completa el dia de la semana
}

// ------------------------------------------------------------------ reloj

////////////////////////////////////////////////////////////////////////
// Name:       Utilidades::ahora(fecha, minutos, diaSemana)
// Purpose:    Implementation of Utilidades::ahora()
// Parameters:
// - fecha
// - minutos
// - diaSemana
// Return:     void
////////////////////////////////////////////////////////////////////////
// Lee la fecha y hora reales del computador. Devuelve los datos por referencia.
void ahora(std::string& fecha, int& minutos, int& diaSemana) {
   std::time_t t = std::time(nullptr);              // segundos desde 1970
   std::tm* local = std::localtime(&t);             // lo convierte a fecha y hora local
   if (local == nullptr) {                          // por si el sistema no puede dar la hora
      fecha = "01/01/2000";
      minutos = 0;
      diaSemana = 0;
      return;
   }
   fecha = dosDigitos(local->tm_mday) + "/" + dosDigitos(local->tm_mon + 1) + "/" +
           std::to_string(local->tm_year + 1900);
   minutos = local->tm_hour * 60 + local->tm_min;
   diaSemana = local->tm_wday;
}

// ------------------------------------------------------------------ teclado

////////////////////////////////////////////////////////////////////////
// Name:       Utilidades::leerTexto(mensaje)
// Purpose:    Implementation of Utilidades::leerTexto()
// Parameters:
// - mensaje
// Return:     std::string
////////////////////////////////////////////////////////////////////////
// Muestra el mensaje, lee una linea completa y le quita los espacios de los extremos.
// Si la entrada se cierra (Ctrl+Z / Ctrl+D) termina el programa sin caerse.
std::string leerTexto(const char* mensaje) {
   std::cout << mensaje;
   std::string linea;
   if (!std::getline(std::cin, linea)) {
      std::cout << "\nEntrada finalizada. Cerrando el programa.\n";
      std::exit(0);
   }
   return limpiar(linea);
}

////////////////////////////////////////////////////////////////////////
// Name:       Utilidades::leerOpcion(mensaje, minimo, maximo)
// Purpose:    Implementation of Utilidades::leerOpcion()
// Parameters:
// - mensaje
// - minimo
// - maximo
// Return:     int
////////////////////////////////////////////////////////////////////////
// Pide un numero entre minimo y maximo. Repite la pregunta mostrando el error exacto.
int leerOpcion(const char* mensaje, int minimo, int maximo) {
   const std::string indicacion = "Elija un numero entre " + std::to_string(minimo) + " y " + std::to_string(maximo) + ".";
   while (true) {
      std::string texto = leerTexto(mensaje);
      if (texto.empty()) {
         std::cout << "  Error: no ingreso nada. " << indicacion << "\n";
      } else if (!soloNumeros(texto)) {
         std::cout << "  Error: solo se permiten numeros, sin letras, espacios ni simbolos. " << indicacion << "\n";
      } else if (texto.size() > 4) {
         std::cout << "  Error: el numero ingresado es demasiado largo. " << indicacion << "\n";   // evita desbordar el int
      } else {
         int valor = std::stoi(texto);
         if (valor >= minimo && valor <= maximo) return valor;
         std::cout << "  Error: la opcion " << valor << " no existe. " << indicacion << "\n";
      }
   }
}

////////////////////////////////////////////////////////////////////////
// Name:       Utilidades::leerCedula(mensaje)
// Purpose:    Implementation of Utilidades::leerCedula()
// Parameters:
// - mensaje
// Return:     std::string
////////////////////////////////////////////////////////////////////////
// Pide una cedula hasta que sea valida.
std::string leerCedula(const char* mensaje) {
   while (true) {
      std::string texto = leerTexto(mensaje);
      std::string motivo = motivoCedulaInvalida(texto);
      if (motivo.empty()) return texto;
      std::cout << "  Error: " << motivo << "\n";
   }
}

////////////////////////////////////////////////////////////////////////
// Name:       Utilidades::leerNrc(mensaje)
// Purpose:    Implementation of Utilidades::leerNrc()
// Parameters:
// - mensaje
// Return:     std::string
////////////////////////////////////////////////////////////////////////
// Pide un NRC hasta que sea valido.
std::string leerNrc(const char* mensaje) {
   while (true) {
      std::string texto = leerTexto(mensaje);
      std::string motivo = motivoNrcInvalido(texto);
      if (motivo.empty()) return texto;
      std::cout << "  Error: " << motivo << "\n";
   }
}

////////////////////////////////////////////////////////////////////////
// Name:       Utilidades::leerTextoLetras(mensaje, campo, maximo, ejemplo)
// Purpose:    Implementation of Utilidades::leerTextoLetras()
// Parameters:
// - mensaje
// - campo
// - maximo
// - ejemplo
// Return:     std::string
////////////////////////////////////////////////////////////////////////
// Pide un texto formado solo por letras (nombres, apellidos, materia).
std::string leerTextoLetras(const char* mensaje, const std::string& campo, int maximo, const std::string& ejemplo) {
   while (true) {
      std::string texto = leerTexto(mensaje);
      std::string motivo = motivoTextoInvalido(texto, campo, maximo, ejemplo);
      if (motivo.empty()) return texto;
      std::cout << "  Error: " << motivo << "\n";
   }
}

////////////////////////////////////////////////////////////////////////
// Name:       Utilidades::leerHora(mensaje)
// Purpose:    Implementation of Utilidades::leerHora()
// Parameters:
// - mensaje
// Return:     int
////////////////////////////////////////////////////////////////////////
// Pide una hora HH:MM hasta que sea valida y la devuelve en minutos.
int leerHora(const char* mensaje) {
   while (true) {
      std::string texto = leerTexto(mensaje);
      std::string motivo = motivoHoraInvalida(texto);
      if (motivo.empty()) return horaAMinutos(texto);
      std::cout << "  Error: " << motivo << "\n";
   }
}

////////////////////////////////////////////////////////////////////////
// Name:       Utilidades::leerFecha(mensaje, diaSemana)
// Purpose:    Implementation of Utilidades::leerFecha()
// Parameters:
// - mensaje
// - diaSemana
// Return:     std::string
////////////////////////////////////////////////////////////////////////
// Pide una fecha dd/mm/aaaa hasta que sea valida. Devuelve el texto y,
// por referencia, el dia de la semana de esa fecha.
std::string leerFecha(const char* mensaje, int& diaSemana) {
   while (true) {
      std::string texto = leerTexto(mensaje);
      std::string motivo = motivoFechaInvalida(texto);
      if (motivo.empty()) {
         diaSemana = diaDeLaSemana(texto);
         return texto;
      }
      std::cout << "  Error: " << motivo << "\n";
   }
}

////////////////////////////////////////////////////////////////////////
// Name:       Utilidades::leerConfirmacion(mensaje)
// Purpose:    Implementation of Utilidades::leerConfirmacion()
// Parameters:
// - mensaje
// Return:     bool
////////////////////////////////////////////////////////////////////////
// Pregunta s/n. Acepta s, si, n, no (mayusculas o minusculas). true = si.
bool leerConfirmacion(const char* mensaje) {
   while (true) {
      std::string texto = leerTexto(mensaje);
      for (char& c : texto) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));   // a minusculas
      if (texto == "s" || texto == "si") return true;
      if (texto == "n" || texto == "no") return false;
      if (texto.empty()) std::cout << "  Error: no ingreso nada. Responda con la letra s (si) o n (no).\n";
      else std::cout << "  Error: respuesta no valida. Responda con la letra s (si) o n (no).\n";
   }
}

////////////////////////////////////////////////////////////////////////
// Name:       Utilidades::pausar()
// Purpose:    Implementation of Utilidades::pausar()
// Return:     void
////////////////////////////////////////////////////////////////////////
// Espera a que el usuario presione ENTER.
void pausar() { leerTexto("\nPresione ENTER para continuar..."); }

}  // namespace Utilidades
