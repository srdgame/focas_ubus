#!/bin/sh

git pull
premake5 --file=premake5_arm.lua gmake
make -C build clean
make -C build &&

scp bin/Debug/focas_ubus cch@172.30.11.139:~
scp bin/Debug/focas_ubus root@172.30.11.232:~
#scp bin/Debug/focas_ubus root@172.30.1.173:~
