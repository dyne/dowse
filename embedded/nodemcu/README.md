nodemcu dowse examples
======================

https://github.com/nodemcu/
https://nodemcu.readthedocs.io/


what are these files?
---------------------

* `init.lua` is the init script that the nodemcu runs on every boot.
  the wifi credentials can be configured here.

* `main.lua` is a middleman to other programs

* `mqtt_basic.lua` is a basic dowse/mqtt example, with some string
  operations

* `mqtt_gpio.lua` is an example of using the nodemcu gpio pins



"flashing"
----------

use [luatool](https://github.com/4refr0nt/luatool/) to upload your lua
files to the nodemcu. a script called `upload.sh` in this directory does
the same

```
sudo luatool.py -b 115200 -p /dev/ttyUSB0 -f init.lua -t init.lua
```


rgb table
---------

3v3 is always connected to a rgb led

0 is gpio.LOW
1 is gpio.HIGH

(consider it reverse logic)

		   gpio1     gpio2    gpio3
		+---------+---------+--------+
		|   RED   |  GREEN  |  BLUE  |
		+---------+---------+--------+
		|    0    |    0    |    0   | = white
		+---------+---------+--------+
		|    0    |    0    |    1   | = yellow
		+---------+---------+--------+
		|    0    |    1    |    0   | = purple
		+---------+---------+--------+
		|    0    |    1    |    1   | = red
		+---------+---------+--------+
		|    1    |    0    |    0   | = cyan
		+---------+---------+--------+
		|    1    |    0    |    1   | = green
		+---------+---------+--------+
		|    1    |    1    |    0   | = blue
		+---------+---------+--------+
		|    1    |    1    |    1   | = off
		+---------+---------+--------+
