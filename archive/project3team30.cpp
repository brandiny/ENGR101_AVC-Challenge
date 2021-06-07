#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include "E101.h"

using namespace std;

int main() {
	init(0); // sets up the hardware
	
	// stop the motors
	set_motors(1, 48);
	set_motors(5, 48);
	hardware_exchange();
	
	// quadrant1
	open_screen_stream();
	while(true) {
		take_picture();
		
		
		vector<int> error_scale; // -160, -159, ..., 0, 1, ... 159 - dotproduct with centerrow 
		for (int i = -160; i < 160; i++) {
			error_scale.push_back(i);
		}
		
		int threshold = 180; 
		vector<int> center_row(320); // center rows - 0 if white, 1 if black
		// for (int row = 119; row < 121; row++) 
		
		for (int col = 0; col < 320; col++) {
			int luma = get_pixel(120, col, 3);
			cout<<luma<<" ";
			luma = luma < threshold ? 0: 1;
			center_row[col] = luma;
		}
		//::cout<<std::endl;
		//int dum;
		//cin>>dum;
		
		double error = inner_product(error_scale.begin(), error_scale.end(), center_row.begin(), 0);
		cout << "Error: " << error << endl;
		
		double kp = 1.0/1200;
		double adjustment = (int)(kp*error);
		cout << adjustment << endl;
		
		set_motors(1, 54+adjustment);
		set_motors(5, 30-adjustment);
		hardware_exchange();
		update_screen();
	}
	
	close_screen_stream();
	return 0;
}
