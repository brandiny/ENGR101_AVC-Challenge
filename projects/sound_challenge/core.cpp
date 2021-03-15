#include <iostream>             // input-output library 
#include <math.h>               // library for sin function 
#include "wav.hpp"              // arthurs holy sound library

using namespace std; 
 
int main()
{ 
    WavSound sound;                             // sound helper object
    double duration    = 5;                     // in seconds
    double sample_rate = 41400;                 // samples per second, Hz
    double dt          = 1.0 / sample_rate;     // time between samples (seconds)
    uint n_samples     = duration / dt;         // total number of samples

    // waveform array memory reservation
    int* waveform = new int[n_samples];

    // sine wave variables
    int amplitude    = 30000;               
    double frequency = 440;                 
    
    // add samples of the sine wave to the waveform array
    for (int i = 0; i < n_samples; ++i)
        waveform[i] = amplitude*sin(2*M_PI*frequency*(i*dt));

    // generates .wav sound file from waveform array
    sound.MakeWavFromInt("core.wav",sample_rate, waveform, n_samples); 

    // deallocate memory and exit from program.
    delete(waveform);
    return 0; 
} 