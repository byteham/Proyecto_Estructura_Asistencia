/***********************************************************************
 * Module:  Menu.cpp
 * Author:  Usuario
 * Purpose: Implementation of the class Menu
 ***********************************************************************/

#include "Menu.h"
#include <iomanip>
#include <iostream>

using std::cout;
using namespace Utilidades;

namespace {

// Imprime un titulo enmarcado con lineas de "=".
void encabezado(const char* titulo) {
   cout << "\n==========================================\n  " << titulo
        << "\n==========================================\n";
}

}  // namespace

////////////////////////////////////////////////////////////////////////
// Name:       Menu::Menu(ruta)
// Purpose:    Implementation of Menu::Menu()
// Parameters:
// - ruta
// Return:     
////////////////////////////////////////////////////////////////////////
Menu::Menu(const std::string& ruta) : sistema(ruta), horaSimulada(false), minutos(0), dia(0) {}

////////////////////////////////////////////////////////////////////////
// Name:       Menu::actualizarReloj()
// Purpose:    Implementation of Menu::actualizarReloj()
// Return:     void
////////////////////////////////////////////////////////////////////////
// Si no hay una hora simulada, toma la fecha y hora reales del computador.
void Menu::actualizarReloj() {
   if (!horaSimulada) ahora(fecha, minutos, dia);
}

////////////////////////////////////////////////////////////////////////
// Name:       Menu::mostrarResultado(exito)
// Purpose:    Implementation of Menu::mostrarResultado()
// Parameters:
// - exito
// Return:     void
////////////////////////////////////////////////////////////////////////
// Muestra el mensaje que dejo Sistema ([OK] o [ERROR]). Si salio bien, guarda en el archivo.
void Menu::mostrarResultado(bool exito) {
   cout << (exito ? "\n  [OK] " : "\n  [ERROR] ") << sistema.getMensaje() << "\n";
   if (exito) guardarCambios();
}

////////////////////////////////////////////////////////////////////////
// Name:       Menu::cargarDatos()
// Purpose:    Implementation of Menu::cargarDatos()
// Return:     void
////////////////////////////////////////////////////////////////////////
// Carga el archivo al iniciar y muestra cuantos datos entraron y que lineas se ignoraron.
void Menu::cargarDatos() {
   const int ignorados = sistema.cargar();
   if (ignorados < 0) {
      cout << "\n  No existe " << sistema.getRuta() << ": se creara al guardar el primer cambio.\n";
      return;
   }
   cout << "\n  Datos cargados desde " << sistema.getRuta() << ": " << sistema.getDocentes().obtenerTamanio()
        << " docente(s), " << sistema.getCursos().obtenerTamanio() << " curso(s), "
        << sistema.getRegistros().obtenerTamanio() << " registro(s).\n";
   if (ignorados > 0) {
      cout << "  Aviso: " << ignorados << " linea(s) del archivo se ignoraron:\n";
      for (Nodo<std::string>* n = sistema.getAvisos().getCabeza(); n != nullptr; n = n->siguiente)
         cout << "    - " << *(n->dato) << "\n";
   }
}

////////////////////////////////////////////////////////////////////////
// Name:       Menu::guardarCambios()
// Purpose:    Implementation of Menu::guardarCambios()
// Return:     void
////////////////////////////////////////////////////////////////////////
// Guarda todo en el archivo e informa si hubo algun problema.
void Menu::guardarCambios() {
   if (sistema.guardar()) cout << "  Datos guardados en " << sistema.getRuta() << ".\n";
   else cout << "  [AVISO] No se pudieron guardar los datos: " << sistema.getMensaje() << "\n";
}

////////////////////////////////////////////////////////////////////////
// Name:       Menu::ejecutar()
// Purpose:    Implementation of Menu::ejecutar()
// Return:     void
////////////////////////////////////////////////////////////////////////
// Bucle del menu principal: se repite hasta que el usuario elige 0 (Salir).
void Menu::ejecutar() {
   cargarDatos();
   while (true) {
      actualizarReloj();
      encabezado("CONTROL DE ASISTENCIA DOCENTE - ESPE");
      cout << "  Fecha y hora: " << nombreDia(dia) << " " << fecha << " " << formatoHora(minutos)
           << (horaSimulada ? "  (SIMULADA)" : "") << "\n\n"
           << "  [1] Registrar ingreso\n"
           << "  [2] Registrar salida\n"
           << "  [3] Menu de administrador\n"
           << "  [4] Simular fecha y hora (para pruebas)\n"
           << "  [0] Salir\n\n";

      switch (leerOpcion("  Seleccione una opcion: ", 0, 4)) {
         case 1: registrarMarca(true); break;
         case 2: registrarMarca(false); break;
         case 3: menuAdministrador(); break;
         case 4: simularFechaHora(); break;
         case 0: cout << "\nHasta luego.\n"; return;
      }
   }
}

