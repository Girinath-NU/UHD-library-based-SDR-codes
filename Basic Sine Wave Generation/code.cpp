#include <uhd/usrp/multi_usrp.hpp>   // This header lets your program talk to the USRP hardware
#include <uhd/stream.hpp>           // This header is used for streaming IQ samples to or from the USRP.
#include <complex>                  // Standard C++ library for complex numbers.
#include <vector>                   // Standard C++ dynamic array container.
#include <iostream>                 // Standard C++ input/output stream library.
#include <cmath>                    // Standard math library.
#include <thread>                   // This header lets your program wait or multitask 
#include <chrono>                   // High-resolution time and duration library.

int main()

{

	/* ---------------- CREATE USRP ---------------- */ 
	/* Create a shared-pointer object that connects to the first available USRP device using UHD, and store it in a variable called usrp */
	
	auto usrp = uhd::usrp::multi_usrp::make(uhd::device_addr_t());
	
	 /* ---------------- BASIC SETTINGS ---------------- */
   	 double tx_rate = 200e3;   // 200 kS/s
   	 double tx_freq = 100e6;   // 100 MHz
   	 double tx_gain = 40;      // dB
	
   	 usrp->set_tx_rate(tx_rate);
   	 usrp->set_tx_freq(tx_freq);
   	 usrp->set_tx_gain(tx_gain);

   	 std::cout << "TX Rate: " << usrp->get_tx_rate() << std::endl;
   	 std::cout << "TX Freq: " << usrp->get_tx_freq() << std::endl;
   	 std::cout << "TX Gain: " << usrp->get_tx_gain() << std::endl;
   	 
   	/* ---------------- STREAM SETUP ---------------- */
   	/* Create a transmit (TX) data stream for the USRP using 32-bit complex floating-point samples, and store the stream handle in tx_stream */
   	
    	uhd::stream_args_t stream_args("fc32");
    	auto tx_stream = usrp->get_tx_stream(stream_args);
 	 
 	/* ---------------- METADATA ---------------- */
 	/* TX metadata is a small control object that tells the USRP when to start and stop transmitting the IQ samples you send. */
 	 
    	uhd::tx_metadata_t md;
    	md.start_of_burst = true;
    	md.end_of_burst   = false;
    	
    	/* ---------------- TONE GENERATION ---------------- */
    	/* Tone generation creates a pure sinusoidal signal by incrementing the phase each sample and computing cos(phase) and sin(phase) to form a complex sine wave. */
    	
   	 const size_t buffer_size = 1024;
   	 std::vector<std::complex<float>> buffer(buffer_size);

   	 double tone_freq = 10e3; // 10 kHz baseband tone
   	 double phase = 0.0;
   	 double phase_inc = 2 * M_PI * tone_freq / tx_rate; // x[n]=sin(2πfn/Fs​+ϕ)
   	 
   	 /* ---------------- TRANSMIT LOOP ---------------- */
   	 
   	 for (int n = 0; n < 2000; n++)  // transmit for a short time
   	 {  
       		 for (size_t i = 0; i < buffer_size; i++) 
       		 {
         		   buffer[i] = std::complex<float>(cos(phase), sin(phase));
          		   phase += phase_inc;
        	 }

        tx_stream->send(buffer.data(), buffer.size(), md);
        md.start_of_burst = false;
    	 }
 	
 	/* ---------------- END BURST ---------------- */
 	
    	md.end_of_burst = true;
   	tx_stream->send("", 0, md);

    	std::cout << "Transmission done" << std::endl;
    	return 0;
	
}
