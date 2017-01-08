#ifndef SIDDON_H_INCLUDED
#define SIDDON_H_INCLUDED

#include <vector>
using namespace std;

class Siddon {
public:
	Siddon();
	Siddon(int, double);
	//~Siddon();
	vector<vector<double> > getIntersect(vector<double>&, vector<double>&);
	vector<vector<double> > getIntersect2(vector<double>&, vector<double>&);


	//tuple<vector<vector<double> >, double> getIntersect(vector<double>&, vector<double>&);

private:
	int N;  // number of planes
	double d; // spacing between planes
	vector<double> Xp; // position of x planes
	vector<double> Yp; // position of y planes
	double Xp0;
	double Yp0;
	void writeVector(vector<double>);
};


#endif // SIDDON_H_INCLUDED
