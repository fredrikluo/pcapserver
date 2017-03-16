#include <stdio.h>
#include <stdlib.h>

#include "pcapserver.h"
#include "utility.h"

int main(int argc, char* argv[])
{
    if (argc < 3) {
        ERROR_OUT(("NO file name specified or not port"));
        return -1;
    }

    int port = atoi(argv[2]);

    if (port <= 0) {
        ERROR_OUT(("Bad port"));
        return -2;
    }

    pcapserver ps;
    ps.start(argv[1], port);
    return 0;
}
