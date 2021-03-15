#include <iostream>             // input-output library 
#include <fstream>
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
    double amplitude        = 30000;
    double frequencyFirst   = 440;
    double frequencySecond  = 880;
    double frequencyCurrent = 440;

    // sine wave smoother
    double df = (frequencySecond - frequencyFirst) / n_samples_tone;

    // waveform array memory reservation
    int* waveform = new int[n_samples];         

    // populate array with alternating tones
    ofstream file;
    file.open("dump.txt");
    for (int i = 0; i < n_samples; ++i) {
        waveform[i] = amplitude*sin(2*M_PI*frequencyCurrent*(i*dt));

        double amplitudeGradient = (frequencySecond - frequencyFirst) / 2.0;
        double frequencyGradient = 1.0 / n_samples_tone;
        

        df = -amplitudeGradient*frequencyGradient*cos(2.0*M_PI*double(i)*double(dt));
        frequencyCurrent += df;
        file << df << endl;
        cout << frequencyCurrent << endl;
    }
    
    // generate sound file from waveform array
    sound.MakeWavFromInt("completion2.wav", sample_rate, waveform, n_samples); 

    // deallocate memory and exit from program.
    delete(waveform);
    return 0; 
} 