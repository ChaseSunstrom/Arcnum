#include <core/logging/log.hpp>
#include <core/spark_main.hpp>
#include <functional>

#include "game.hpp"

int main() {
  SPARK_INFO("[COMPILED AT]: " << __DATE__ << " " << __TIME__);

  // Dont change these
  register_functions();
  spark::spark_main();
}
