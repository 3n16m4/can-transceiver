#ifndef CAN_TRANSCEIVER_CAN_TRANSCEIVER_H
#define CAN_TRANSCEIVER_CAN_TRANSCEIVER_H

// version 2 of the GNU General Public License

/**
 * bitrate 500000 sample_point 0.875 (87.5%)
 * https://www.kernel.org/doc/Documentation/networking/can.txt
 */

#include <linux/can.h>
#include <linux/can/raw.h>

#include <net/if.h>
#include <sys/socket.h>

#include <stdint.h>

typedef int32_t sockfd_t;

struct can_config {
    char *device;
    uint8_t data[CAN_MAX_DLEN] __attribute__((aligned(8)));
    uint32_t filter_canid;
};

struct can_transceiver {
    struct can_config *config;
    struct ifreq ifr;
    struct sockaddr_can addr;
    sockfd_t sfd;
};

typedef enum { CAN_SUCCESS = 0, CAN_FAILURE = -1 } can_types;

int can_transceiver_open(struct can_transceiver *ctrans);

int can_transceiver_close(struct can_transceiver *ctrans);

int can_transceiver_send(void const *data, size_t size);

int can_transceiver_recv(void *data, size_t size);

#endif // CAN_TRANSCEIVER_CAN_TRANSCEIVER_H
