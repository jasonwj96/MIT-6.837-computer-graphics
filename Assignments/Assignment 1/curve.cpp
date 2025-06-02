#include "curve.h"
#include "extra.h"
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
using namespace std;

namespace {
  // Approximately equal to.  We don't want to use == because of
  // precision issues with floating point.
  inline bool approx(const Vector3f &lhs, const Vector3f &rhs) {
    const float eps = 1e-8f;
    return (lhs - rhs).absSquared() < eps;
  }
}


Curve evalBezier(const vector<Vector3f> &P, unsigned steps) {
  // Check
  if (P.size() < 4 || P.size() % 3 != 1) {
    cerr << "evalBezier must be called with 3n+1 control points." << endl;
    exit(0);
  }

  // TODO:
  // You should implement this function so that it returns a Curve
  // (e.g., a vector< CurvePoint >).  The variable "steps" tells you
  // the number of points to generate on each piece of the spline.
  // At least, that's how the sample solution is implemented and how
  // the SWP files are written.  But you are free to interpret this
  // variable however you want, so long as you can control the
  // "resolution" of the discretized spline curve with it.

  // Make sure that this function computes all the appropriate
  // Vector3fs for each CurvePoint: V,T,N,B.
  // [NBT] should be unit and orthogonal.

  // Also note that you may assume that all Bezier curves that you
  // receive have G1 continuity.  Otherwise, the TNB will not be
  // be defined at points where this does not hold.

  cerr << "\t>>> evalBezier has been called with the following input:" << endl;

  cerr << "\t>>> Control points (type vector< Vector3f >): " << endl;
  for (const auto &i: P) {
    cerr << "\t>>> " << i << endl;
  }

  cerr << "\t>>> Steps (type steps): " << steps << endl;
  cerr << "\t>>> Returning empty curve." << endl;


  int subCurveCount = 0;
  int pCount = 0;

  for (unsigned i = 0; i < P.size(); i++) {
    pCount++;

    if (pCount == 4) {
      pCount = 1;
      subCurveCount++;
    }
  }

  // Preallocate a curve with steps+1 CurvePoints
  Curve curve(subCurveCount * steps + 1);

  Matrix4f matBernstein(
    1.0f, -3.0f, 6.0f, -1.0f,
    0.0f, 3.0f, -6.0f, 3.0f,
    0.0f, 0.0f, 3.0f, 3.0f,
    0.0f, 0.0f, 0.0f, 3.0f
  );

  Matrix4f matBernsteinPrime(
    1.0f, -3.0f, 6.0f, -3.0f,
    0.0f, 3.0f, -12.0f, 9.0f,
    0.0f, 0.0f, 6.0f, -9.0f,
    0.0f, 0.0f, 0.0f, 3.0f
  );


  Vector3f binorm;
  Vector3f prevBinorm;

  int k = 0;

  for (unsigned i = 0; i < P.size() - 3; i += 3) {
    binorm = (i == 0) ? Vector3f(0.0f, 0.0f, 1.0f) : prevBinorm;


    Matrix4f controlPoints(
      P[i][0], P[i + 1][0], P[i + 2][0], P[i + 3][0],
      P[i][1], P[i + 1][1], P[i + 2][1], P[i + 3][1],
      P[i][2], P[i + 1][2], P[i + 2][2], P[i + 3][2],
      0.0f, 0.0f, 0.0f, 0.0f
    );

    for (unsigned j = 0; j <= steps; ++j) {
      const float t = static_cast<float>(j) / static_cast<float>(steps);

      Vector4f monoBase(1, t, t * t, t * t * t);

      // Initialize position
      // We're pivoting counterclockwise around the y-axis
      curve[k].V = Vector3f(
        (controlPoints * matBernstein * monoBase)[0],
        (controlPoints * matBernstein * monoBase)[1],
        (controlPoints * matBernstein * monoBase)[2]
      );

      // Tangent vector is first derivative
      curve[k].T = Vector3f(
        (controlPoints * matBernsteinPrime * monoBase)[0],
        (controlPoints * matBernsteinPrime * monoBase)[1],
        (controlPoints * matBernsteinPrime * monoBase)[2]
      ).normalized();

      // Normal vector is second derivative
      curve[k].N = Vector3f::cross(binorm, curve[k].T).normalized();

      // Finally, binormal is facing up.
      curve[k].B = Vector3f::cross(curve[k].T, curve[k].N).normalized();

      ++k;
    }
  }


  // Fill it in counterclockwise
  for (unsigned i = 0; i <= steps; ++i) {
    // step from 0 to 2pi
    const float t = M_PI * 2.0f * static_cast<float>(i) / steps;

    // Initialize position
    // We're pivoting counterclockwise around the y-axis
    curve[i].V = 2.0f * Vector3f(cos(t), sin(t), 0);

    // Tangent vector is first derivative
    curve[i].T = Vector3f(-sin(t), cos(t), 0);

    // Normal vector is second derivative
    curve[i].N = Vector3f(-cos(t), -sin(t), 0);

    // Finally, binormal is facing up.
    curve[i].B = Vector3f(0, 0, 1);
  }

  return curve;
}

