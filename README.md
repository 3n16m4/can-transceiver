# can-transceiver
simple CAN-Transceiver with BCM sockets written in c99

# setup CAN Bus
sudo ifconfig canX up
or 
sudo ifconfig canX down
when you're done.

## bitrate
sudo ip link set canX type can bitrate 500000
