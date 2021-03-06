# Tlahuilli
A photography incident light exposure meter

Tlahuilli is an Aztec word that mean "light". This project goal is to build the hardware and write software
needed to accurately measure incident light falling on a subject and calculate photographic parameters to
have a properly exposed picture.

Hardware used in this project is: an Arduino Pro mini, a TSL2591 light sensor and an OLED display, plus
switches, wires, etc. This include a five switch in one device to navigate the screen and modify mode, step,
increments, ISO, exposure time and also to trigger light measurement.

Software is written in C++ using Arduino IDE, three operational modes are supported: Ambient, Flash and Infra Red.

Ambient light measure light falling constantly on the subject, and given an ISO and exposure time will display
correct lens aperture. If for any reason shown aperture is not appropriate, time exposure and ISO can be modified
to get a better set of values using saved light sample. 

Flash mode allow same settings as ambient light mode, even when it may not make sense like using a faster exposure
time speed than allowed by your camera. Because there is no link between device and flash, when measure light button
is pressed, meter wait for one minute for the
flash to fire. After flash fires, it will display lens aperture. One key difference with ambient light is that after
measuring flash light, it is possible to modify the ISO sensitivity to display a new lens aperture, but it clear
lens aperture field if exposure time is modified. It is designed this way because flash duration is very short compared to
flash exposure times allowed in cameras, using twice or half as much exposure time will not affect final outcome of
photograph.

There is third operational mode used in Infra Red photography, same controls and behavior as in ambient light.

If you like this project, leave a note and if you want to build this device, let me know, hopefully you will enjoy
it same i have. It is free for personal use, but if you want to use any of it in commercial devices, I want to know.

Enjoy life, get out and take great pictures.
