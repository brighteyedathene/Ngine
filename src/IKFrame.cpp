#include "IKFrame.h"



IKFrame::IKFrame()
{
	numJoints = 2;

	//initialize vectors with default values
	joints.resize(numJoints);
	points.resize(numJoints + 1);

	joints[0].transform.position = root;
	points[0] = root;
	points[1] = root + joints[0].transform.Forward * joints[0].length;

	for (int i = 1; i < numJoints; i++)
	{
		joints[i].transform.position = 
			joints[i - 1].transform.position +
			joints[i - 1].transform.Forward() * joints[i - 1].length;

		points[i + 1] =
			joints[i].transform.position +
			joints[i].transform.Forward() * joints[i].length;
	}

}


IKFrame::~IKFrame()
{
}


void IKFrame::UpdateIK()
{
	float totalLength = 0;
	for (int i = 0; i < numJoints; i++)
	{
		totalLength += joints[i].length;
	}

	float distance = (goal - root).length;

	if (distance)


}