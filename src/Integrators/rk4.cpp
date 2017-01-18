/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/*
------------Copyright (C) 2016 University of Strathclyde--------------
------------ e-mail: annalisa.riccardi@strath.ac.uk ------------------
------------ e-mail: carlos.ortega@strath.ac.uk ----------------------
--------- Author: Annalisa Riccardi and Carlos Ortega Absil ----------
*/


#include "../../include/Integrators/rk4.h"


using namespace smartmath;
using namespace smartmath::integrator;

template < class T >
rk4<T>::rk4(const dynamics::base_dynamics<T> *dyn) : base_integrator<T>("Runge Kutta 4 fixed step time", dyn)
{
}

template < class T >
rk4<T>::~rk4(){

}

template < class T >
int rk4<T>::integrate(const double &ti, const double &tend, const int &nsteps, const std::vector<T> &x0, std::vector<T> &xfinal) const{

	xfinal.clear();

    unsigned int n = x0.size();
    std::vector<T> x(x0), xtemp(x0), k1(x0), k2(x0), k3(x0), k4(x0);

	double h = (tend-ti)/nsteps;

    for(int i=0; i<nsteps; i++){
		double t1, t2, t3, t4;
		t1 = ti + i*h;
		t2 = t1 + h/2.0;
		t3 = t1 + h/2.0;
		t4 = ti + (i+1)*h;

		//* Evaluate k1 = f(x).
		m_dyn->evaluate(t1, x, k1);

		//* Evaluate k2 = f(x+h/2*k1),
		for(int j=0; j<n; j++)
		    xtemp[j] = x[j]+k1[j]*h/2.0;
		m_dyn->evaluate(t2, xtemp, k2);

		//* Evaluate k3 = f(x+h/2*k2),
		for(int j=0; j<n; j++)
		    xtemp[j] = x[j]+k2[j]*h/2.0;
		m_dyn->evaluate(t3, xtemp, k3);

		//* Evaluate k4 = f(x+h*k3),
		for(int j=0; j<n; j++)
		    xtemp[j] = x[j]+k3[j]*h;
		m_dyn->evaluate(t4, xtemp, k4);

		//* Return x(t+h) computed from fourth-order Runge Kutta.
		for(int j=0; j<n; j++)
		    x[j] += (k1[j]+2.0*k2[j]+2.0*k3[j]+k4[j])*h/6.0;

	}

	for(int i=0; i<x0.size(); i++)
	    xfinal.push_back(x[i]);

	return 0;
}



template class rk4<double>;
template class rk4<float>;
template class rk4<long double>;
#ifdef ENABLE_SMARTUQ
template class rk4<smartuq::polynomial::chebyshev_polynomial<double> >;
template class rk4<smartuq::polynomial::chebyshev_polynomial<float> >;
template class rk4<smartuq::polynomial::chebyshev_polynomial<long double> >;
template class rk4<smartuq::polynomial::taylor_polynomial<double> >;
template class rk4<smartuq::polynomial::taylor_polynomial<float> >;
template class rk4<smartuq::polynomial::taylor_polynomial<long double> >;
#endif
