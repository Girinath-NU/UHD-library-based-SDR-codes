
# USRP C++ Sine Wave generation
- This program generates a 10 kHz complex baseband sine wave and continuously transmits it through a USRP at 100 MHz RF center frequency, using the UHD library and C++.
- Signal Flow
```bash
C++ (IQ samples)
 → UHD TX streamer
 → USRP FPGA
 → DAC
 → RF front-end
 → Antenna
```

# 1️⃣ Header Files (Why each include is needed)
```cpp
#include <uhd/usrp/multi_usrp.hpp>
```
- Gives access to the USRP device object
- Used to:
  - Create the USRP
  - Set frequency, gain, sample rate
- Without this → you cannot talk to the hardware
##
```cpp
#include <uhd/stream.hpp>
```
- Required for streaming IQ samples
- Provides:
  - `uhd::stream_args_t`
  - `uhd::tx_metadata_t`
  - TX/RX streamer classes
 ##
```cpp
#include <complex>
```
- Provides `std::complex<T>`

-  IQ samples are represented as:
```cpp
std::complex<float> = I + jQ
```
##
```cpp
#include <vector>
```
- Used for dynamic buffers
- Stores IQ samples efficiently
##
```cpp
#include <iostream>
```
 - Used for printing information to the terminal
##
```cpp
#include<cmath>
```
- Provides:
    - `sin()`
    - `cos()`
    - `M_PI`
##
```cpp
#include <thread>
#include <chrono>
```
- Used for timing, delays, multitasking
- (Not actively used here, but useful for SDR timing control)
##
# 2️⃣ `main()` Function

```cpp
int main()
{
```
- Entry point of the program
- Execution starts here
##
# 3️⃣ USRP Creation
```cpp
auto usrp = uhd::usrp::multi_usrp::make(uhd::device_addr_t());
```
What this does
- Creates a **shared pointer** to a USRP object
- `device_addr_t()` is an **empty device address**
- UHD interprets this as:
| “Find and connect to the first available USRP”

Internally:
1. UHD scans USB / Ethernet
2. Detects USRP
3. Loads firmware/FPGA
4. Creates control interface



## ⚙️ Build
```cpp
g++ code.cpp -o tx_tone $(pkg-config --cflags --libs uhd) -std=c++17
