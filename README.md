# ENGR101 Autonomous Vehicle Challenge

This project revolves around building a small autonomous vehicle that utilises PID control theory within a team based environment for the Victoria University AVC challenge. The code within this repository received a rare 100% on the project.
This is a copy of the code, the original code is on the VUW Gitlab.

## Build instructions
On a computer with the E101 library code, to compile AND run, execute in the linux terminal:

`./run.sh`

If you wish to organically compile and run the code in main.cpp separately use the commands below:

`g++ main.cpp -o main -le101`

`./main`

## Connecting to the Raspberry Pi remotely
Ensure the Pi has a power connection. First edit the IP address within remoteconnect.sh to meet the IP address of your own Raspberry Pi. This can be found by running `ifconfig` on your Pi, and finding the ipV4 address.
Then, for everytime you want to connect to the Pi, simply run:

`./remoteconnect.sh`

## Syncing changes with Git
Ensure git is installed on your machine before running. To do so, use the command `sudo apt install git`

<table>
  <tr>
    <td><strong>Cloning the folder</strong></td>
    <td>git clone https://github.com/brandiny/ENGR101_AVC-Challenge.git</td>
  </tr>
  
  <tr>
    <td><strong>Commiting current changes</strong></td>
    <td>git commit -am "descriptive message"</td>
  </tr>

  
  <tr>
    <td><strong>Pushing all local commits to server</strong></td>
    <td>git push</td>
  </tr>
  
  <tr>
    <td><strong>Pulling all unsynced changes from server</strong></td>
    <td>git pull</td>
  </tr>
<table>

## Software Architecture
There are two helper classes within the program: the Car class and the Camera class. A class is a “package of code that allows for object oriented organising. A struct is an “abstract data type” that organises data together.

The Car class is responsible for directly interacting with the motor servos of the hardware, and maneuvering the robot. The Camera class is responsible for interacting with the video stream, and using algorithms to determine the direction to move the robot in. The Camera class is dependent on Car. The Pixel struct represents an image pixel with RGB attributes. The HSV struct represents an image pixel with HSV attributes.

  
## Further reading
If you would like further insight into algorithms used, project planning, constraints, hindsight realisations, please read the engineering report attached within the repo.
