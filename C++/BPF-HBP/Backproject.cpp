#include "Backproject.h"
#include <iostream>
#include "Path.h"
#include "Siddon.h"
#include <cmath>

using namespace std;
using namespace Eigen;

Backproject::Backproject() {

}

Backproject::Backproject(vector<vector<double> > &data) {
    //std::cout << data[0][0] << std::endl;
    dataTmp = &data;
    (*dataTmp)[0][0] = 1000;
    //std::cout << (*dataTmp)[0][0] << std::endl;

}

void Backproject::setAngle(double & angle) {
    this->angle = angle;
}

// passing bpAngle and chordLength by reference to radon...
// method takes bpAngle, chordLength as vectors
void Backproject::radon(vector<double> & data, vector<vector<double> > & bpAngle, vector<vector<double> > & chordLength, Siddon & s) {

    // first compute the path
    Path * p = new Path(data, angle);

    vector<vector<double> > & path = p->getPath();
    vector<vector<double> > indices;

    //tie(indices, angleX) = s.getIntersect(path[0], path[1]);
    indices = s.getIntersect(path[0], path[1]);
    for (vector<vector<double> >::iterator i = ++path.begin(); i != --path.end(); ++i) {
        //tie(yVal, angleXNew) = (s.getIntersect(*i, *(i+1)));
        yVal = (s.getIntersect(*i, *(i+1)));
        indices.insert(indices.end(), yVal.begin(), yVal.end());
        // check if it's the same index and if it is add them together
        /*
        if ((indices.back()[0] == yVal[0][0]) && (indices.back()[1] == yVal[0][1])) {
            indices.back()[2] = indices.back()[2] + yVal[0][2];
            indices.insert(indices.end(), yVal.begin()+1, yVal.end());
        } else
            indices.insert(indices.end(), yVal.begin(), yVal.end());
        */
    }

    for (vector<vector<double> > ::const_iterator i = indices.begin(); i != indices.end(); ++i) {
        bpAngle[(*i)[0]-1][(*i)[1]-1] = (*i)[2] * data[4] + bpAngle[(*i)[0]-1][(*i)[1]-1];
        //bpAngle[(*i)[0]-1][(*i)[1]-1] = data[4] + bpAngle[(*i)[0]-1][(*i)[1]-1];
        chordLength[(*i)[0]-1][(*i)[1]-1] = chordLength[(*i)[0]-1][(*i)[1]-1] + (*i)[2];
    }

    delete p;

}

// radon BPF
void Backproject::radonBPF(vector<double> & data, MatrixXd & bpAngle, MatrixXd & chordLength, Siddon & s) {

    // first compute the path
    Path * p = new Path(data, angle);

    vector<vector<double> > & path = p->getPath();
    vector<vector<double> > indices;

    indices = s.getIntersect(path[0], path[1]);

    for (vector<vector<double> >::iterator i = ++path.begin(); i != --path.end(); ++i) {
        yVal = (s.getIntersect(*i, *(i+1)));
        indices.insert(indices.end(), yVal.begin(), yVal.end());
    }

    // think i need to lock this if i want to multithread the backprojection
    for (vector<vector<double> > ::const_iterator i = indices.begin(); i != indices.end(); ++i) {
        bpAngle((*i)[0]-1,(*i)[1]-1) = (*i)[2] * data[4] + bpAngle((*i)[0]-1,(*i)[1]-1);
        chordLength((*i)[0]-1,(*i)[1]-1) = chordLength((*i)[0]-1,(*i)[1]-1) + (*i)[2];
    }

    delete p;

}
// method takes bpAngle, chordLength as vector of matrices
void Backproject::radon(vector<double> & data, vector<MatrixXd> & bpAngle, vector<MatrixXd> & chordLength, Siddon & s, mutex & mut1) {

    // first compute the path
    Path * p = new Path(data, angle);

    vector<vector<double> > & path = p->getPath();
    vector<vector<double> > indices;

    indices = s.getIntersect(path[0], path[1]);
    for (vector<vector<double> >::iterator i = ++path.begin(); i != --path.end(); ++i) {
        yVal = (s.getIntersect(*i, *(i+1)));
        indices.insert(indices.end(), yVal.begin(), yVal.end());
    }


    int angleXInd, tmpAng;
    double angleX;
    tmpAng = round(angle*180/M_PI);
    for (vector<vector<double> > ::const_iterator i = indices.begin(); i != indices.end(); ++i) {
        //bpAngle((*i)[0]-1,(*i)[1]-1) = (*i)[2] * data[4] + bpAngle((*i)[0]-1, (*i)[1]-1);
        //chordLength((*i)[0]-1, (*i)[1] -1) = chordLength((*i)[0]-1, (*i)[1]-1) + (*i)[2];
        //bpAngle(round((*i)[3]),(*i)[0]-1,(*i)[1]-1) = (*i)[2] * data[4] + bpAngle(round((*i)[3]), (*i)[0]-1, (*i)[1]-1);
        angleXInd = round((*i)[3]*180/M_PI);
        if (angleXInd < 0)
            angleXInd = angleXInd + 180;
        else if (angleXInd > 179)
            angleXInd = angleXInd - 180;


        mut1.lock();
        /*
            if (tmpAng != angleXInd) {
                cout << tmpAng << " : " << angleXInd << ", " << (*i)[0] << " : " << (*i)[1] << endl;
            }
            */
            bpAngle[angleXInd]((*i)[0]-1,(*i)[1]-1) = (*i)[2] * data[4] + bpAngle[angleXInd]((*i)[0]-1, (*i)[1]-1);
            chordLength[angleXInd]((*i)[0]-1, (*i)[1] -1) = chordLength[angleXInd]((*i)[0]-1, (*i)[1]-1) + (*i)[2];

        mut1.unlock();


    }

    delete p;

}

void Backproject::radonBPF() {
}

// HBP radon (weighted backprojections)
void Backproject::radonHBP(vector<double> & data, vector<MatrixXd> & bpAngle, vector<MatrixXd> & chordLength, Siddon & s, mutex & mut1) {

    //cout << "HEY" << endl;
    // first compute the path
    Path * p = new Path(data, angle);

    vector<vector<double> > & path = p->getPath();
    vector<vector<double> > indices;

    indices = s.getIntersect(path[0], path[1]);
    for (vector<vector<double> >::iterator i = ++path.begin(); i != --path.end(); ++i) {
        yVal = (s.getIntersect(*i, *(i+1)));
        indices.insert(indices.end(), yVal.begin(), yVal.end());
    }


    int angleXInd;
    double angleX;

    for (vector<vector<double> > ::const_iterator i = indices.begin(); i != indices.end(); ++i) {

        angleXInd = round((*i)[3]*180/M_PI);
        if (angleXInd < 0)
            angleXInd = angleXInd + 180;
        else if (angleXInd > 179)
            angleXInd = angleXInd - 180;

        angleX = angleXInd * M_PI/180.0; // nearest angle in radians

        mut1.lock();
            bpAngle[angleXInd]((*i)[0]-1,(*i)[1]-1) = (*i)[2] * data[4] + bpAngle[angleXInd]((*i)[0]-1, (*i)[1]-1);
            chordLength[angleXInd]((*i)[0]-1, (*i)[1] -1) = chordLength[angleXInd]((*i)[0]-1, (*i)[1]-1) + (*i)[2];
        mut1.unlock();


    }

    delete p;

}


Backproject::Backproject(vector<double> & data) {

}

void Backproject::testFn(string & x) {

}
