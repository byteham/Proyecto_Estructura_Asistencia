/***********************************************************************
 * Module:  Entidades.h
 * Author:  Usuario
 * Purpose: Declaration of the classes Docente, Horario, Curso y RegistroAsistencia
 ***********************************************************************/

#if !defined(__Asistencia_Entidades_h)
#define __Asistencia_Entidades_h

#include <string>
#include <utility>
#include "ListaDoble.h"
#include "Utilidades.h"

// ---------------------------------------------------------------------
// Docente: cedula, nombres y apellidos.
// ---------------------------------------------------------------------
class Docente
{
public:
   Docente(std::string cedula, std::string nombres, std::string apellidos)
      : cedula_(std::move(cedula)), nombres_(std::move(nombres)), apellidos_(std::move(apellidos)) {}

   const std::string& getCedula() const { return cedula_; }
   const std::string& getNombres() const { return nombres_; }
   const std::string& getApellidos() const { return apellidos_; }
   std::string nombreCompleto() const { return nombres_ + " " + apellidos_; }   // "Carlos Mendoza"

private:
   std::string cedula_;
   std::string nombres_;
   std::string apellidos_;
};

// ---------------------------------------------------------------------
// Horario semanal de un curso: dia (1 = lunes ... 6 = sabado) y franja (1..7).
// ---------------------------------------------------------------------
class Horario
{
public:
   Horario(int dia, int franja) : dia_(dia), franja_(franja) {}
   int getDia() const { return dia_; }
   int getFranja() const { return franja_; }

private:
   int dia_;
   int franja_;
};

// ---------------------------------------------------------------------
// Curso (NRC) con su lista de horarios y el docente que lo dicta.
// ---------------------------------------------------------------------
class Curso
{
public:
   Curso(std::string nrc, std::string materia) : nrc_(std::move(nrc)), materia_(std::move(materia)) {}

   const std::string& getNrc() const { return nrc_; }
   const std::string& getMateria() const { return materia_; }
   const std::string& getCedulaDocente() const { return cedulaDocente_; }
   bool tieneDocente() const { return !cedulaDocente_.empty(); }   // vacio = sin docente
   void asignarDocente(const std::string& cedula) { cedulaDocente_ = cedula; }

   // Quien llama debe comprobar antes con tieneHorario que ese dia y franja no existan.
   void agregarHorario(int dia, int franja) { horarios_.insertarFinal(new Horario(dia, franja)); }

   // true si el curso ya tiene clase ese dia y en esa franja (recorre la lista de horarios).
   bool tieneHorario(int dia, int franja) const {
      for (Nodo<Horario>* n = horarios_.getCabeza(); n != nullptr; n = n->siguiente)
         if (n->dato->getDia() == dia && n->dato->getFranja() == franja) return true;
      return false;
   }
   const ListaDoble<Horario>& getHorarios() const { return horarios_; }

private:
   std::string nrc_;
   std::string materia_;
   std::string cedulaDocente_;
   ListaDoble<Horario> horarios_;      // un curso puede tener varios horarios
};

// ---------------------------------------------------------------------
// Asistencia de un docente a un NRC en una fecha y franja concretas.
// Las horas se guardan en minutos desde la medianoche (07:05 -> 425).
// -1 significa "sin registrar".
// El estado no se almacena: se deduce de la hora de ingreso.
// ---------------------------------------------------------------------
class RegistroAsistencia
{
public:
   RegistroAsistencia(std::string fecha, std::string nrc, std::string cedulaDocente,
                      int franja, int ingreso, int salida)
      : fecha_(std::move(fecha)), nrc_(std::move(nrc)), cedulaDocente_(std::move(cedulaDocente)),
        franja_(franja), ingreso_(ingreso), salida_(salida) {}

   void registrarSalida(int minutos) { salida_ = minutos; }
   bool tieneIngreso() const { return ingreso_ >= 0; }
   bool tieneSalida() const { return salida_ >= 0; }

   const std::string& getFecha() const { return fecha_; }
   const std::string& getNrc() const { return nrc_; }
   const std::string& getCedulaDocente() const { return cedulaDocente_; }
   int getFranja() const { return franja_; }
   int getHoraIngreso() const { return ingreso_; }
   int getHoraSalida() const { return salida_; }
   const char* getEstado() const { return estadoTexto(franja_, ingreso_); }

   // PUNTUAL, ATRASO o AUSENTE (sin ingreso) segun la hora de ingreso en esa franja.
   static const char* estadoTexto(int franja, int ingreso) {
      if (ingreso < 0) return "AUSENTE";
      return ingreso <= Utilidades::limitePuntual(franja) ? "PUNTUAL" : "ATRASO";
   }

private:
   std::string fecha_;
   std::string nrc_;
   std::string cedulaDocente_;
   int franja_;
   int ingreso_;        // minutos desde la medianoche, -1 si no hay
   int salida_;         // minutos desde la medianoche, -1 si no hay
};

#endif
