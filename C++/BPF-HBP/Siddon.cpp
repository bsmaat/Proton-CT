#include "Siddon.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <tuple>

// default simple constructor (1 pixel...)
Siddon::Siddon() {
	d = 1;
	N = 2;
}

Siddon::Siddon(int N, double d) {
	this->N = N;
	this->d = d;

    // first plane position in Siddon's algorithm (i.e edge of backprojection space)
	this->Xp0 = -125;
	this->Yp0 = -125;
    for (int i=0; i<N; i++) {
        Xp.push_back(Xp0 + i*d);
        Yp.push_back(Yp0 + i*d);
    }

}

//intersect through voxel space given beginning point (P0) and end point (P1)
vector<vector<double> > Siddon::getIntersect(vector<double> & P0, vector<double> & P1) {

    double& X1 = P0[0];
    double& X2 = P1[0];
    double& Y1 = P0[1];
    double& Y2 = P1[1];

    double angleX;

    angleX = std::atan2((Y2-Y1), (X2-X1));

    vector<double> ax, ay;
    if ( (X2-X1) != 0 ) {
        ax.resize(Xp.size());
        std::transform(Xp.begin(), Xp.end(), ax.begin(), bind2nd(std::plus<double>(), -X1));
        std::transform(ax.begin(), ax.end(), ax.begin(), bind1st(std::multiplies<double>(), 1.0/(X2-X1)));
    }

    if ( (Y2-Y1) != 0 ) {
        ay.resize(Yp.size());
        std::transform(Yp.begin(), Yp.end(), ay.begin(), bind2nd(std::plus<double>(), -Y1));
        std::transform(ay.begin(), ay.end(), ay.begin(), bind1st(std::multiplies<double>(), 1.0/(Y2-Y1)));
    }


    // calculate amin and amax
    double amin, amax;
    vector<double> compare;
    if (ax.empty() == 0 && ay.empty() == 0) {

        compare.push_back(0);
        if (ax[0] < ax.back())
            compare.push_back(ax[0]);
        else
            compare.push_back(ax.back());
        if (ay[0] < ay.back())
            compare.push_back(ay[0]);
        else
            compare.push_back(ay.back());

        amin = *std::max_element(compare.begin(), compare.end());

        compare.clear();
        compare.push_back(1);
        if (ax[0] > ax.back())
            compare.push_back(ax[0]);
        else
            compare.push_back(ax.back());
        if (ay[0] > ay.back())
            compare.push_back(ay[0]);
        else
            compare.push_back(ay.back());

        amax = *std::min_element(compare.begin(), compare.end());

    } else if (ax.empty()) {

        compare.push_back(0);
        if (ay[0] < ay.back()) {
            compare.push_back(ay[0]);
        }
        else {
            compare.push_back(ay.back());
        }

        amin = *std::max_element(compare.begin(), compare.end());

        compare.clear();
        compare.push_back(1);
        if (ay[0] > ay.back())
            compare.push_back(ay[0]);
        else
            compare.push_back(ay.back());

        amax = *std::min_element(compare.begin(), compare.end());

    } else if (ay.empty()) {
        compare.push_back(0);
        if (ax[0] < ax.back())
            compare.push_back(ax[0]);
        else
            compare.push_back(ax.back());

        amin = *std::max_element(compare.begin(), compare.end());

        compare.clear();
        compare.push_back(1);
        if (ax[0] > ax.back())
            compare.push_back(ax[0]);
        else
            compare.push_back(ax.back());

        amax = *std::min_element(compare.begin(), compare.end());
    }


    // calculate imin, imax, ax, jmin, jmax, ay
    double imin, imax;
    if (!ax.empty()) {
        ax.erase(unique(ax.begin(), ax.end()), ax.end());

        if ((X2 - X1) >= 0) {
            imin = N - (Xp.back() - amin * (X2-X1) - X1) / d;
            imax = 1 + (X1 + amax * (X2 - X1) - Xp[0])/d;
            imin = ceil(imin);
            imax = floor(imax);
            if (imin<=imax)
                ax = std::vector<double>(ax.begin() + (int)imin-1, ax.begin()+(int)imax);
            else
                ax.clear();

        } else if ((X2 - X1) < 0) {
            imin = N - (Xp.back() - amax * (X2 - X1) - X1) / d;
            imax = 1 + (X1 + amin * (X2 - X1) - Xp[0]) / d;
            imin = ceil(imin);
            imax = floor(imax);
            if (imin<=imax)
                ax = std::vector<double>(ax.begin() + (int)imin-1, ax.begin() + (int)imax);
            else
                ax.clear();
        }
    }

    double jmin, jmax;
    if(!ay.empty()) {
        //sort(ay.begin(), ay.end());
        ay.erase(unique(ay.begin(), ay.end()), ay.end());
        if ((Y2 - Y1) >= 0) {
            jmin = N - (Yp.back() - amin * (Y2-Y1) - Y1) / d;
            jmax = 1 + (Y1 + amax * (Y2 - Y1) - Yp[0])/d;
            jmin = ceil(jmin);
            jmax = floor(jmax);
            if (jmin<=jmax)
                ay = std::vector<double>(ay.begin() + (int)jmin-1, ay.begin()+(int)jmax);
            else
                ay.clear();
        } else if ((Y2 - Y1) < 0) {
            jmin = N - (Yp.back() - amax * (Y2 - Y1) - Y1) / d;
            jmax = 1 + (Y1 + amin * (Y2 - Y1) - Yp[0]) / d;
            jmin = ceil(jmin);
            jmax = floor(jmax);
            if (jmin<=jmax)
                ay = std::vector<double>(ay.begin() + (int)jmin-1, ay.begin() + (int)jmax);
            else
                ay.clear();
        }
    }


    vector<double> a;
    a.push_back(amin);
    a.push_back(amax);
    a.insert(a.end(), ax.begin(), ax.end());
    a.insert(a.end(), ay.begin(), ay.end());
    sort(a.begin(), a.end());
    a.erase(std::unique(a.begin(), a.end()), a.end());


    double amid;
    int i, j;
    double pathLength;
    vector<vector<double> > indices;
    std::vector<double> v;
    v.resize(4);

    for(unsigned int m = 0; m<a.size()-1; m++) {
        amid = (a[m+1] + a[m])/2.0;
        // i = x position, j = y position
        i = floor( 1 + (X1 + amid*(X2-X1) - Xp[0])/d );
        j = floor( 1 + (Y1 + amid*(Y2-Y1) - Yp[0])/d );
        if ((i>0 && j > 0) && (i <= N-1 && j <= N-1)) {
            pathLength = (a[m+1] - a[m])* sqrt( pow((X2-X1),2) + pow((Y2-Y1),2) );
            v[0] = j;
            v[1] = i;
            v[2] = pathLength;
            v[3] = angleX;
            indices.push_back(v);
        }
    }


    //return make_tuple(indices, angleX);
    return indices;
}

