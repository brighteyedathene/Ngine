#include "Input.h"



Input::Input()
{
	f_forward = 0;
	f_up = 0;
	f_right = 0;

	f_rx = 0;
	f_ry = 0;
	f_rz = 0;

	f_sx = 1;
	f_sy = 1;
	f_sz = 1;

	translate_delta = 0.02;
	rotate_delta = 0.02;
	scale_delta = 0.01;

}


Input::~Input()
{
}

void Input::Clear()
{
	b_forward  = false;
	b_backward = false;
	b_up       = false;
	b_down     = false;
	b_left     = false;
	b_right    = false;

	f_forward = 0;
	f_up = 0;
	f_right = 0;


}