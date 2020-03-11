# can-transceiver
simple CAN-Transceiver with BCM sockets written in c99

# setup CAN Bus
```bash
sudo ifconfig canX up
```
or 
```bash
sudo ifconfig canX down
```
when you're done.

## bitrate
```bash
sudo ip link set canX type can bitrate 500000
```
