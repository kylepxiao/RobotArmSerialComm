#include <windows.h>
#include <conio.h>
#include <iostream>
#include <stdio.h>
#include <tchar.h>
#include "Serial.h"	// Library described above
#include <string>
#include <sstream>

using namespace std;

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 77
#define KEY_RIGHT 75

//declaration for function that responds to key events
void keyResponse(Serial*);

//declarations for variables required to handle key inputs
string port;
HANDLE rhnd = GetStdHandle(STD_INPUT_HANDLE);  // handle to read console
DWORD Events = 0;     // Event count
DWORD EventsRead = 0; // Events read from console
bool Running = true;  // Stores state of whether application is running
const int dataLength = 256;  // size of data packets received and sent
bool restart = false;
//integer tags to check for integrity of data sent
const int tag[] = {
    24, //check tag for claw
    42, //check tag for wrist
    59, //check tag for elbow
    76, //check tag for shoulder
    93  //check tag for base
};

//declarations for variables involved in motor controls
int wrist_angle = 0;
bool claw_closed = false;
int elbow_pwm = 90;
bool elbow_running = false;
int shoulder_pwm = 90;
bool shoulder_running = false;
int base_pwm = 90;
bool base_running = false;

// application reads from the specified serial port and reports the collected data
int _tmain(int argc, _TCHAR* argv[])
{
	printf("Which serial port would you like to connect to?\n\n"); // '4' for COM4 by Default

	cin >> port;

	cout << endl;

	port.insert(0, "\\\\.\\COM");

	Serial* SP = new Serial(port.c_str());    // adjust as needed

	if (SP->IsConnected()){
		printf("Connection Successful!\n\n");
	}

	char incomingData[256] = "";			// don't forget to pre-allocate memory

	int readResult = 0;

	while(SP->IsConnected() && Running)
	{
	    keyResponse(SP);
		readResult = SP->ReadData(incomingData,dataLength);

        //if data has been recieved, respond
		if(readResult != -1){
            std::string data(incomingData);

            printf("RESPONSE: %s\n",incomingData);

            data = "";
		}

		if(restart){
		    cout << "Reconnecting...\n\n";
            delete SP;
            Serial* SP = new Serial(port.c_str());
            if (SP->IsConnected()){
                printf("Connection Successful!\n\n");
            }
            restart = false;
		}
	}
	return 0;
}

