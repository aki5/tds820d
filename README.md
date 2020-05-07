
# TDS 820 and PC setup

First connect your Tektronix TDS 820 to a PC with a null-modem and a (usb-) serial cable.

Open up the hardcopy menu (shift-hardcopy on the front panel) to
- set Hardcopy Format to EPS Color, and
- set Hardcopy Port to RS-232

<img src="intro-2.svg" style="background-color:white;width:40%" />
<img src="intro-3.svg" style="background-color:white;width:40%" />

Next, open up the Utility menu (shift-display on the front panel)
- click on bottom left button until I/O is highlighted, then
- click on the button right to it until RS-232 is highlighted.

<img src="intro-4.svg" style="background-color:white;width:40%" />
<img src="intro-5.svg" style="background-color:white;width:40%" />

- in Hardware Setup, set to 19200-N-1 with hardware flagging off, and
- in Software Setup, set Soft Flagging off.

<img src="intro-6.svg" style="background-color:white;width:40%" />
<img src="intro-7.svg" style="background-color:white;width:40%" />

## Running on Windows

A usb-serial adapter on windows typically shows up as com3 (some would write \\.\COM3). The second argument is basename for created files. This will receive
intro2-1.eps and then convert it into intro2-1.pdf and intro2-1.png using ghostscript if available.

    C:\Users\you\src\tds820d>tds820d-win.exe com3 intro2 
    ready
    started receiving hardcopy
    received intro2-1.eps (30740 bytes)

## Running on Linux
    you@pc:~/src/tds820d$ ./tds820d /dev/ttyUSB0 intro2
    started receiving hardcopy
    received intro2-1.eps (30740 bytes)


# Raspberry Pi signals

1. Find the signal
2. adjust the knobs
3. hit hardcopy

## 25 MHz oscillator
<img src="tst-6.svg" style="background-color:white;width:75%" />

## 54MHz oscillator
<img src="tst-12.svg" style="background-color:white;width:75%" />

## PCIe 100 MHz differential pair with two probes
<img src="tst-14.svg" style="background-color:white;width:75%" />
