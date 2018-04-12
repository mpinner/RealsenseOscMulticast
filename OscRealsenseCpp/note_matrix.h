#pragma once

#include <osc_messages.h>

#include <iostream>
#include <vector>

class note_matrix
{
public:
	osc_messages *osc;
	bool finger_activated[10];

	int finger_threshold[10] = {
		1500,
		1500,
		1500,
		1500,
		1500,
		1500,
		1500,
		1500,
		1500,
		1500
	};

	//note_matrix();
	note_matrix(osc_messages *o) : osc(o) { all_off(); };

	~note_matrix();

	void bendLeft(int finger[5]);
	void bendRight(int finger[5]);
	void bendFinger(int index, int fingerValue);

	void all_off();

};