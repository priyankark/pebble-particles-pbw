# Squid Game Watchface

A Pebble watchface inspired by the Netflix series "Squid Game", featuring an animated guard mask with the iconic triangle symbol. This is just meant as a sample for my own reference. The code was mostly AI generated.

## Features

- Animated guard mask with breathing effect
- Player 456 identifier

## Project Structure

```
.
├── src/
│   └── c/
│       └── main.c         # Main watchface implementation
├── resources/
│   └── fonts/            # Font resources
├── package.json          # Project metadata and Pebble configuration
└── wscript              # Build system configuration
```

## Build Instructions

The project uses the Rebble tool for building and running. To build the project:

```bash
rebble build
```

This will generate a `build/squidgames.pbw` file.

## Running in Emulator

To run the watchface in the emulator:

```bash
rebble install --emulator basalt build/squidgames.pbw
```

To interact with the emulator:

```bash
rebble emu-control
```

This will provide a web interface accessible at a local URL (displayed in the terminal) where you can:
- View the watchface
- Test different time formats (12h/24h)
- Simulate different times of day
- Test the watchface on different platforms

## Implementation Details

### Main Components

1. **Guard Design**
   - Animated mask with breathing effect
   - White triangle symbol

2. **Time Display**
   - Uses Pebble's time service
   - Large, centered digits
   - Player 456 identifier

3. **Animation System**
   - Smooth breathing effect
   - 50ms update interval
   - Memory-efficient implementation
   - Battery-friendly design

### Platform Support

The watchface supports Pebble platforms with specific optimizations:
- aplite: Original Pebble and Pebble Steel (B&W)

### Memory Usage
- Efficient resource management
- Clean cleanup in window unload
- Proper timer handling
- Optimized drawing routines

## Development Notes

- Built using the Rebble tool (modern replacement for Pebble SDK)
- Uses platform-specific conditional compilation
- Implements efficient animation system
- Supports B&W displays


## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Inspired by Netflix's "Squid Game"
- Built with Rebble development tools
- Thanks to the Pebble development community
