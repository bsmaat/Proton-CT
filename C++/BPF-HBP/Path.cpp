#include "Path.h"
#include <iostream>
#include <algorithm>
#include <cmath>

double detPos = 230; // actual position of detector relative to centre of pCT set up
double extraDetPos = 1000; // extrapolation position
int pathType = 2; // 0 = straight path, 1 = cubic path, 2 = most likely path


Path::Path() {
    //CubicSpline(1, 2, 3, 4, 5, 6);
}

Path::Path(double & entryPos, double & entryAngle, double & exitPos, double & exitAngle) {
    this->entryPos = entryPos;
    this->entryAngle = entryAngle;
    this->exitPos = exitPos;
    this->exitAngle = exitAngle;
}

Path::Path(vector<double> & row, double & angle) {

    this->entryPos = row[0];
    this->entryAngle = row[1];
    this->exitPos = row[2];
    this->exitAngle = row[3];

    vector<double> A, B;

    // A is the first (x,y) coordinate, and B is the final (x,y) coordinate

    /* //this is without extrapolation
    A.push_back(-230);
    A.push_back(row[0]);
    B.push_back(+230);
    B.push_back(row[2]);
    */

    // if we want to extrapolate, knowing A and B are the first and last coordiantes, change them accordingly...
    // can do this with entryAngle and exitAngle?
    double m1;
    double m2;

    m1 = sin(entryAngle)/cos(entryAngle);
    m2 = sin(exitAngle)/cos(exitAngle);

    double c1 = row[0] - m1 * (-detPos);
    double c2 = row[2] - m2 * (+detPos);

    double y1, y2;
    y1 = m1 * (-extraDetPos) + c1;
    y2 = m2 * (+extraDetPos) + c2;
    A.push_back(-extraDetPos);
    A.push_back(y1);
    B.push_back(+extraDetPos);
    B.push_back(y2);


    vector<vector<double> > ellipseCoords,circleCoords;
    ellipseCoords =  ellipseIntersect(A, row[1], B, row[3], angle); // will be empty if it doesn't intersect

    /*
    circleCoords =  circleIntersect(A, row[1], B, row[3], angle); // will be empty if it doesn't intersect

    for (int i = 0; i < ellipseCoords.size(); i++) {
        cout << ellipseCoords[i][0] << " " << ellipseCoords[i][1] << endl;
        cout << circleCoords[i][0] << " " << circleCoords[i][1] << endl;
    }
*/
    //vector<vector<double> > ellipseCoords =  ellipseIntersect(A, entryAngleNew, B, exitAngleNew, angTmp); // will be empty if it doesn't intersect


    vector<double> point;
    /*
    point.push_back(-230 * cos(angle) - row[0] * sin(angle));
    point.push_back(-230 * sin(angle) + row[0] * cos(angle));
    */
    point.push_back(-extraDetPos * cos(angle) - y1 * sin(angle));
    point.push_back(-extraDetPos * sin(angle) + y1 * cos(angle));
    path.push_back(point);

    // the -angle is kind of a guess
    //entrydydx = sin(row[1]-angle)/cos(row[1]-angle);
    //exitdydx = sin(row[3]-angle)/cos(row[3]-angle);

    entrydydx = sin(row[1])/cos(row[1]);
    exitdydx = sin(row[3])/cos(row[3]);


    // if path would intersect with ellipse, then find CubicSpline stuff
    if (!ellipseCoords.empty()) {
        if (pathType == 1) { // cubic path
        Vector4d spline = CubicSpline(ellipseCoords[0][0], ellipseCoords[1][0], ellipseCoords[0][1], ellipseCoords[1][1], entrydydx, exitdydx);
        vector<double> u1;


        int numOfSteps = 0;
        double stepSize = 1.0;
        for (double i = ellipseCoords[0][0] ; i < ellipseCoords[1][0]; i=i+stepSize) {
            numOfSteps = numOfSteps+1;
            u1.push_back(i);
        }
        u1.push_back(ellipseCoords[1][0]);
        vector<vector<double> > cubicPath = CubicSplinePath(u1, spline, angle);
        path.insert(path.end(), cubicPath.begin(), cubicPath.end() );
        } else if (pathType == 2) { // most likely path
            vector<double> u1;
            int numOfSteps = 0;
            //double stepSize = (ellipseCoords[1][0] - ellipseCoords[0][0])/numOfSteps;
            double stepSize = 1.0;
            for (double i = ellipseCoords[0][0]; i < ellipseCoords[1][0]; i=i+stepSize) {
                numOfSteps = numOfSteps + 1;
                //u1.push_back(i);
                //u1.push_back(ellipseCoords[0][0] + stepSize*i);
                u1.push_back(i);
            }
            u1.push_back(ellipseCoords[1][0]);
            //cout << numOfSteps << endl;
            Vector2d y0(ellipseCoords[0][1], entryAngle);
            Vector2d y1(ellipseCoords[1][1], exitAngle);

            vector<vector<double> > mostLikelyPath = mlp(u1, y0, y1, angle);
            path.insert(path.end(), mostLikelyPath.begin(), mostLikelyPath.end() );

/*
            cout << "y0: " << y0 << endl;
            cout << "y1: " << y1 << endl;
            cout << "Ellipse intersect: " << "x0: "<< ellipseCoords[0][0] << "x1: " << ellipseCoords[1][0] << endl;
            cout << "Ellipse intersect: " << "y0: "<< ellipseCoords[0][1] << "y1: " << ellipseCoords[1][1] << endl;
*/
        } else if (pathType == 0)
        {
            double xTmp, yTmp;
            xTmp = ellipseCoords[0][0];
            yTmp = ellipseCoords[1][0];
            point.clear();
            point.push_back(xTmp); // x1 intersection
            point.push_back(yTmp); // y1 intersection

            xTmp = ellipseCoords[0][1];
            yTmp = ellipseCoords[1][1];
            point.push_back(xTmp); // x2 intersection
            point.push_back(yTmp); // y2 intersection
        }

    }

    point.clear();

    point.push_back(+extraDetPos * cos(angle) - y2 * sin(angle));
    point.push_back(+extraDetPos * sin(angle) + y2 * cos(angle));
    path.push_back(point);



/*
    cout << "x: ";
    for (int i = 0; i < path.size(); i++) {
        cout << path[i][0] << " ";
    }
    cout << endl;

    cout << "y: ";
    for (int i = 0; i < path.size(); i++) {
        cout << path[i][1] << " ";
    }
    cout << endl;
*/



}

