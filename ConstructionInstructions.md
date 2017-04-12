
# Constructing your own harp
The material list and instructions below are for exactly replicating the laser harp built by Stanford OSA, but note that materials and processes can be changed to your liking. For example, the box is currently made from wood pieces commonly found at any hardware improvement store (e.g., Home Depot), but it could be replaced with any number of other materials and/or thicknesses.
Where applicable, we have included part numbers from specific manufacturers. 

## Materials Needed

### Wooden Frame
- Plywood:
  - 1 piece: 36 x 18 x 0.5 inch (base)
  - 1 piece: 33 x 10 x 0.5 inch (top crossbar)
  - 2 pieces: 35.5 x 8.5 x 0.25 inch (cover)
- Pine wood:
  - 2 pieces: 1 x 4 x 36 inch (long sidewalls)
  - 2 pieces: 1 x 4 x 16.5 inch (short sidewalls)
- Around 20 pieces: 1.5” wood screws to hold wood together
- Plastic-coated steel rail, 1 inch OD (McMaster Carr: [2534T13](https://www.mcmaster.com/#catalog/123/1964/=175omqx))
  - 2 pieces: 30 inches in length (side bars)
  - 1 piece: 32 inches in length (for crossbar)
- 2 pieces: round flange rail end (McMaster Carr: [2534T82](https://www.mcmaster.com/#catalog/123/1964/=175omfq))
- 2 pieces: open tee rail connector (McMaster Carr: [2534T22](https://www.mcmaster.com/#catalog/123/1964/=175omb7))
- 6 pieces: Carriage bolts and nuts (McMaster Carr: [93180A220](https://www.mcmaster.com/#carriage-bolts/=1760sma))
- 2 pieces: Routing clamps (McMaster Carr: [8874T43](https://www.mcmaster.com/#pipe-routing-clamps/=1760tv2))
- Wood glue

Note: decorations are made using a laser cutter and vinyl cutter

### Electronics
- 8 pieces: 5mW 532nm laser diodes (color is not important, but they should operate on 5V power supply. We used [these ones](https://m.dhgate.com/product/50mw-532nm-green-laser-diode-module-line/158930720.html))
- 8 pieces: Routing Clamps or other mounting hardware to attach lasers onto the crossbar. (McMaster Carr: [8874T14](https://www.mcmaster.com/#pipe-routing-clamps/=1760xm5))
- 16 pieces: screws for attaching conduit straps (should not be thicker than the crossbar, we used ⅜)
- Power supply for the lasers. We recommend a 1.5 to 2A supply (each laser uses ~100mA of power; we assume: 8 * 100mA + safety factor (Digikey: [993-1296-ND](https://www.digikey.com/product-detail/en/phihong-usa/PSC15A-050/993-1296-ND/5247140))
- Arduino Nano v3 (Digikey: [1050-1001-ND](https://www.digikey.com/product-detail/en/arduino/A000005/1050-1001-ND/2638989))
- MP3/audio board compatible with the Arduino (Gravitech: [MP3-4NANO](http://www.gravitech.us/mp3pladforar.html))
- Breadboard
- 8 pieces: photo-resistors (SparkFun: [SEN-09088](https://www.sparkfun.com/products/9088))
- 8 pieces: 10kOhm resistors
- USB power supply, including cable (used to power the arduino + audio board)
- Audio cable (3.5mm male to male)
- Guitar amp with a 3.5mm line in port
- Various wires to connect everything
- Electrical tape
- Cardboard
- Push pins


## Constructing the wooden frame

Below is an image from the 3D modeling of the wooden frame. The files can be find in the links at the end of the document. 
![CAD Base](https://github.com/mcleung/LaserHarp/blob/master/Images/CADBase.png)


1. Cut all wooden pieces to the sizes specified in the materials list.
2. Create a 0.25 x 0.25 inch notch in each sidewall so that the top cover piece fits nicely inside.
3. Use wood glue to attached the side walls to the base piece.
Secure each side wall to the base with screws (4 screws for the long sides and 2 screws for the short sides).
4. Using the carriage nuts and bolts attach the rail ends to the base of the box (centered in the box and separated by 30 inches).
5. Attach open tee rail connectors to the 30 inch pieces of rail. 
6. Fix the longer rail (32 inches) to the plywood used for the crossbar using conduit straps. 

For decorations (all files are available in the link at the end of the document):
1. Laser cut the staff pattern onto the sheet wood. If the bed of the laser cutter cannot fit the entire piece then you can split the staff into two parts. This is what we did for our laser harp. 
2. Vinyl cut the patterns for the top of the laser harp and affix to the crossbar. 

Notes:
- All the rail pieces, including the tee on the side bars, should be sized to fit within the box.
- The cover simply lays on top of the box. Alternatively, the top piece could be screwed into position or hinges could be used. 

## Constructing the software
  The latest version of the Arduino code is found on this [Github repository](https://github.com/mcleung/LaserHarp/tree/master/Arduino/LaserHarp). If you have any changes or suggestions, please let us know so we can update it!

## Constructing the electronics
1. Upload Arduino code found on this [Github repository](https://github.com/mcleung/LaserHarp/tree/master/Arduino/LaserHarp).

2. Connect Arduino onto the MIDI shield.

3. Connect midi shield output to speakers.

4. Evenly space and secure all lasers to the crossbar using conduit straps. We found that something between 1.5 to 2.5 inches works well.

5. Solder all the laser power supplies in parralel, and connect to the 5V 2A supply.

6. Affix each photoresistor to a small piece of cardboard.

7. Photoresistors should be connected to the Arduino as active low (pull-up) sensors. They should be wired as follows. 5V supply - 10k resistor(or other pull up resistor) - wire connecting back to Analog input - photoresistor - ground.

8. Turn on the lasers by connecting the power supply, and align detectors on cardboard piece.

9. Using the push pins, attached the cardboard/photoresistor to  larger piece of cardboard. The push pins give the flexibility to move the photoresistors for alignment with the lasers. 
  Beware! Avoid staring into the laser or direct reflections. Remove all jewlery and other shiny accessories.

![Laser aligned on photoresistor](https://github.com/mcleung/LaserHarp/blob/master/Images/LaserOnDetector.jpg)


Notes:
  The chip on the audio board does not just support MP3 but also synthesizing sound based on MIDI commands. We are generating MIDI signals with the Arduino and playing them with the audio board.

