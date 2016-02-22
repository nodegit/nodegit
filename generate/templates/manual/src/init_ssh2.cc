// We are initializing libssh2 from a separate .cc file to avoid ssize_t
// redefinition conflicts caused by incliding both node.h and libssh2.h from
// the same file (e.g. nodegit.cc)
//
// The redefinition can also be avoided by #defines but that is risky in case
// the libraries depend on the different definitions.

#include <libssh2.h>

void init_ssh2() {
  libssh2_init(0);
}
