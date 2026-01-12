adb connect 192.168.50.108
wget -O serinfo  https://kg2x.com/serinfo
adb push serinfo  /sysyem/vendor/moduels/serinfo
adb shell chmod 777 /sysyem/vendor/moduels/serinfo
adb shell ls -l /sysyem/vendor/moduels/
adb shell /sysyem/vendor/moduels/serinfo /dev/ttyUSB0 

