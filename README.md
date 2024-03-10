# ESP32_PC_Monitor
Works on windows
Read AIDA64 data and display it to a TFT screen

### Aida64
File > Preferences > External Applications
> Check "Enable shared memory"
Then check the values you want to send to the monitor

### Arduino
Compile the sketch for ESP32
I use the TFT_eSPI lib that must be configured as you need
Check "User_Setup.h"

The code can be adapted to whatever you need

## Windows
Compile from "srcs" :
`g++ main.cpp aida.cpp serial.cpp -o esp_aida`

And run .exe from cmd
`esp_aida.exe COM3 [debug=0/1]`

### Windows UI
You might be able to use the UI I made using QT. It's my first app so it might have some bugs..
Check in the releases
<img src="winapp.png" width="500"/>

### Work in progress....

<img src="preview.jpg" width="350"/>

<img src="preview2.jpg" width="350"/>
