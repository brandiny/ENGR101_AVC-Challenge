#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include "E101.h"

// these are macros, i.e. every occurrence of "MOTOR_LEFT" replaced by compiler by 1 .. so on
#define MOTOR_LEFT 1
#define MOTOR_RIGHT 5
#define MOTOR_CAMERA 3
#define MOTOR_STILLPOINT 48
#define MOTOR_LIMIT 12

// PID global constants.
const double K_P = 1.0/500.0;
const double K_D = 1.0/1700.0;
const double K_P2 = 1.0/370.0;

// speed, angle, error current state globals.
double saved_speed;
double saved_angle;
std::vector<double> saved_errors{0.0,0.0};

// q4 globals - true when we have knocked over the cylinders
bool redfound = false;
bool greenfound = false;
bool bluefound = false;

// pixel struct - rgb essentially
struct Pixel {int red; int green; int blue;};
struct HSV {double h; double s; double v;};

// rgb --> hsv forwards declaration
HSV rgb2hsv(Pixel in);



class Car {
public:
    /**
     * Sets power to left and right motor, give it value between -1 and +1 (UNSAFE)
     */
    static void setMotors(double leftPower, double rightPower) { 
        set_motors(MOTOR_LEFT, MOTOR_STILLPOINT + leftPower * MOTOR_LIMIT);
        set_motors(MOTOR_RIGHT, MOTOR_STILLPOINT - rightPower * MOTOR_LIMIT);
        hardware_exchange();
    }

    /**
     * Sets both motors at given speed, between -1 and 1 (SAFE)
     */
    static void setSpeed(float speed) {
	    speed *= -1; 
        if (speed > 1)  speed = 1.0;
        if (speed < -1) speed = -1.0;
        saved_speed = speed;
        Car::setMotors(speed, speed);
    }

    /**
     * Sets the wheels to turn by making one wheel turn slower, moving at last known speed
     * Negative is left, Positive is right
     */
    static void setAngle(float direction) { 
        direction *= -1;
        if (direction > 1) direction = 1;
        if (direction < -1) direction = -1;
        if (direction <= 0) {
            Car::setMotors((direction+1) * saved_speed, saved_speed);
        } else if (direction > 0) {
            Car::setMotors(saved_speed, (1-direction) * saved_speed);
        }
	
    }
    
    /**
     * Sets the camera angle between angle [-1, 1] :::: -1 is down :::: +1 looks straight forwards
     */
    static void setCameraAngle(double angle) {
        set_motors(MOTOR_CAMERA, MOTOR_STILLPOINT + angle * MOTOR_LIMIT);
        hardware_exchange();
    }

    /**
     * Makes camera look down at ground
     */
    static void lookDownCameraAngle() {
        set_motors(MOTOR_CAMERA, MOTOR_STILLPOINT + -1 * MOTOR_LIMIT);
        hardware_exchange();
    }   

    /**
     * Makes the camera look straight forwards
     */
    static void lookUpCameraAngle() {
        set_motors(MOTOR_CAMERA, MOTOR_STILLPOINT * MOTOR_LIMIT);
        hardware_exchange();
    }

    /**
     * 180 degree turn in place
     */
    static void UTurn() {
        int U_TURN_TIME = 900;

        Car::setMotors(-1, 1); 
        sleep1(U_TURN_TIME);
        Car::setMotors(0, 0);
        sleep1(U_TURN_TIME);
	}
};


/**
 * CAMERA class = static methods for navigating the quadrants using the camera
 */
class Camera {
private:
    const static int WIDTH  = 320; 
    const static int HEIGHT = 240; 
    const static int MAX_BLACK_VAL = 50;
    const static int REDCOUNT_TO_SWITCH_QUADRANTS = 3000;

    /**
     * Returns a BINARY 1D vector of given row (1s and 0s)
     */
    static std::vector<double> getRow(int row) {
        take_picture();
        std::vector<double> v;
        for (int col = 0; col < WIDTH; ++col) {
            double lum   = get_pixel(120, col, 3);
            double bit   = lum <= 50 ? 1 : 0;
            v.push_back(bit);
        }
        return v;
    }
    
