#include <stdio.h>  /* printf, fprintf */
#include <stdlib.h> /* strtol, EXIT_FAILURE, EXIT_SUCCESS */
#include <errno.h>  /* errno */
#include <string.h> /* strlen */

#include "can_transceiver.h"

#define MAX_CAN_HEX_STR (8 << 1)

static char const *usage_msg =
    "Usage: can-transceiver <device> <data> [<filter-canid>]\n"
    "       can-transceiver --help\n";

void *hex2bytes(void *dst, void const *src, size_t len) {
    uint8_t *d = dst;
    char const *s = src;
    char *endptr;

    /* go through each byte and store in dst */
    for (size_t i = 0; i < len; ++i) {
        char buf[] = {'0', 'x', s[0], s[1], 0};
        d[i] = (uint8_t)strtol(buf, &endptr, 0);
        s += 2;

        /* not a hex num */
        if (endptr[0] != '\0') {
            return NULL;
        }
    }

    return dst;
}

int parse(int argc, char *argv[], struct can_config *out) {
    if (argc < 3) {
        fprintf(stderr, "gimme more arguments!\n%s", usage_msg);
        return -1;
    }
    /* set filter_canid to 0 since this is optional.
     * 0 means ignore.
     * any other value than 0 will be taken as a valid filter.
     * */
    out->filter_canid = 0;
    if (strlen(argv[1]) == 0) {
        fprintf(stderr, "huh? a device name can't be empty!\n");
        return -1;
    }
    out->device = argv[1];
    memset(out->data, 0, sizeof(out->data));
    /*
     * A valid CAN-Data
     * 00 11 22 33 44 55 66 77
     *
     * A NON-valid CAN-Data
     * 00 11 22 33 44 55 66 77 88
     *                         ^
     *
     */
    size_t const data_size = strlen(argv[2]);
    if (data_size & 1u) {
        fprintf(stderr, "Please make sure the CAN-Data is even!\n"
                        "Examples:\n"
                        "Valid CAN-Data\n0011223344556677\n\n"
                        "NON-valid CAN-Data\n"
                        "001122334455667788\n"
                        "                ^\n"
                        "                exceeding length here.\n\n"
                        "NON-Valid CAN-Data\n"
                        "00112\n"
                        "    ^\n"
                        "    data is not even (5 bytes).\n");
        return -1;
    }
    if (data_size > MAX_CAN_HEX_STR) {
        fprintf(stderr, "The CAN-Data can only be 8 bytes at max!\n");
        return -1;
    }
    /* copy CAN-Data to internal config data
     * here we only take half of the data size since the hex2bytes
     * function works by parsing one byte instead of each character.
     * */
    if (hex2bytes(out->data, argv[2], data_size / 2) == NULL) {
        fprintf(stderr, "Invalid character in CAN-Data!\n");
        return -1;
    }

    /* optional CAN-ID for filtering frames */
    if (argc > 3) {
        char *endptr;
        out->filter_canid = (uint32_t)strtol(argv[4], &endptr, 16);
        if (errno == ERANGE) {
            fprintf(stderr,
                    "Please provide a valid CAN-ID in HEX/DEC-Format\n");
            errno = 0;
            return -1;
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    struct can_config cfg;
    if (parse(argc, argv, &cfg) == -1) {
        return EXIT_FAILURE;
    }
    printf("CAN-Data in HEX: ");
    for (size_t i = 0; i < sizeof(cfg.data); ++i) {
        printf("0x%02X ", cfg.data[i]);
    }
    printf("\n");
    printf("CAN-Data in ASCII: %s\n", cfg.data);
    printf("can-interface: %s - filter: 0x%x\n", cfg.device, cfg.filter_canid);

    /* setup can transceiver */
    struct can_transceiver can = {.config = &cfg};

    /* open the can device.
     * this will do all the work (receiving, sending using the BCM-Socket)
     * */
    if (can_transceiver_open(&can) == CAN_FAILURE) {
        return EXIT_FAILURE;
    }
    /* clean up */
    if (can_transceiver_close(&can) == CAN_FAILURE) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
