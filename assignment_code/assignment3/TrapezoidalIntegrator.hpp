#ifndef TRAPEZOIDAL_INTEGRATOR_H_
#define TRAPEZOIDAL_INTEGRATOR_H_

#include "IntegratorBase.hpp"

namespace GLOO {
template <class TSystem, class TState>
class TrapezoidalIntegrator : public IntegratorBase<TSystem, TState> {
  TState Integrate(const TSystem& system,
                   const TState& state,
                   float start_time,
                   float dt) const override {
    // TODO: Here we are returning the state at time t (which is NOT what we
    // want). Please replace the line below by the state at time t + dt using
    // forward Euler integration.
    return state + (
        system.ComputeTimeDerivative(state, start_time) 
        + system.ComputeTimeDerivative(state + system.ComputeTimeDerivative(state, start_time) * dt, start_time + dt)
    ) * (dt/2);
  }
};
}  // namespace GLOO

#endif
