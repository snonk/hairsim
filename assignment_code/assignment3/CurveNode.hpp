#ifndef CURVE_NODE_H_
#define CURVE_NODE_H_

#include <string>
#include <vector>

#include "gloo/SceneNode.hpp"
#include "gloo/VertexObject.hpp"
#include "gloo/shaders/ShaderProgram.hpp"

namespace GLOO {

enum class SplineBasis { Bezier, BSpline };

struct CurvePoint {
  glm::vec3 P;
  glm::vec3 T;
};

class CurveNode : public SceneNode {
 public:
  CurveNode(SplineBasis basis, std::vector<glm::vec3> cp);
  void Update(double delta_time) override;
  // Update control points of the curve.
  void UpdateControlPoints(GLOO::PositionArray points);

 private:
  void ToggleSplineBasis();
  void ConvertGeometry(GLOO::SplineBasis);
  CurvePoint EvalCurve(float t);
  void InitCurve();
  void PlotCurve();
  void PlotControlPoints();
//   void PlotTangentLine();



  SplineBasis spline_basis_;
  glm::mat4 basis_matrix_;

  std::shared_ptr<VertexObject> curve_polyline_;

  std::shared_ptr<ShaderProgram> polyline_shader_;
  std::vector<glm::vec3> control_points_;

  VertexObject* polyline;



  const int N_SUBDIV_ = 50;
  const glm::mat4 BBezier = glm::mat4(
    1, 0, 0, 0, -3, 3, 0, 0, 3, -6, 3, 0, -1, 3, -3, 1
  );
  const glm::mat4 BBSpline = glm::mat4(
    1/6., 4/6., 1/6., 0/6., -3/6., 0/6., 3/6., 0/6., 3/6., -6./6., 3/6., 0/6., -1/6., 3/6., -3/6., 1/6.
  );
  const glm::mat4 BBezierInv = glm::mat4(
    1, 0,0,0, 1, 1/3.,0,0,1,2/3.,1/3.,0,1,1,1,1
  );
  const glm::mat4 BBSplineInv = glm::mat4(
    1, -1, 2/3., 0, 1, 0, -1/3., 0, 1, 1, 2/3., 0, 1, 2, 11/3., 6
    
  );

};
}  // namespace GLOO

#endif
