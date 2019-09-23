#include <stdlib.h>
#include "proboard.hpp"

void
lastcallers(char *data)
{
 run_sdkfile(String("_LC ") + data);
}
