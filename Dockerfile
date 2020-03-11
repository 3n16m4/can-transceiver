FROM gcc:latest
COPY . /usr/src/can-transceiver
WORKDIR /usr/src/can-transceiver
RUN gcc -o can-transceiver main.c can_transceiver.c -Wall -Wextra -Wconversion -Wunused -Wduplicated-branches -Wduplicated-cond -Wshadow
CMD ["./can-transceiver", "can0", "13371337", "500000", "0x123"]