// ---------------------------------------------------------------- docente

////////////////////////////////////////////////////////////////////////
// Name:       Menu::registrarMarca(esIngreso)
// Purpose:    Implementation of Menu::registrarMarca()
// Parameters:
// - esIngreso
// Return:     void
////////////////////////////////////////////////////////////////////////
// Pide la cedula del docente y registra su ingreso (true) o su salida (false).
void Menu::registrarMarca(bool esIngreso) {
   encabezado(esIngreso ? "REGISTRAR INGRESO" : "REGISTRAR SALIDA");
   actualizarReloj();
   if (dia < 1) {                   // 0 = domingo
      cout << "\n  [ERROR] Hoy es domingo: no hay clases.\n";
      pausar();
      return;
   }
   const std::string cedula = leerCedula("  Ingrese su cedula: ");
   mostrarResultado(esIngreso ? sistema.registrarIngreso(cedula, dia, minutos, fecha)
                              : sistema.registrarSalida(cedula, dia, minutos, fecha));
   pausar();
}

////////////////////////////////////////////////////////////////////////
// Name:       Menu::simularFechaHora()
// Purpose:    Implementation of Menu::simularFechaHora()
// Return:     void
////////////////////////////////////////////////////////////////////////
// Permite probar el sistema con una fecha y hora inventadas, o volver a la hora real.
void Menu::simularFechaHora() {
   encabezado("SIMULAR FECHA Y HORA");
   cout << "  [1] Usar una fecha y hora de prueba\n"
        << "  [2] Volver a la hora real\n"
        << "  [0] Cancelar\n\n";
   const int opcion = leerOpcion("  Seleccione una opcion: ", 0, 2);
   if (opcion == 0) return;
   if (opcion == 2) {
      horaSimulada = false;
      cout << "\n  Se usa la hora real del equipo.\n";
      pausar();
      return;
   }
   const int nuevosMinutos = leerHora("  Hora (HH:MM, 24 horas): ");
   int nuevoDia = 0;
   fecha = leerFecha("  Fecha (dd/mm/aaaa): ", nuevoDia);
   minutos = nuevosMinutos;
   dia = nuevoDia;
   horaSimulada = true;
   cout << "\n  Fecha y hora simuladas configuradas.\n";
   pausar();
}

// ---------------------------------------------------------------- administrador

////////////////////////////////////////////////////////////////////////
// Name:       Menu::menuAdministrador()
// Purpose:    Implementation of Menu::menuAdministrador()
// Return:     void
////////////////////////////////////////////////////////////////////////
// Submenu del administrador. Se repite hasta que el usuario elige 0 (Volver).
void Menu::menuAdministrador() {
   while (true) {
      encabezado("MENU DE ADMINISTRADOR");
      cout << "  [1] Gestion de docentes (buscar, insertar, eliminar, imprimir)\n"
           << "  [2] Gestion de NRC (buscar, insertar, eliminar, imprimir)\n"
           << "  [3] Asignar docente a un NRC\n"
           << "  [4] Agregar horario a un NRC\n"
           << "  [5] Reporte de asistencia\n"
           << "  [6] Formatear informacion\n"
           << "  [7] Cargar datos de ejemplo\n"
           << "  [0] Volver\n\n";

      switch (leerOpcion("  Seleccione una opcion: ", 0, 7)) {
         case 1: menuDocentes(); break;
         case 2: menuCursos(); break;
         case 3: asignarDocente(); break;
         case 4: agregarHorario(); break;
         case 5: reporteAsistencia(); break;
         case 6: formatearInformacion(); break;
         case 7: cargarDatosEjemplo(); break;
         case 0: return;
      }
   }
}

