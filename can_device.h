#ifndef CAN_TRANSCEIVER_CAN_DEVICE_H
#define CAN_TRANSCEIVER_CAN_DEVICE_H

/**
 *
 * main communication with underyling linux network layer.
 *
 * NOTE: this is mainly copy and pasted from the linux kernel.
 *       slight changes were made to fit the needs for this purpose.
 */

#include <linux/can/netlink.h>

#include <stdint.h>

typedef uint32_t bitrate_t;
typedef int32_t sockfd_t;

struct can_device {
    const char *device;
};

// front-end functions 
int can_start(struct can_device *can);

int can_stop(struct can_device *can);

int can_restart(struct can_device *can);

int can_get_bittiming(struct can_device *can, struct can_bittiming *bt);

int can_set_bittiming(struct can_device *can, struct can_bittiming *bt);

int can_get_clock(struct can_device *can, struct can_clock *clk);

int can_set_bitrate(struct can_device *can, bitrate_t rate);

// should be hidden
static int can_open_nl_sock();

int can_get_nl_link(struct can_device *can, sockfd_t fd, uint8_t acquire, void *out);
int can_get_link(struct can_device *can, uint8_t acquire, void *out);

int can_set_nl_link(struct can_device *can, sockfd_t fd, uint8_t acquire, struct req_info *req_info);
int can_set_link(struct can_device *can, uint8_t acquire, struct req_info *req_info);

#endif // CAN_TRANSCEIVER_CAN_DEVICE_H
