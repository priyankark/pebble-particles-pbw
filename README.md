# Pebble Particles

An elegant and mesmerizing watchface that transforms your Pebble into a dynamic particle system where time flows through beautiful animated particles. Watch as smooth, flowing particles dance across your screen, creating an enchanting display while clearly showing the current time.

## Features

- Dynamic particle animation system
- Clear time display
- Battery-efficient animation implementation

## Project Structure

```
.
├── src/
│   └── c/
│       └── main.c         # Main watchface implementation
├── package.json          # Project metadata and Pebble configuration
└── wscript              # Build system configuration
```

## Build Instructions

The project uses the Rebble tool for building and running. To build the project:

```bash
rebble build
```

This will generate a `build/pebble-particles.pbw` file.

## Running in Emulator

To run the watchface in the emulator:

```bash
rebble install --emulator basalt build/pebble-particles.pbw
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

1. **Particle System**
   - Smooth particle animation
   - Configurable particle behavior
   - Memory-efficient particle management

2. **Time Display**
   - Uses Pebble's time service
   - Clear, legible time presentation
   - Integrated with particle flow

3. **Animation System**
   - Fluid particle movement
   - Optimized update intervals
   - Memory-efficient implementation
   - Battery-friendly design

### Platform Support

The watchface supports multiple Pebble platforms:
- aplite: Original Pebble and Pebble Steel
- chalk: Pebble Time Round
- diorite: Pebble 2

### Memory Usage
- Efficient particle management
- Clean cleanup in window unload
- Proper timer handling
- Optimized drawing routines

## Development Notes

- Built using the Rebble tool (modern replacement for Pebble SDK)
- Uses platform-specific optimizations
- Implements efficient particle animation system
- Supports various Pebble displays

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Built with Rebble development tools
- Thanks to the Pebble development community
