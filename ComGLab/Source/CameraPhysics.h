#ifndef CAMERAPHYSICS_H_
#define CAMERAPHYSICS_H_

namespace Physics
{
    float force_Grav = 10;
    template<typename T>
	T gravitational_pulled(T f_down, T mass, double dt, int gravity = force_Grav)
    {
		f_down = f_down - (static_cast<T>(dt * (gravity * gravity)));
		return f_down;
    }

    template<typename T>
    void speed(T& speed, T force, T mass, double dt)
    {
        if (speed * mass < force)
            speed += (T)(force * dt);
    }

    template<typename T>
    T getAcceleration(T force, T mass)
    {
        return (force / mass);
    }
}

#endif