// particle.h
#pragma once

#include "includes.h"

using namespace soundmath;

class Particle
{
public:
	double mass;
	double position;
	double velocity;
	double acceleration;
	double drag;

	void initialize(double m, double x, double v = 0, double a = 0)
	{
		mass = m;
		position = x;
		velocity = v;
		acceleration = a;
	}

	Particle(double d = 0.01)
	// Particle(double d = 10)
	{
		drag = relaxation(d);
	}

	void reset(double x)
	{
		position = x;
		velocity = 0;
		acceleration = 0;
	}

	void tick()
	{
		velocity += acceleration / SR;
		velocity *= drag;
		position += velocity / SR;
	}

	void prepare()
	{
		acceleration = 0;
	}

	void pull(double force)
	{
		acceleration += force / mass;
	}

	double operator()()
	{
		return position;
	}
};

class Spring
{
public:
	Particle* first;
	Particle* second;
	double k;
	double equilibrium;

	void bind(Particle* first, Particle* second)
	{
		this->first = first;
		this->second = second;
	}

	void strength(double k)
	{
		this->k = k;
	}

	void target(double equilibrium)
	{
		this->equilibrium = equilibrium;
	}

	Spring() { }

	void tick()
	{
		double distance = second->position - first->position;
		double displacement = distance - equilibrium;
		first->pull(k * displacement);
		second->pull(-k * displacement);
	}
};

class Gravity
{
public:
	Particle* first;
	Particle* second;
	double G;
	double epsilon;

	void bind(Particle* first, Particle* second)
	{
		this->first = first;
		this->second = second;
	}

	void strength(double G, double epsilon = 0.0001)
	{
		this->G = G;
		this->epsilon = epsilon;
	}

	Gravity() { }

	void tick()
	{
		double distance = second->position - first->position;
		double cubed = abs(distance * distance * distance);
		double force = G * first->mass * second->mass * distance / (epsilon + cubed);
		if (abs(distance) >= 0.5)
			force = 0;
		first->pull(force);
		second->pull(-force);
	}
};
