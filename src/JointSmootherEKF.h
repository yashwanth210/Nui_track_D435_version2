#pragma once

#define Nsta 3
#define Mobs 3
#include "TinyEKF/src/TinyEKF.h"

class JointSmootherEKF : public TinyEKF
{
public:
	//JointSmootherEKF();
	//~JointSmootherEKF();
	
	JointSmootherEKF()
	{
		// We approximate the process noise using a small constant
		this->setQ(0, 0, .0001);
		this->setQ(1, 1, .0001);
		this->setQ(2, 2, .0001);

		// Same for measurement noise
		this->setR(0, 0, .0001);
		this->setR(1, 1, .0001);
		this->setR(2, 2, .0001);
	}

protected:
	void model(double fx[Nsta], double F[Nsta][Nsta], double hx[Mobs], double H[Mobs][Nsta]) override
	{
		// Process model is f(x) = x
		fx[0] = this->x[0];
		fx[1] = this->x[1];
		fx[2] = this->x[2];

		// So process model Jacobian is identity matrix
		F[0][0] = 1;
		F[1][1] = 1;
		F[2][2] = 1;

		// Measurement function simplifies the relationship between state and sensor readings for convenience.
		// A more realistic measurement function would distinguish between state value and measured value; e.g.:
		//   hx[0] = pow(this->x[0], 1.03);
		//   hx[1] = 1.005 * this->x[1];
		//   hx[2] = .9987 * this->x[1] + .001;
		hx[0] = this->x[0];
		hx[1] = this->x[1];
		hx[2] = this->x[1];

		// Jacobian of measurement function
		H[0][0] = 1;
		H[1][1] = 1;
		H[2][1] = 1;
	}
};