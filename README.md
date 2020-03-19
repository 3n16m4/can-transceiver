# can-transceiver
simple CAN-Transceiver with BCM sockets written in c99

# setup CAN Bus
```bash
sudo ifconfig DEVICE up
```
or 
```bash
sudo ifconfig DEVICE down
```
when you're done.

## bitrate
### for 500 kbit/s with a sample rate of 87.5% (.875)
```bash
sudo ip link set DEVICE type can bitrate 500000 sample_point 0.875
```
where DEVICE is your connected CAN interface e.g. can0
