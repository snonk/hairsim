#ifndef UTIL
#define UTIL

#define PI 3.1415

#include "ParticleSystemBase.hpp"

namespace GLOO {
class Util {
    public:

    Util() {

    };

    static glm::vec2 RandomPtInCirc(float R, float centerX, float centerY) {
        float r = R * sqrt((float) rand() / (RAND_MAX));
        float theta = (float) rand() / (RAND_MAX) * 2 * PI;

        return glm::vec2(centerX + r * cos(theta), centerY + r * sin(theta));
    };


    private:






};
}  // namespace GLOO

#endif