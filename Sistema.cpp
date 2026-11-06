/***********************************************************************
 * Module:  Sistema.cpp
 * Author:  Usuario
 * Purpose: Implementation of the class Sistema
 ***********************************************************************/

#include "Sistema.h"
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <sstream>

using namespace Utilidades;

// Funciones auxiliares privadas de este archivo.
namespace {

// Convierte un texto de hasta 6 digitos en entero. false si no es un numero valido.
bool leerEntero(const std::string& texto, int& valor) {
   if (texto.size() > 6 || !soloNumeros(texto)) return false;
   valor = std::stoi(texto);
   return true;
}

// Lee minutos del archivo. "-" significa "sin registrar" (-1). El maximo es 1439 (23:59).
bool leerMinutos(const std::string& texto, int& valor) {
   if (texto == "-") {
      valor = -1;
      return true;
   }
   return leerEntero(texto, valor) && valor <= 1439;
}

// Escribe minutos en el archivo; si no hay hora escribe "-".
void escribirMinutos(std::ostream& salida, int minutos) {
   if (minutos < 0) salida << '-';
   else salida << minutos;
}

}  // namespace

////////////////////////////////////////////////////////////////////////
// Name:       Sistema::Sistema(r)
// Purpose:    Implementation of Sistema::Sistema()
// Parameters:
// - r
// Return:     
////////////////////////////////////////////////////////////////////////
Sistema::Sistema(const std::string& r) : ruta(r) {}

// ---------------------------------------------------------------- consultas