// calculate cubic spline coefficients
Vector4d Path::CubicSpline(double & x0, double & x1, double & y0, double & y1, double & dy0, double & dy1) {
    Matrix4d A;
    Vector4d x, y;

    for (int i=0; i<3; i++) {
        A(0, i) = pow(x0, 3-i);
        A(1, i) = pow(x1, 3-i);
        A(2, i) = (3-i)*pow(x0,2-i);
        A(3, i) = (3-i)*pow(x1,2-i);
    }
    A(0, 3) = 1;
    A(1, 3) = 1;
    A(2, 3) = 0;
    A(3, 3) = 0;


    y(0) = y0;
    y(1) = y1;
    y(2) = dy0;
    y(3) = dy1;

    x = A.inverse() * y;

    //vector<double> coefficients(y.data(), y.data() + y.size());
    //return coefficients;

    return x;
}

// given x coordinates of path and the coefficients of the cubic function, and phi, find the cubic path
vector<vector<double> > Path::CubicSplinePath(vector<double> & x, Vector4d & coeff, double & phi) {
    vector<vector<double> > path;
    vector<double> coord;
    Vector4d argument;
    double y, xTmp, yTmp;
    for(unsigned int i = 0; i<x.size(); i++) {
        argument = Vector4d(pow(x[i],3), pow(x[i], 2), x[i], 1);
        yTmp = coeff.dot(argument);
        xTmp = x[i];
        x[i] = xTmp * cos(phi) - yTmp * sin(phi);
        y = xTmp * sin(phi) + yTmp * cos(phi);
        coord.push_back(x[i]);
        coord.push_back(y);
        path.push_back(coord);
        coord.clear();
    }

    return path;
}


