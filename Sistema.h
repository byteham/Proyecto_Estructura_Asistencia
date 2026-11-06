/***********************************************************************
 * Module:  Sistema.h
 * Author:  Usuario
 * Purpose: Declaration of the class Sistema
 ***********************************************************************/

#if !defined(__Asistencia_Sistema_h)
#define __Asistencia_Sistema_h

#include <string>
#include "Entidades.h"
#include "ListaDoble.h"

// Logica del sistema y persistencia en un archivo .txt. No imprime ni lee del
// teclado: eso lo hace Menu.
// Cada operacion devuelve true/false y deja en getMensaje() el texto para
// mostrar al usuario.
// El dia es un entero (1 = lunes ... 6 = sabado) y la hora se recibe en
// minutos, para poder probarlo en cualquier momento.
//
// Formato del archivo (una linea por dato, campos separados por '|'):
//   D|cedula|nombres|apellidos                                  docente
//   C|nrc|materia|cedulaDocente  (o '-' si no tiene)            curso
//   H|nrc|dia(1-6)|franja(1-7)                                  horario de un curso
//   R|fecha|nrc|cedula|franja|ingreso|salida|estado             asistencia (minutos, '-' si no hay)
// Al cargar, cada linea pasa por las mismas validaciones que el menu: una
// linea invalida se ignora y se avisa, pero el programa nunca se cae.
class Sistema
{
public:
   explicit Sistema(const std::string& ruta);

   // --- Administrador ---
   bool registrarDocente(const std::string& cedula, const std::string& nombres, const std::string& apellidos);
   bool crearCurso(const std::string& nrc, const std::string& materia);
   bool asignarDocente(const std::string& nrc, const std::string& cedula);
   bool agregarHorario(const std::string& nrc, int dia, int franja);
   bool eliminarDocente(const std::string& cedula);   // sus cursos quedan sin docente y se borran sus registros
   bool eliminarCurso(const std::string& nrc);        // se borran sus horarios y sus registros

   // Cuentan lo que se perderia al eliminar (el Menu lo muestra antes de pedir confirmacion).
   int contarCursosDeDocente(const std::string& cedula) const;
   int contarRegistrosDeDocente(const std::string& cedula) const;
   int contarRegistrosDeCurso(const std::string& nrc) const;

   // --- Docente ---
   bool registrarIngreso(const std::string& cedula, int dia, int minutos, const std::string& fecha);
   bool registrarSalida(const std::string& cedula, int dia, int minutos, const std::string& fecha);

   // --- Archivo ---
   int cargar();        // -1 si el archivo no existe; si no, cantidad de lineas ignoradas
   bool guardar();      // si falla, el motivo queda en getMensaje()
   bool formatear();    // vacia todo y borra el archivo; true si el archivo se pudo borrar

   // --- Consultas ---
   const std::string& getMensaje() const { return mensaje; }
   const std::string& getRuta() const { return ruta; }
   const Docente* obtenerDocente(const std::string& cedula) const;
   const Curso* obtenerCurso(const std::string& nrc) const;
   const ListaDoble<Docente>& getDocentes() const { return docentes; }
   const ListaDoble<Curso>& getCursos() const { return cursos; }
   const ListaDoble<RegistroAsistencia>& getRegistros() const { return registros; }
   const ListaDoble<std::string>& getAvisos() const { return avisos; }   // primeras lineas rechazadas al cargar

private:
   std::string ruta;                            // nombre del archivo de datos
   std::string mensaje;                         // ultimo mensaje (exito o error)
   ListaDoble<Docente> docentes;
   ListaDoble<Curso> cursos;
   ListaDoble<RegistroAsistencia> registros;
   ListaDoble<std::string> avisos;

   // Guardan el mensaje y devuelven false / true, para escribir "return fallo(...)" en una linea.
   bool fallo(const std::string& texto) { mensaje = texto; return false; }
   bool exito(const std::string& texto) { mensaje = texto; return true; }

   Nodo<Docente>* buscarNodoDocente(const std::string& cedula) const;
   Nodo<Curso>* buscarNodoCurso(const std::string& nrc) const;
   Curso* buscarCurso(const std::string& nrc) const;
   int borrarRegistrosDeDocente(const std::string& cedula);
   int borrarRegistrosDeCurso(const std::string& nrc);
   RegistroAsistencia* buscarRegistro(const std::string& fecha, const std::string& nrc, int franja) const;
   const Curso* cursoEnConflicto(const std::string& cedula, int dia, int franja,
                                 const std::string& nrcExcluido) const;
   std::string mensajeCruce(const std::string& cedula, const Curso& otro, int dia, int franja) const;
   Curso* ubicarClase(const std::string& cedula, int dia, int minutos, int& franja);
   bool cargarRegistro(const std::string& fecha, const std::string& nrc, const std::string& cedula,
                       int franja, int ingreso, int salida);
   bool procesarLinea(const std::string& linea);
};

#endif
