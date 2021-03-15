#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <vector>

using namespace std;

class WavSound{
	//
private:
   // wav waveform parameters as defined by the standard
   unsigned int      data_length;
   short	audio_format;
   short	num_channels;
   int	    sample_rate;
   int	    byte_rate;
   int  	block_align;
   int  	bits_per_sample;	
   int      n_samples;
   char*    wave;	    // raw byte data
   int*     wave_int;   // CORRESPONDING array of samples
   vector<int> waveform;
	
public:	
	WavSound(); //constructor
	int ReadWavFile(string,int* &wf, int& ns,int& sr);
	int SaveWavFile(string);
	int SaveAsText(string file_name);
	int MakeWavFromInt(string file_name,int sample_rate, int* wf, int n);
    void AddSample(short new_sample);
    int MakeWavFromVector(string file_name,int sample_rate_arg);
};

// constructor
WavSound::WavSound(){
	waveform.clear();
}

int ExtractIntFrom2Chars(char* p,int addr){
	int extr;
	extr =  (((unsigned char)p[addr]));
	extr =  extr + (((unsigned char)p[addr+1])<<8);
	return extr;
}

int ExtractIntFrom4Chars(char* p,int addr){
	int extr;
	extr =  (((unsigned char)p[addr]));
	extr =  extr + (((unsigned char)p[addr+1])<<8);
	extr =  extr + (((unsigned char)p[addr+2])<<16);
	extr =  extr + (((unsigned char)p[addr+3])<<24);
	return extr;
}
// reads wav file 
int WavSound::ReadWavFile(string file_name,int* &wf, int& ns,int& sr){
	cout<<"Reading wav file "<<file_name<<endl;
	ifstream wavf(file_name,ifstream::binary);
	
	if (wavf.is_open()){
		// get length of file:
        wavf.seekg (0, wavf.end);
        int length = wavf.tellg();
        wavf.seekg (0, wavf.beg);
		cout<<"File length = "<<length<<endl;
		char * buffer = new char [length];
        std::cout << "Reading " << length << " characters... ";
        // read data as a block:
        wavf.read (buffer,length);
        if (wavf)
          std::cout << "all characters read successfully."<<endl;
        else {
          std::cout << "error: only " << wavf.gcount() << " could be read"<<endl;
          return -1;
	    }
		// audio format
		audio_format = ExtractIntFrom2Chars(buffer,20);
		cout<<"audio_format="<<audio_format<<endl;
		// number of channels
		num_channels = ExtractIntFrom2Chars(buffer,22);
		cout<<"num_channels="<<num_channels<<endl;
		// sample rate 
		sample_rate = ExtractIntFrom4Chars(buffer,24);
		cout<<"sample_rate="<<sample_rate<<endl;
		sr = sample_rate;
		// byte rate 
		byte_rate = ExtractIntFrom4Chars(buffer,28);
		cout<<"byte_rate="<<byte_rate<<endl;
		// block_align
		block_align = ExtractIntFrom2Chars(buffer,32);
		cout<<"block_align="<<block_align<<endl;
		// bits per sample
		bits_per_sample = ExtractIntFrom2Chars(buffer,34);
		cout<<"bits per sample="<<bits_per_sample<<endl;
		
		// number of bytes - data_length, little endian
		data_length = ExtractIntFrom4Chars(buffer,40);
		cout<<"data_lenght="<<dec<<data_length<<endl;
		
		int bytes_per_sample = bits_per_sample/8;
		cout<<" bytes per sample="<<bytes_per_sample<<endl;
		wave = new char[data_length];
	    n_samples = data_length/bytes_per_sample;
	    ns = n_samples;
        cout<<"Samples in raw data="<<n_samples<<endl;
		wave_int = new int[n_samples];
		wf = new int[n_samples];
	
		// convert raw char[] data to integers
		unsigned int count = 0;
		unsigned int count_samples = 0;
		// count in bytes
		int a;
		for (count = 0 ; count < data_length; count = count + bytes_per_sample){
			// read 2 bytes from "buffer"
			if (bytes_per_sample == 2){
              a =  (((int)buffer[44+count]));
		      a =  a + (((int)buffer[44+count+1])<<8);
		    }
		    if (bytes_per_sample == 1){
              a =  (((int)buffer[44+count]));
		    }
		    wave_int[count_samples] = a;
		    wf[count_samples] = a;
		    //if (count_samples<100){
  		  	//  cout<<"data_length="<<data_length<<" count="<<count;
  		  	//  cout<<" input byte = "<<hex<<buffer[44+count];
  		  	//  cout<<" a="<<dec<<a;
  		  	//  cout<<" wave_int="<<wave_int[count_samples];
  		  	//  cout<<" wf="<<wf[count_samples]<<endl;
		    //}
		  	
		  	count_samples++;
		  	//int stop;
		  	//cin>>stop;
	    }
	    cout<<count<<" samples read from file"<<endl;
		wavf.close();
		delete buffer;
	} //file_open
	else { return -2;}
	return 0;
}

