/***********************************************************************
 * Module:  main.cpp
 * Author:  Usuario
 * Purpose: Punto de entrada del programa
 ***********************************************************************/

#include <exception>
#include <iostream>
#include "Menu.h"

////////////////////////////////////////////////////////////////////////
// Name:       main()
// Purpose:    Implementation of main()
// Return:     int
////////////////////////////////////////////////////////////////////////
int main() {
   // Todo el programa corre dentro de un try/catch: si ocurre un error
   // inesperado se muestra un mensaje en lugar de cerrarse de golpe.
   try {
      Menu menu;          // crea el menu (que a su vez crea el Sistema)
      menu.ejecutar();    // bucle principal del programa
   } catch (const std::exception& e) {
      std::cerr << "Error inesperado: " << e.what() << "\n";
      return 1;
   } catch (...) {
      std::cerr << "Error inesperado.\n";
      return 1;
   }
   return 0;
}
