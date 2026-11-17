# chesslib

A chess library that supports Chess960 and is decently fast enough.

## Features

- **Chess960 Support**: Full support for Chess960 (Fischer Random Chess) with specialized castling rules
- **Fast Move Generation**: Optimized move generation using precomputed attack tables and magic bitboards
- **Zobrist Hashing**: Efficient position hashing for transposition tables and repetition detection
- **Perft Testing**: Built-in perft testing framework for performance measurement
- **UCI Compatibility**: Can operate in UCI mode for integration with chess GUIs
- **Extensive API**: Comprehensive chess position representation with all standard chess features

## Building

The library uses CMake and requires C++17. To build:

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

The build produces:
- A static library `libchesslib.a`
- Test executables: `test_normal`, `test_chess960`, `NonImportantTests`, `perft_example`

## Usage Examples

### Basic Position Setup
```cpp
#include "position.h"
#include "printers.h"
#include <iostream>

using namespace chess;

int main() {
    Position pos;
    
    // Set up standard starting position
    pos.setFEN(Position::START_FEN);
    
    // Print the position
    std::cout << pos << std::endl;
    
    // Generate all legal moves for white
    Movelist moves;
    pos.legals(moves);
    
    std::cout << "Legal moves for white: " << moves.size() << std::endl;
    
    return 0;
}
```

### Move Execution
```cpp
Move move = pos.parse_uci("e2e4");
pos.doMove(move);  // Execute the move
std::cout << pos << std::endl;
```

### Perft Testing
Build the existing `perft_example` executable, then run a depth-6 perft with one
thread from the repository root:

```bash
cmake --build build --target perft_example
./build/perft_example --depth 6 --threads 1
```

## License

This project is licensed under the GNU General Public License v3.0. See the [LICENSE](LICENSE) file for details.

## Third-Party Dependencies

- **Stockfish**: The enums, structures, and Move class in `types.h` are derived from Stockfish and are included under the GNU GPLv3 license.
- **chess-library**: References to the chess-library project (MIT license) are for documentation purposes.

## Documentation

The documentation is available in the `doc` directory and can also be generated using Doxygen.

## Contributing

Contributions are welcome! Please feel free to open issues or submit pull requests.

## Contact

For questions or support, please open an issue on the GitHub repository.