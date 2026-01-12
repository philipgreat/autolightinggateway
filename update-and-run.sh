adb connect 192.168.50.108
wget -O lights https://kg2x.com/lights-armv7
adb push lights  /sysyem/vendor/moduels/lights
adb shell chmod 777 /sysyem/vendor/moduels/lights
adb shell /sysyem/vendor/moduels/lights  /dev/ttyUSB0 

