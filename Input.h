#pragma once
class Input
{
public:
	bool b_forward;
	bool b_backward;
	bool b_up;
	bool b_down;
	bool b_left;
	bool b_right;

	float f_forward;
	float f_up;
	float f_right;

	float f_rx;
	float f_ry;
	float f_rz;

	float f_sx;
	float f_sy;
	float f_sz;

	float translate_delta;
	float rotate_delta;
	float scale_delta;

	Input();
	~Input();

	void Clear();
};

