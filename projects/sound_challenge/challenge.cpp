#include <iostream> // input-output library
#include <fstream>  // input-output for files
#include <string>   // string library
#include <vector>   // give up on memory allocation
#include <math.h>   // library for sin function
#include "wav.hpp"  // arthurs holy sound library

using namespace std;

int main()
{
    WavSound sound;
    double sample_rate      = 41400;                // hertz (sound quality)
    double dt               = 1.0 / sample_rate;    // time between samples
    
    double bpm              = 120.0;                // tempo of song
    double tone_duration    = 60.0 / bpm;           // length of note (seconds)
    int n_samples_tone      = tone_duration / dt;   // samples per tone

    // sine wave variables
    double amplitude = 10000; 

    // frequencies of notes to be played in order
    vector<double> notes;          

    // read musical file;
    ifstream fileobject("odetojoy.txt");
    if (fileobject.is_open())
    {
        string line;
        while (getline(fileobject, line))
        {
            notes.push_back(stod(line));    // converts string to decimal
        }
    }
    
    // the a(t) values for the ADSR volume envelope
    vector<double> adsr(n_samples_tone, 0);

    // the amplitude modulation constant target, a(t)
    // a(t) should be this by the end of the phase
    double target[4]   = {0.99999, 0.25, 0.25, 0};           // {a, d, s, r}

    // how quickly a(t) exponentially moves towards target
    // higher gain, faster movement
    // *note: sustain gain should be 0;
    double gain[4]     = {0.005, 0.0004, 0.0, 0.00075};      // as above

    // when each phase should end with respect to sample
    // measured in - fraction of the tone
    double duration[4] = {0.02, 0.75, 0.75, 1};               // as above

    // converts duration to sample checkpts
    for (int i = 0; i < 4; ++i)
        duration[i] = (int)((double(adsr.size()) * duration[i]));

    // attack phase
    for (int i = 1; i < duration[0]; i++)
    {
        adsr[i] = target[0] * gain[0] + (1.0 - gain[0]) * adsr[i - 1];
    }

    // decay phase
    for (int i = duration[0]; i < duration[1]; i++)
    {
        adsr[i] = target[1] * gain[1] + (1.0 - gain[1]) * adsr[i - 1];
    }

    // sustain phase
    for (int i = duration[1]; i < duration[2]; i++)
    {
        adsr[i] = target[2] * gain[2] + (1.0 - gain[2]) * adsr[i - 1];
    }

    // release phase
    for (int i = duration[2]; i < duration[3]; i++)
    {
        adsr[i] = target[3] * gain[3] + (1.0 - gain[3]) * adsr[i - 1];
    }

    // rropagate array with waveform
    // vector holds frequences of waveform in individual samples
    vector<int> waveform;       
    for (auto frequency : notes)
    {
        frequency *=2;
        for (int i = 0; i < n_samples_tone; i++)
        {   
            // fundamental frequency
            int a = amplitude * adsr[i] * sin(2 * M_PI * frequency * (i * dt)) ;

            // extra (higher) harmonics
            for (int j = 2; j < 15; j++) 
                a += amplitude * adsr[i] * sin(2 * M_PI * j*frequency * (i * dt)) / pow(2, j-1);

            waveform.push_back(a);
        }
    }

    // copy vector to c style array
    int waveform_c[waveform.size()];
    for (int i = 0; i < waveform.size(); ++i)
        waveform_c[i] = waveform[i];

    // Generates sound file from waveform array
    sound.MakeWavFromInt("challenge.wav", sample_rate, waveform_c, waveform.size());

    return 0;
}