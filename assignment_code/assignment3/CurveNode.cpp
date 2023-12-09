#include "CurveNode.hpp"

#include "gloo/debug/PrimitiveFactory.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/shaders/PhongShader.hpp"
#include "gloo/shaders/SimpleShader.hpp"
#include "gloo/InputManager.hpp"

namespace GLOO {
CurveNode::CurveNode(SplineBasis basis, std::vector<glm::vec3> cp) {
  // TODO: this node should represent a single spline curve.
  // Think carefully about what data defines a curve and how you can
  // render it.

  // Initialize the VertexObjects and shaders used to render the control points,
  // the curve, and the tangent line.
  curve_polyline_ = std::make_shared<VertexObject>();
  polyline_shader_ = std::make_shared<SimpleShader>();

  spline_basis_ = basis;
  if (basis == GLOO::SplineBasis::BSpline)
    basis_matrix_ = BBSpline;
  else  
    basis_matrix_ = BBezier;

  control_points_ = cp;

  InitCurve();
  PlotCurve();
}

void CurveNode::Update(double delta_time) {

}


// void CurveNode::ConvertGeometry(GLOO::SplineBasis to) {
//   // TODO: implement converting the control points between bases.
//   glm::mat4x3 points;
//   for (int i = 0; i < 4; i++) {
//     points[i] = control_points_[i];
//   }
//   if (to == GLOO::SplineBasis::Bezier)
//     points = points * BBezier * BBSplineInv;
//   else
//     points = points * BBSpline * BBezierInv;
//   for (int i = 0; i < 4; i++) {
//     control_points_[i] = points[i];
//   }
  
// }

void CurveNode::UpdateControlPoints(GLOO::PositionArray points){
    for (int i = 0; i < control_points_.size(); i++) {
        control_points_[i] = points[i];
    }
    PlotCurve();
}

CurvePoint CurveNode::EvalCurve(float t) {
  // TODO: implement evaluating the spline curve at parameter value t.
  glm::mat4x3 points;
  glm::vec4 time = glm::vec4(1, t, t*t, t*t*t);
  glm::vec4 dtime = glm::vec4(0, 1, 2*t, 3*t*t);
  for (int i = 0; i < 4; i++) {
    points[i] = control_points_[i];
  }
  // std::cout << points;
  CurvePoint point = CurvePoint{
    points * basis_matrix_ * time,
    points * basis_matrix_ * dtime,
  };
  return point;
}

void CurveNode::InitCurve() {
  // TODO: create all of the  nodes and components necessary for rendering the
  // curve, its control points, and its tangent line. You will want to use the
  // VertexObjects and shaders that are initialized in the class constructor.

//   for (glm::vec3 p : control_points_) {
//     auto point = make_unique<SceneNode>();
//     point->CreateComponent<ShadingComponent>(shader_);
//     point->CreateComponent<RenderingComponent>(sphere_mesh_);
//     point->GetTransform().SetPosition(p);

//     scene_points.push_back(point.get());
    
//     AddChild(std::move(point));
//   }

  CurvePoint p;

  auto line = std::make_shared<VertexObject>();
  auto positions = make_unique<PositionArray>();
  auto indices = make_unique<IndexArray>();

  for (int i = 0; i < N_SUBDIV_; i++) {
    p = EvalCurve((double)i/N_SUBDIV_);
    indices->push_back(i);
    indices->push_back(i+1);
    positions->push_back(p.P);
  }
  positions->push_back(EvalCurve(1).P);

  line->UpdatePositions(std::move(positions));
  line->UpdateIndices(std::move(indices));

  polyline = line.get();

  auto shader = std::make_shared<SimpleShader>();

  auto line_node = make_unique<SceneNode>();
  line_node->CreateComponent<ShadingComponent>(shader);

  auto& rc = line_node->CreateComponent<RenderingComponent>(line);
  rc.SetDrawMode(DrawMode::Lines);

  glm::vec3 color(1.f, 1.f, 1.f);
  auto material = std::make_shared<Material>(color, color, color, 0);
  line_node->CreateComponent<MaterialComponent>(material);

  // curve_lines.push_back(line_node.get());

  AddChild(std::move(line_node));


}

void CurveNode::PlotCurve() {
  // TODO: plot the curve by updating the positions of its VertexObject.
//   for (unsigned int i = 0; i < scene_points.size(); i++) {
//     scene_points[i]->GetTransform().SetPosition(control_points_[i]);
//   }

  CurvePoint p;
  auto positions = make_unique<PositionArray>();
  for (int i = 0; i < N_SUBDIV_ + 1; i++) {

    p = EvalCurve((double)i/N_SUBDIV_);
    positions->push_back(p.P);
      
  }
  polyline->UpdatePositions(std::move(positions));
}

void CurveNode::PlotControlPoints() {
  // TODO: plot the curve control points.

}

// void CurveNode::PlotTangentLine() {
//   // TODO: implement plotting a line tangent to the curve.
//   // Below is a sample implementation for rendering a line segment
//   // onto the screen. Note that this is just an example. This code
//   // currently has nothing to do with the spline.

//   auto positions = make_unique<PositionArray>();
//   // // positions->push_back();
//   // positions->push_back(EvalCurve(N_SUBDIV_/2).T);
//   // positions->push_back(EvalCurve(N_SUBDIV_/2).T);
//   auto scale = glm::mat3(0.2, 0, 0, 0, 0.2, 0, 0, 0, 0.2);
//   positions->push_back(EvalCurve(1./2).P + glm::normalize(EvalCurve(1./2).T) * scale);
//   positions->push_back(EvalCurve(1./2).P - glm::normalize(EvalCurve(1./2).T) * scale);

//   tangentline->UpdatePositions(std::move(positions));
// }
}  // namespace GLOO
