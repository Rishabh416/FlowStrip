#include <cmath>
#include <iostream>
#include <algorithm>
#include <vector>
#include <stdexcept>

struct Vector2{
    float x = 0.0f;
    float y = 0.0f;
};

struct State{
    float position = 0.0f; // distance along track (meters)
    float velocity = 0.0f; // speed along track (meters/second)
    Vector2 positionVector; // position in 2D space (meters)
};

struct PhysicsInputs{
    float gravity = 9.81f; // acceleration due to gravity (meters/second^2)
    float frictioncoefficient = 0.05f; // friction coefficient 
    float mass = 0.2f; // mass of the object (kilograms)
    };

float height(float x, const std::vector<float>& polynomial){
    float result = 0.0f;
    for(float coefficient : polynomial){
        result = result * x + coefficient;
        }
        return result;
    }

float slopeAt(
    float x, const std::vector<float>& polynomial){
    float result = 0.0f;
    int degree = static_cast<int>(polynomial.size()) - 1;
    
    for(int i = 0; i < degree; ++i){
        result = result * x + polynomial[i] * (degree - i);
        }
        return result;
    }

void update(State& state, const PhysicsInputs& inputs, float dt, const std::vector<float>& polynomial){

float slope = slopeAt(state.positionVector.x, polynomial);
float angle = std::atan(slope);

// calculation for friction
// adjust normal force for curves with varying curvature
     float friction = inputs.frictioncoefficient * (inputs.mass * inputs.gravity * std::cos(angle));

//calculation for acceleration
    float gravityacceleration = -inputs.gravity * std::sin(angle);

    float frictionacceleration = friction / inputs.mass;



//update velocity
    state.velocity += gravityacceleration * dt;

    if(state.velocity > 0.0f){
        state.velocity = std::max(0.0f, state.velocity - frictionacceleration * dt);
    }
    else if(state.velocity < 0.0f){
        state.velocity = std::min(0.0f, state.velocity + frictionacceleration * dt);
    }

    state.velocity += gravityacceleration * dt;

//update position
   float distance = state.velocity * dt;
    state.position += distance;

//update position vector
    state.positionVector.x += distance * std::cos(angle);
    state.positionVector.y = height(state.positionVector.x, polynomial);

}

int main() {
    State state;
    PhysicsInputs inputs;

 std::vector<float> polynomial = {1.0f, 0.0f, 0.0f}; // Represents the curve y = x^2
// initial velocity
    state.velocity = 0.5f;

// starting position based on curve y = x^2
    state.positionVector.x = 0.0f;
    state.positionVector.y = height(state.positionVector.x, polynomial);


    inputs.frictioncoefficient = 0.05f;
    inputs.mass = 0.2f;
    inputs.gravity = 9.81f;
    const float dt = 0.02f; // time step (seconds)
   
    
    for(int i = 0; i < 1000; ++i){
        update(state, inputs, dt, polynomial);
    
        std::cout 
        //     << "Position" << state.position << "m"
        //     << "Velocity" << state.velocity << "m/s"
            << "X Position " << state.positionVector.x << " m"
        //     << "Y Position" << state.positionVector.y << "m"
        //     << "Total Energy" << (0.5*inputs.mass*state.velocity*state.velocity)+(inputs.mass*9.81*state.positionVector.y) << "J"
            << "\n";
    }
    
    return 0;
    
}