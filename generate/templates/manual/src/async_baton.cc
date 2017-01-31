#include "../include/async_baton.h"

void deleteBaton(AsyncBaton *baton) {
  delete baton;
}