    /**
     * Returns 1D array of Pixel structs (have red, green, blue)
     */
    static std::vector<Pixel> getRowPixels(int row) {
        take_picture();
        std::vector<Pixel> v;
        for (int col = 0; col < WIDTH; ++col) {
            Pixel p{get_pixel(row, col, 0), get_pixel(row, col, 1), get_pixel(row, col, 2)};
            v.push_back(p);
        }
        return v;
    }

    /**
     * Returns current number of black pixels in whole video
     */
    static int getBlackPixelCount() {
        take_picture();
        int count = 0;
        for (int row = 0; row < HEIGHT; ++row) {
            for (int col = 0; col < WIDTH; ++col) {
                count += get_pixel(row, col, 3) <= MAX_BLACK_VAL ? 1 : 0;
            }
        } 
        return count;
    }

    /**
     * Returns current number of red pixels in whole video
     */
    static int getRedPixelCount() {
        take_picture();
        int count = 0;
        for (int row = 0; row < HEIGHT; ++row) {
            for (int col = 0; col < WIDTH; ++col) {
                count += get_pixel(row, col, 0)>2*get_pixel(row, col, 1) && get_pixel(row, col, 0)>get_pixel(row, col, 2) ? 1:0;
            }
        }
        return count;
    }

    /**
     * Returns error of row, given a binary vector. (calculated with dot product)
     */
    static double getError(std::vector<double> row) {
        double error = 0.0;
        for (int col = 0; col < WIDTH; ++col)
            error += (col-WIDTH/2) * row[col];
        
        saved_errors.push_back(error);
        return error;
    }

    /**
     * Returns de/dt (t = 1 frame)
     */
    static double getDerivative() {
        return saved_errors[saved_errors.size()-1] - saved_errors[saved_errors.size()-2];
    }

    /**
     * Returns true if Pixel is red
     */
    static bool pixelIsRed(Pixel p) {return p.red > p.blue*2 && p.red > p.green*2;}
    
    /**
     * Returns true if Pixel is green
     */
    static bool pixelIsGreen(Pixel p) {return p.green > p.blue*2 && p.green > p.red*2;}
    
    /**
     * Returns true if Pixel is blue
     */
    static bool pixelIsBlue(Pixel p) {
        HSV h = rgb2hsv(p);
        return abs(h.h - 235) < 3;
    }

public:
    /**
     * doQ1() --> open the gate, move the robot through the gate and stop
     */ 
	static void doQ1() {
        sleep1(2000);

		char ip[15] = "130.195.6.196";
        char msg[24] = "Please";
		char password[24];

		
		if(connect_to_server(ip, 1024) == 0) {
			std::cout << "Connected to server @ " << ip << std::endl;
			send_to_server(msg);
			std::cout << "Message sent to server..." << std::endl;
			receive_from_server(password);
			std::cout << "Message received: " << password << std::endl;
			send_to_server(password);
			std::cout << "Password sent!" << std::endl;

            // move forwards so gate doesn't fall on us
            Car::setSpeed(0.3);
            sleep1(1000);
            Car::setSpeed(0);

		} else {
			std::cout << "Connection failed" << std::endl;
		}
	}
	
    /**
     * doQ2() --> follow the line with a PD controller
     */
    static void doQ2() {  
        double black_pixels = Camera::getBlackPixelCount();
        double error = Camera::getError(Camera::getRow(120));
        double deriv = Camera::getDerivative();

        // turn angle based on error
        double angle = K_P*error + K_D*deriv;

        // speed based on turn angle, ka = how sensitive to angle
        double velocity = 0.9;
        
        Car::setSpeed(velocity);
        Car::setAngle(angle);
    }

