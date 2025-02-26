#include "logs_test.h"
#include "nsdt_test.h"

#define NSTD_TEST 1
#define LOGS_TEST 1

int main() {

#if NSTD_TEST
    setlocale(0, "ukr");
    nstd_test();
#endif

#if LOGS_TEST
    logs_test();
#endif

}
