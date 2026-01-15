
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

## 1️⃣ Header Files (Why each include is needed)
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
##
# 4️⃣ Basic Transmitter Settings
```cpp
double tx_rate = 200e3;   // 200 kS/s
double tx_freq = 100e6;   // 100 MHz
double tx_gain = 40;      // dB
```
 ### **Meanings**
- **Sample rate**: 200,000 samples/sec
- **RF center frequency**: 100 MHz
- **TX gain**: 40 dB RF amplification
##
```cpp
usrp->set_tx_rate(tx_rate);
usrp->set_tx_freq(tx_freq);
usrp->set_tx_gain(tx_gain);
```
**What happens internally**
- Sample rate configures:
	      - FPGA interpolation
	      - DAC clocking
- Frequency sets:	
        - RF local oscillator
- Gain controls:
         - RF amplifier stages
##
```cpp
std::cout << "TX Rate: " << usrp->get_tx_rate() << std::endl;
```
- Prints actual values
- Useful because USRP may **round** values to supported hardware rates
##
# 5️⃣ Stream Setup (TX Streamer)
```cpp
uhd::stream_args_t stream_args("fc32");
```
**Meaning of "`fc32`"**
		- `f` → floating point
		- `c` → complex
		- `32` → 32-bit per component
##
```cpp
auto tx_stream = usrp->get_tx_stream(stream_args);
```
### What this creates

-   A **TX streaming interface**
-   Handles:
    -   Buffering
    -   Packetization   
    -   CPU → FPGA data transfer
    
This object is what actually **sends samples** to the USRP.
##
# 6️⃣ TX Metadata
```cpp
uhd::tx_metadata_t md;
```
Metadata tells the USRP **how to interpret the stream**.
```cpp
md.start_of_burst = true;
md.end_of_burst   = false;
```
### Meaning

-   `start_of_burst = true` 
    -   “Transmission starts now”       
-   `end_of_burst = false`    
    -   “More samples will follow”        
This is important for:
-   Timing 
-   Clean start/stop behavior
##
# 7️⃣ Tone Generation Setup
```cpp
const size_t buffer_size = 1024;
std::vector<std::complex<float>> buffer(buffer_size);
```
	- Creates a buffer of 1024 IQ samples
	- This buffer is sent repeatedly
##
```cpp
double tone_freq = 10e3; // 10 kHz
```
-   Baseband tone frequency
-   RF output will be:
```cpp
100 MHz ± 10 kHz
```
##
```cpp
double phase = 0.0;
double phase_inc = 2 * M_PI * tone_freq / tx_rate;
```
### DSP equation used

x[n]=ej(2πfn/Fs)x[n] = e^{j(2\pi f n / F_s)}x[n]=ej(2πfn/Fs​)

This ensures:
-   Correct frequency    
-   Phase continuity (no clicks)
##
# 8️⃣ Transmit Loop
```cpp
for (int n = 0; n < 2000; n++)
```
- Sends 2000 buffers
- Total samples sent:
```cpp
2000 × 1024 = ~2 million samples
```
##
```cpp
buffer[i] = std::complex<float>(cos(phase), sin(phase));
phase += phase_inc;
```
###  What this generates
-   A **complex sinusoid**   
-   I = cos(phase)    
-   Q = sin(phase)    
### This produces:
-   Single-tone carrier    
-   No amplitude modulation    
-   Perfect spectral purity
##
```cpp
tx_stream->send(buffer.data(), buffer.size(), md);
```
**What happens here**
    - Samples are copied into UHD buffers
    - Packetized
    - Sent to FPGA
    - Converted to RF
**After first send:**
```cpp
md.start_of_burst = false;
```
So UHD knows transmission is ongoing.
##
# 9️⃣ End of Burst
```cpp
md.end_of_burst = true;
tx_stream->send("", 0, md);
```
### Meaning

-   No samples sent    
-   Only control signal    
-   Tells USRP:    
    > “Transmission finished cleanly”
    
This avoids
-   Stuck TX   
-   FPGA underrun issues
##
# 🔟 Program End
```cpp
std::cout << "Transmission done" << std::endl;
return 0;
```
-   Prints completion message    
-   `return 0` → successful execution
##

# ⚙️ Build

```cpp
g++ code.cpp -o tx_tone $(pkg-config --cflags --libs uhd) -std=c++17
