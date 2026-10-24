#pragma once

#include <iostream>

namespace tuix {

/**
 * @brief Imprime uma ou mais informações no terminal.
 *
 * Função genérica que utiliza templates variádicos e fold expressions
 * para imprimir múltiplos argumentos de diferentes tipos no terminal.
 *
 * Os argumentos são impressos sequencialmente, sem separadores
 * automáticos, e uma quebra de linha é adicionada ao final.
 *
 * @tparam Args Tipos dos argumentos que serão impressos.
 * @param args Argumentos a serem impressos no terminal.
 *
 * @note Requer C++17 ou superior.
 * @note Os tipos fornecidos devem suportar o operador de inserção
 *       em fluxo (operator<<) com std::ostream.
 *
 * @code
 * tuix::tprint("Hello, World!");
 * tuix::tprint("FPS: ", 60);
 * tuix::tprint("Player: ", "Pedro", " | Health: ", 100);
 * @endcode
 */
template <typename... Args> void tprint(const Args &...args) {
  (std::cout << ... << args) << '\n';
}

} // namespace tuix