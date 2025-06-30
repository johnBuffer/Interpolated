# Interpolated

A very simple tool to perform automatic interpolation on assignment.

```cpp
#include "interpolated/interpolated.hpp"

// --- main ---

// No interpolation
float x{0.0f};
x = 1.0f; // Instant value change

// With interpolation
Interpolated<float> x1{0.0f};
x1 = 1.0f; // x1 will go from 0 to 1 in 1s
```

The code for the interpolation is contained in the `src/interpolated` folder.
The rest is just for the graphical demo.

To build the demo (from the repo):
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

When running the demo, press `space` to assign a new value to the circle position.
Using the auto interpolation, the position change is naturally animated.
