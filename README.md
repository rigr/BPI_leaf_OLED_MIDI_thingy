# BPI Leaf OLED MIDI Thingy

ESP32 based MIDI-sender playing scales

## Overview

This project is an ESP32-based MIDI device that plays musical scales. It's designed for the BPI Leaf development board with OLED display functionality.

## Features

- ESP32 microcontroller-based implementation
- MIDI output capabilities
- Scale playing functionality
- OLED display support (for BPI Leaf board)

## Hardware Requirements

- BPI Leaf development board
- ESP32 microcontroller
- OLED display
- Rotary controller (for navigation and parameter control)
- MIDI interface hardware

## Software Requirements

- Arduino IDE or compatible development environment
- ESP32 board support package
- Relevant libraries for OLED and MIDI functionality

## Installation

1. Clone this repository:
   ```bash
   git clone https://github.com/rigr/BPI_leaf_OLED_MIDI_thingy.git
   ```

2. Open the project in your Arduino IDE

3. Install required libraries (see dependencies section)

4. Configure hardware settings as needed

5. Upload the sketch to your ESP32

## Usage

Once uploaded, the device will:
- Initialize the OLED display
- Set up MIDI communication
- Begin playing scales based on the programmed logic

## Configuration

Modify the code to adjust:
- Scale types and patterns
- MIDI channel settings
- Display content
- Timing and tempo

## Dependencies

- ESP32 core libraries
- OLED display library
- MIDI library

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is open source. Please check the license file for details.

## Author

Created by [rigr](https://github.com/rigr)

## Support

For issues and questions, please use the GitHub Issues section of this repository.