// x = x positions, y0 = (entry y, entry angle), y1 = (exit y, exit angle), phi = rotation angle
vector<vector<double> > Path::mlp(vector<double> & x, Vector2d & y0, Vector2d & y1, double & phi) {
    vector<vector<double> > path;
    vector<double> coord;
    vector<double> a(6); // coefficient of polynomial for 1/beta^2 p^2
    a[0] = 7.436128800432859*pow(10,-6);
    a[1] = 5.019911854542805*pow(10,-7) / 10.0;
    a[2] = -7.807127329887146*pow(10,-8) / pow(10.0,2);
    a[3] = 1.58604114807080*pow(10,-8) / pow(10.0,3);
    a[4] = -1.09118882708652*pow(10,-9)/ pow(10.0,4);
    a[5] = 3.018453764279442*pow(10,-11)/ pow(10.0,5);
    double X0 = 361; // radiation length, mm
    double E0 = 1.36; // MeV/mm


    auto fst = [&X0, &E0, &a](double & u1, double & u0) -> double {
        double out = (pow(E0,2) * pow(1 + 0.038*log((u1 - u0)/X0),2)/X0) *
        ( (1/1.0) * (pow(u1,2)*a[0]   ) * (u1 - u0)
        + (1/2.0) * (pow(u1,2)*a[1] - 2*u1*a[0]) * (pow(u1,2) - pow(u0,2))
        + (1/3.0) * (pow(u1,2)*a[2] - 2*u1*a[1] + a[0]) * (pow(u1,3) - pow(u0,3))
        + (1/4.0) * (pow(u1,2)*a[3] - 2*u1*a[2] + a[1]) * (pow(u1,4) - pow(u0,4))
        + (1/5.0) * (pow(u1,2)*a[4] - 2*u1*a[3] + a[2]) * (pow(u1,5) - pow(u0,5))
        + (1/6.0) * (pow(u1,2)*a[5] - 2*u1*a[4] + a[3]) * (pow(u1,6) - pow(u0,6))
        + (1/7.0) * ( - 2 * u1 * a[5] + a[4]) * (pow(u1,7) - pow(u0,7))
        + (1/8.0) * (+ a[5]) * (pow(u1,8) - pow(u0,8)) ) ;
        return out;
    };

    auto fsth = [&X0, &E0, &a](double & u1, double & u0) -> double {
        double out = (pow(E0,2) * pow(1 + 0.038*log((u1-u0)/X0),2)/X0 ) *
        ( a[0] * (u1-u0) + a[1] * (pow(u1,2) - pow(u0,2))/2.0 + a[2] * (pow(u1,3) - pow(u0,3))/3.0
        + a[3] * (pow(u1,4) - pow(u0,4))/4.0 + a[4] * (pow(u1,5) - pow(u0,5))/5.0
        + a[5] * (pow(u1,6) - pow(u0,6))/6.0 );
        return out;
    };

    auto fstth = [&X0, &E0, &a](double & u1, double & u0) -> double {
        double out = (pow(E0,2) * pow(1 + 0.038*log((u1 - u0)/X0),2)/X0) *
        ( (1/1.0) * u1 * a[0] * (u1 - u0)
        + (1/2.0) * (u1 * a[1] - a[0]) * (pow(u1,2) - pow(u0,2))
        + (1/3.0) * (u1 * a[2] - a[1]) * (pow(u1,3) - pow(u0,3))
        + (1/4.0) * (u1 * a[3] - a[2]) * (pow(u1,4) - pow(u0,4))
        + (1/5.0) * (u1 * a[4] - a[3]) * (pow(u1,5) - pow(u0,5))
        + (1/6.0) * (u1 * a[5] - a[4]) * (pow(u1,6) - pow(u0,6))
        + (1/7.0) * (- a[5]) * (pow(u1,7) - pow(u0,7)) );
        return out;
    };

    vector<Vector2d> ymlp(x.size());
    ymlp[0] = y0;
    ymlp.back() = y1;
    double st1, sth1, stth1, st2, sth2, stth2;
    double yTmp;
    double xTmp;
    Matrix2d SIG1, SIG2, R0, R1;
    double xl, xEnd;

    // add ellipse intersection point first
    double xShift = x[0];
    xTmp = x[0];// - x[0]; // shifting so that x[0] = 0;
    yTmp = ymlp[0](0);
    x[0] = xTmp * cos(phi) - yTmp * sin(phi);
    ymlp[0](0) = xTmp * sin(phi) + yTmp * cos(phi);
    coord.push_back(x[0]);
    coord.push_back(ymlp[0](0));
    path.push_back(coord);
    coord.clear();
    double u0 = 0;

    for(unsigned int i=1; i < x.size()-1; i++) {
        xl = (x[i]-xShift); // xl like u1(i), x[0] like u0 (= 0), xEnd = u2
        xEnd = x.back() + abs(xShift);

        st1 = fst(xl, u0);
        sth1 = fsth(xl, u0);
        stth1 = fstth(xl, u0);

        st2 = fst(xEnd, xl);
        sth2 = fsth(xEnd, xl);
        stth2 = fstth(xEnd, xl);

        SIG1 << st1, stth1,
                stth1, sth1;

        SIG2 << st2, stth2,
                stth2, sth2;

        R0 << 1, xl,
              0, 1;

        R1 << 1, xEnd - xl,
              0, 1;


        //cout << "SIG1: " <<SIG1 << endl;
        //cout << "xl: " << xl << ", u0: " << u0 << " x[i]: "  << x[i] <<  " x[0]: " << x[0] << " xTmp: " << xTmp << endl;

        Matrix2d firstTerm;
        Vector2d secondTerm, thirdTerm;
        firstTerm = SIG1.inverse() + R1.transpose() * SIG2.inverse() * R1;
        secondTerm = (SIG1.inverse() * R0) * y0;
        thirdTerm = R1.transpose() * (SIG2.inverse() * y1);

        ymlp[i] = firstTerm.inverse() * (secondTerm + thirdTerm);

        xTmp = x[i];// - x[0]; // shifting so that x[0] = 0;
        yTmp = ymlp[i](0);
        x[i] = xTmp * cos(phi) - yTmp * sin(phi);
        ymlp[i](0) = xTmp * sin(phi) + yTmp * cos(phi);
        coord.push_back(x[i]);
        coord.push_back(ymlp[i](0));
        path.push_back(coord);
        coord.clear();
    }
    // add ellipse intersection point last
    xTmp = x.back();// - x[0]; // shifting so that x[0] = 0;
    yTmp = ymlp.back()(0);
    x.back() = xTmp * cos(phi) - yTmp * sin(phi);
    ymlp.back()(0) = xTmp * sin(phi) + yTmp * cos(phi);
    coord.push_back(x.back());
    coord.push_back(ymlp.back()(0));
    path.push_back(coord);
    coord.clear();

    return path;


}