//responds to key responses and sends packets to the Serial Port
void keyResponse(Serial *s){
    char message [256];
    // gets the systems current "event" count
    GetNumberOfConsoleInputEvents(rhnd, &Events);

    if(Events != 0){ // if something happened we will handle the events we want

        // create event buffer the size of how many Events
        INPUT_RECORD eventBuffer[Events];

        // fills the event buffer with the events and saves count in EventsRead
        ReadConsoleInput(rhnd, eventBuffer, Events, &EventsRead);

        // loop through the event buffer using the saved count
        for(DWORD i = 0; i < EventsRead; ++i){

            // check if event[i] is a key event && if so is a press not a release
            if(eventBuffer[i].EventType == KEY_EVENT){
                //Key Pressed Events
                if(eventBuffer[i].Event.KeyEvent.bKeyDown){
                    // check and account for key press
                    switch(eventBuffer[i].Event. KeyEvent.wVirtualKeyCode){

                        case VK_SPACE: case 'E': case 'A': case 'D': break;

                        case 'W':
                            if(!elbow_running){
                                elbow_pwm = 145;
                                sprintf(message, "%d%d", elbow_pwm, tag[2]);
                                cout << "ELBOW OUTPUT: " << message << endl;
                                s->WriteData(message, dataLength);
                                elbow_running = true;
                            }
                            break;

                        case 'S':
                            if(!elbow_running){
                                elbow_pwm = 55;
                                sprintf(message, "%d%d", elbow_pwm, tag[2]);
                                cout << "ELBOW OUTPUT: " << message << endl;
                                s->WriteData(message, dataLength);
                                elbow_running = true;
                            }
                            break;

                        case VK_UP:
                            if(!shoulder_running){
                                shoulder_pwm = 145;
                                sprintf(message, "%d%d", shoulder_pwm, tag[3]);
                                cout << "SHOULDER OUTPUT: " << message << endl;
                                s->WriteData(message, dataLength);
                                shoulder_running = true;
                            }
                            break;

                        case VK_DOWN:
                            if(!shoulder_running){
                                shoulder_pwm = 60;
                                sprintf(message, "%d%d", shoulder_pwm, tag[3]);
                                cout << "SHOULDER OUTPUT: " << message << endl;
                                s->WriteData(message, dataLength);
                                shoulder_running = true;
                            }
                            break;

                        case VK_RIGHT:
                            if(!base_running){
                                base_pwm = 70;
                                sprintf(message, "%d%d", base_pwm, tag[4]);
                                cout << "BASE OUTPUT: " << message << endl;
                                s->WriteData(message, dataLength);
                                base_running = true;
                            }
                            break;

                        case VK_LEFT:
                            if(!base_running){
                                base_pwm = 120;
                                sprintf(message, "%d%d", base_pwm, tag[4]);
                                cout << "BASE OUTPUT: " << message << endl;
                                s->WriteData(message, dataLength);
                                base_running = true;
                            }
                            break;

                        case VK_BACK:
                            restart = true;
                            break;

                        case VK_ESCAPE: // if escape key was pressed end program loop
                            std::cout<< "PROGRAM TERMINATED\n";
                            Running = false;
                            break;

                        default:        // no handled cases where pressed
                            std::cout<< "Invalid Key Argument.\n\n";
                            break;
                    }
                //Key Released Events
                }else{
                    switch(eventBuffer[i].Event.KeyEvent.wVirtualKeyCode){
                        case 'E':
                            claw_closed = !claw_closed;
                            if(claw_closed){
                                sprintf(message, "%d%d", 180, tag[0]);
                            }else{
                                sprintf(message, "%d%d", 0, tag[0]);
                            }
                            cout << "CLAW OUTPUT: " << message << endl;
                            s->WriteData(message, dataLength);
                            break;

                        case 'A':
                            wrist_angle = 180;
                            sprintf(message, "%d%d", wrist_angle, tag[1]);
                            cout << "WRIST OUTPUT: " << message << endl;
                            s->WriteData(message, dataLength);
                            break;

                        case 'D':
                            wrist_angle = 0;
                            sprintf(message, "%d%d", wrist_angle, tag[1]);
                            cout << "WRIST OUTPUT: " << message << endl;
                            s->WriteData(message, dataLength);
                            break;

                        case 'W':
                            elbow_pwm = 90;
                            sprintf(message, "%d%d", elbow_pwm, tag[2]);
                            cout << "ELBOW OUTPUT: " << message << endl;
                            s->WriteData(message, dataLength);
                            elbow_running = false;
                            break;

                        case 'S':
                            elbow_pwm = 90;
                            sprintf(message, "%d%d", elbow_pwm, tag[2]);
                            cout << "ELBOW OUTPUT: " << message << endl;
                            s->WriteData(message, dataLength);
                            elbow_running = false;
                            break;

                        case VK_UP:
                            shoulder_pwm = 95;
                            sprintf(message, "%d%d", shoulder_pwm, tag[3]);
                            cout << "SHOULDER OUTPUT: " << message << endl;
                            s->WriteData(message, dataLength);
                            shoulder_running = false;
                            break;

                        case VK_DOWN:
                            shoulder_pwm = 95;
                            sprintf(message, "%d%d", shoulder_pwm, tag[3]);
                            cout << "SHOULDER OUTPUT: " << message << endl;
                            s->WriteData(message, dataLength);
                            shoulder_running = false;
                            break;

                        case VK_LEFT:
                            base_pwm = 90;
                            sprintf(message, "%d%d", base_pwm, tag[4]);
                            cout << "BASE OUTPUT: " << message << endl;
                            s->WriteData(message, dataLength);
                            base_running = false;
                            break;

                        case VK_RIGHT:
                            base_pwm = 90;
                            sprintf(message, "%d%d", base_pwm, tag[4]);
                            cout << "BASE OUTPUT: " << message << endl;
                            s->WriteData(message, dataLength);
                            base_running = false;
                            break;

                        case VK_SPACE:
                            shoulder_pwm = 90;
                            sprintf(message, "%d%d", shoulder_pwm, tag[3]);
                            cout << "SHOULDER OUTPUT: " << message << endl;
                            s->WriteData(message, dataLength);
                            shoulder_running = false;

                            base_pwm = 90;
                            sprintf(message, "%d%d", base_pwm, tag[4]);
                            cout << "BASE OUTPUT: " << message << endl;
                            s->WriteData(message, dataLength);
                            base_running = false;

                            elbow_pwm = 90;
                            sprintf(message, "%d%d", elbow_pwm, tag[2]);
                            cout << "ELBOW OUTPUT: " << message << endl;
                            s->WriteData(message, dataLength);
                            elbow_running = false;
                            break;

                        case VK_ESCAPE:
                            break;

                        default:
                            break;
                    }
                    break; // break out of checking events if key is released
                }
            }
        } // end EventsRead loop
    }
}