////////////////////////////////////////////////////////////////////////
// Name:       Menu::registrarDocente()
// Purpose:    Implementation of Menu::registrarDocente()
// Return:     void
////////////////////////////////////////////////////////////////////////
// Pide cedula, nombres y apellidos. Cada dato se vuelve a pedir si es invalido.
void Menu::registrarDocente() {
   encabezado("INSERTAR DOCENTE");
   std::string cedula;
   while (true) {
      cedula = leerCedula("  Cedula: ");
      if (sistema.obtenerDocente(cedula) == nullptr) break;      // no existe: se puede usar
      cout << "  Error: ya existe un docente registrado con la cedula " << cedula << ". Ingrese otra cedula.\n";
   }
   const std::string nombres = leerTextoLetras("  Nombres: ", "Nombres", MAX_NOMBRE, "Carlos");
   const std::string apellidos = leerTextoLetras("  Apellidos: ", "Apellidos", MAX_NOMBRE, "Mendoza");
   mostrarResultado(sistema.registrarDocente(cedula, nombres, apellidos));
   pausar();
}

////////////////////////////////////////////////////////////////////////
// Name:       Menu::crearCurso()
// Purpose:    Implementation of Menu::crearCurso()
// Return:     void
////////////////////////////////////////////////////////////////////////
// Pide el NRC (que no debe existir) y el nombre de la materia.
void Menu::crearCurso() {
   encabezado("INSERTAR NRC");
   std::string nrc;
   while (true) {
      nrc = leerNrc("  NRC: ");
      if (sistema.obtenerCurso(nrc) == nullptr) break;           // no existe: se puede usar
      cout << "  Error: ya existe un curso con el NRC " << nrc << ". Ingrese otro NRC.\n";
   }
   const std::string materia = leerTextoLetras("  Materia: ", "Materia", MAX_MATERIA, "Estructura de Datos");
   mostrarResultado(sistema.crearCurso(nrc, materia));
   pausar();
}

////////////////////////////////////////////////////////////////////////
// Name:       Menu::pedirCurso(nrc)
// Purpose:    Implementation of Menu::pedirCurso()
// Parameters:
// - nrc
// Return:     const Curso*
////////////////////////////////////////////////////////////////////////
// Pide un NRC. Si no existe, avisa, espera ENTER y devuelve nullptr.
const Curso* Menu::pedirCurso(std::string& nrc) {
   nrc = leerNrc("  NRC: ");
   const Curso* curso = sistema.obtenerCurso(nrc);
   if (curso == nullptr) {
      cout << "\n  [ERROR] No existe un curso con el NRC " << nrc
           << ". Ingrese un NRC registrado (use Gestion de NRC > Imprimir para verlos).\n";
      pausar();
   }
   return curso;
}

////////////////////////////////////////////////////////////////////////
// Name:       Menu::asignarDocente()
// Purpose:    Implementation of Menu::asignarDocente()
// Return:     void
////////////////////////////////////////////////////////////////////////
// Asigna un docente a un NRC. Si el curso ya tenia otro docente, pide confirmacion.
void Menu::asignarDocente() {
   encabezado("ASIGNAR DOCENTE A UN NRC");
   std::string nrc;
   const Curso* curso = pedirCurso(nrc);
   if (curso == nullptr) return;
   cout << "  Curso: " << curso->getMateria() << "\n";
   const std::string cedula = leerCedula("  Cedula del docente: ");

   if (sistema.obtenerDocente(cedula) == nullptr) {
      cout << "\n  [ERROR] No existe un docente con la cedula " << cedula
           << ". Ingrese una cedula registrada (use Gestion de docentes > Imprimir) o registrelo primero.\n";
      pausar();
      return;
   }
   // Si el curso ya tiene otro docente, se pide confirmacion antes de reemplazarlo.
   if (curso->tieneDocente() && curso->getCedulaDocente() != cedula) {
      const Docente* actual = sistema.obtenerDocente(curso->getCedulaDocente());
      cout << "\n  Este NRC ya tiene asignado a "
           << (actual != nullptr ? actual->nombreCompleto() : curso->getCedulaDocente()) << ".\n";
      if (!leerConfirmacion("  Desea reemplazarlo? (s/n): ")) {
         cout << "\n  Operacion cancelada.\n";
         pausar();
         return;
      }
   }
   mostrarResultado(sistema.asignarDocente(nrc, cedula));
   pausar();
}