int WavSound::SaveWavFile(string file_name){
	cout<<"Saving "<<file_name<<" file as a sound"<<endl;
	ofstream wavf(file_name,ofstream::binary);
	if (wavf.is_open()){
 		cout<<"File is open for saving"<<endl;
		return 0;
    }
	else return -1;
}

int WavSound::SaveAsText(string file_name){
	// remove file if it exists
	//if (access(file_name.c_str(),F_OK)) {
		
	 if ( remove(file_name.c_str()) != 0 )
	    cout<<" No file"<<endl;
	 else
	    cout<<" Old file removed"<<endl;
	//}   
	ofstream ofs;
	ofs.open (file_name, ofstream::out | ofstream::app);
	if (ofs.is_open()){
	   double dt = 1000.0/sample_rate; // sample interval
       cout<<"Saving waveform as text"<<endl;
       cout<<"n_samples="<<n_samples<<endl;
       for (int i = 0; i < n_samples; i++){
		   ofs<<dt*((double)i)<<" "<<wave_int[i]<<endl;
  		   if ( i < 20 ){
			cout<<"wav.hpp, saving: i="<<i<<" w[i]="<<wave_int[i]<<endl;
		   }
	    }	  
	   ofs.flush();  
       ofs.close();
    } else return -1;
	return 0;
}

void Set4CharsSEndFromInt(int n, char* a,int addr){
	a[addr] = (n)& 0xFF;
    a[addr+1] = (n>>8)& 0xFF;
    a[addr+2] = (n>>16)& 0xFF;
    a[addr+3] = (n>>24)& 0xFF;
    
}

// takes array of integers and makes wav 

int WavSound::MakeWavFromInt(string file_name,int sample_rate_arg, int* wf, int n){
	// compute wav file parameters
	cout<<"Making wav file"<<endl;
	sample_rate = sample_rate_arg;
	n_samples = n;
	cout<<"sample rate = "<<sample_rate<<endl;
	cout<<"n_samples = "<<n_samples<<endl;
	bits_per_sample = 16;
	int n_chars = n*(bits_per_sample/8); //wav file data chaunk size
	cout<<"n_chars(data) = "<<n_chars<<endl;
	char* raw = new char[n_chars+44];
	// prepare header
    raw[0]='R';
    raw[1]='I';
    raw[2]='F';
    raw[3]='F';
    unsigned int chunk_size = 36+n_chars;
    Set4CharsSEndFromInt(chunk_size, raw , 4);
	raw[8]='W'; raw[9]='A'; raw[10]='V';  raw[11]='E';
    raw[12] = 'f';raw[13] = 'm';raw[14] = 't';raw[15] = ' ';
    unsigned int r = 16;
    Set4CharsSEndFromInt(r, raw , 16);
    
    raw[20] = 1;  raw[21] = 0; // PCM
    num_channels = 1;
    raw[22] = 1; // number of channels
    raw[23] = 0;
    
    Set4CharsSEndFromInt(sample_rate, raw , 24);
    //raw[27] = (sample_rate>>24)& 0xFF;
    //raw[26] = (sample_rate>>16)& 0xFF;
    //raw[25] = (sample_rate>>8)& 0xFF;
    //raw[24] = (sample_rate)& 0xFF;
    
    bits_per_sample = 16;
    byte_rate = sample_rate * num_channels*(bits_per_sample/8);
    cout<<" sample_rate="<<sample_rate<<endl;
    cout<<" num_channels="<<num_channels<<endl;
    cout<<" bits_per_sample="<<bits_per_sample<<endl;
    
    cout<<" byte_rate="<<byte_rate<<endl;
    raw[31] = (byte_rate>>24)& 0xFF;
    raw[30] = (byte_rate>>16)& 0xFF;
    raw[29] = (byte_rate>>8)& 0xFF;
    raw[28] = (byte_rate)& 0xFF;
    
    block_align = num_channels*(bits_per_sample/8);
    cout<<" block_align="<<block_align<<endl;
    raw[33] = (block_align>>8)& 0xFF;
    raw[32] = (block_align)& 0xFF;
    
    raw[35] = (bits_per_sample>>8)& 0xFF;
    raw[34] = (bits_per_sample)& 0xFF;
    // subchunk2
    raw[36] = 'd';
    raw[37] = 'a';
    raw[38] = 't';
    raw[39] = 'a';
    // subchunk2 size
    raw[43] = (n_chars>>24)& 0xFF;
    raw[42] = (n_chars>>16)& 0xFF;
    raw[41] = (n_chars>>8)& 0xFF;
    raw[40] = (n_chars)& 0xFF;
    // save samples data
    int pos= 0;
    for ( int iint = 0 ; iint < n_samples ; iint++){
        raw[44+pos] = (wf[iint])& 0xFF;
        raw[44+pos+1] = (wf[iint]>>8)& 0xFF;
        pos = pos + 2;
    }
    cout<<" Waveform data copied to wav sound"<<endl;
    // check wav_int. If not NULL, delete and make new one
    // copying from input argument
    //if (wave_int != NULL){
	//	delete(wave_int);
	//	wave_int = new int[n_samples];
	//} else {
	//	wave_int = new int[n_samples];
	//}
    //for ( int i = 0 ; i < n_samples ; i++){
	//	wave_int[i] = wf[i];
	//	if ( i < 20 ){
	//		cout<<"wav.hpp, copying: i="<<i<<" w[i]="<<wave_int[i]<<endl;
	//	}

	//}
    
	// save raw data to the wav. file
	ofstream ofs;
	ofs.open (file_name, ofstream::out | ofstream::binary);
	if (ofs.is_open()){
		ofs.write(raw,n_chars+44);
		ofs.close();
	} else return -1;
	return 1;
}

