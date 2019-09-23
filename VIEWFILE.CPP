#include <string.h>
#include "proboard.hpp"

void
view_named_file(char *data)
{
io.color(7);
io.sendfile(data);
}

void
view_file(char *data)
{
String total(data);
char filename[13];
io << '\n' << S_ENTER_FILENAME_TO_DISPLAY;
io.read(filename,12);
total << filename;

io.color(7);
io.sendfile(total);
}

void
view_file_wait(char *data)
{
io.color(7);
io.sendfile(data);
io << '\n' << S_PRESS_ENTER_TO_CONTINUE;
}
