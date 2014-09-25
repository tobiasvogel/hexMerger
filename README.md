hexMerger
=========

## What this is about

Usually you would upload an Arduino-Sketch you wrote using the Arduino-IDE to the Hardware Arduino Board.

However, sometimes it was just really handy if the Arduino-specific Bootloader and your own Sketch could be quickly merged into one file that can be deployed independently - anywhere, anytime! 

This is exactly what hexMerger-Utility does: combine the two files into one compliant Intel HEX file. 

## How can I get this wonderful piece of Software? 

You can download hexMerger on [this page](https://github.com/tobiasvogel/hexMerger/releases) for Windows, Mac, Linux and the source code. 
See the 'releases' above to find the Application file for your corresponding OS or check out the master branch for the latest development snapshot of the source code.

## For free? 

Yes. The software is free of charge. But there is no warranty for the program, to the extent permitted by applicable law. Except when otherwise stated in writing the copyright holders and/or other parties provide the program "AS IS" without warranty of any kind, either expressed or implied, including, but not limited to, the implied warranties of merchantability and fitness for a particular purpose. The entire risk as to the quality and performance of the program is with you. Should the program prove defective, you assume the cost of all necessary servicing, repair or correction.

## I see... anything else?

Warning: this program is in its early beta stage and has not been tested thoroughly. Burning a Arduino- (or any other similar AVR-) Chip with a damaged File may render your Arduino/Chip useless and thereby destroy it. Use this tool under greatest precautions and only if you know exactly, what you are doing.

## Can I help?

Definitely. If you don't know how to code, you might want to consider translating the software into your language.

In case you find a bug, defect or if you have any kind of feedback, suggestion, hint, wish; please file it as an "[Issue](https://github.com/tobiasvogel/hexMerger/issues)" (even if a Wish is not literally an Issue...) here on this GitHub page.

If you know how to code, you're welcome to submit patches!

If you're an artist, you could help improving the appearence of the software.

Any help is greatly appreciated!

## Ok, where do I start?

Have a look at the 'ToDo'-List!

## TODO List:

- [ ] Clean up the code. (Yes, it's a mess right now..!) 
- [ ] Polish the user interface (aka make it more maker-stylish)
- [ ] Improve error handling (currently the user is just presented with the error - period)
- [ ] Implement a self-update function
- [ ] Write a "On-Demand" Arduino Sketch-Compiling function
- [ ] Create a direct 'Avrdude'-Interface
