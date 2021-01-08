# Welcome!
Welcome to the wonderful world of Pablo! This repository includes 3D models, code, and instructions for building and operating Pablo, an (initially-meant-to-paint) drawing robot!

## Parts:
There are a number of components which must be 3D printed in the `parts` folder. **Note:** I purchased from AliExpress so I can't confirm the quality of the the following links.

- [NEMA 17 Motors](https://www.pololu.com/product/2267) (x2)
- [Pololu A4988 Motor Drivers](https://www.ebay.com/itm/191098138506) (x2)
- 100 uF Capacitors (x2)
- [Servo Motor](https://www.adafruit.com/product/169?gclid=CjwKCAiA_9r_BRBZEiwAHZ_v1xuUvUAO2K8Xt4t4YNSHllKryod81foavyVAp1UO9cKwBOy5H2z3FRoCVq8QAvD_BwE) (x1)
- [Arduino Uno](https://store.arduino.cc/usa/arduino-uno-rev3?gclid=CjwKCAiA_9r_BRBZEiwAHZ_v14rwK5Mszd9f0BfZHaQFFZYYCZ_xDWnanpOPhvhxGTY_052WABukKBoCCH0QAvD_BwE) (x1)
- GT2 Timing Belt (3 m)
- GT2 Idler Pulleys (x2)
- GT2 5mm Inner Diameter Pulleys (x2)
- M3 screws/washers/nuts (x19)
- M4 screws/washers/nuts (x24)
- M5 screws/washers/nuts (x2)
- 8 mm Diameter; 330 mm Shaft (x2)
- 8 mm Diameter; 300 mm Shaft (x2)

## Requirements:
**These aren't strict, just the versions I used.**
- python (3.8)
- pyserial (3.5)
- numpy (1.19)
- pillow (8.0)


## Serial Interface:

To interface between the computer and arduino over the serial port, simple commmands are sent using `src/pabloneSerial.py`. Commands all take the form:

	<char> <args> \n

Commands currently supported are shown in the following table:

| Command   | Action        | Arguments     | Description           |
| :-------: | :-----------: | :-------:     | :---------:           |
| `s`       | Start Program | -             | Enable Movement       |
| `e`       | End Program   | -             | Disable Movement      |
| `p`       | Pen State     | `u`/`d`       | Move pen up or down   |
| `m`       | Move          | `x y`         | Move in a straight path to position. |
| `l`       | Line          | `x1 y1 x2 y2` | Draw a line from (`x1`,`y1`) to (`x2`,`y2`) |
| `r`       | Rectangle     | `x1 y1 x2 y2` | Draw a rectangle with opposite corners at (`x1`,`y1`) and (`x2`,`y2`). |
| `c`       | Circle        | `x y r`       | Draw a circle centered at (`x`,`y`) with radius `r`
| `a`       | Arc           | `x y r s e`   | Draw an arc centered at (`x`,`y`) with radius `r` from angle `s` to angle `e` (in degrees). |
| `d`       | Dot           | `x y`           | Draw a dot at (`x`,`y`) |

To ensure commands are parsed correctly, and to maintain availability in the Arduino serial buffer, commmands are sent one-by-one. The arduino completes the task associated with a command, then echoes the command back to the computer, indicating readiness for the next instruction.

Commands can be entered manually or read from a plain text file from top-to-bottom. Two basic compilers are included in `src` to convert images to command files.

------
ijbd@umich.edu
