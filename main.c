#include <stdio.h>  /* printf, fprintf */
#include <stdlib.h> /* EXIT_FAILURE, EXIT_SUCCESS */
#include <string.h> /* strcmp, strncpy */
#include <errno.h>  /* errno */

#include "can_transceiver.h"

static char const *usage_msg =
"Usage: can-transceiver <device> <data> <bitrate> [<filter-canid>]\n"
"       can-transceiver --help\n";

int parse(int argc, char *argv[], struct can_config *out) {
    if (argc < 4) {
        fprintf(stderr, "too few arguments!\n");
        fprintf(stderr, "%s", usage_msg);
        return -1;
    }

    out->device = argv[1];
    out->data = (uint32_t)atoi(argv[2]);
    out->bitrate = (uint32_t)atoi(argv[3]);
    /* optional CAN-ID for filtering frames */
    if (argc >= 4) {
        char *endp;
        out->filter_canid = (uint32_t)strtol(argv[4], &endp, 16);
        if (errno == ERANGE){
            fprintf(stderr, "Please provide a valid CAN-ID in HEX/DEC-Format\n");
            errno = 0;
            return -1;
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    struct can_config config;
    if (parse(argc, argv, &config) == -1) {
        return EXIT_FAILURE;
    }
    printf("can-interface: %s - data-frame: %d - bitrate: %d kbit/s - filter: 0x%x",
                                                                      config.device,
                                                                      config.data,
                                                                      config.bitrate,
                                                                      config.filter_canid);
    return EXIT_SUCCESS;
}

