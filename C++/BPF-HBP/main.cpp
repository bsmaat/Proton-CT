#include <iostream>

#include <fstream>

#include <stdexcept>
#include "Siddon.h"
#include "Path.h"
#include "CSVRow.h"
#include "Backproject.h"
#include "ReadWrite.h"
#include <thread>
#include <mutex>
#include <numeric>
#include <iomanip>
#include <sstream>

std::mutex m;
std::mutex mut;
int counter = 0;


int N = 251; // numer of voxels + 1 in each direction
double d = 1.0; // length of each voxel
//int N = 181;
//double d = 1.0;

int noOfAngles = 720; // number of angles we are rotating around


template <class T1, class T2>
T1 lexical_cast(const T2& t2) {
    std::stringstream s;
    s << t2;
    T1 t1;
    if (s >> t1 && s.eof()) {
        return t1;
    } else {
        throw std::runtime_error("bad conversion");
        return T1();
        exit(1);
    }

}

std::istream& operator>>(std::istream& str, CSVRow& data) {
    data.readNextRow(str);
    return str;
}

//find mean and standard deviation of vector
vector<double> stats(vector<double> & v) {
    double sum = std::accumulate(v.begin(), v.end(), 0.0);
    double mean = sum / v.size();


    std::vector<double> diff(v.size());
    std::transform(v.begin(), v.end(), diff.begin(), [mean](double x) { return (x - mean);});
    double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
    double stdev = std::sqrt(sq_sum/v.size());


    vector<double> vec(2);
    vec[0] = mean;
    vec[1] = stdev;
    return vec;
}

// perform sigma filtering on proton data
void filterData(vector<vector<double> > & data) {

    vector<double> relAngle(data.size()), relPos(data.size());

    for (int i =0; i < data.size(); i++) {
        relAngle[i] = data[i][3] - data[i][1];
        relPos[i] = data[i][2] - data[i][0];
    }

    vector<double> angleStats = stats(relAngle);
    vector<double> posStats = stats(relPos);

    // filter elements
    for (int i = relAngle.size() - 1; i >= 0; i--) {
        if ((abs(relAngle[i] - angleStats[0]) > 3 * angleStats[1]) || (abs(relPos[i] - posStats[0]) > 3 * posStats[1])) {
            data.erase(data.begin() + i, data.begin() + i + 1);
        }
    }

}


void backprojectAngleBPF(int id) {

    std::ostringstream filename;
    filename << "proj" << id << ".csv"; // file names of projection files
    std::string str = filename.str();
	std::ifstream file(str);
	CSVRow row;
    vector<vector<double> > detected;
    vector<double> tmp;
    tmp.resize(5);
    double entryAngle, exitAngle;

    //read in projection file
	while(file >> row)
	{
        entryAngle = atan(lexical_cast<double>(row[4])/lexical_cast<double>(row[6]));
        exitAngle = atan(lexical_cast<double>(row[12])/lexical_cast<double>(row[14]));

        tmp[0] = lexical_cast<double>(row[1])*1000; // lateral entry position (entryPos) (mm)
        tmp[1] = entryAngle;
        tmp[2] = lexical_cast<double>(row[9])*1000; // lateral exit position (exitPos) (mm)
        tmp[3] = exitAngle;
        tmp[4] = lexical_cast<double>(row[16]); // projection
        detected.push_back(tmp);

	}

	//filter data
	cout << "Before filtering: " << detected.size() << ", ";
	filterData(detected);
    cout << "After filtering: " << detected.size() << endl;


    ReadWrite* r = new ReadWrite();
    Siddon* s = new Siddon(N, d);

    double angle = id*2*M_PI/noOfAngles;
    MatrixXd bpAngleBPF;
    MatrixXd chordLengthBPF;
    bpAngleBPF = MatrixXd::Zero(N-1,N-1);
    chordLengthBPF = MatrixXd::Zero(N-1,N-1);

    // backproject each proton onto bpAngleBPF!
    for (int i = 0; i < detected.size(); i++) {
        Backproject* bp = new Backproject();
        bp->setAngle(angle);
        bp->radonBPF(detected[i], bpAngleBPF, chordLengthBPF, *s);

        delete bp;
    }

    for (int i = 0; i < bpAngleBPF.rows(); i++) {
        for (int j = 0; j < bpAngleBPF.rows(); j++) {
            if (chordLengthBPF(i,j) != 0) {
                bpAngleBPF(i,j) = bpAngleBPF(i,j) / chordLengthBPF(i,j);
            }
            //fill the air parts
            if (bpAngleBPF(i,j) == 0.0) {
                bpAngleBPF(i,j) = 0.443; // air - can't remember exact number!
            }
        }
    }



    filename.str("");
    filename << "bp" << id << ".csv";
    str = filename.str();
    const char* fileStr = str.c_str();
    r->writeCSV(bpAngleBPF, fileStr);
    m.lock();
        counter++;
        std::cout << "Counter: " << counter << std::endl;
    m.unlock();
}


int main(int argc, char* argv[]) {

    int id;

    istringstream ss(argv[1]);
    if (!(ss >> id))
        std::cerr << "Invalid number " << argv[1] << std::endl;

    std::cout << "MLP fixed (1)" << std::endl;
    std::cout << "Starting..." << std::endl;
    backprojectAngleBPF(id);



}


