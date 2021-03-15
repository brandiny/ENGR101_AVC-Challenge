#include <iostream>             // input-output library 
#include <math.h>               // library for sin function 
#include "wav.hpp"              // arthurs holy sound library

using namespace std; 
 
int main()
{ 
    WavSound sound;                                 // sound helper
    double duration      = 9;                       // full duration of file
    double toneDuration  = 0.5;                     // sound of one (wee) or (woo)
    
    double sample_rate   = 41400;                   // hertz
    double dt            = 1.0 / sample_rate;       // time in seconds between samples

    int n_samples        = duration / dt;           // total number of samples
    int n_samples_tone   = toneDuration / dt;       // samples in a single tone

    // sine wave variables
    double amplitude       = 30000;
    double frequencyFirst  = 440;
    double frequencySecond = 880;

    // waveform array memory reservation
    int* waveform = new int[n_samples];         

    // populate array with alternating tones
    for (int i = 0; i < n_samples; ++i)
        waveform[i] = (int)((i / n_samples_tone) % 2) ? 
            amplitude*sin(2*M_PI*frequencyFirst*(i*dt)): 
            amplitude*sin(2*M_PI*frequencySecond*(i*dt));
    
    // generate sound file from waveform array
    sound.MakeWavFromInt("completion.wav", sample_rate, waveform, n_samples); 

    // deallocate memory and exit from program.
    delete(waveform);
    return 0; 
} 