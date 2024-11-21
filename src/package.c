#include <unistd.h>
#include <stdio.h>

int package(char* filename, char* outfile, int flags) {
    // access outfile (executable)
    int _res = 0;
    // access filename (.sdom)
    _res = access(filename, R_OK);
    if (_res != 0) {
        perror("Failed to access file for writing!");
        goto exit;
    }
    // process flags
    // read sdom
    // load config.ini [package] section and apply settings
    // required gcc run

exit:
    return 0;
}
