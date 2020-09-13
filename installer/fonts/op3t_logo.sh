#!/usr/bin/bash

echo =================================================================
echo comma boot logo & bootanimation change by op3t
echo =================================================================
dd if=./logo.bin of=/dev/block/bootdevice/by-name/LOGO
mount -o rw,remount /system
cp ./bootanimation.zip /system/media/
echo =================================================================
echo = eon by lepro3
echo dd if=./splash.img of=/dev/block/bootdevice/by-name/splash
echo mount -o rw,remount /system
echo cp ./bootanimation.zip /system/media/
echo =================================================================
