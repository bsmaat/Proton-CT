#ifndef READWRITE_H_INCLUDED
#define READWRITE_H_INCLUDED

#include <iostream>
#include <vector>
#include <Eigen/Dense>
using namespace std;

class ReadWrite {

public:
    //void readCSV();
    void writeCSV(vector<vector<double > > & vec, const char* filename);
    void writeCSV(Eigen::MatrixXd & vec, const char* filename);
    void writeCSV(Eigen::Matrix2d & vec, const char* filename);


private:

};

#endif // READWRITE_H_INCLUDED
