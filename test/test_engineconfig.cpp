
// This needs to be in a library for test code. =(
//
//
//

#include <iostream>
#include <string>
#include <stdlib.h>


int
main(int argc, char ** argv) {

 /* This is what the main does for handling the engine and
  * blocking configuration files. We'll mirror it for now.
  * TODO: Simplify the file handling for engine and blocking.
  */
  std::string engineconf("./testdata/engine.txt");
  const char * EngineConfigFile = engineconf.c_str();
  EngineConfiguration::config_engine(EngineConfigFile, std::cout)

  return 0;

}