//work in progress - quicker intersection method
vector<vector<double> > Siddon::getIntersect2(vector<double> & P0, vector<double> & P1) {

    double& X1 = P0[0];
    double& X2 = P1[0];
    double& Y1 = P0[1];
    double& Y2 = P1[1];

    double angleX;

    angleX = std::atan2((Y2-Y1), (X2-X1));


    vector<double> ax, ay;
    if ( (X2-X1) != 0 ) {
        ax.resize(Xp.size());
        std::transform(Xp.begin(), Xp.end(), ax.begin(), bind2nd(std::plus<double>(), -X1));
        std::transform(ax.begin(), ax.end(), ax.begin(), bind1st(std::multiplies<double>(), 1.0/(X2-X1)));
    }
    cout << Y2 << ", " << Y1<<endl;
    if ( (Y2-Y1) != 0 ) {
        ay.resize(Yp.size());
        std::transform(Yp.begin(), Yp.end(), ay.begin(), bind2nd(std::plus<double>(), -Y1));
        std::transform(ay.begin(), ay.end(), ay.begin(), bind1st(std::multiplies<double>(), 1.0/(Y2-Y1)));
    }


    // calculate amin and amax
    double amin, amax;
    vector<double> compare;
    if (ax.empty() == 0 && ay.empty() == 0) {

        compare.push_back(0);
        if (ax[0] < ax.back())
            compare.push_back(ax[0]);
        else
            compare.push_back(ax.back());
        if (ay[0] < ay.back())
            compare.push_back(ay[0]);
        else
            compare.push_back(ay.back());

        amin = *std::max_element(compare.begin(), compare.end());

        compare.clear();
        compare.push_back(1);
        if (ax[0] > ax.back())
            compare.push_back(ax[0]);
        else
            compare.push_back(ax.back());
        if (ay[0] > ay.back())
            compare.push_back(ay[0]);
        else
            compare.push_back(ay.back());

        amax = *std::min_element(compare.begin(), compare.end());

    } else if (ax.empty()) {

        compare.push_back(0);
        if (ay[0] < ay.back()) {
            compare.push_back(ay[0]);
        }
        else {
            compare.push_back(ay.back());
        }

        amin = *std::max_element(compare.begin(), compare.end());

        compare.clear();
        compare.push_back(1);
        if (ay[0] > ay.back())
            compare.push_back(ay[0]);
        else
            compare.push_back(ay.back());

        amax = *std::min_element(compare.begin(), compare.end());

    } else if (ay.empty()) {
        compare.push_back(0);
        if (ax[0] < ax.back())
            compare.push_back(ax[0]);
        else
            compare.push_back(ax.back());

        amin = *std::max_element(compare.begin(), compare.end());

        compare.clear();
        compare.push_back(1);
        if (ax[0] > ax.back())
            compare.push_back(ax[0]);
        else
            compare.push_back(ax.back());

        amax = *std::min_element(compare.begin(), compare.end());
    }



    double axmin = min(ax[0], ax.back());
cout << "hi\n";
    double axmax = max(ax[0], ax.back());
cout << "hi1\n";
    double aymin = min(ay[0], ay.back());
cout << "hi2\n";
    double aymax = max(ay[0], ay.back());
cout << "hi3\n";

    //calculate imin, imax, jmin, jmax
    int imin, imax, jmin, jmax;
    if (X1 < X2) {
        if (amin == axmin) {
            imin = 1;
        } else {
            imin = ceil((X1 + amin*(X2-X1) - Xp0)/d);
        }

        if (amax == axmax) {
            imax = N-1;
        } else {
            imax = floor((X1 + amax*(X2-X1) - Xp0)/d);
        }
    } else {
        if (amin == axmin) {
            imax = N-2;
        }
        else {
            imax = floor((X1 + amin*(X2-X1) - Xp0)/d);
        }

        if (amax == axmax) {
            imin = 0;
        } else {
            imin = ceil((X1 + amax*(X2-X1) - Xp0)/d);
        }
    }

    if (Y1 < Y2) {
        if (amin == aymin) {
            jmin = 1;
        } else {
            jmin = ceil((Y1 + amin*(Y2-Y1) - Yp0)/d);
        }

        if (amax == aymax) {
            jmax = N-1;
        } else {
            jmax = floor((Y1 + amax*(Y2-Y1) - Yp0)/d);
        }
    } else {
        if (amin == aymin) {
            jmax = N-2;
        }
        else {
            jmax = floor((Y1 + amin*(Y2-Y1) - Yp0)/d);
        }

        if (amax == aymax) {
            jmin = 0;
        } else {
            jmin = ceil((Y1 + amax*(Y2-Y1) - Yp0)/d);
        }
    }

    double ax0, ay0;
    if (X1 < X2)
        ax0 = (Xp0 + imin * d - X1)/(X2-X1);
    else
        ax0 = (Xp0 + imax * d - X1)/(X2-X1);


    if (Y1 < Y2)
        ay0 = (Yp0 + jmin * d - Y1)/(Y2-Y1);
    else
        ay0 = (Yp0 + jmax * d - Y1)/(Y2-Y1);

    double Np = (imax - imin + 1) + (jmax - jmin + 1);

    double minaxay = min(ax0,ay0);
    double aTmp = (minaxay + amin)/2.0;

    double i, j;

    i = floor((X1 + aTmp*(X2-X1) - Xp0)/d);
    j = floor((Y1 + aTmp*(Y2-Y1) - Yp0)/d);

    double axu = d/(X2-X1);
    double ayu = d/(Y2-Y1);

    double iu, ju;
    if (X1<X2)
        iu = 1;
    else
        iu = -1;

    if (Y1<Y2)
        ju = 1;
    else
        ju = -1;


    double d12 = 0;
    double ac = amin;

    double Nv = Np + 1;

    double dconv = sqrt(pow(X2-X1,2) + pow(Y2-Y1,2));

    double amid;
    double pathLength;
    vector<vector<double> > indices;
    std::vector<double> v;
    v.resize(4);

    double lij;
    for (int m = 0; m<Nv; m++) {
        if (ax0 < ay0) {
            lij = (ax0 - ac) * dconv;
            d12 = d12 + lij;
            i = i + iu;
            ac = ax0;
            ax0 = ax0 + axu;
        } else {
            lij = (ay0 - ac) * dconv;
            d12 = d12 + lij;
            j = j + ju;
            ac = ay0;
            ay0 = ay0 + ayu;
        }
        v[0] = i;
        v[1] = j;
        v[2] = lij;
        v[3] = angleX;
        indices.push_back(v);
    }



    //return make_tuple(indices, angleX);
    return indices;
}

//print out vector to terminal
void Siddon::writeVector(vector<double> vec) {
    for (vector<double>::const_iterator i = vec.begin(); i != vec.end(); ++i)
        std::cout << *i << ' ';
        std::cout << std::endl;
}