// check where the intersection with an ellipse occurs
// return a vector with intersection1, intersection2, and some kind of bool
vector<vector<double> > Path::ellipseIntersect(vector<double> & A, double & entryAngle, vector<double> & B, double & exitAngle, double & phi) {
    vector<vector<double> > intersect;
    vector<double> p0;
    vector<double> p1;
    //double a = 70, b = 80;
    double a = 100, b = 100;
    vector<double> dirVecA, dirVecB;
    dirVecA.push_back(cos(entryAngle));
    dirVecA.push_back(sin(entryAngle));
    dirVecB.push_back(cos(exitAngle));
    dirVecB.push_back(sin(exitAngle));
    double cosphia2, cosphib2, sinphia2, sinphib2;
    cosphia2 = pow(cos(phi)/a,2);
    cosphib2 = pow(cos(phi)/b,2);
    sinphia2 = pow(sin(phi)/a,2);
    sinphib2 = pow(sin(phi)/b,2);

    double a1, a2, a3, x1, y1, x2, y2;

    a1 = pow(dirVecA[0],2) * ( cosphia2 + sinphib2 ) + pow(dirVecA[1],2) * (sinphia2 + cosphib2)
        - 2 * cos(phi) * sin(phi) * (1.0/pow(a, 2) - 1.0/pow(b,2)) * dirVecA[0] * dirVecA[1];

    a2 = 2 * A[0] * dirVecA[0] * (cosphia2 + sinphib2) + 2 * A[1] * dirVecA[1] * (sinphia2 + cosphib2)
        - 2 * cos(phi) * sin(phi) * (1.0/pow(a,2) - 1.0/pow(b,2)) * (A[0] * dirVecA[1] + dirVecA[0] * A[1]);

    a3 = pow(A[0],2) * ( cosphia2 + sinphib2) + pow(A[1],2) * (sinphia2 + cosphib2) - 2 * cos(phi) * sin(phi)
        * (1.0/pow(a,2) - 1.0/pow(b,2)) * A[0] * A[1] - 1;

    vector<complex<double> > rts;
    rts = quadraticSolver(a1, a2, a3);
    //check that all roots are real
    bool allReal;
    allReal = std::all_of(rts.begin(), rts.end(), [](complex<double> i) { return (i.imag() == 0);} );

    bool allMoreThanZero;
    allMoreThanZero = std::all_of(rts.begin(), rts.end(), [](complex<double> i) { return (i.real() > 0);});

    double d;
    if (allReal && allMoreThanZero) {
        //std::max_element(rts.begin(), rts.end(), real_less); <- don't know why this had a problem!
        d = (*std::min_element(rts.begin(), rts.end(), [](complex<double> const & lhs, complex<double> const & rhs) {return lhs.real() < rhs.real();})).real();
        x1 = A[0] + dirVecA[0] * d;
        y1 = A[1] + dirVecA[1] * d;
        p0.push_back(x1);
        p0.push_back(y1);
    } else {
        return intersect;
    }

    // computations for output line

    a1 = pow(dirVecB[0],2) * ( cosphia2 + sinphib2 ) + pow(dirVecB[1],2) * (sinphia2 + cosphib2)
        - 2 * cos(phi) * sin(phi) * (1.0/pow(a, 2) - 1.0/pow(b,2)) * dirVecB[0] * dirVecB[1];

    a2 = 2 * B[0] * dirVecB[0] * (cosphia2 + sinphib2) + 2 * B[1] * dirVecB[1] * (sinphia2 + cosphib2)
        - 2 * cos(phi) * sin(phi) * (1.0/pow(a,2) - 1.0/pow(b,2)) * (B[0] * dirVecB[1] + dirVecB[0] * B[1]);

    a3 = pow(B[0],2) * ( cosphia2 + sinphib2) + pow(B[1],2) * (sinphia2 + cosphib2) - 2 * cos(phi) * sin(phi)
        * (1.0/pow(a,2) - 1.0/pow(b,2)) * B[0] * B[1] - 1;

    rts.clear();
    rts = quadraticSolver(a1, a2, a3);

    //check that all roots are real
    allReal = std::all_of(rts.begin(), rts.end(), [](complex<double> i) { return (i.imag() == 0);} );

    double allLessThanZero = std::all_of(rts.begin(), rts.end(), [](complex<double> i) { return (i.real() < 0);});

    if (allReal && allLessThanZero) {
        //std::max_element(rts.begin(), rts.end(), real_less); <- don't know why this had a problem!
        d = (*std::max_element(rts.begin(), rts.end(), [](complex<double> const & lhs, complex<double> const & rhs) {return lhs.real() < rhs.real();})).real();
        x2 = B[0] + dirVecB[0] * d;
        y2 = B[1] + dirVecB[1] * d;
        p1.push_back(x2);
        p1.push_back(y2);
    } else {
        return intersect;
    }

    if (x2 <= x1) {
        return intersect;
    }
    intersect.push_back(p0);
    intersect.push_back(p1);
    return intersect;

}

