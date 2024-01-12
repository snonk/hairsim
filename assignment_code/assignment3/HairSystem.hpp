#ifndef HAIR_SYSTEM_H
#define HAIR_SYSTEM_H

#include "ParticleSystemBase.hpp"
#include "glm/ext.hpp"
#include <set>

// CONSTANTS
#define g 9.8
#define E 3.65 * 10e9
#define VOXES 20
// unit distance in voxel grid used for hair-hair interactions
#define VOXEL_GRID_UNIT 0.1


namespace GLOO {
class HairSystem : public ParticleSystemBase {
 public:

  struct Spring {
    int i;
    int j;
    float rest_len;
    float k;
  };

  HairSystem() {

  };
  HairSystem(float dragk) {
    k = dragk;
    wind_ = glm::vec3(0,0,0);
    s_damping = 0.9;
    s_friction = 0.4;
    s_repulsion = 0.05;
    l0 = 0.3;
  };

  void SetWind(float wind) {
    wind_ = glm::vec3(2,2,wind);
  }

  ParticleState ComputeTimeDerivative(const ParticleState& state,
                                              float time) const override {
    return state;
  }

  std::vector<ParticleState> ComputeNextState(const std::vector<ParticleState>& state,
                                              float time) const {
    std::vector<ParticleState> ret = std::vector<ParticleState>();

    
    // if (masses.size() == 0) {
    //     return state;
    // }

    // Add forces for each node of hair.
    for (size_t strand = 0; strand < state.size(); strand++) {
        glm::vec3 gravity;
        glm::vec3 drag;

        std::vector<glm::vec3> forces;

        ParticleState cur_strand;

        for (size_t i = 0; i < state[strand].positions.size(); i++) {
            float jitter = 0.3;
            gravity = glm::vec3(((float) rand() / (RAND_MAX)) * jitter,
                                ((float) rand() / (RAND_MAX)) * jitter,
                                ((float) rand() / (RAND_MAX)) * jitter);
            gravity += masses[strand][i] * glm::vec3(0,-10.,0);
            drag = -k * state[strand].velocities[i];
            
            forces.push_back(gravity + drag + wind_);
            // forces.push_back(gravity + wind_);
            // forces.push_back(state[strand].positions[i]);
        }

        ParticleState tmp;
        std::vector<glm::vec3> d;
        for (size_t i = 0; i < state[strand].positions.size(); i++) {
            cur_strand.positions.push_back(state[strand].positions[i] + state[strand].velocities[i] * time + forces[i] * time * time);
            // solve constraints if not the first node
            if (i > 0) {
                glm::vec3 pos_vec = l0 * glm::normalize(cur_strand.positions[i] - cur_strand.positions[i - 1]);
                d.push_back(cur_strand.positions[i - 1] + pos_vec - cur_strand.positions[i]);
                cur_strand.positions[i] = cur_strand.positions[i - 1]
                                + pos_vec;
            }
            else {
                d.push_back(glm::vec3(0));
            }
        }
        for (size_t i = 0; i < state[strand].velocities.size(); i++) {
            if (i < state[strand].velocities.size() - 1) {
                cur_strand.velocities.push_back((cur_strand.positions[i] - state[strand].positions[i]) / time
                                    + s_damping * (-d[i+1])/time);
            }
            else {
                cur_strand.velocities.push_back((cur_strand.positions[i] - state[strand].positions[i]) / time);
            }
        }
        

        // if fixed, set position to original position and zero out velocity
        for (size_t i = 0; i < state[strand].positions.size(); i++) {
            if (fixed_.find(i) != fixed_.end()) {
                cur_strand.positions[i] = state[strand].positions[i];
                cur_strand.velocities[i] = glm::vec3(0);
            }
        }

        ret.push_back(cur_strand);

    }


    //////////////////////////////// VOXEL APPROACH ///////////////////////////////////

    float voxel_pos[VOXES*2][VOXES*2][VOXES*2] = {{{0}}};
    glm::vec3 voxel_vel[VOXES*2][VOXES*2][VOXES*2] = {{{glm::vec3(0)}}};

    for (size_t strand = 0; strand < state.size(); strand++) {

        for (size_t i = 0; i < state[strand].positions.size(); i++) {
            std::vector<std::vector<float>> cells;
            std::vector<float> max_cell;
            std::vector<float> min_cell;
            for (int j = 0; j < 3; j++) {
                min_cell.push_back(std::floor(ret[strand].positions[i][j] / VOXEL_GRID_UNIT) * VOXEL_GRID_UNIT);
                max_cell.push_back(std::ceil(ret[strand].positions[i][j] / VOXEL_GRID_UNIT) * VOXEL_GRID_UNIT);
            }
            cells.push_back(min_cell);
            cells.push_back(max_cell);

            //std::cout << "ret[strand].positions: " << glm::to_string(ret[strand].positions[i]) << std::endl;

            //std::cout << "min: " << min_cell[0] << " " << min_cell[1] << " " << min_cell[2] << std::endl;
            //std::cout << "max: " << max_cell[0] << " " << max_cell[1] << " " << max_cell[2] << std::endl;

            
            for (uint cell = 0; cell < 8; cell++) {
                int x = cell & 1;
                int y = (cell >> 1) & 1;
                int z = (cell >> 2) & 1;
                glm::vec3 cell_coords = glm::vec3(cells[x][0], cells[y][1], cells[z][2]);
                glm::vec3 interp_pos = glm::abs(ret[strand].positions[i] - glm::vec3(cell_coords));


                cell_coords /= VOXEL_GRID_UNIT;
                cell_coords += VOXES;


                voxel_pos[(int)(cell_coords.x + 0.5)]
                        [(int)(cell_coords.y + 0.5)]
                        [(int)(cell_coords.z + 0.5)] += glm::length(interp_pos) / VOXEL_GRID_UNIT;
                // voxel_pos[(int)(cell_coords.x + 0.5)]
                //         [(int)(cell_coords.y + 0.5)]
                //         [(int)(cell_coords.z + 0.5)] += interp_pos.x * interp_pos.y * interp_pos.z;
                // //std::cout << "interp prod: " << interp_pos.x * interp_pos.y * interp_pos.z << std::endl;
                //std::cout << "voxel pos: " << voxel_pos[(int)(cell_coords.x + 0.5)]
                        // [(int)(cell_coords.y + 0.5)]
                        // [(int)(cell_coords.z + 0.5)] << std::endl;
                // //std::cout << "ret[strand].velocities[i]: " << glm::to_string(ret[strand].velocities[i]) << std::endl;
                

                glm::vec3 interp_vel = ret[strand].velocities[i] * (glm::length(interp_pos)/ (float)VOXEL_GRID_UNIT) ; 
                // glm::vec3 interp_vel = ret[strand].velocities[i] * (interp_pos.x * interp_pos.y * interp_pos.z); 
                // //std::cout << "cell coords" << (int)(cell_coords.x + 0.5) << "\t" << (int)(cell_coords.y + 0.5) << "\t" << (int)(cell_coords.z + 0.5) << std::endl;
                // //std::cout << "interp_vel: " << glm::to_string(interp_vel) << std::endl;
                voxel_vel[(int)(cell_coords.x + 0.5)]
                        [(int)(cell_coords.y + 0.5)]
                        [(int)(cell_coords.z + 0.5)] += interp_vel;
                //std::cout << "voxel_vel: " << glm::to_string(voxel_vel[(int)(cell_coords.x + 0.5)]
                        // [(int)(cell_coords.y + 0.5)]
                        // [(int)(cell_coords.z + 0.5)]) << std::endl;
            }
        }

    }
    for (int i = 0; i < VOXES * 2; i++) {
        for (int j = 0; j < VOXES * 2; j++) {
            for (int k = 0; k < VOXES * 2; k++) {
                if (voxel_pos[i][j][k] > 0) {
                    voxel_vel[i][j][k] /= voxel_pos[i][j][k]; // make sure not zero1!!
                }
            }
        }   
    }
    for (size_t strand = 0; strand < state.size(); strand++) {

        // using voxel grids, implement friction and repulsion
        for (size_t i = 0; i < state[strand].positions.size(); i++) {
            std::vector<std::vector<float>> cells;
            std::vector<float> max_cell;
            std::vector<float> min_cell;
            for (int j = 0; j < 3; j++) {
                min_cell.push_back(std::floor(ret[strand].positions[i][j] / VOXEL_GRID_UNIT) * VOXEL_GRID_UNIT);
                max_cell.push_back(std::ceil(ret[strand].positions[i][j] / VOXEL_GRID_UNIT) * VOXEL_GRID_UNIT);
            }
            cells.push_back(min_cell);
            cells.push_back(max_cell);

            glm::vec3 v_grid = glm::vec3(0);

            float Fx = 0;
            float Fy = 0;
            float Fz = 0;

            glm::vec3 grad = glm::vec3(0);

            for (uint cell = 0; cell < 8; cell++) {
                int x = cell & 1;
                int y = (cell >> 1) & 1;
                int z = (cell >> 2) & 1;
                glm::vec3 cell_coords = glm::vec3(cells[x][0], cells[y][1], cells[z][2]);
                glm::vec3 interp_pos = glm::abs(ret[strand].positions[i] - glm::vec3(cell_coords));

                cell_coords /= VOXEL_GRID_UNIT;
                cell_coords += VOXES;

                x = (int)(cell_coords.x + 0.5);
                y = (int)(cell_coords.y + 0.5);
                z = (int)(cell_coords.z + 0.5);

                float ax = VOXEL_GRID_UNIT - interp_pos.x;
                float ay = VOXEL_GRID_UNIT - interp_pos.y;
                float az = VOXEL_GRID_UNIT - interp_pos.z;

                Fx = ay * az * (voxel_pos[x][y][z] - voxel_pos[x + 1][y][z])
                   + (VOXEL_GRID_UNIT - ay) * az * (voxel_pos[x][y + 1][z] - voxel_pos[x + 1][y + 1][z])
                   + ay * (VOXEL_GRID_UNIT - az) * (voxel_pos[x][y][z + 1] - voxel_pos[x + 1][y][z + 1])
                   + (VOXEL_GRID_UNIT - ay) * (VOXEL_GRID_UNIT - az) * (voxel_pos[x][y + 1][z + 1] - voxel_pos[x + 1][y + 1][z + 1]);
                Fy = ax * az * (voxel_pos[x][y][z] - voxel_pos[x][y + 1][z])
                   + (VOXEL_GRID_UNIT - ax) * az * (voxel_pos[x + 1][y][z] - voxel_pos[x + 1][y + 1][z])
                   + ax * (VOXEL_GRID_UNIT - az) * (voxel_pos[x][y][z + 1] - voxel_pos[x][y + 1][z + 1])
                   + (VOXEL_GRID_UNIT - ax) * (VOXEL_GRID_UNIT - az) * (voxel_pos[x + 1][y][z + 1] - voxel_pos[x + 1][y + 1][z + 1]);
                Fz = ay * ax * (voxel_pos[x][y][z] - voxel_pos[x][y][z + 1])
                   + (VOXEL_GRID_UNIT - ay) * ax * (voxel_pos[x][y + 1][z] - voxel_pos[x][y + 1][z + 1])
                   + ay * (VOXEL_GRID_UNIT - ax) * (voxel_pos[x + 1][y][z] - voxel_pos[x + 1][y][z + 1])
                   + (VOXEL_GRID_UNIT - ay) * (VOXEL_GRID_UNIT - ax) * (voxel_pos[x + 1][y + 1][z] - voxel_pos[x + 1][y + 1][z + 1]);

                // glm::vec3 interp_pos = glm::vec3(VOXEL_GRID_UNIT) - glm::abs(state.positions[i] - glm::vec3(cell_coords)); 
                // //std::cout << (int)(cell_coords.x + 0.5) << "\t" << (int)(cell_coords.y + 0.5) << "\t" << (int)(cell_coords.z + 0.5) << std::endl;
                // v_grid += voxel_vel[0][0][0];
                v_grid += voxel_vel[x][y][z] / (float) 8; 
                grad += glm::vec3(Fx, Fy, Fz);
                //std::cout << "voxel_vel: " << glm::to_string(voxel_vel[x][y][z]) << std::endl;
                //std::cout << "v_grid: " << glm::to_string(v_grid) << std::endl;
            }



            if (glm::length(ret[strand].velocities[i]) < 100 && glm::length(v_grid) < 100 && glm::length(grad) < 10) {
                ret[strand].velocities[i] = ret[strand].velocities[i] * (1 - s_friction) + v_grid * s_friction;
                ret[strand].velocities[i] = ret[strand].velocities[i] + s_repulsion * glm::normalize(grad);
            }


        }

    }
    return ret;


    
    };

