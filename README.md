# Pablo 

## Requirements:
*These aren't well-defined, so I've just included the versions I used*
- python (3.8)
- pyserial (3.5)
- numpy (1.19)
- pillow (8.0)

---------

## Serial Interface

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


