#ifdef WIN32
#include <windows.h>
#else
#include <time.h>
#endif // win32

void sleep_for_ms(int milliseconds) {
  #ifdef WIN32
  Sleep(milliseconds);
  #else
  struct timespec t;
  t.tv_sec = 0;
  t.tv_nsec = milliseconds * 1000000; // 1 milliseconds == 1,000,000 nanoseconds
  nanosleep(&t, NULL);
  #endif
}