////////////////////////////////////////////////////////////////////////
// Name:       Menu::agregarHorario()
// Purpose:    Implementation of Menu::agregarHorario()
// Return:     void
////////////////////////////////////////////////////////////////////////
// Pide un NRC, un dia (1 a 6) y una franja (1 a 7) y agrega ese horario al curso.
void Menu::agregarHorario() {
   encabezado("AGREGAR HORARIO A UN NRC");
   std::string nrc;
   const Curso* curso = pedirCurso(nrc);
   if (curso == nullptr) return;
   cout << "  Curso: " << curso->getMateria() << "\n\n";
   for (int d = 1; d <= 6; ++d) cout << "  [" << d << "] " << nombreDia(d) << "\n";
   const int numeroDia = leerOpcion("  Dia: ", 1, 6);
   cout << "\n";
   for (int f = 1; f <= FRANJAS; ++f) cout << "  [" << f << "] " << textoFranja(f) << "\n";
   const int numeroFranja = leerOpcion("  Franja: ", 1, FRANJAS);
   mostrarResultado(sistema.agregarHorario(nrc, numeroDia, numeroFranja));
   pausar();
}

////////////////////////////////////////////////////////////////////////
// Name:       Menu::listarDocentes()
// Purpose:    Implementation of Menu::listarDocentes()
// Return:     void
////////////////////////////////////////////////////////////////////////
// Muestra una tabla con cada docente y los NRC que dicta.
void Menu::listarDocentes() const {
   encabezado("IMPRIMIR DOCENTES");
   if (sistema.getDocentes().estaVacia()) {
      cout << "  No hay docentes registrados. Use la opcion Insertar docente.\n";
   } else {
      cout << "  " << std::left << std::setw(12) << "Cedula" << std::setw(30) << "Docente" << "NRC asignados\n";
      for (Nodo<Docente>* n = sistema.getDocentes().getCabeza(); n != nullptr; n = n->siguiente) {
         const Docente* d = n->dato;
         const std::string nrcs = nrcsDeDocente(d->getCedula());
         cout << "  " << std::left << std::setw(12) << d->getCedula() << std::setw(30) << d->nombreCompleto()
              << (nrcs.empty() ? "-" : nrcs) << "\n";
      }
      cout << "\n  Total: " << sistema.getDocentes().obtenerTamanio() << " docente(s).\n";
   }
   pausar();
}

////////////////////////////////////////////////////////////////////////
// Name:       Menu::mostrarFila(registro)
// Purpose:    Implementation of Menu::mostrarFila()
// Parameters:
// - registro
// Return:     void
////////////////////////////////////////////////////////////////////////
// Imprime una fila del reporte de asistencia (un registro).
void Menu::mostrarFila(const RegistroAsistencia* r) const {
   const Docente* d = sistema.obtenerDocente(r->getCedulaDocente());
   std::string nombre = (d != nullptr) ? d->nombreCompleto() : r->getCedulaDocente();
   if (nombre.size() > 20) nombre.resize(20);        // se recorta para que la tabla no se desalinee
   cout << "  " << std::left << std::setw(12) << r->getFecha() << std::setw(8) << r->getNrc()
        << std::setw(22) << nombre << std::setw(13) << textoFranja(r->getFranja())
        << std::setw(8) << (r->tieneIngreso() ? formatoHora(r->getHoraIngreso()) : "--")
        << std::setw(8) << (r->tieneSalida() ? formatoHora(r->getHoraSalida()) : "--")
        << r->getEstado() << "\n";
}

