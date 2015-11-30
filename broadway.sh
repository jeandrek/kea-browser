#!/bin/bash
broadwayd &
GDK_BACKEND=broadway kea --kiosk
kill $!
