#include <iostream>
#include <string>
#include <stdlib.h>

#include <getopt.h>

int disambiguate_main(std::string & engineconf, std::string & blockconf);

int
main(int argc, char ** argv) {

  if (argc < 3) {
    std::cout << "usage: disambiguate <engine_conf> <block_conf>" << std::endl;
    exit(0);
  }

  std::string engineconf(argv[1]);
  std::string blockconf(argv[2]);

  return disambiguate_main(engineconf, blockconf);
}
