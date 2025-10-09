# ILI9341 Tests on the Daisy Seed using the driver written by brbrr in this repo: [ili9341_daisy](https://github.com/brbrr/ili9341_daisy/tree/main)

## Overview copied from brbrr's repo

- In current state, it is capable producing solid 40 FPS.
- Uses DMA for SPI communication.
- Uses DMA2D for faster writing to a buffer.

## Usage

The driver expects specific pin configuration. You can change the pins in the ILI9341SpiTransport::Init() function.

Below is the default configuration:

```
D7 (SPI1_CS)    -> ILI_CS
D8 (SPI1_SCK)   -> ILI_SCK
D10 (SPI1_MOSI) -> ILI_MOSI
D17             -> ILI_DC
D23             -> ILI_RESET
```

```cpp
  spi_config.pin_config.nss  = {DSY_GPIOG, 10}; // D7
  spi_config.pin_config.sclk = {DSY_GPIOG, 11}; // D8
  spi_config.pin_config.mosi = {DSY_GPIOB, 5};  // D10

  auto dc_pin    = seed::D17;
  auto reset_pin = seed::D23;
```

Then, follow `main.cpp` to draw stuff on the screen.