////////////////////////////////////////////////////////////////////////
// Name:       Menu::reporteAsistencia()
// Purpose:    Implementation of Menu::reporteAsistencia()
// Return:     void
////////////////////////////////////////////////////////////////////////
// Muestra todos los registros recorriendo la lista hacia adelante (antiguo a reciente)
// o hacia atras (reciente a antiguo), segun lo que elija el usuario.
void Menu::reporteAsistencia() const {
   encabezado("REPORTE DE ASISTENCIA");
   if (sistema.getRegistros().estaVacia()) {
      cout << "  Aun no hay registros de asistencia.\n";
      pausar();
      return;
   }
   cout << "  [1] Del mas antiguo al mas reciente\n"
        << "  [2] Del mas reciente al mas antiguo\n\n";
   const int orden = leerOpcion("  Recorrido: ", 1, 2);

   cout << "\n  " << std::left << std::setw(12) << "Fecha" << std::setw(8) << "NRC" << std::setw(22) << "Docente"
        << std::setw(13) << "Franja" << std::setw(8) << "Ingr." << std::setw(8) << "Sal." << "Estado\n";

   if (orden == 1) {
      // Hacia adelante: de la cabeza a la cola, usando "siguiente".
      for (Nodo<RegistroAsistencia>* n = sistema.getRegistros().getCabeza(); n != nullptr; n = n->siguiente)
         mostrarFila(n->dato);
   } else {
      // Hacia atras: de la cola a la cabeza, usando "anterior".
      for (Nodo<RegistroAsistencia>* n = sistema.getRegistros().getCola(); n != nullptr; n = n->anterior)
         mostrarFila(n->dato);
   }

   cout << "\n  Total de registros: " << sistema.getRegistros().obtenerTamanio() << "\n";
   pausar();
}

// ---------------------------------------------------------------- funciones de apoyo

////////////////////////////////////////////////////////////////////////
// Name:       Menu::nrcsDeDocente(cedula)
// Purpose:    Implementation of Menu::nrcsDeDocente()
// Parameters:
// - cedula
// Return:     std::string
////////////////////////////////////////////////////////////////////////
// Junta en un texto los NRC que dicta un docente: "14872 16210 ". Vacio si no tiene ninguno.
std::string Menu::nrcsDeDocente(const std::string& cedula) const {
   std::string nrcs;
   for (Nodo<Curso>* n = sistema.getCursos().getCabeza(); n != nullptr; n = n->siguiente)
      if (n->dato->getCedulaDocente() == cedula) nrcs += n->dato->getNrc() + " ";
   return nrcs;
}

////////////////////////////////////////////////////////////////////////
// Name:       Menu::horariosTexto(curso)
// Purpose:    Implementation of Menu::horariosTexto()
// Parameters:
// - curso
// Return:     std::string
////////////////////////////////////////////////////////////////////////
// Arma un texto con todos los horarios del curso: "Lunes 07:00-09:00, Martes 09:00-11:00".
std::string Menu::horariosTexto(const Curso* curso) const {
   std::string texto;
   for (Nodo<Horario>* n = curso->getHorarios().getCabeza(); n != nullptr; n = n->siguiente) {
      if (!texto.empty()) texto += ", ";
      texto += nombreDia(n->dato->getDia()) + " " + textoFranja(n->dato->getFranja());
   }
   return texto.empty() ? "Sin horarios" : texto;
}

////////////////////////////////////////////////////////////////////////
// Name:       Menu::mostrarDocente(docente)
// Purpose:    Implementation of Menu::mostrarDocente()
// Parameters:
// - docente
// Return:     void
////////////////////////////////////////////////////////////////////////
// Imprime todos los datos de un docente.
void Menu::mostrarDocente(const Docente* docente) const {
   const std::string nrcs = nrcsDeDocente(docente->getCedula());
   cout << "    Cedula:    " << docente->getCedula() << "\n"
        << "    Nombres:   " << docente->getNombres() << "\n"
        << "    Apellidos: " << docente->getApellidos() << "\n"
        << "    NRC:       " << (nrcs.empty() ? "Sin NRC asignados" : nrcs) << "\n";
}

////////////////////////////////////////////////////////////////////////
// Name:       Menu::mostrarCurso(curso)
// Purpose:    Implementation of Menu::mostrarCurso()
// Parameters:
// - curso
// Return:     void
////////////////////////////////////////////////////////////////////////
// Imprime todos los datos de un curso, incluido su docente y sus horarios.
void Menu::mostrarCurso(const Curso* curso) const {
   std::string docente = "Sin docente asignado";
   if (curso->tieneDocente()) {
      const Docente* d = sistema.obtenerDocente(curso->getCedulaDocente());
      docente = (d != nullptr ? d->nombreCompleto() : "Desconocido") + " (cedula " + curso->getCedulaDocente() + ")";
   }
   cout << "    NRC:       " << curso->getNrc() << "\n"
        << "    Materia:   " << curso->getMateria() << "\n"
        << "    Docente:   " << docente << "\n"
        << "    Horarios:  " << horariosTexto(curso) << "\n";
}