    /**
     * doQ3() --> navigate the maze
     */
    static void doQ3() {
        take_picture();

        // margin = distance from edge
        // outer  = the error array
        int margin = 20;
        std::vector<int> outer;
	
        // southwest --> northwest 
        for (int y = HEIGHT-20; y >= 10; --y) 
            outer.push_back(get_pixel(y, margin, 3) <= MAX_BLACK_VAL ? 1:0);

        // northwest --> northeast
        for (int x = 2; x <= WIDTH-2; ++x) 
            outer.push_back(get_pixel(margin, x, 3) <= MAX_BLACK_VAL ? 1:0);

        // northeast --> southeast
        for (int y = 10; y <= HEIGHT-20; ++y) 
            outer.push_back(get_pixel(y, WIDTH-margin, 3) <= MAX_BLACK_VAL ? 1:0);


        // calculate error = first contiguous black segment
        int error = 0;
        int middle_val = outer.size() / 2;
        for (int i = 0; i < outer.size()-5; ++i) {
            if (outer[i] && outer[i+1] && outer[i+2] && outer[i+3] && outer[i+4]) {
		        error = i - middle_val;
		        break;
	        }
        }
            
        // velocity
        double velocity = 0.27;
        Car::setSpeed(velocity);

        // angle
        double angle = K_P2*error + K_D*Camera::getDerivative();
	    
        double RIGHT_TURN_THRESH = 0.3;         // angle
        double U_TURN_THRESH     = 150;         // black pixels
        double TURN_SPEED        = 0.55;        // % max speed, max speed = 1
        double TURN_TIME         = 400;         // milliseconds

        // if we don't have enough black pixels --> u turn
	    if (Camera::getBlackPixelCount() < U_TURN_THRESH) {Car::UTurn();} 
        
        // if the turn angle is LARGE enough, make a SHARP turn
        else if (angle > RIGHT_TURN_THRESH) {
            Car::setSpeed(TURN_SPEED); Car::setAngle(1); sleep1(TURN_TIME);
        } else if (angle < -RIGHT_TURN_THRESH) {
            Car::setSpeed(TURN_SPEED); Car::setAngle(-1); sleep1(TURN_TIME);
        } 
        
        // otherwise, do quadrant 2 basic line following
        else {Car::setAngle(angle);}

        // kd saving    
	    saved_errors.push_back(error);
    }

    /**
     * Q4 code
     */
    static void findRedCylinder() {
        // Get RGB pixels for this frame
        std::vector<Pixel> rowPixels = Camera::getRowPixels(120);
        
        // Count up the number of red pixels
        int error = 0;
        int count = 0;
        int middle_val = rowPixels.size()/2;
        for (int i = 0; i < rowPixels.size(); ++i) {
            if (Camera::pixelIsRed(rowPixels[i])) {
                count++;
                error += i - middle_val;
            }
        }
            
        // If too little red, rotate and search
        if (count < 10) {
            std::cout << "Searching for red ..." << std::endl;
            Car::setMotors(-0.3, 0.3); 
        } 

        else {
            // If enough red - ram it, reverse and move on.
            if (count > 310) {redfound = true; Car::setSpeed(0.6); sleep1(500); Car::setSpeed(-0.6); sleep1(1000);}
            
            // Otherwise follow it 
            double angle = K_P*error;
            double velocity = 0.6;
            Car::setSpeed(velocity);
            Car::setAngle(angle);    
        }
    }

    static void findGreenCylinder() {
        // Get RGB pixels for this frame
        std::vector<Pixel> rowPixels = Camera::getRowPixels(120);
        
        // Count up the number of green pixels and get error
        int error = 0;
        int count = 0;
        int middle_val = rowPixels.size()/2;
        for (int i = 0; i < rowPixels.size(); ++i) {
            if (Camera::pixelIsGreen(rowPixels[i])) {
                count++;
                error += i - middle_val;
            }
        }
            
        // If too little green, rotate and search
        if (count < 10) {
            std::cout << "Searching for green ..." << std::endl;
            Car::setMotors(-0.3, 0.3); 
        } 

        else {
            // If enough red - ram it, reverse and move on.
            if (count > 310) {greenfound = true; Car::setSpeed(0.6); sleep1(500); Car::setSpeed(-0.6); sleep1(1000);}
            
            // Otherwise follow it 
            double angle = K_P*error;
            double velocity = 0.6;
            Car::setSpeed(velocity);
            Car::setAngle(angle);    
        }
    }

