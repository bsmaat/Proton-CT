#ifndef PATH_H_INCLUDED
#define PATH_H_INCLUDED
#include <Eigen/LU>
#include <Eigen/Dense>
#include <vector>
using namespace std;
using namespace Eigen;

class Path {
public:
    Path();
    Path(double&, double&, double&, double&);
    Path(vector<double> &, double & );
    vector<complex<double> > quadraticSolver(double & a, double & b, double & c);
    Vector4d CubicSpline();
    vector<vector<double> > ellipseIntersect(vector<double> & A, double & entryAngle, vector<double>& B, double& exitAngle, double& angle);
    vector<vector<double> > circleIntersect(vector<double> & A, double & entryAngle, vector<double> & B, double & exitAngle, double & phi);
    Vector4d CubicSpline(double & x0, double & x1, double & y0, double & y1, double & dy0, double & dy1);
    vector<vector<double> > CubicSplinePath(vector<double> & x, Vector4d & coeff, double & phi);
    vector<vector<double> > mlp(vector<double> & x, Vector2d & y0, Vector2d & y1, double & phi);
    void testFn(double&);


    vector<vector<double> >& getPath() { return path; };
    void rotate(double&);

private:
    double entryPos, entryAngle, exitPos, exitAngle;
    double entrydydx, exitdydx;
    //Vector4d CubicSpline(double x0, double x1, double y0, double y1, double dy0, double dy1);
    //vector<vector<double> > CubicSplinePath(vector<double> x, Vector4d coeff);
    //vector<vector<double> > ellipseIntersect(vector<double> A, double entryAngle, vector<double> B, double exitAngle);
    //vector<complex<double> > quadraticSolver(double a, double b, double c);
    bool real_less(const std::complex<double> & lhs, const std::complex<double> & rhs);
    vector<vector<double> > path;


};

#endif
