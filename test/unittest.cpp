
#include "test_typedefs.h"
#include "test_comparators.h"
#include "test_attribute.h"
#include "test_blocking.h"

/**
 * Invoking program specific defines for adding main into
 * standalone files. This is handy when module or class can
 * be used as a standalone code or as a library.
 */
/*
 #-D$*_STANDALONE

 $(objects): %.o: %.c
         $(CC) -c $(CFLAGS) $(INCLUDES) -D$*_STANDALONE $<
#ifdef rose_STANDALONE
 */

void
test_all() {

}

int
main(int argc, char ** argv) {

  return 0;
}
