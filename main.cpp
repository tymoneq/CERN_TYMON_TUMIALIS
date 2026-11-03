#include "include/EventGenerator.hpp"
#include <iostream>

int main() {

  EventGenerator eg;
  if (!eg.init(2212, 2212, "MinimumBias", 13600.0)) {
    return 1;
  }
  eg.generate();

  return 0;
}