// ---------------------------------------------------------------- gestion de docentes

////////////////////////////////////////////////////////////////////////
// Name:       Menu::menuDocentes()
// Purpose:    Implementation of Menu::menuDocentes()
// Return:     void
////////////////////////////////////////////////////////////////////////
// Submenu de docentes: buscar, insertar, eliminar e imprimir.
void Menu::menuDocentes() {
   while (true) {
      encabezado("GESTION DE DOCENTES");
      cout << "  [1] Buscar docente\n"
           << "  [2] Insertar docente\n"
           << "  [3] Eliminar docente\n"
           << "  [4] Imprimir docentes\n"
           << "  [0] Volver\n\n";

      switch (leerOpcion("  Seleccione una opcion: ", 0, 4)) {
         case 1: buscarDocente(); break;
         case 2: registrarDocente(); break;
         case 3: eliminarDocente(); break;
         case 4: listarDocentes(); break;
         case 0: return;
      }
   }
}

////////////////////////////////////////////////////////////////////////
// Name:       Menu::buscarDocente()
// Purpose:    Implementation of Menu::buscarDocente()
// Return:     void
////////////////////////////////////////////////////////////////////////
// BUSCAR: pide una cedula y muestra los datos del docente si existe.
void Menu::buscarDocente() {
   encabezado("BUSCAR DOCENTE");
   if (sistema.getDocentes().estaVacia()) {
      cout << "  No hay docentes registrados. Use la opcion Insertar docente.\n";
      pausar();
      return;
   }
   const std::string cedula = leerCedula("  Cedula del docente a buscar: ");
   const Docente* docente = sistema.obtenerDocente(cedula);
   if (docente == nullptr) {
      cout << "\n  [ERROR] No existe un docente con la cedula " << cedula
           << ". Ingrese una cedula registrada (use la opcion Imprimir docentes para verlas).\n";
   } else {
      cout << "\n  Docente encontrado:\n";
      mostrarDocente(docente);
   }
   pausar();
}

////////////////////////////////////////////////////////////////////////
// Name:       Menu::eliminarDocente()
// Purpose:    Implementation of Menu::eliminarDocente()
// Return:     void
////////////////////////////////////////////////////////////////////////
// ELIMINAR: muestra el docente y lo que se perderia, y pide confirmacion (s/n).
void Menu::eliminarDocente() {
   encabezado("ELIMINAR DOCENTE");
   if (sistema.getDocentes().estaVacia()) {
      cout << "  No hay docentes registrados. No hay nada que eliminar.\n";
      pausar();
      return;
   }
   const std::string cedula = leerCedula("  Cedula del docente a eliminar: ");
   const Docente* docente = sistema.obtenerDocente(cedula);
   if (docente == nullptr) {
      cout << "\n  [ERROR] No existe un docente con la cedula " << cedula
           << ". Ingrese una cedula registrada (use la opcion Imprimir docentes para verlas).\n";
      pausar();
      return;
   }
   cout << "\n  Se eliminara al siguiente docente:\n";
   mostrarDocente(docente);
   cout << "\n  Consecuencias:\n"
        << "    - " << sistema.contarCursosDeDocente(cedula) << " NRC quedaran sin docente asignado.\n"
        << "    - Se borraran " << sistema.contarRegistrosDeDocente(cedula) << " registro(s) de asistencia.\n\n";
   if (!leerConfirmacion("  Esta seguro de eliminarlo? (s/n): ")) {
      cout << "\n  Operacion cancelada. No se elimino nada.\n";
      pausar();
      return;
   }
   mostrarResultado(sistema.eliminarDocente(cedula));
   pausar();
}

// ---------------------------------------------------------------- gestion de NRC

////////////////////////////////////////////////////////////////////////
// Name:       Menu::menuCursos()
// Purpose:    Implementation of Menu::menuCursos()
// Return:     void
////////////////////////////////////////////////////////////////////////
// Submenu de NRC: buscar, insertar, eliminar e imprimir.
void Menu::menuCursos() {
   while (true) {
      encabezado("GESTION DE NRC");
      cout << "  [1] Buscar NRC\n"
           << "  [2] Insertar NRC\n"
           << "  [3] Eliminar NRC\n"
           << "  [4] Imprimir NRC\n"
           << "  [0] Volver\n\n";

      switch (leerOpcion("  Seleccione una opcion: ", 0, 4)) {
         case 1: buscarCurso(); break;
         case 2: crearCurso(); break;
         case 3: eliminarCurso(); break;
         case 4: listarCursos(); break;
         case 0: return;
      }
   }
}