void WavSound::AddSample(short new_sample){
	waveform.push_back(new_sample);
}

int WavSound::MakeWavFromVector(string file_name,int sample_rate_arg){
	// compute wav file parameters
	cout<<"Making wav file from vector"<<endl;
	sample_rate = sample_rate_arg;
	n_samples = waveform.size();
	cout<<"sample rate = "<<sample_rate<<endl;
	cout<<"n_samples = "<<n_samples<<endl;
	bits_per_sample = 16;
	int n_chars = n_samples*(bits_per_sample/8); //wav file data chaunk size
	cout<<"n_chars(data) = "<<n_chars<<endl;
	char* raw = new char[n_chars+44];
	// prepare header
    raw[0]='R';
    raw[1]='I';
    raw[2]='F';
    raw[3]='F';
    unsigned int chunk_size = 36+n_chars;
    Set4CharsSEndFromInt(chunk_size, raw , 4);
	raw[8]='W'; raw[9]='A'; raw[10]='V';  raw[11]='E';
    raw[12] = 'f';raw[13] = 'm';raw[14] = 't';raw[15] = ' ';
    unsigned int r = 16;
    Set4CharsSEndFromInt(r, raw , 16);
    
    raw[20] = 1;  raw[21] = 0; // PCM
    num_channels = 1;
    raw[22] = 1; // number of channels
    raw[23] = 0;
    
    Set4CharsSEndFromInt(sample_rate, raw , 24);
    //raw[27] = (sample_rate>>24)& 0xFF;
    //raw[26] = (sample_rate>>16)& 0xFF;
    //raw[25] = (sample_rate>>8)& 0xFF;
    //raw[24] = (sample_rate)& 0xFF;
    
    bits_per_sample = 16;
    byte_rate = sample_rate * num_channels*(bits_per_sample/8);
    cout<<" sample_rate="<<sample_rate<<endl;
    cout<<" num_channels="<<num_channels<<endl;
    cout<<" bits_per_sample="<<bits_per_sample<<endl;
    
    cout<<" byte_rate="<<byte_rate<<endl;
    raw[31] = (byte_rate>>24)& 0xFF;
    raw[30] = (byte_rate>>16)& 0xFF;
    raw[29] = (byte_rate>>8)& 0xFF;
    raw[28] = (byte_rate)& 0xFF;
    
    block_align = num_channels*(bits_per_sample/8);
    cout<<" block_align="<<block_align<<endl;
    raw[33] = (block_align>>8)& 0xFF;
    raw[32] = (block_align)& 0xFF;
    
    raw[35] = (bits_per_sample>>8)& 0xFF;
    raw[34] = (bits_per_sample)& 0xFF;
    // subchunk2
    raw[36] = 'd';
    raw[37] = 'a';
    raw[38] = 't';
    raw[39] = 'a';
    // subchunk2 size
    raw[43] = (n_chars>>24)& 0xFF;
    raw[42] = (n_chars>>16)& 0xFF;
    raw[41] = (n_chars>>8)& 0xFF;
    raw[40] = (n_chars)& 0xFF;
    // save samples data
    int pos= 0;
    for ( int iint = 0 ; iint < n_samples ; iint++){
        raw[44+pos] = (waveform.at(iint))& 0xFF;
        raw[44+pos+1] = (waveform.at(iint)>>8)& 0xFF;
        pos = pos + 2;
    }
    // check wav_int. If not NULL, delete and make new one
    // copying from input argument
    //if (wave_int != NULL){
	//	delete(wave_int);
	//	wave_int = new int[n_samples];
	//} else {
	//	wave_int = new int[n_samples];
	//}
    //for ( int i = 0 ; i < n_samples ; i++){
	//	wave_int[i] = wf[i];
	//	if ( i < 20 ){
	//		cout<<"wav.hpp, copying: i="<<i<<" w[i]="<<wave_int[i]<<endl;
	//	}

	//}
    
	// save raw data to the wav. file
	ofstream ofs;
	ofs.open (file_name, ofstream::out | ofstream::binary);
	if (ofs.is_open()){
		ofs.write(raw,n_chars+44);
		ofs.close();
	} else return -1;
    cout<<" Waveform data copied to wav sound file "<<file_name<<endl;


	return 0;
}