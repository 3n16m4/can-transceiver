#!/bin/sh

docker build -t can-transceiver .
docker run -it --rm --name can-app can-transceiver
