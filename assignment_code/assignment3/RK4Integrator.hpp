#ifndef RK4_INTEGRATOR_H_
#define RK4_INTEGRATOR_H_

#include "IntegratorBase.hpp"

namespace GLOO {
template <class TSystem, class TState>
class RK4Integrator : public IntegratorBase<TSystem, TState> {
  TState Integrate(const TSystem& system,
                   const TState& state,
                   float start_time,
                   float dt) const override {
    TState k1 = system.ComputeTimeDerivative(state, start_time);
    TState k2 = system.ComputeTimeDerivative(state + k1 * (dt / 2), start_time + dt/2);
    TState k3 = system.ComputeTimeDerivative(state + k2 * (dt / 2), start_time + dt/2);
    TState k4 = system.ComputeTimeDerivative(state + k3 * dt, start_time + dt);
    return state + dt/6 * (k1 + 2*k2 + 2*k3 + k4);
  }
};
}  // namespace GLOO

#endif
