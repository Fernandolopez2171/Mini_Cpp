
# MiniCpp Compiler

## Descripción

MiniCpp es un compilador simple desarrollado para la clase de Compiladores 2. Este proyecto incluye un analizador léxico (lexer) y un analizador sintáctico (parser) diseñados para procesar un subconjunto del lenguaje C++. MiniCpp permite analizar archivos de código fuente escritos en este subconjunto y generar la estructura sintáctica del programa.

## Estructura del Proyecto

El proyecto está compuesto por los siguientes archivos principales:

- **main.cpp**: Contiene el punto de entrada del programa. Este archivo inicializa el lexer y el parser y se encarga de manejar el flujo principal de ejecución.
  
- **MiniCppLexer.hpp**: Define la clase `Lexer`, que se encarga de la tokenización del código fuente de entrada, identificando palabras clave, identificadores, operadores, y otros elementos sintácticos del lenguaje.
  
- **MiniCppLexer.re**: Contiene las expresiones regulares que definen los patrones de los tokens utilizados por el lexer.
  
- **MiniCppParser.cpp**: Implementa la clase `Parser`, que toma los tokens generados por el lexer y los organiza en una estructura de árbol sintáctico abstracto (AST) que representa la lógica del programa.
  
- **MiniCppParser.hpp**: Declara la clase `Parser` y sus métodos auxiliares para el análisis sintáctico, como el manejo de expresiones aritméticas, declaraciones de variables, y más.

## Requisitos

Para compilar y ejecutar MiniCpp, necesitas tener instalado:

- Un compilador de C++ compatible con C++11 o superior.

## Contribución

Este proyecto es parte de un curso académico, pero si tienes sugerencias o mejoras, siéntete libre de crear un issue o enviar un pull request.

## Licencia

Este proyecto está bajo la Licencia MIT. Puedes ver más detalles en el archivo `LICENSE`.