////////////////////////////////////////////////////////////////////////
// Name:       Menu::buscarCurso()
// Purpose:    Implementation of Menu::buscarCurso()
// Return:     void
////////////////////////////////////////////////////////////////////////
// BUSCAR: pide un NRC y muestra los datos del curso si existe.
void Menu::buscarCurso() {
   encabezado("BUSCAR NRC");
   if (sistema.getCursos().estaVacia()) {
      cout << "  No hay NRC registrados. Use la opcion Insertar NRC.\n";
      pausar();
      return;
   }
   const std::string nrc = leerNrc("  NRC a buscar: ");
   const Curso* curso = sistema.obtenerCurso(nrc);
   if (curso == nullptr) {
      cout << "\n  [ERROR] No existe un curso con el NRC " << nrc
           << ". Ingrese un NRC registrado (use la opcion Imprimir NRC para verlos).\n";
   } else {
      cout << "\n  NRC encontrado:\n";
      mostrarCurso(curso);
   }
   pausar();
}

////////////////////////////////////////////////////////////////////////
// Name:       Menu::eliminarCurso()
// Purpose:    Implementation of Menu::eliminarCurso()
// Return:     void
////////////////////////////////////////////////////////////////////////
// ELIMINAR: muestra el curso y lo que se perderia, y pide confirmacion (s/n).
void Menu::eliminarCurso() {
   encabezado("ELIMINAR NRC");
   if (sistema.getCursos().estaVacia()) {
      cout << "  No hay NRC registrados. No hay nada que eliminar.\n";
      pausar();
      return;
   }
   const std::string nrc = leerNrc("  NRC a eliminar: ");
   const Curso* curso = sistema.obtenerCurso(nrc);
   if (curso == nullptr) {
      cout << "\n  [ERROR] No existe un curso con el NRC " << nrc
           << ". Ingrese un NRC registrado (use la opcion Imprimir NRC para verlos).\n";
      pausar();
      return;
   }
   cout << "\n  Se eliminara el siguiente NRC:\n";
   mostrarCurso(curso);
   cout << "\n  Consecuencias:\n"
        << "    - Se borraran sus " << curso->getHorarios().obtenerTamanio() << " horario(s).\n"
        << "    - Se borraran " << sistema.contarRegistrosDeCurso(nrc) << " registro(s) de asistencia.\n\n";
   if (!leerConfirmacion("  Esta seguro de eliminarlo? (s/n): ")) {
      cout << "\n  Operacion cancelada. No se elimino nada.\n";
      pausar();
      return;
   }
   mostrarResultado(sistema.eliminarCurso(nrc));
   pausar();
}

////////////////////////////////////////////////////////////////////////
// Name:       Menu::listarCursos()
// Purpose:    Implementation of Menu::listarCursos()
// Return:     void
////////////////////////////////////////////////////////////////////////
// IMPRIMIR: tabla con todos los NRC, su materia, su docente y sus horarios.
void Menu::listarCursos() const {
   encabezado("IMPRIMIR NRC");
   if (sistema.getCursos().estaVacia()) {
      cout << "  No hay NRC registrados. Use la opcion Insertar NRC.\n";
   } else {
      cout << "  " << std::left << std::setw(8) << "NRC" << std::setw(26) << "Materia" << std::setw(24) << "Docente"
           << "Horarios\n";
      for (Nodo<Curso>* n = sistema.getCursos().getCabeza(); n != nullptr; n = n->siguiente) {
         const Curso* c = n->dato;
         std::string materia = c->getMateria();
         if (materia.size() > 24) materia.resize(24);       // se recorta para no desalinear la tabla
         std::string docente = "Sin docente";
         if (c->tieneDocente()) {
            const Docente* d = sistema.obtenerDocente(c->getCedulaDocente());
            docente = (d != nullptr) ? d->nombreCompleto() : c->getCedulaDocente();
         }
         if (docente.size() > 22) docente.resize(22);
         cout << "  " << std::left << std::setw(8) << c->getNrc() << std::setw(26) << materia
              << std::setw(24) << docente << horariosTexto(c) << "\n";
      }
      cout << "\n  Total: " << sistema.getCursos().obtenerTamanio() << " NRC.\n";
   }
   pausar();
}

