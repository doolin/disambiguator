#include <iostream>
#include <stdlib.h>

int disambiguate_main(void);

int
main(int argc, char ** argv) {

  if (argc < 3) {
    std::cout << "usage: disambiguate <engine_conf> <block_conf>" << std::endl;
    exit(0);
  }

  return disambiguate_main();
}
