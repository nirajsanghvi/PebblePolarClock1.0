#Polar Clock
=============

I've always loved the look of polar clocks and figured I'd take a crack at it on the Pebble.
There's a lot of design options with such a clock, so I decided to make several variations, as follows:

Clocks that show seconds (look cooler, but increase battery use):
* Polar Clock - the standard polar clock, with the seconds arc on the outside, minutes arc in the middle, and hour arc on the inside. This one also displays the time in text in the center.
* Polar Clock no time - same as above but omits the text time in the middle
* Polar Clock Inseconds - An experiment with flipping the seconds arc to the inside so it can be larger (no text time)

Clock that don't show seconds (to save battery, look more minimal/less ditracting):
* Polar Clock Lite - 
* Polar Clock Lite no time - same as above but omits the text time in the middle


By making use of compiler define options, I was able to create the 5 variations above in only 2 projects:
* Polar Clock - source of Polar Clock and Polar Clock no time
* Polar Clock Inseconds - source of Polar Clock Inseconds, Polar Clock Lite, and Polar Clock Lite no time
