# Voice_Recognition

The project consisted of two modules, the signal processing module, which ran on a laptop system, and a data collection system, which was a microcontroller system.

The signal processing system simply collected all the data which the microcontroller collected and performed a FFT transform on the same. The code was written in MATLAB.

The data collection unit was based on a PIC18F4520. The ADC was connectedd to a microphone, which ran at 8000 samples per second. This was downsampled to 1300 samples per unit to adjust for the transmission capacity of the UART transmission protocol. The signal was upsampled again at the laptop side. 
