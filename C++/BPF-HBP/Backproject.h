#ifndef BACKPROJECT_H_INCLUDED
#define BACKPROJECT_H_INCLUDED

#include <vector>
#include "Siddon.h"
#include <Eigen/LU>
#include <Eigen/Dense>
#include <mutex>
using namespace std;
using namespace Eigen;

class Backproject {

public:
    Backproject();
    Backproject(vector<vector<double> > &data);
    Backproject(vector<double> & data);
    void radon(vector<double> & data, vector<vector<double> > & bpAngle, vector<vector<double> > & chordLength, Siddon & s);
    void radon(vector<double> & data, vector<MatrixXd> & bpAngle, vector<MatrixXd> & chordLength, Siddon & s, mutex &);
    void radonHBP(vector<double> & data, vector<MatrixXd> & bpAngle, vector<MatrixXd> & chordLength, Siddon & s, mutex & mut1);
    //void radonBPF(vector<double> & data, Matrix2d & bpAngle, Matrix2d & chordLength, Siddon & s, mutex & mut1);

    void rotate(vector<double>&);
    vector<vector<double> > getYValues() { return yVal; };
    void setAngle(double & angle);

    void radonBPF(vector<double> & data, MatrixXd & bpAngle, MatrixXd & chordLength, Siddon & s);
    void radonBPF();
    void testFn(string &);
private:
    vector<vector<double> >* dataTmp;
    vector<vector<double> > yVal;
    double angle;
};

#endif // BACKPROJECT_H_INCLUDED
