#include "IKFrame.h"



IKFrame::IKFrame(int numberOfJoints)
{
	numJoints = numberOfJoints;
	joints.resize(numJoints);
	points.resize(numJoints + 1);
}

void IKFrame::Init()
{
	joints[0].transform.position = transform.position;
	joints[0].transform.scale.z = joints[0].length;
	points[0] = transform.position;
	points[1] = transform.position + joints[0].transform.Forward() * joints[0].length;

	for (int i = 1; i < numJoints; i++)
	{
		joints[i].transform.position =
			joints[i - 1].transform.position +
			joints[i - 1].transform.Forward() * joints[i - 1].length;

		joints[i].transform.scale.z = joints[i].length;

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
	goalReachable = false;

	// First, check if the target is too far
	float totalLength = 0;
	for (int i = 0; i < numJoints; i++)
	{
		totalLength += joints[i].length;
	}
	float distance = glm::length(goal - transform.position);

	if (distance < totalLength)
	{

		// do the ik
		for (int iters = 0; iters < maxIterations; iters++)
		{
			DoFABRIK();
			UpdateJoints();
			//UpdateJointsWithConstraints();


			//glm::vec3 endpoint = joints[numJoints - 1].transform.position + joints[numJoints - 1].transform.Forward() * joints[numJoints - 1].length;
			//std:cout << "goal: " << goal.x << " " << goal.y << " " << goal.z << "   endpoint: " << endpoint.x <<" "<< endpoint.y <<" "<< endpoint.z << std::endl;
			//std:cout << "points: " << points[numJoints].x << " " << points[numJoints].y << " " << points[numJoints].z << "   endpoint: " << endpoint.x << " " << endpoint.y << " " << endpoint.z << std::endl;
			if (glm::distance(points[numJoints], goal) < threshold)
			{
				goalReachable = true;
				//std::cout << "exiting because goal reached" << std::endl;
				break;
			}
		}
	}
	else
	{
		// point straight at the target
		StraightenOut();
		UpdateJoints();
	}

}

void IKFrame::DoFABRIK()
{
	ReachBackwards();
	if (constrain)
	{
		ReachForwardsAndConstrain();
	} 
	else
	{
		ReachForwards();
	}
}

void IKFrame::ReachBackwards()
{
	points[numJoints] = goal;
	for (int i = numJoints - 1; i >= 0; i--)
	{
		// find distance between new point_i+1' and point_i
		float distance = glm::length(points[i + 1] - points[i]);

		// new point_i' is found by interpolating back towards old point_i by joint_i.length
		float alpha = joints[i].length / distance;
		points[i] = (1 - alpha) * points[i + 1] + alpha * points[i];
	}
}


void IKFrame::ReachForwards()
{
	points[0] = transform.position;
	for (int i = 1; i < numJoints; i++)
	{
		// it's same as forward reaching but backwards
		float distance = glm::length(points[i] - points[i - 1]);
		float alpha = joints[i - 1].length / distance;
		points[i] = (1 - alpha) * points[i - 1] + alpha * points[i];
	}
}

void IKFrame::ReachForwardsAndConstrain()
{
	points[0] = transform.position;
	glm::vec3 previousDirection = transform.Forward();

	for (int i = 1; i < numJoints + 1; i++)
	{
		// it's same as forward reaching but backwards
		float distance = glm::length(points[i] - points[i - 1]);
		float alpha = joints[i - 1].length / distance;
		points[i] = (1 - alpha) * points[i - 1] + alpha * points[i];

		// Constrain now...
		glm::vec3 constrainedDirection = Constrain(points[i] - points[i - 1], previousDirection, joints[i-1].dof);
		points[i] = points[i - 1] + constrainedDirection;
		
		previousDirection = constrainedDirection;
	}
}


glm::vec3 IKFrame::Constrain(glm::vec3 desiredDirection, glm::vec3 oldDirection, IKJointConstraint dof)
{
	float scalar = glm::dot(desiredDirection, oldDirection) / glm::length(oldDirection);
	glm::vec3 proj = scalar * glm::normalize(oldDirection);
	glm::vec3 adjust = desiredDirection - proj;
	if (scalar < 0) {
		proj = -proj;
	}


	// Get a matrix representing the rotation of the new (desired) direction
	glm::mat4 frame = glm::lookAt(glm::vec3(0.0), desiredDirection, transform.Up());
	
	// Get the axes 
	//glm::vec3 frameUp = glm::vec3(glm::vec4(0.0, 1.0, 0.0, 0.0) * frame);
	//glm::vec3 frameRight = glm::vec3(glm::vec4(0.0, 0.0, 1.0, 0.0) * frame);
	glm::vec3 frameUp = glm::normalize(glm::vec3(0.0, 1.0, 0.0) * glm::mat3(frame));
	glm::vec3 frameRight = glm::normalize(glm::vec3(0.0, 0.0, 1.0) * glm::mat3(frame));


	// Flip axes if necessary
	if (glm::length(frameUp - desiredDirection) > glm::length(-frameUp - desiredDirection))
	{
		frameUp = -frameUp;
	}
	if (glm::length(frameRight - desiredDirection) > glm::length(-frameRight - desiredDirection))
	{
		frameRight = -frameRight;
	}

	// Get 2d components
	float xaspect = glm::dot(adjust, frameRight);
	float yaspect = glm::dot(adjust, frameUp);

	// Get cross section of cone
	float left  = -(glm::length(proj) * tanf(glm::radians(dof.left)));
	float right = glm::length(proj)   * tanf(glm::radians(dof.right));
	float up    = glm::length(proj)   * tanf(glm::radians(dof.up));
	float down  = -(glm::length(proj) * tanf(glm::radians(dof.down)));

	// Find the quadrant
	float xbound = (xaspect >= 0) ? right : left;
	float ybound = (yaspect >= 0) ? up : down;

	glm::vec3 finalDirection = desiredDirection;
	// Check if the desired direction passes through the allowable ellipse
	float ellipse = pow(xaspect, 2) / pow(xbound, 2) + pow(yaspect, 2) / pow(ybound, 2);
	bool inbounds = ellipse <= 1 && scalar >= 0;


	if (!inbounds)
	{
		// get angle of nearest point in the ellipse
		float angle = atan2f(yaspect, xaspect);
		// find nearest point at the angle
		float x = xbound * cosf(angle);
		float y = ybound * sinf(angle);
		// convert back to 3D
		finalDirection = glm::normalize(proj + frameRight * x + frameUp * y) * glm::length(desiredDirection);
	} 

	return finalDirection;
}



void IKFrame::StraightenOut()
{
	points[0] = transform.position;
	for (int i = 0; i < numJoints; i++)
	{
		float distance = glm::length(goal - points[i]);
		float alpha = joints[i].length / distance;
		points[i + 1] = (1 - alpha) * points[i] + alpha * goal;
	}
}


void IKFrame::UpdateJoints()
{
	// Set joint positions and rotations
	for (int i = 0; i < numJoints; i++)
	{
		joints[i].transform.position = points[i];
		joints[i].transform.SetRotationFromDirection(points[i + 1] - points[i]);
	}
}

void IKFrame::UpdateJointsWithConstraints()
{
	glm::vec3 previousDirection = transform.Forward();

	for (int i = 0; i < numJoints; i++)
	{
		joints[i].transform.position = points[i];
		glm::vec3 direction = points[i + 1] - points[i];

		float angle = glm::dot(glm::normalize(previousDirection), glm::normalize(direction));
		//std::cout << "angle: " << angle << std::endl;
		if (abs(angle) > glm::radians(joints[i].degreesOfFreedom))
		{
			glm::vec3 normal = glm::cross(previousDirection, direction);
			direction = glm::rotate(previousDirection, glm::radians(joints[i].degreesOfFreedom), normal);
		}
		// update joint
		joints[i].transform.SetRotationFromDirection(direction);
		// update next point
		points[i + 1] = joints[i].transform.position + glm::normalize(direction) * joints[i].length;
		
		previousDirection = direction;
	}
}


void IKFrame::Draw(Shader* pShader)
{
	for (int i = 0; i < numJoints; i++)
	{
		pShader->SetMat4("model", joints[i].transform.GetMatrix());

		//glm::mat4 rot = glm::lookAt(points[i], points[i+1], transform.Up());
		//glm::mat4 mod = glm::translate(glm::mat4(1.0), points[i]);
		//mod = mod * rot;
		//pShader->SetMat4("model", mod);

		jointModel->Draw(pShader);
	}

}