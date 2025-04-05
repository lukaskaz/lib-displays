# lib-displays
Library for displays utilization
<br><br>
**Example of the output (w/o and w/ debug logs):**<br>
pi@raspberrypi:\~/git/lib-displays/build $ ./examples/sevseg/m74hc595/single/bicolor/sbiprog 0 test<br>
[INFO] Created bicolor single 7segm display [dev/typ/col/size/speedhz]: /dev/spidev0.0/1/0/1/500000<br>
[INFO] Released bicolor single 7segm display<br>
pi@raspberrypi:\~/git/lib-displays/build $ ./examples/sevseg/m74hc595/single/bicolor/sbiprog 1 test<br>
[INFO] Created bicolor single 7segm display [dev/typ/col/size/speedhz]: /dev/spidev0.0/1/0/1/500000<br>
[DBG ] Requested text to display(col:0): 'b'/1<br>
[DBG ] Code sent to display(ok): 00000010 -> 11111101<br>
[DBG ] Requested text to display(col:1): 'b'/1<br>
[DBG ] Code sent to display(ok): 10000010 -> 01111101<br>
[DBG ] Requested text to display(col:0): '5'/1<br>
[DBG ] Code sent to display(ok): 01101101 -> 10010010<br>
[DBG ] Requested text to display(col:1): '5'/1<br>
[DBG ] Code sent to display(ok): 11101101 -> 00010010<br>
[DBG ] Requested text to display(col:0): '8'/1<br>
[DBG ] Code sent to display(ok): 01111111 -> 10000000<br>
[DBG ] Requested text to display(col:1): '8'/1<br>
[DBG ] Code sent to display(ok): 11111111 -> 00000000<br>
[DBG ] Requested text to display(col:0): ' '/1<br>
[DBG ] Code sent to display(ok): 00000000 -> 11111111<br>
[INFO] Released bicolor single 7segm display<br>
<br>
![Ex. of controlling single 7seg bicolor display, whole view](pictures/sevseg/m74hc595/single/single_7seg_bicolor_whole.png "Displaying four characters in two colors, complete spi transfers view")<br><br>
![Ex. of controlling single 7seg bicolor display, first stage view](pictures/sevseg/m74hc595/single/single_7seg_bicolor_begin.png "Displaying four characters in two colors, first stage of spi transfers view")<br><br>
![Ex. of controlling single 7seg bicolor display, middle stage view](pictures/sevseg/m74hc595/single/single_7seg_bicolor_middle.png "Displaying four characters in two colors, middle stage of spi transfers view")<br><br>
![Ex. of controlling single 7seg bicolor display, last stage view](pictures/sevseg/m74hc595/single/single_7seg_bicolor_end.png "Displaying four characters in two colors, last stage of spi transfers view")<br><br>
![Ex. of controlling single 7seg bicolor display, diagnosctics view](pictures/sevseg/m74hc595/single/single_7seg_bicolor_diagnostics.gif "Displaying characters in two color, diagnostics with pcb probes")<br><br>
pi@raspberrypi:\~/git/lib-displays/build $ ./examples/sevseg/m74hc595/multi/onecolor/moneprog 0 "1234"<br>
[INFO] Created onecolor multi 7segm display w/ shifting [dev/typ/mxtimeus/shtimems/speedhz]: /dev/spidev0.0/1/1000/500/4/500000<br>
[INFO] Display multiplexing started<br>
[INFO] Display multiplexing stopped<br>
[INFO] Released onecolor multi 7segm display<br>
pi@raspberrypi:\~/git/lib-displays/build $ ./examples/sevseg/m74hc595/multi/onecolor/moneprog 1 "1234"<br>
[INFO] Created onecolor multi 7segm display w/ shifting [dev/typ/mxtimeus/shtimems/speedhz]: /dev/spidev0.0/1/1000/500/4/500000<br>
[DBG ] Requested text to display: '1234'/4<br>
[DBG ] Code sent to display(ok): 00000000 -> 11111111<br>
[DBG ] Code sent to display(ok): 00000000 -> 11111111<br>
[DBG ] Code sent to display(ok): 00000000 -> 11111111<br>
[DBG ] Code sent to display(ok): 00000000 -> 11111111<br>
[INFO] Display multiplexing started<br>
[DBG ] Code sent to display(ok): 00000110 -> 11111001<br>
[DBG ] Code sent to display(ok): 01011011 -> 10100100<br>
[DBG ] Code sent to display(ok): 01001111 -> 10110000<br>
[DBG ] Code sent to display(ok): 01100110 -> 10011001<br>
[DBG ] Code sent to display(ok): 00000000 -> 11111111<br>
[DBG ] Code sent to display(ok): 00000000 -> 11111111<br>
[DBG ] Code sent to display(ok): 00000000 -> 11111111<br>
[DBG ] Code sent to display(ok): 00000000 -> 11111111<br>
[INFO] Display multiplexing stopped<br>
[INFO] Released onecolor multi 7segm display<br>
<br>
![Ex. of controlling multi 7seg onecolor display, whole view + freq](pictures/sevseg/m74hc595/multi/multi_7seg_onecolor_whole_freq.png "Displaying multiplexed text in onecolor, complete spi transfers view w/ transfer freqency")<br><br>
![Ex. of controlling multi 7seg onecolor display, whole view + mxtime](pictures/sevseg/m74hc595/multi/multi_7seg_onecolor_whole_mxtime.png "Displaying multiplexed text in onecolor, complete spi transfers view w/ multiplexing time")<br><br>
![Ex. of controlling multi 7seg onecolor display, setting third segment](pictures/sevseg/m74hc595/multi/multi_7seg_onecolor_setting_third_segment.png "Displaying multiplexed text in onecolor, setting third segment stage of spi transfers view")<br><br>
![Ex. of controlling multi 7seg onecolor display, clearing whole display](pictures/sevseg/m74hc595/multi/multi_7seg_onecolor_clearing_display.png "Displaying multiplexed text in onecolor, clearing all segments stage of spi transfers view")<br>
