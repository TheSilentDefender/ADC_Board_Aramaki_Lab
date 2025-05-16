# ADC Hub for pGRAMS
- Has 48 ADC channels and an optional voltage divider can be used for RTDs.
- The ADCs only support a range of 0-5V. Any input outside the range may damage the ADCs on board.
- The board outputs ADC integer values with a 12-bit resolution(0-4095)

## How to build
- Install platform.io on Visual Studio Code. You can also install it manually by following the documenation on [the website](https://platformio.org).
- Open the folder using VSC and wait for platform.io to initialize
- Open the platform.io side panel and click build. This should automatically build with the settings in the configuation file.

## How to flash
- Connect to the board in DFU mode or enter in "d" through serial (press the button on the top while you plug in the board)
- Assuming you followed the instuctions for building, you can click on build and upload which will automatic flash the board and restart it.
- If not you can download the bin file and flash it using dfu-util or any other DFU upgrade tool. For example, `sudo dfu-util -a 0 -i 0 -s 0x08000000:leave -D firmware.bin`.
