/***********************************************************************
 * Module:  Menu.h
 * Author:  Usuario
 * Purpose: Declaration of the class Menu
 ***********************************************************************/

#if !defined(__Asistencia_Menu_h)
#define __Asistencia_Menu_h

#include <string>
#include "Sistema.h"

// Interfaz de consola. Solo pide datos, llama a Sistema y muestra los resultados.
class Menu
{
public:
   explicit Menu(const std::string& rutaArchivo = "datos_asistencia.txt");
   void ejecutar();

private:
   Sistema sistema;
   bool horaSimulada;       // true si se esta usando una fecha/hora de prueba
   std::string fecha;       // fecha "dd/mm/aaaa" vigente (real o simulada)
   int minutos;             // hora vigente en minutos desde la medianoche
   int dia;                 // 0 = domingo ... 6 = sabado

   void actualizarReloj();                 // si no hay simulacion, lee la hora real
   void mostrarResultado(bool exito);      // muestra el mensaje y, si salio bien, guarda en el archivo
   void cargarDatos();
   void guardarCambios();
   const Curso* pedirCurso(std::string& nrc);
   void mostrarFila(const RegistroAsistencia* registro) const;
   void contarResultado(bool exito, int& correctos, int& omitidos);
   std::string nrcsDeDocente(const std::string& cedula) const;     // "14872 16210 " o ""
   std::string horariosTexto(const Curso* curso) const;            // "Lunes 07:00-09:00, ..."
   void mostrarDocente(const Docente* docente) const;
   void mostrarCurso(const Curso* curso) const;

   // Menu principal
   void registrarMarca(bool esIngreso);
   void simularFechaHora();

   // Menu de administrador
   void menuAdministrador();
   void asignarDocente();
   void agregarHorario();
   void reporteAsistencia() const;
   void formatearInformacion();
   void cargarDatosEjemplo();

   // Gestion de docentes: buscar, insertar, eliminar, imprimir
   void menuDocentes();
   void buscarDocente();
   void registrarDocente();          // insertar
   void eliminarDocente();
   void listarDocentes() const;      // imprimir

   // Gestion de NRC: buscar, insertar, eliminar, imprimir
   void menuCursos();
   void buscarCurso();
   void crearCurso();                // insertar
   void eliminarCurso();
   void listarCursos() const;        // imprimir
};

#endif