////////////////////////////////////////////////////////////////////////
// Name:       Menu::formatearInformacion()
// Purpose:    Implementation of Menu::formatearInformacion()
// Return:     void
////////////////////////////////////////////////////////////////////////
// Borra todos los datos si el usuario escribe exactamente "delete".
void Menu::formatearInformacion() {
   encabezado("FORMATEAR INFORMACION");
   cout << "  Esta opcion borrara de forma irreversible TODA la informacion:\n"
        << "  docentes, NRC's, horarios y registros de asistencia.\n"
        << "  Si esta seguro/a de ejecutar la opcion ingrese la palabra \"delete\"\n\n";
   const std::string confirmacion = leerTexto("  Confirmacion: ");
   if (confirmacion != "delete") {
      cout << "\n  Para confirmar debe escribir exactamente la palabra delete (en minusculas). Operacion cancelada.\n";
   } else if (sistema.formatear()) {
      cout << "\n  [OK] Informacion formateada y archivo eliminado.\n";
   } else {
      cout << "\n  [OK] Memoria vaciada. El archivo no existia o no se pudo eliminar.\n";
   }
   pausar();
}

////////////////////////////////////////////////////////////////////////
// Name:       Menu::contarResultado(exito, correctos, omitidos)
// Purpose:    Implementation of Menu::contarResultado()
// Parameters:
// - exito
// - correctos
// - omitidos
// Return:     void
////////////////////////////////////////////////////////////////////////
// Cuenta el resultado de una operacion; si fallo, muestra el motivo real.
void Menu::contarResultado(bool exito, int& correctos, int& omitidos) {
   if (exito) {
      ++correctos;
   } else {
      ++omitidos;
      cout << "  - Omitido: " << sistema.getMensaje() << "\n";
   }
}

////////////////////////////////////////////////////////////////////////
// Name:       Menu::cargarDatosEjemplo()
// Purpose:    Implementation of Menu::cargarDatosEjemplo()
// Return:     void
////////////////////////////////////////////////////////////////////////
// Carga 2 docentes, 3 cursos y sus horarios para poder probar el sistema.
// Si algo ya existia o falla, se informa en lugar de decir que todo salio bien.
void Menu::cargarDatosEjemplo() {
   encabezado("DATOS DE EJEMPLO");
   int correctos = 0, omitidos = 0;
   contarResultado(sistema.registrarDocente("1710034065", "Carlos", "Mendoza"), correctos, omitidos);
   contarResultado(sistema.registrarDocente("0912345675", "Ana", "Paredes"), correctos, omitidos);
   contarResultado(sistema.crearCurso("14872", "Estructura de Datos"), correctos, omitidos);
   contarResultado(sistema.crearCurso("16210", "Redes de Datos"), correctos, omitidos);
   contarResultado(sistema.crearCurso("15033", "Programacion Avanzada"), correctos, omitidos);
   contarResultado(sistema.asignarDocente("14872", "1710034065"), correctos, omitidos);
   contarResultado(sistema.asignarDocente("16210", "1710034065"), correctos, omitidos);
   contarResultado(sistema.asignarDocente("15033", "0912345675"), correctos, omitidos);
   contarResultado(sistema.agregarHorario("14872", 1, 1), correctos, omitidos);
   contarResultado(sistema.agregarHorario("16210", 1, 2), correctos, omitidos);
   contarResultado(sistema.agregarHorario("15033", 1, 1), correctos, omitidos);

   cout << "\n  Operaciones realizadas: " << correctos << " de " << (correctos + omitidos) << ".\n";
   if (omitidos > 0) cout << "  Las omitidas ya existian o tenian un error (ver arriba).\n";
   cout << "  Datos de ejemplo:\n"
        << "    Carlos Mendoza (cedula 1710034065): NRC 14872 Lunes 07:00-09:00 y NRC 16210 Lunes 09:00-11:00\n"
        << "    Ana Paredes    (cedula 0912345675): NRC 15033 Lunes 07:00-09:00\n"
        << "  Tip: use la opcion 4 del menu principal para simular un Lunes a las 07:05.\n";
   if (correctos > 0) guardarCambios();
   pausar();
}
