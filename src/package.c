#include <unistd.h>
#include <stdio.h>
#include <inip/ini.h>

int package(char* filename, char* outfile, int flags) {
    /* do this all through fs instead */
    // access outfile (executable)
    int _res = 0;
    // access filename (.sdom)
    _res = access(filename, R_OK);
    if (_res != 0) {
        perror("Failed to access file for writing!");
        goto exit;
    }
    // process flags
    // make a `.s` that .incbin's filename
    // gcc -c thing.s -o thing.o
    // load config.ini [package] section and apply settings | oh i forgot i was gonna do it like that
    // build executable moment

exit:
    return 0;
}
