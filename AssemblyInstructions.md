# Assembly Instructions

## Materials required

Laser Harp box containing electronics and power supply (See image)

![Laser Harp Box](https://github.com/mcleung/LaserHarp/blob/master/Images/Contents.jpg)

External speakers (or Guitar Amplifier)


## Setup and Operation

Note: Be sure to set up the laser harp under the lighting conditions where it will be played to ensure that the sensors accurately detect a beam break activity. 

1. Open the laser harp box to expose the contents.

2. Inset two metal posts into the post holders, and tighten the screws (hand tight).
![Assembly of posts](https://github.com/mcleung/LaserHarp/blob/master/Images/Assembly1.jpg)

3. Remove the laser crossbar

![Crossbar](https://github.com/mcleung/LaserHarp/blob/master/Images/CrossBar.jpg)

4. Place the cross bar into the posts

![CrossbarPost](https://github.com/mcleung/LaserHarp/blob/master/Images/CrossbarPost.jpg)

5. Connect the laser diodes’ power supply so that the lasers turn on

![Powersupply](https://github.com/mcleung/LaserHarp/blob/master/Images/Powersupply.jpg)

6. Adjust the photo-resistor positions such that the lasers are aligned (i.e., the laser beam overlaps the photo-resistor).

![Laser aligned on photoresistor](https://github.com/mcleung/LaserHarp/blob/master/Images/LaserOnDetector.jpg)

7. Connect the Arduino to the speaker.

8. Turn on the speaker.

9. Connect Arduino to power supply.

10. The Arduino will go through an auto-threshold adjustment code to determine the right threshold to trigger. 
This should take 10-15 seconds, and will play a chord when it is ready.

11. Replace cover pieces onto harp box.

12. Play. If external lighting changes, reset the Arduino.

