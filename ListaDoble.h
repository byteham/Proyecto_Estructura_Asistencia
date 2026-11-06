/***********************************************************************
 * Module:  ListaDoble.h
 * Author:  Usuario
 * Purpose: Declaration of the class ListaDoble (y de su clase Nodo)
 ***********************************************************************/

#if !defined(__Asistencia_ListaDoble_h)
#define __Asistencia_ListaDoble_h

#include <cstddef>

// ---------------------------------------------------------------------
// Nodo: cada elemento de la lista doble.
// Guarda un puntero al dato (creado con new) y los enlaces al nodo
// siguiente y al anterior. Cuando el nodo se destruye, tambien libera
// el dato que guarda (asi no quedan fugas de memoria).
// ---------------------------------------------------------------------
template <typename T>
class Nodo
{
public:
   T* dato;                  // puntero al dato guardado (memoria dinamica)
   Nodo<T>* siguiente;       // nodo que viene despues (nullptr si es el ultimo)
   Nodo<T>* anterior;        // nodo que viene antes (nullptr si es el primero)

   explicit Nodo(T* nuevoDato) : dato(nuevoDato), siguiente(nullptr), anterior(nullptr) {}
   ~Nodo() { delete dato; }  // libera el dato al destruir el nodo
};

// ---------------------------------------------------------------------
// ListaDoble: lista doblemente enlazada generica.
// Es duena de los datos que recibe: los libera en vaciar() y en el
// destructor. Para recorrerla se usa getCabeza() (hacia adelante) o
// getCola() (hacia atras) dentro de un ciclo for.
// ---------------------------------------------------------------------
template <typename T>
class ListaDoble
{
public:
   ListaDoble() : cabeza(nullptr), cola(nullptr), tamanio(0) {}
   // La lista no se puede copiar: dos listas liberarian los mismos datos.
   ListaDoble(const ListaDoble&) = delete;
   ListaDoble& operator=(const ListaDoble&) = delete;
   ~ListaDoble() { vaciar(); }

   // Agrega al final un dato creado con new y devuelve ese mismo puntero.
   T* insertarFinal(T* dato) {
      Nodo<T>* nuevo = new Nodo<T>(dato);
      nuevo->anterior = cola;                  // el nuevo apunta atras al antiguo ultimo
      if (cola != nullptr) cola->siguiente = nuevo;   // el antiguo ultimo apunta adelante al nuevo
      else cabeza = nuevo;                     // si la lista estaba vacia, es tambien la cabeza
      cola = nuevo;
      ++tamanio;
      return dato;
   }

   Nodo<T>* getCabeza() const { return cabeza; }   // primer nodo (para recorrer hacia adelante)
   Nodo<T>* getCola() const { return cola; }       // ultimo nodo (para recorrer hacia atras)
   std::size_t obtenerTamanio() const { return tamanio; }
   bool estaVacia() const { return tamanio == 0; }

   // Elimina un nodo concreto de la lista y libera su dato.
   // El nodo debe pertenecer a esta lista (se obtiene al recorrerla con getCabeza()).
   // Hay que reenlazar los vecinos del nodo para que la lista quede continua.
   void eliminar(Nodo<T>* nodo) {
      if (nodo == nullptr) return;
      if (nodo->anterior != nullptr) nodo->anterior->siguiente = nodo->siguiente;
      else cabeza = nodo->siguiente;           // no tenia anterior: era la cabeza
      if (nodo->siguiente != nullptr) nodo->siguiente->anterior = nodo->anterior;
      else cola = nodo->anterior;              // no tenia siguiente: era la cola
      delete nodo;                             // el destructor del nodo libera tambien su dato
      --tamanio;
   }

   // Borra todos los nodos (y sus datos) y deja la lista vacia.
   void vaciar() {
      while (cabeza != nullptr) {
         Nodo<T>* siguiente = cabeza->siguiente;   // guardar el siguiente antes de borrar
         delete cabeza;
         cabeza = siguiente;
      }
      cola = nullptr;
      tamanio = 0;
   }

private:
   Nodo<T>* cabeza;          // primer nodo de la lista
   Nodo<T>* cola;            // ultimo nodo de la lista
   std::size_t tamanio;      // cantidad de nodos
};

#endif