    void SetFixed(int i) {
        fixed_.insert(i);
    };

    void AddStrand() {
        ParticleState tmp = ParticleState();
        std::vector<float> mass_tmp = std::vector<float>();

        particles_.push_back(tmp);
        masses.push_back(mass_tmp);
    }

    void AddParticle(int strand, float mass, glm::vec3 position) {
        particles_[strand].positions.push_back(position);
        masses[strand].push_back(mass);
    };

    // void AddSpring(int i, int j, float stiffness) {
    //     springs_.push_back(Spring{
    //         i,
    //         j,
    //         glm::length(particles_.positions[i] - particles_.positions[j]),
    //         stiffness,
    //     });
    // };

    private:
    std::set<int> fixed_;
    // std::vector<Spring> springs_;

    // running list of current particle states for each hair strand
    std::vector<ParticleState> particles_ = std::vector<ParticleState>();

    // mass values for each node
    std::vector<std::vector<float>> masses;

    // drag constant
    float k;

    // wind
    glm::vec3 wind_;

    // distance between nodes of hair cantilever beam
    float d_;

    // number of nodes in a hair strand (k + 1)
    // int num_nodes;

    // damping coefficient
    float s_damping;
    // friction coefficient
    float s_friction;
    // friction coefficient
    float s_repulsion;

    // enforced distance between every node and its predecessor
    float l0;

    // voxel grid 
    // std::vector<std::vector<std::vector<glm::vec3>>> voxel_vel;
    // std::vector<std::vector<std::vector<glm::vec3>>> voxel_vel;

    // float voxel_pos[VOXES*2][VOXES*2][VOXES*2] = {{{0}}};
    // float voxel_vel[VOXES*2][VOXES*2][VOXES*2] = {{{0}}};


};
}  // namespace GLOO

#endif
