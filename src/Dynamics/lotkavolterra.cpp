/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/*
------------Copyright (C) 2016 University of Strathclyde--------------
------------ e-mail: annalisa.riccardi@strath.ac.uk ------------------
------------ e-mail: carlos.ortega@strath.ac.uk ----------------------
--------- Author: Annalisa Riccardi and Carlos Ortega Absil ----------
*/


#include "../../include/Dynamics/lotkavolterra.h"

using namespace smartmath;
using namespace dynamics;

template < class T >
lotkavolterra<T>::lotkavolterra(const std::vector<T> &param) : base_dynamics<T>("Lotka-Volterra dynamical system"), m_param(param)
{
    //sanity checks
    if(m_param.size()!=4)
        smart_throw(m_name+": the size of the parameters vector need to be 4");

}

template < class T >
lotkavolterra<T>::~lotkavolterra()
{

}

template < class T >
int lotkavolterra<T>::evaluate(const double &t, const std::vector<T> &state, std::vector<T> &dstate) const
{
    //sanity checks
    if(t<0)
        smart_throw(m_name+": negative time supplied in evaluation of the dynamical system");
    if(state.size()!=2)
        smart_throw(m_name+": the state dimension needs to be 2");

    dstate.clear();

    T xy = state[0]*state[1];
    dstate.push_back(m_param[0]*state[0]-m_param[1]*xy);
    dstate.push_back(-m_param[2]*state[1]+m_param[3]*xy);

    return 0;
}


template class lotkavolterra<double>;
template class lotkavolterra<float>;
template class lotkavolterra<long double>;
#ifdef ENABLE_SMARTUQ
template class lotkavolterra<polynomial::chebyshev_polynomial<double> >;
template class lotkavolterra<polynomial::chebyshev_polynomial<float> >;
template class lotkavolterra<polynomial::chebyshev_polynomial<long double> >;
template class lotkavolterra<polynomial::taylor_polynomial<double> >;
template class lotkavolterra<polynomial::taylor_polynomial<float> >;
template class lotkavolterra<polynomial::taylor_polynomial<long double> >;
#endif
