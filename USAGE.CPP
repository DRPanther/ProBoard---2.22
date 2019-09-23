#include <string.h>
#include "proboard.hpp"

void
show_graph(char *data)
{
    run_sdkfile(String("_GRAPH ") + data);
}

void
usage_graph(char *data)
{
    show_graph(form("HOURLY %s",data));
}

void
days_graph(char *data)
{
    show_graph(form("LASTDAYS %s",data));
}

void
weeks_graph(char *data)
{
    show_graph(form("WEEKLY %s",data));
}
