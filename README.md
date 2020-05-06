
# tds820d 

Connect your tektronix tds-820 to a Linux box with a null-modem
and (probably) a usb-serial cable. In the utility menu, set hardcopy port to RS-232 and speed to 19200bps. Set output format to EPS in
the hardcopy menu and run tds820d on the computer.

Pressing hardcopy on the scope will result in the screen being
captured as eps,pdf and svg files on the computer.

## Example session

    aki@lapdell:~/src/tds820d$ ./tds820d /dev/ttyUSB0 tst
    started receiving hardcopy
    tst-1.eps exists
    tst-2.eps exists
    received tst-3.eps (17814 bytes)
    started receiving hardcopy
    tst-3.eps exists
    received tst-4.eps (34680 bytes)
    started receiving hardcopy
    tst-4.eps exists


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