Curve evalBspline(const vector<Vector3f> &P, unsigned steps) {
  // Check
  if (P.size() < 4) {
    cerr << "evalBspline must be called with 4 or more control points." << endl;
    exit(0);
  }

  // TODO:
  // It is suggested that you implement this function by changing
  // basis from B-spline to Bezier.  That way, you can just call
  // your evalBezier function.

  cerr << "\t>>> evalBSpline has been called with the following input:" << endl;

  cerr << "\t>>> Control points (type vector< Vector3f >): " << endl;
  for (unsigned i = 0; i < P.size(); ++i) {
    cerr << "\t>>> " << P[i] << endl;
  }

  cerr << "\t>>> Steps (type steps): " << steps << endl;
  cerr << "\t>>> Returning empty curve." << endl;

  // Return an empty curve right now.
  return Curve();
}

Curve evalCircle(float radius, unsigned steps) {
  // This is a sample function on how to properly initialize a Curve
  // (which is a vector< CurvePoint >).

  // Preallocate a curve with steps+1 CurvePoints
  Curve R(steps + 1);

  // Fill it in counterclockwise
  for (unsigned i = 0; i <= steps; ++i) {
    // step from 0 to 2pi
    float t = 2.0f * M_PI * float(i) / steps;

    // Initialize position
    // We're pivoting counterclockwise around the y-axis
    R[i].V = radius * Vector3f(cos(t), sin(t), 0);

    // Tangent vector is first derivative
    R[i].T = Vector3f(-sin(t), cos(t), 0);

    // Normal vector is second derivative
    R[i].N = Vector3f(-cos(t), -sin(t), 0);

    // Finally, binormal is facing up.
    R[i].B = Vector3f(0, 0, 1);
  }

  return R;
}

void drawCurve(const Curve &curve, float framesize) {
  // Save current state of OpenGL
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  // Setup for line drawing
  glDisable(GL_LIGHTING);
  glColor4f(1, 1, 1, 1);
  glLineWidth(1);

  // Draw curve
  glBegin(GL_LINE_STRIP);
  for (unsigned i = 0; i < curve.size(); ++i) {
    glVertex(curve[i].V);
  }
  glEnd();

  glLineWidth(1);

  // Draw coordinate frames if framesize nonzero
  if (framesize != 0.0f) {
    Matrix4f M;

    for (unsigned i = 0; i < curve.size(); ++i) {
      M.setCol(0, Vector4f(curve[i].N, 0));
      M.setCol(1, Vector4f(curve[i].B, 0));
      M.setCol(2, Vector4f(curve[i].T, 0));
      M.setCol(3, Vector4f(curve[i].V, 1));

      glPushMatrix();
      glMultMatrixf(M);
      glScaled(framesize, framesize, framesize);
      glBegin(GL_LINES);
      glColor3f(1, 0, 0);
      glVertex3d(0, 0, 0);
      glVertex3d(1, 0, 0);
      glColor3f(0, 1, 0);
      glVertex3d(0, 0, 0);
      glVertex3d(0, 1, 0);
      glColor3f(0, 0, 1);
      glVertex3d(0, 0, 0);
      glVertex3d(0, 0, 1);
      glEnd();
      glPopMatrix();
    }
  }

  // Pop state
  glPopAttrib();
}
