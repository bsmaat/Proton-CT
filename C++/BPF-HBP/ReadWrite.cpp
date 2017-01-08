#include "ReadWrite.h"
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

void ReadWrite::writeCSV(vector<vector<double> > & vec, const char* filename) {

    ofstream output(filename);

    for (vector<vector<double> >::const_iterator i = vec.begin(); i != vec.end(); ++i) {
        for (vector<double>::const_iterator j = i->begin(); j != --i->end(); ++j) {
            output << setprecision(15) << *j << ", ";
        }
        output << *(--i->end()) << "\n";
    }

}

void ReadWrite::writeCSV(Eigen::MatrixXd & vec, const char* filename) {

    ofstream output(filename);

    for(int i = 0; i < vec.rows(); i++) {
        for (int j = 0; j < vec.rows()-1; j++) {
            output << setprecision(15) << vec(i, j) << ", ";
        }
        output << setprecision(15) << vec(i, vec.rows()-1) << "\n";
    }
}


void ReadWrite::writeCSV(Eigen::Matrix2d & vec, const char* filename) {

    ofstream output(filename);

    for(int i = 0; i < vec.rows(); i++) {
        for (int j = 0; j < vec.rows()-1; j++) {
            output << setprecision(15) << vec(i, j) << ", ";
        }
        output << setprecision(15) << vec(i, vec.rows()-1) << "\n";
    }
}



