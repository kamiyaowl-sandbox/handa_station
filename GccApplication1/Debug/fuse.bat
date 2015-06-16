echo off
cls
avrsp -RF -pb5:312500 -d6
echo "20MHz XTAL 1/1 FL(01100010 -> 11100111)"
pause
avrsp -FL11100111 -pb5:312500 -d6
avrsp -RF -pb5:312500 -d6
pause