    static void findBlueCylinder() {
        // Get RGB pixels for this frame
        std::vector<Pixel> rowPixels = Camera::getRowPixels(120);
        
        // Count up the number of green pixels and get error
        int error = 0;
        int count = 0;
        int middle_val = rowPixels.size()/2;
        for (int i = 0; i < rowPixels.size(); ++i) {
            if (Camera::pixelIsBlue(rowPixels[i])) {
                count++;
                error += i - middle_val;
            }
        }
            
        // If too little green, rotate and search
        if (count < 20) {
            std::cout << "Searching for green ..." << std::endl;
            Car::setMotors(-0.3, 0.3); 
        } 

        else {
            // If enough red - ram it, reverse and move on.
            if (count > 310) {bluefound = true; Car::setSpeed(0.6); sleep1(500); Car::setSpeed(-0.6); sleep1(1000);}
            
            // Otherwise follow it 
            double angle = K_P*error;
            double velocity = 0.4;
            Car::setSpeed(velocity);
            Car::setAngle(angle);    
        }
    }

    /**
     * True if red, else false
     */
    static bool quadrantCheck() {
        take_picture();
        return getRedPixelCount() > REDCOUNT_TO_SWITCH_QUADRANTS;
    }
};


/**
 * MAIN LOGIC OF THE AUTOMONOUS VEHICLE
 */

int main() {
    init(0);        // init hardware
    sleep1(1000);   // and wait for it to be ready

    // Quadrant 1
    Car::lookDownCameraAngle();
    Camera::doQ1();

    // Quadrant 2
    while(true) {
        Camera::doQ2();
        if (Camera::quadrantCheck()) {      // when red detected --> exit
            sleep1(1000);
            Car::setSpeed(0.6);
            sleep1(2000);
            break;
        }
    }
    
    // Quadrant 3
    while(true) {
        Camera::doQ3();
        if (Camera::quadrantCheck()) {      // when red detected --> exit
            sleep1(1000);
            Car::setSpeed(0.6);
            sleep1(2000);
            break;
        }
    }

    // Quadrant 4
    Car::lookUpCameraAngle();
    while(true) {
        if (!redfound) {
           Camera::findRedCylinder();
        } else if (!greenfound) {
	        Camera::findGreenCylinder();
        } else if (!bluefound) {
            Camera::findBlueCylinder();
        } else {
		Camera::findRedCylinder();          // warning - program does not end
	    }
    }

    return 0;   
}


// rgb --> hsv algorithm sourced from 
// https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
HSV rgb2hsv(Pixel in) {
    HSV     out;
    double  min, max, delta;

    min = in.red < in.green ? in.red : in.green;
    min = min < in.blue ? min : in.blue;
    
    max = in.red > in.green ? in.red : in.green;
    max = max > in.blue ? max : in.blue;

    out.v = max;
    delta = max - min;
    if (delta < 0.00001) {
        out.s = 0;
        out.h = 0;
        return out;
    } if (max > 0.0) {
        out.s = delta/max;
    } else {
        out.s = 0.0;
        out.h = 0.0;
        return out;
    }

    if (in.red >= max) {
        out.h = (in.green - in.blue) / delta;
    } else if (in.green > max) {
        out.h = 2.0 + (in.blue - in.red) / delta;
    } else {
        out.h = 4.0 + (in.red - in.green) / delta;
    } 

    out.h *= 60.0;

    if (out.h < 0.0) {
        out.h += 360.0;
    } 

    return out;
}





