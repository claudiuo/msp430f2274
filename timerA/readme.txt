This project was originally created with CCS v4.2 and it is stored in github:

https://github.com/claudiuo/msp430f2274.git

The repository doesn't store project specific files so it may work in CCS 5 also.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

What I want to do:
- init the MSP430 and let it sleep
- wait for a button press - in the ISR code, start the timer
- init TimerA sourced from VLO (if possible divide /8 so it takes longer to count)
- if nothing happens when TimerA interrupts, place the MCU into sleep again
- if more stuff happens (to start with, button is pressed again), restart the timer
- add P2.3 interrupt on edge low-to-high to use with the 0g accelerometer pin
