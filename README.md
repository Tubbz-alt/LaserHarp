# LaserHarp
The laser harp is a harp that uses lasers in place of strings. It is built to visually resemble a harp, consisting of a rectangular wooden frame and a set of 8 “strings”. Each “string” is a beam of an independent downward-pointing laser diode. A string is “plucked” by breaking the laser beam (most often with a hand, but it could be any object). There are sensors for each laser beam that detect when the beam has been broken. An Arduino board constantly reads out the sensors and if it detects an “pluck” event it will generate a sound. 

![Assembled Laser Harp](https://github.com/mcleung/LaserHarp/blob/master/Images/AssembledHarp.jpg)

![Laser Harp in action](https://github.com/mcleung/LaserHarp/blob/master/Images/HarpInAction.jpg)

The laser harp works best in constant lighting conditions with the cover closed, as the laser beams will provide cleaner visual feedback when reflected off the harpist’s hands. Fog machines can also help visualize the beams but too much fog can trigger the harp sounds (the beam becomes too scattered and the sensors detect that as a broken beam).

It is a fun interactive demo suitable for all ages.

## [Assembly Instructions](https://github.com/mcleung/LaserHarp/blob/master/AssemblyInstructions.md)

## [Constructing your own Harp](https://github.com/mcleung/LaserHarp/blob/master/ConstructionInstructions.md)


# Credits
Thanks to everyone in the Stanford Optical Society for helping me build this, and to previous members who built the first version of the laser harp!

You can learn more about the Stanford Optical Society, and all of the work that we do at https://photons.stanford.edu