//work in progress - find intersection wiht a circle
vector<vector<double> > Path::circleIntersect(vector<double> & A, double & entryAngle, vector<double> & B, double & exitAngle, double & phi) {
    vector<vector<double> > intersect;
    vector<double> p0;
    vector<double> p1;
    //double a = 70, b = 80;
    double r = 100;

    double x1, y1, x2, y2;
    vector<double> dirVecA, dirVecB;
    dirVecA.push_back(cos(entryAngle));
    dirVecA.push_back(sin(entryAngle));
    dirVecB.push_back(cos(exitAngle));
    dirVecB.push_back(sin(exitAngle));

    double m1 = dirVecA[1]/dirVecA[0];
    double m2 = dirVecB[1]/dirVecB[0];
    double c1 = A[1] - A[0]*m1;
    double c2 = B[1] - B[0]*m2;

    double a = m1*m1 + 1;
    double b = 2 * (m1 * c1);
    double c = c1*c1 - r*r;

    vector<complex<double> > rts;
    rts = quadraticSolver(a, b, c);
    for (int i = 0; i < rts.size(); i++) {
        cout << rts[i] << endl;
    }
    //check that all roots are real
    bool allReal;
    allReal = std::all_of(rts.begin(), rts.end(), [](complex<double> i) { return (i.imag() == 0);} );

    bool allMoreThanZero;
    allMoreThanZero = std::all_of(rts.begin(), rts.end(), [](complex<double> i) { return (i.real() > 0);});

    double d;
    if (allReal && allMoreThanZero) {
        //std::max_element(rts.begin(), rts.end(), real_less); <- don't know why this had a problem!
        d = (*std::min_element(rts.begin(), rts.end(), [](complex<double> const & lhs, complex<double> const & rhs) {return lhs.real() < rhs.real();})).real();
        x1 = A[0] + dirVecA[0] * d;
        y1 = A[1] + dirVecA[1] * d;
        p0.push_back(x1);
        p0.push_back(y1);
    } else {
        return intersect;
    }

    a = m2*m2 + 1;
    b = 2 * (m2 * c2);
    c = c2*c2 - r*r;

    rts.clear();
    rts = quadraticSolver(a, b, c);

    //check that all roots are real
    allReal = std::all_of(rts.begin(), rts.end(), [](complex<double> i) { return (i.imag() == 0);} );

    double allLessThanZero = std::all_of(rts.begin(), rts.end(), [](complex<double> i) { return (i.real() < 0);});

    if (allReal && allLessThanZero) {
        //std::max_element(rts.begin(), rts.end(), real_less); <- don't know why this had a problem!
        d = (*std::max_element(rts.begin(), rts.end(), [](complex<double> const & lhs, complex<double> const & rhs) {return lhs.real() < rhs.real();})).real();
        x2 = B[0] + dirVecB[0] * d;
        y2 = B[1] + dirVecB[1] * d;
        p1.push_back(x2);
        p1.push_back(y2);
    } else {
        return intersect;
    }

    if (x2 <= x1) {
        return intersect;
    }
    intersect.push_back(p0);
    intersect.push_back(p1);
    return intersect;


}

// find quadratic coefficients
vector<complex<double> > Path::quadraticSolver(double & a, double & b, double & c) {

    vector<complex<double> > roots;
    complex<double> sol1(0,0), sol2(0,0);
    double discriminant = pow(b,2) - 4 * a * c;
    if (discriminant >= 0) {
        sol1.real( (-b + sqrt(discriminant))/(2*a) );
        sol2.real( (-b - sqrt(discriminant))/(2*a) );
    } else {
        sol1.real(-b/(2*a));
        sol2.real(-b/(2*a));
        sol1.imag( (sqrt(-1 * discriminant))/(2*a) );
        sol2.imag( -1 * (sqrt(-1 * discriminant))/(2*a) );
    }

    roots.push_back(sol1);
    roots.push_back(sol2);
    return roots;

}

