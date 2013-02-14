This project was originally created with CCS v4.2 and it is stored in github:

https://github.com/claudiuo/msp430f2274.git

The repository doesn't store project specific files so it may work in CCS 5 also.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

What I want to do:
- build on the existing code in the timerA project
- added serial communication so I can see values in terminal
- add ADC to use with the accelerometer - at this time, I am just reading the internal temperature sensor

Originally, I use code for serial communication from the app that came with the ez430.
However, this increased the size of the program significantly and added complexities
that I don't need for my small projects.

Looking around for a better way I found on NJC's MSP430 LaunchPad blog a great post
about a tiny version of printf: http://www.msp430launchpad.com/2012/06/using-printf.html

Thousand thanks to NJC and oPossum on 43oh forum for sharing his code: please see the
note in printf.c: http://www.43oh.com/forum/viewtopic.php?f=10&t=1732