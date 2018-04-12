#include "note_matrix.h"


note_matrix::~note_matrix()
{
}



void note_matrix::bendLeft(int fingers[5]) {
	for( int i=0; i<5; i++)
		bendFinger(i, fingers[i]);
}

void note_matrix::bendRight(int fingers[5]) {
	for (int i = 0; i < 5; i++)
		bendFinger(i + 5, fingers[i]);
}

void note_matrix::bendFinger(int index, int fingerValue) {
	int play_value = fingerValue - finger_threshold[index];

	if (finger_activated[index]) {
		//finger is current on

		// if finger below threshold
		if (play_value < 0) {

			//turn it off
			finger_activated[index] = false;
			std::cout << "OFF  " << index << "," << fingerValue << std::endl;
		}

	}
	else {
		//finger is current off

		// if finger above threshold
		if (play_value > 0) {

			// turn it on
			finger_activated[index] = true;
			std::cout << "ON  " << index << "," << fingerValue << std::endl;
		}

	} 
}

void note_matrix::all_off() {
	for (int i = 0; i < 10; i++) {
		i = false;
	}

}



