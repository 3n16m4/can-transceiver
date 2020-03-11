#include "can_transceiver.h"

#include <sys/ioctl.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

/* 
 * modified version for lightweight usage of:
 * https://elixir.bootlin.com/linux/v3.2/source/Documentation/networking/ifenslave.c#L988
 * https://git.busybox.net/busybox/tree/networking/ifenslave.c?h=1_10_stable
 * https://elixir.bootlin.com/linux/latest/source/include/uapi/linux/can/bcm.h#L67
 *
 * */

static int get_if_settings(sockfd_t fd, char const *ifname, struct ifreq *ifr) {
    int res = 0;

	strncpy(ifr->ifr_name, ifname, IFNAMSIZ);
	res |= ioctl(fd, SIOCGIFFLAGS, &ifr);

    if (res < 0) {
        fprintf(stderr, "Interface '%s': Error: %s failed: %s\n", 
                ifname, 
                ifr->ifr_name,
                strerror(errno));
        return errno;
    }

	return 0;
}

static int set_if_flags(sockfd_t fd, char const *ifname, short flags) {
	struct ifreq ifr;
    int res = 0;

	ifr.ifr_flags = flags;
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);

	if ((res = ioctl(fd, SIOCSIFFLAGS, &ifr)) < 0) {
		fprintf(stderr, "Interface '%s': Error: SIOCSIFFLAGS failed: %s\n",
			ifname, strerror(errno));
    }
	else {
		fprintf(stderr, "Interface '%s': flags set to %04X.\n", ifname, flags);
	}

	return res;
}

static int set_if_up(sockfd_t fd, char const *ifname, short flags) {
	return set_if_flags(fd, ifname, flags | IFF_UP);
}

static int set_if_down(sockfd_t fd, char const *ifname, short flags) {
	return set_if_flags(fd, ifname, flags & ~IFF_UP);
}

int can_transceiver_open(struct can_transceiver *ctrans) {
    int res = 0;
    ssize_t nbytes = 0;

    if ((res = get_if_settings(ctrans->sfd, ctrans->config->device, &ctrans->ifr)) < 0) {
        return CAN_FAILURE;
    }

    /* set up if was down */
    if (ctrans->ifr.ifr_flags & ~IFF_UP) {
        fprintf(stdout, "Interface %s is down. Trying to wake up...\n", ctrans->config->device);
        if (set_if_up(ctrans->sfd, ctrans->config->device, ctrans->ifr.ifr_flags) < 0) {
            return CAN_FAILURE;
        }
    }

    /* setup socket */
    if ((ctrans->sfd = socket(AF_CAN, SOCK_DGRAM, CAN_BCM)) < 0) {
        fprintf(stderr, "failed setting up socket for BCM\n");
        return CAN_FAILURE;
    }

    /* set if flags and name for CAN (just forward the flags retrieved from the device) */
    if (set_if_flags(ctrans->sfd, ctrans->config->device, ctrans->ifr.ifr_flags) < 0) {
        return CAN_FAILURE;
    }

    /* request the interface index */
    if ((res = ioctl(ctrans->sfd, SIOCGIFINDEX, &ctrans->ifr)) < 0) {
        fprintf(stderr, "failed to retrieve interface index for Interface %s!\n", ctrans->config->device);
        return CAN_FAILURE;
    }

    /* connect socket to CAN */
    memset(&ctrans->addr, 0, sizeof(ctrans->addr));
    if ((res = connect(ctrans->sfd, (struct sockaddr *)&ctrans->addr, sizeof(ctrans->addr))) < 0) {
        fprintf(stderr, "failed connecting to CAN-BUS %s!\n", ctrans->config->device);
        return CAN_FAILURE;
    }
    puts("successfully connected to CAN-BUS!");

    struct can_frame frame;

    /* read CAN frame */
    if ((nbytes = read(ctrans->sfd, &frame, sizeof(struct can_frame))) != sizeof(struct can_frame)) {
        fprintf(stderr, "read error: incomplete CAN frame! (%s)\n", strerror(errno));
        return CAN_FAILURE;
    }
    fprintf(stdout, "read %zd bytes from CAN-BUS\n", nbytes);

    /* modify CAN frame and write back */
    if ((nbytes = write(ctrans->sfd, &frame, sizeof(struct can_frame))) < 0) {
        fprintf(stderr, "write error: %s\n", strerror(errno));
        return CAN_FAILURE;
    }
    fprintf(stdout, "sent %zd bytes to CAN-BUS\n", nbytes);

#if 0
    struct can_frame {
            canid_t can_id;                              /* 32 bit CAN_ID + EFF/RTR/ERR flags */
            __u8    can_dlc;                             /* frame payload length in byte (0 .. 8) */
            __u8    __pad;                               /* padding */
            __u8    __res0;                              /* reserved / padding */
            __u8    __res1;                              /* reserved / padding */
            __u8    data[8] __attribute__((aligned(8))); /* frame payload */
    };
#endif

    /* cleanup */
    if (can_transceiver_close(ctrans) == -1) {
        return CAN_FAILURE;
    }

    return CAN_SUCCESS;
}

int can_transceiver_close(struct can_transceiver *ctrans) {
    int res = 0;
    if ((res = close(ctrans->sfd)) < 0) {
        return CAN_FAILURE;
    }
    if ((res = set_if_down(ctrans->sfd, ctrans->config->device, ctrans->ifr.ifr_flags)) < 0) {
        return CAN_FAILURE;
    }
    return CAN_SUCCESS;
}










