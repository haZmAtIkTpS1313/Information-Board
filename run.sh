#!/bin/bash
xhost +local:root 2>/dev/null
docker run -it --rm \
    -e DISPLAY=$DISPLAY \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    -e QT_X11_NO_MITSHM=1 \
    information-board:test