////////////////////////////////////////////////////////////////////////
// Name:       Sistema::buscarNodoDocente(cedula)
// Purpose:    Implementation of Sistema::buscarNodoDocente()
// Parameters:
// - cedula
// Return:     Nodo<Docente>*
////////////////////////////////////////////////////////////////////////
// BUSCAR: recorre la lista de docentes y devuelve el nodo que tiene esa cedula (nullptr si no existe).
// Se devuelve el nodo (no solo el docente) porque para eliminar hace falta el nodo.
Nodo<Docente>* Sistema::buscarNodoDocente(const std::string& cedula) const {
   for (Nodo<Docente>* n = docentes.getCabeza(); n != nullptr; n = n->siguiente)
      if (n->dato->getCedula() == cedula) return n;
   return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       Sistema::buscarNodoCurso(nrc)
// Purpose:    Implementation of Sistema::buscarNodoCurso()
// Parameters:
// - nrc
// Return:     Nodo<Curso>*
////////////////////////////////////////////////////////////////////////
// BUSCAR: recorre la lista de cursos y devuelve el nodo que tiene ese NRC (nullptr si no existe).
Nodo<Curso>* Sistema::buscarNodoCurso(const std::string& nrc) const {
   for (Nodo<Curso>* n = cursos.getCabeza(); n != nullptr; n = n->siguiente)
      if (n->dato->getNrc() == nrc) return n;
   return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       Sistema::obtenerDocente(cedula)
// Purpose:    Implementation of Sistema::obtenerDocente()
// Parameters:
// - cedula
// Return:     const Docente*
////////////////////////////////////////////////////////////////////////
// Busca un docente por cedula recorriendo la lista. nullptr si no existe.
const Docente* Sistema::obtenerDocente(const std::string& cedula) const {
   Nodo<Docente>* nodo = buscarNodoDocente(cedula);
   return nodo != nullptr ? nodo->dato : nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       Sistema::buscarCurso(nrc)
// Purpose:    Implementation of Sistema::buscarCurso()
// Parameters:
// - nrc
// Return:     Curso*
////////////////////////////////////////////////////////////////////////
// Busca un curso por NRC recorriendo la lista. nullptr si no existe.
Curso* Sistema::buscarCurso(const std::string& nrc) const {
   Nodo<Curso>* nodo = buscarNodoCurso(nrc);
   return nodo != nullptr ? nodo->dato : nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       Sistema::obtenerCurso(nrc)
// Purpose:    Implementation of Sistema::obtenerCurso()
// Parameters:
// - nrc
// Return:     const Curso*
////////////////////////////////////////////////////////////////////////
// Version de solo lectura de buscarCurso, para usar desde Menu.
const Curso* Sistema::obtenerCurso(const std::string& nrc) const { return buscarCurso(nrc); }

////////////////////////////////////////////////////////////////////////
// Name:       Sistema::buscarRegistro(fecha, nrc, franja)
// Purpose:    Implementation of Sistema::buscarRegistro()
// Parameters:
// - fecha
// - nrc
// - franja
// Return:     RegistroAsistencia*
////////////////////////////////////////////////////////////////////////
// Busca el registro de asistencia de un NRC en una fecha y franja. nullptr si no hay.
RegistroAsistencia* Sistema::buscarRegistro(const std::string& fecha, const std::string& nrc, int franja) const {
   for (Nodo<RegistroAsistencia>* n = registros.getCabeza(); n != nullptr; n = n->siguiente) {
      const RegistroAsistencia* r = n->dato;
      if (r->getFranja() == franja && r->getFecha() == fecha && r->getNrc() == nrc) return n->dato;
   }
   return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       Sistema::cursoEnConflicto(cedula, dia, franja, nrcExcluido)
// Purpose:    Implementation of Sistema::cursoEnConflicto()
// Parameters:
// - cedula
// - dia
// - franja
// - nrcExcluido
// Return:     const Curso*
////////////////////////////////////////////////////////////////////////
// Curso (distinto de nrcExcluido) del mismo docente que ya tiene clase en ese dia y franja.
// Sirve para detectar cruces de horario.
const Curso* Sistema::cursoEnConflicto(const std::string& cedula, int dia, int franja,
                                       const std::string& nrcExcluido) const {
   for (Nodo<Curso>* n = cursos.getCabeza(); n != nullptr; n = n->siguiente) {
      const Curso* c = n->dato;
      if (c->getCedulaDocente() == cedula && c->getNrc() != nrcExcluido && c->tieneHorario(dia, franja))
         return c;
   }
   return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       Sistema::mensajeCruce(cedula, otro, dia, franja)
// Purpose:    Implementation of Sistema::mensajeCruce()
// Parameters:
// - cedula
// - otro
// - dia
// - franja
// Return:     std::string
////////////////////////////////////////////////////////////////////////
// Arma el texto de error cuando hay un cruce de horarios.
std::string Sistema::mensajeCruce(const std::string& cedula, const Curso& otro, int dia, int franja) const {
   const Docente* d = obtenerDocente(cedula);
   return "Cruce de horarios: " + (d != nullptr ? d->nombreCompleto() : cedula) + " ya dicta el NRC " +
          otro.getNrc() + " el " + nombreDia(dia) + " de " + textoFranja(franja) + ".";
}

// ---------------------------------------------------------------- administrador

////////////////////////////////////////////////////////////////////////
// Name:       Sistema::registrarDocente(cedula, nombres, apellidos)
// Purpose:    Implementation of Sistema::registrarDocente()
// Parameters:
// - cedula
// - nombres
// - apellidos
// Return:     bool
////////////////////////////////////////////////////////////////////////
// Valida los datos uno por uno (cada error tiene su mensaje) y, si todo esta bien,
// crea el docente con new y lo agrega a la lista.
bool Sistema::registrarDocente(const std::string& cedula, const std::string& nombres,
                               const std::string& apellidos) {
   std::string motivo = motivoCedulaInvalida(cedula);
   if (!motivo.empty()) return fallo(motivo);
   motivo = motivoTextoInvalido(nombres, "Nombres", MAX_NOMBRE, "Carlos");
   if (!motivo.empty()) return fallo(motivo);
   motivo = motivoTextoInvalido(apellidos, "Apellidos", MAX_NOMBRE, "Mendoza");
   if (!motivo.empty()) return fallo(motivo);
   if (obtenerDocente(cedula) != nullptr) return fallo("Ya existe un docente registrado con la cedula " + cedula + ". Ingrese otra cedula.");

   docentes.insertarFinal(new Docente(cedula, nombres, apellidos));
   return exito("Docente registrado correctamente.");
}

////////////////////////////////////////////////////////////////////////
// Name:       Sistema::crearCurso(nrc, materia)
// Purpose:    Implementation of Sistema::crearCurso()
// Parameters:
// - nrc
// - materia
// Return:     bool
////////////////////////////////////////////////////////////////////////
// Valida el NRC y la materia, comprueba que el NRC no exista y crea el curso.
bool Sistema::crearCurso(const std::string& nrc, const std::string& materia) {
   std::string motivo = motivoNrcInvalido(nrc);
   if (!motivo.empty()) return fallo(motivo);
   motivo = motivoTextoInvalido(materia, "Materia", MAX_MATERIA, "Estructura de Datos");
   if (!motivo.empty()) return fallo(motivo);
   if (obtenerCurso(nrc) != nullptr) return fallo("Ya existe un curso con el NRC " + nrc + ". Ingrese otro NRC.");

   cursos.insertarFinal(new Curso(nrc, materia));
   return exito("Curso creado: NRC " + nrc + " - " + materia + ". Aun sin docente asignado.");
}

////////////////////////////////////////////////////////////////////////
// Name:       Sistema::asignarDocente(nrc, cedula)
// Purpose:    Implementation of Sistema::asignarDocente()
// Parameters:
// - nrc
// - cedula
// Return:     bool
////////////////////////////////////////////////////////////////////////
// Asigna un docente a un curso, siempre que el docente no tenga otro curso
// en alguno de los mismos dias y franjas (cruce de horarios).
bool Sistema::asignarDocente(const std::string& nrc, const std::string& cedula) {
   Curso* curso = buscarCurso(nrc);
   if (curso == nullptr) return fallo("No existe un curso con el NRC " + nrc + ". Ingrese un NRC registrado o creelo primero en Gestion de NRC.");
   const Docente* docente = obtenerDocente(cedula);
   if (docente == nullptr) return fallo("No existe un docente con la cedula " + cedula + ". Ingrese una cedula registrada o registrelo primero en Gestion de docentes.");
   if (curso->getCedulaDocente() == cedula) return fallo("Ese docente ya esta asignado a este NRC.");

   // Se revisa cada horario que ya tiene el curso contra los otros cursos del docente.
   for (Nodo<Horario>* n = curso->getHorarios().getCabeza(); n != nullptr; n = n->siguiente) {
      int dia = n->dato->getDia();
      int franja = n->dato->getFranja();
      const Curso* otro = cursoEnConflicto(cedula, dia, franja, nrc);
      if (otro != nullptr) return fallo(mensajeCruce(cedula, *otro, dia, franja));
   }

   curso->asignarDocente(cedula);
   return exito("Docente " + docente->nombreCompleto() + " asignado al NRC " + nrc + ".");
}

////////////////////////////////////////////////////////////////////////
// Name:       Sistema::agregarHorario(nrc, dia, franja)
// Purpose:    Implementation of Sistema::agregarHorario()
// Parameters:
// - nrc
// - dia
// - franja
// Return:     bool
////////////////////////////////////////////////////////////////////////
// Agrega un dia y franja a un curso. Valida el dia, la franja, que no este repetido
// y que no choque con otro curso del mismo docente.
bool Sistema::agregarHorario(const std::string& nrc, int dia, int franja) {
   Curso* curso = buscarCurso(nrc);
   if (curso == nullptr) return fallo("No existe un curso con el NRC " + nrc + ". Ingrese un NRC registrado o creelo primero en Gestion de NRC.");
   if (dia < 1 || dia > 6) return fallo("El dia debe estar entre 1 (Lunes) y 6 (Sabado) y se recibio " + std::to_string(dia) + ".");
   if (!franjaValida(franja)) return fallo("La franja debe estar entre 1 y 7 y se recibio " + std::to_string(franja) + ".");
   if (curso->tieneHorario(dia, franja)) return fallo("Ese curso ya tiene asignado ese dia y esa franja.");

   // Solo hay que revisar cruces si el curso ya tiene docente.
   if (curso->tieneDocente()) {
      const Curso* otro = cursoEnConflicto(curso->getCedulaDocente(), dia, franja, nrc);
      if (otro != nullptr) return fallo(mensajeCruce(curso->getCedulaDocente(), *otro, dia, franja));
   }

   curso->agregarHorario(dia, franja);
   return exito("Sin cruces. Horario agregado: " + nombreDia(dia) + " " + textoFranja(franja));
}

////////////////////////////////////////////////////////////////////////
// Name:       Sistema::contarCursosDeDocente(cedula)
// Purpose:    Implementation of Sistema::contarCursosDeDocente()
// Parameters:
// - cedula
// Return:     int
////////////////////////////////////////////////////////////////////////
// Cuantos cursos tiene asignados el docente.
int Sistema::contarCursosDeDocente(const std::string& cedula) const {
   int cantidad = 0;
   for (Nodo<Curso>* n = cursos.getCabeza(); n != nullptr; n = n->siguiente)
      if (n->dato->getCedulaDocente() == cedula) ++cantidad;
   return cantidad;
}

////////////////////////////////////////////////////////////////////////
// Name:       Sistema::contarRegistrosDeDocente(cedula)
// Purpose:    Implementation of Sistema::contarRegistrosDeDocente()
// Parameters:
// - cedula
// Return:     int
////////////////////////////////////////////////////////////////////////
// Cuantos registros de asistencia tiene el docente.
int Sistema::contarRegistrosDeDocente(const std::string& cedula) const {
   int cantidad = 0;
   for (Nodo<RegistroAsistencia>* n = registros.getCabeza(); n != nullptr; n = n->siguiente)
      if (n->dato->getCedulaDocente() == cedula) ++cantidad;
   return cantidad;
}

////////////////////////////////////////////////////////////////////////
// Name:       Sistema::contarRegistrosDeCurso(nrc)
// Purpose:    Implementation of Sistema::contarRegistrosDeCurso()
// Parameters:
// - nrc
// Return:     int
////////////////////////////////////////////////////////////////////////
// Cuantos registros de asistencia tiene el curso.
int Sistema::contarRegistrosDeCurso(const std::string& nrc) const {
   int cantidad = 0;
   for (Nodo<RegistroAsistencia>* n = registros.getCabeza(); n != nullptr; n = n->siguiente)
      if (n->dato->getNrc() == nrc) ++cantidad;
   return cantidad;
}

////////////////////////////////////////////////////////////////////////
// Name:       Sistema::borrarRegistrosDeDocente(cedula)
// Purpose:    Implementation of Sistema::borrarRegistrosDeDocente()
// Parameters:
// - cedula
// Return:     int
////////////////////////////////////////////////////////////////////////
// Elimina de la lista todos los registros de ese docente y devuelve cuantos borro.
// Se guarda el "siguiente" ANTES de eliminar, porque al borrar el nodo ya no se puede usar.
int Sistema::borrarRegistrosDeDocente(const std::string& cedula) {
   int borrados = 0;
   Nodo<RegistroAsistencia>* n = registros.getCabeza();
   while (n != nullptr) {
      Nodo<RegistroAsistencia>* siguiente = n->siguiente;
      if (n->dato->getCedulaDocente() == cedula) {
         registros.eliminar(n);
         ++borrados;
      }
      n = siguiente;
   }
   return borrados;
}

////////////////////////////////////////////////////////////////////////
// Name:       Sistema::borrarRegistrosDeCurso(nrc)
// Purpose:    Implementation of Sistema::borrarRegistrosDeCurso()
// Parameters:
// - nrc
// Return:     int
////////////////////////////////////////////////////////////////////////
// Elimina de la lista todos los registros de ese NRC y devuelve cuantos borro.
int Sistema::borrarRegistrosDeCurso(const std::string& nrc) {
   int borrados = 0;
   Nodo<RegistroAsistencia>* n = registros.getCabeza();
   while (n != nullptr) {
      Nodo<RegistroAsistencia>* siguiente = n->siguiente;
      if (n->dato->getNrc() == nrc) {
         registros.eliminar(n);
         ++borrados;
      }
      n = siguiente;
   }
   return borrados;
}

////////////////////////////////////////////////////////////////////////
// Name:       Sistema::eliminarDocente(cedula)
// Purpose:    Implementation of Sistema::eliminarDocente()
// Parameters:
// - cedula
// Return:     bool
////////////////////////////////////////////////////////////////////////
// ELIMINAR: borra al docente. Sus cursos quedan sin docente (siguen existiendo con sus
// horarios) y se borran sus registros de asistencia, para no dejar datos huerfanos.
bool Sistema::eliminarDocente(const std::string& cedula) {
   std::string motivo = motivoCedulaInvalida(cedula);
   if (!motivo.empty()) return fallo(motivo);
   Nodo<Docente>* nodo = buscarNodoDocente(cedula);
   if (nodo == nullptr)
      return fallo("No existe un docente con la cedula " + cedula + ". Ingrese una cedula registrada.");

   const std::string nombre = nodo->dato->nombreCompleto();     // se copia antes de borrar el nodo
   int cursosSinDocente = 0;
   for (Nodo<Curso>* n = cursos.getCabeza(); n != nullptr; n = n->siguiente) {
      if (n->dato->getCedulaDocente() == cedula) {
         n->dato->asignarDocente("");       // cadena vacia = curso sin docente
         ++cursosSinDocente;
      }
   }
   int registrosBorrados = borrarRegistrosDeDocente(cedula);
   docentes.eliminar(nodo);

   return exito("Docente " + nombre + " eliminado. Cursos que quedaron sin docente: " +
                std::to_string(cursosSinDocente) + ". Registros de asistencia borrados: " +
                std::to_string(registrosBorrados) + ".");
}

////////////////////////////////////////////////////////////////////////
// Name:       Sistema::eliminarCurso(nrc)
// Purpose:    Implementation of Sistema::eliminarCurso()
// Parameters:
// - nrc
// Return:     bool
////////////////////////////////////////////////////////////////////////
// ELIMINAR: borra el curso junto con sus horarios (la lista de horarios se libera sola
// al destruir el curso) y con sus registros de asistencia.
bool Sistema::eliminarCurso(const std::string& nrc) {
   std::string motivo = motivoNrcInvalido(nrc);
   if (!motivo.empty()) return fallo(motivo);
   Nodo<Curso>* nodo = buscarNodoCurso(nrc);
   if (nodo == nullptr)
      return fallo("No existe un curso con el NRC " + nrc + ". Ingrese un NRC registrado.");

   const std::string materia = nodo->dato->getMateria();        // se copia antes de borrar el nodo
   int registrosBorrados = borrarRegistrosDeCurso(nrc);
   cursos.eliminar(nodo);

   return exito("Curso NRC " + nrc + " (" + materia + ") eliminado. Registros de asistencia borrados: " +
                std::to_string(registrosBorrados) + ".");
}

// ---------------------------------------------------------------- docente

////////////////////////////////////////////////////////////////////////
// Name:       Sistema::ubicarClase(cedula, dia, minutos, franja)
// Purpose:    Implementation of Sistema::ubicarClase()
// Parameters:
// - cedula
// - dia
// - minutos
// - franja
// Return:     Curso*
////////////////////////////////////////////////////////////////////////
// Valida la cedula, comprueba que el docente exista y localiza el curso que le toca
// en este dia y hora. Devuelve nullptr (con el motivo en mensaje) si no es posible.
// La franja encontrada se devuelve por referencia.
Curso* Sistema::ubicarClase(const std::string& cedula, int dia, int minutos, int& franja) {
   std::string motivo = motivoCedulaInvalida(cedula);
   if (!motivo.empty()) {
      fallo(motivo);
      return nullptr;
   }
   if (obtenerDocente(cedula) == nullptr) {
      fallo("No existe un docente registrado con la cedula " + cedula + ". Ingrese la cedula con la que fue registrado o pida al administrador que lo registre.");
      return nullptr;
   }
   franja = franjaDeMinuto(minutos);       // 0 si la hora esta fuera de las clases
   if (franja == 0) {
      fallo("Fuera del horario de clases (07:00 a 21:00).");
      return nullptr;
   }
   for (Nodo<Curso>* n = cursos.getCabeza(); n != nullptr; n = n->siguiente)
      if (n->dato->getCedulaDocente() == cedula && n->dato->tieneHorario(dia, franja)) return n->dato;

   fallo("No tiene clase asignada el " + nombreDia(dia) + " en la franja " + textoFranja(franja) + ".");
   return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       Sistema::registrarIngreso(cedula, dia, minutos, fecha)
// Purpose:    Implementation of Sistema::registrarIngreso()
// Parameters:
// - cedula
// - dia
// - minutos
// - fecha
// Return:     bool
////////////////////////////////////////////////////////////////////////
// Registra la hora de ingreso del docente a su clase. El estado (PUNTUAL o ATRASO)
// se deduce de la hora. No permite registrar dos veces ni en horario de salida.
bool Sistema::registrarIngreso(const std::string& cedula, int dia, int minutos, const std::string& fecha) {
   int franja = 0;
   Curso* curso = ubicarClase(cedula, dia, minutos, franja);
   if (curso == nullptr) return false;

   const RegistroAsistencia* existente = buscarRegistro(fecha, curso->getNrc(), franja);
   if (existente != nullptr) {
      if (!existente->tieneIngreso())
         return fallo("Ya fue marcado como AUSENTE en esta franja; no puede registrar ingreso.");
      return fallo("Ya registro su ingreso en esta franja a las " + formatoHora(existente->getHoraIngreso()) +
                   ". No es posible registrarse dos veces.");
   }
   if (minutos >= inicioSalida(franja))
      return fallo("Ya es horario de salida; el ingreso ya no se puede registrar.");

   // Se crea el registro con new; la salida queda en -1 (sin registrar).
   const RegistroAsistencia* registro =
      registros.insertarFinal(new RegistroAsistencia(fecha, curso->getNrc(), cedula, franja, minutos, -1));
   std::string texto = "Ingreso registrado a las " + formatoHora(minutos) + " - NRC " + curso->getNrc() + " (" +
                       curso->getMateria() + "), franja " + textoFranja(franja) + ". Estado: " + registro->getEstado();
   if (minutos > limitePuntual(franja))
      texto += " (fuera de la ventana de ingreso, hasta " + formatoHora(limitePuntual(franja)) + ")";
   return exito(texto);
}

////////////////////////////////////////////////////////////////////////
// Name:       Sistema::registrarSalida(cedula, dia, minutos, fecha)
// Purpose:    Implementation of Sistema::registrarSalida()
// Parameters:
// - cedula
// - dia
// - minutos
// - fecha
// Return:     bool
////////////////////////////////////////////////////////////////////////
// Registra la hora de salida. Requiere un ingreso previo y solo se permite en los
// ultimos minutos de la franja.
bool Sistema::registrarSalida(const std::string& cedula, int dia, int minutos, const std::string& fecha) {
   int franja = 0;
   Curso* curso = ubicarClase(cedula, dia, minutos, franja);
   if (curso == nullptr) return false;

   RegistroAsistencia* registro = buscarRegistro(fecha, curso->getNrc(), franja);
   if (registro == nullptr || !registro->tieneIngreso())
      return fallo("No tiene un ingreso registrado en esta franja; no puede registrar salida.");
   if (registro->tieneSalida())
      return fallo("Ya registro su salida en esta franja a las " + formatoHora(registro->getHoraSalida()) + ".");
   if (minutos < inicioSalida(franja))
      return fallo("La salida solo se puede registrar entre las " + formatoHora(inicioSalida(franja)) +
                   " y las " + formatoHora(finFranja(franja) - 1) + ".");

   registro->registrarSalida(minutos);
   return exito("Salida registrada a las " + formatoHora(minutos) + " - NRC " + curso->getNrc() + ", franja " +
                textoFranja(franja) + ".");
}

// ---------------------------------------------------------------- archivo

////////////////////////////////////////////////////////////////////////
// Name:       Sistema::cargarRegistro(fecha, nrc, cedula, franja, ingreso, salida)
// Purpose:    Implementation of Sistema::cargarRegistro()
// Parameters:
// - fecha
// - nrc
// - cedula
// - franja
// - ingreso
// - salida
// Return:     bool
////////////////////////////////////////////////////////////////////////
// Agrega un registro ya armado (al cargar el archivo) comprobando que sea coherente.
bool Sistema::cargarRegistro(const std::string& fecha, const std::string& nrc, const std::string& cedula,
                             int franja, int ingreso, int salida) {
   if (obtenerDocente(cedula) == nullptr) return fallo("el registro es de un docente que no existe");
   if (obtenerCurso(nrc) == nullptr) return fallo("el registro es de un curso que no existe");
   if (ingreso >= 0 && (!franjaContiene(franja, ingreso) || ingreso >= inicioSalida(franja)))
      return fallo("la hora de ingreso no corresponde a la franja");
   if (salida >= 0 && (ingreso < 0 || !franjaContiene(franja, salida) ||
                       salida < inicioSalida(franja) || salida < ingreso))
      return fallo("la hora de salida no es valida");
   if (buscarRegistro(fecha, nrc, franja) != nullptr)
      return fallo("ya existe un registro para esa fecha, NRC y franja");

   registros.insertarFinal(new RegistroAsistencia(fecha, nrc, cedula, franja, ingreso, salida));
   return exito("Registro cargado.");
}

////////////////////////////////////////////////////////////////////////
// Name:       Sistema::procesarLinea(linea)
// Purpose:    Implementation of Sistema::procesarLinea()
// Parameters:
// - linea
// Return:     bool
////////////////////////////////////////////////////////////////////////
// true si la linea del archivo se cargo bien; si no, el motivo queda en mensaje.
// Los campos se leen en orden con getline, separados por '|'. Antes se cuentan los '|'
// para comprobar que la linea trae la cantidad correcta de campos.
bool Sistema::procesarLinea(const std::string& linea) {
   int separadores = static_cast<int>(std::count(linea.begin(), linea.end(), '|'));
   std::istringstream campos(linea);        // permite leer la linea por partes
   std::string tipo;                        // primer campo: D, C, H o R
   std::getline(campos, tipo, '|');

   // D|cedula|nombres|apellidos
   if (tipo == "D") {
      if (separadores != 3) return fallo("formato de docente incorrecto");
      std::string cedula, nombres, apellidos;
      std::getline(campos, cedula, '|');
      std::getline(campos, nombres, '|');
      std::getline(campos, apellidos, '|');
      return registrarDocente(cedula, nombres, apellidos);     // pasa por las mismas validaciones
   }

   // C|nrc|materia|cedulaDocente (o '-')
   if (tipo == "C") {
      if (separadores != 3) return fallo("formato de curso incorrecto");
      std::string nrc, materia, cedula;
      std::getline(campos, nrc, '|');
      std::getline(campos, materia, '|');
      std::getline(campos, cedula, '|');
      return crearCurso(nrc, materia) && (cedula == "-" || asignarDocente(nrc, cedula));
   }

   // H|nrc|dia|franja
   if (tipo == "H") {
      if (separadores != 3) return fallo("formato de horario incorrecto");
      std::string nrc, textoDia, textoFranja;
      std::getline(campos, nrc, '|');
      std::getline(campos, textoDia, '|');
      std::getline(campos, textoFranja, '|');
      int dia = 0, franja = 0;
      if (!leerEntero(textoDia, dia)) return fallo("dia invalido");
      if (!leerEntero(textoFranja, franja)) return fallo("franja invalida");
      return agregarHorario(nrc, dia, franja);
   }

   // R|fecha|nrc|cedula|franja|ingreso|salida|estado
   if (tipo == "R") {
      if (separadores != 7) return fallo("formato de registro incorrecto");
      std::string fecha, nrc, cedula, textoFranja, textoIngreso, textoSalida, estado;
      std::getline(campos, fecha, '|');
      std::getline(campos, nrc, '|');
      std::getline(campos, cedula, '|');
      std::getline(campos, textoFranja, '|');
      std::getline(campos, textoIngreso, '|');
      std::getline(campos, textoSalida, '|');
      std::getline(campos, estado, '|');

      if (!validarFecha(fecha)) return fallo("fecha invalida");
      if (!motivoNrcInvalido(nrc).empty()) return fallo("NRC invalido");
      if (!motivoCedulaInvalida(cedula).empty()) return fallo("cedula invalida");
      int franja = 0, ingreso = -1, salida = -1;
      if (!leerEntero(textoFranja, franja) || !franjaValida(franja)) return fallo("franja invalida");
      if (!leerMinutos(textoIngreso, ingreso) || !leerMinutos(textoSalida, salida)) return fallo("hora invalida");
      if (salida >= 0 && ingreso < 0) return fallo("tiene salida pero no ingreso");
      // El estado guardado debe coincidir con el que se deduce de las horas.
      if (estado != RegistroAsistencia::estadoTexto(franja, ingreso))
         return fallo("el estado no coincide con las horas");
      return cargarRegistro(fecha, nrc, cedula, franja, ingreso, salida);
   }

   return fallo("tipo de linea desconocido");
}

////////////////////////////////////////////////////////////////////////
// Name:       Sistema::cargar()
// Purpose:    Implementation of Sistema::cargar()
// Return:     int
////////////////////////////////////////////////////////////////////////
// Lee el archivo linea por linea. Devuelve -1 si no existe o la cantidad de lineas
// ignoradas por tener errores (el programa nunca se cae por un archivo danado).
int Sistema::cargar() {
   avisos.vaciar();
   std::ifstream entrada(ruta.c_str());
   if (!entrada.is_open()) return -1;      // primera ejecucion: aun no existe

   int ignorados = 0, numero = 0;
   std::string linea;
   while (std::getline(entrada, linea)) {
      ++numero;
      linea = limpiar(linea);
      // Se saltan las lineas vacias, los comentarios (#) y la linea de version.
      if (linea.empty() || linea.front() == '#' || linea.compare(0, 8, "VERSION|") == 0) continue;
      if (!procesarLinea(linea)) {
         ++ignorados;
         if (avisos.obtenerTamanio() < 5)    // se guardan solo los primeros 5 avisos
            avisos.insertarFinal(new std::string("Linea " + std::to_string(numero) + " ignorada: " + mensaje));
      }
   }
   return ignorados;
}

////////////////////////////////////////////////////////////////////////
// Name:       Sistema::guardar()
// Purpose:    Implementation of Sistema::guardar()
// Return:     bool
////////////////////////////////////////////////////////////////////////
// Escribe todos los datos en el archivo. Primero escribe en un archivo temporal y
// luego reemplaza el original, para no dejar el archivo a medias si algo falla.
bool Sistema::guardar() {
   const std::string temporal = ruta + ".tmp";
   {
      std::ofstream salida(temporal.c_str());
      if (!salida.is_open()) return fallo("No se pudo crear el archivo " + temporal);
      salida << "# Sistema de asistencia docente - archivo generado por el programa\n";
      salida << "VERSION|1\n";

      // Docentes
      for (Nodo<Docente>* n = docentes.getCabeza(); n != nullptr; n = n->siguiente) {
         const Docente* d = n->dato;
         salida << "D|" << d->getCedula() << '|' << d->getNombres() << '|' << d->getApellidos() << '\n';
      }
      // Cursos ('-' si no tienen docente)
      for (Nodo<Curso>* n = cursos.getCabeza(); n != nullptr; n = n->siguiente) {
         const Curso* c = n->dato;
         salida << "C|" << c->getNrc() << '|' << c->getMateria() << '|'
                << (c->tieneDocente() ? c->getCedulaDocente() : std::string("-")) << '\n';
      }
      // Horarios de cada curso
      for (Nodo<Curso>* n = cursos.getCabeza(); n != nullptr; n = n->siguiente) {
         const Curso* c = n->dato;
         for (Nodo<Horario>* h = c->getHorarios().getCabeza(); h != nullptr; h = h->siguiente)
            salida << "H|" << c->getNrc() << '|' << h->dato->getDia() << '|' << h->dato->getFranja() << '\n';
      }
      // Registros de asistencia
      for (Nodo<RegistroAsistencia>* n = registros.getCabeza(); n != nullptr; n = n->siguiente) {
         const RegistroAsistencia* r = n->dato;
         salida << "R|" << r->getFecha() << '|' << r->getNrc() << '|' << r->getCedulaDocente() << '|'
                << r->getFranja() << '|';
         escribirMinutos(salida, r->getHoraIngreso());
         salida << '|';
         escribirMinutos(salida, r->getHoraSalida());
         salida << '|' << r->getEstado() << '\n';
      }

      salida.flush();
      if (!salida.good()) return fallo("Error al escribir en " + temporal + " (disco lleno o sin permisos).");
   }
   std::remove(ruta.c_str());      // borra el archivo anterior
   if (std::rename(temporal.c_str(), ruta.c_str()) != 0) return fallo("No se pudo reemplazar el archivo " + ruta);
   return true;
}

////////////////////////////////////////////////////////////////////////
// Name:       Sistema::formatear()
// Purpose:    Implementation of Sistema::formatear()
// Return:     bool
////////////////////////////////////////////////////////////////////////
// Vacia las tres listas (libera la memoria) y borra el archivo de datos.
bool Sistema::formatear() {
   docentes.vaciar();
   cursos.vaciar();
   registros.vaciar();
   return std::remove(ruta.c_str()) == 0;
}
