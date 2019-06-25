# Silo

This is a quick and dirty port of the example code from the [Nuklear](https://github.com/vurtun/nuklear) IMGUI library
with a Cairo+Pango backend. The important bit, rendering a Nuklear command list with Cairo, is in silo.cpp in the silo_launch
function. There's an initialization function that must be called first (silo_init, intializes Pango), as well as helpers 
that must be used to allow Nuklear to know the pixel size of text (calls through to Pango).

The demo as a whole is C++, but the code in silo.cpp could probably be tweaked to C99 or greater with minimal effort
(maybe even just file extension change; I haven't tried) as it doesn't use classes.

Also, the demo uses SDL for OS input and window, as well as blitting the final Cairo/Nuklear buffer to the actual window.

## Todo

Implementing the image command is a task for another time. Probably not a difficult one, I should just sleep eventually.

