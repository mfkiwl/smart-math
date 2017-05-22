/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/*
------------Copyright (C) 2016 University of Strathclyde--------------
------------ e-mail: romain.serra@strath.ac.uk -----------------------
----------------- Author: Romain Serra -------------------------------
*/



#ifndef SMARTMATH_BASE_SYMPLECTIC_H
#define SMARTMATH_BASE_SYMPLECTIC_H

#include "../LinearAlgebra/Eigen/Core"
#include "../Dynamics/base_hamiltonian.h"
#include "../exception.h"

namespace smartmath
{
    namespace integrator {

        /**
         * @brief The base_integrator class is a template abstract class. Any sympletic integrator added to the toolbox needs to inherit from it
         *
         * The base_symplectic class is a template abstract class. Any symplectic integrator added to the toolbox needs to inherit from it
         */
        template < class T >
        class base_symplectic
        {

        public:

            /**
             * @brief base_integrator constructor
             *
             * The constructor initialize the name of the integrator, a pointer to the dynamical system to be integrated and the order of the integration
             * @param name integrator name
             * @param dyn pointer to a base_dynamics object
             * @param order integer stating the order of the integration scheme
             */
            base_symplectic(const std::string &name, const dynamics::base_hamiltonian<T> *dyn, const int &order): m_name(name), m_dyn(dyn), m_order(order){}

            /**
             * @brief ~base_symplectic deconstructor
             */
            virtual ~base_symplectic(){}

            /**
             * @brief integration_step performs one integration step from the symplectic integrator
             *
             * The method implements one step of a symplectic scheme to integrate with given initial time,
             * final time, initial state condition(constant stepsize)
             * @param[in] ti initial time instant
             * @param[in] tau time step
             * @param[in] x0 vector of initial states
             * @param[out] xfinal vector of final states
             * @return
             */
            int integration_step(const double &ti, const double &tau, const std::vector<T> &x0, std::vector<T> &xfinal) const{

                std::vector<T> q0, p0;
                int n = m_dyn->get_dim();
                for(int i = 0; i < n; i++){
                    q0.push_back(x0[i]);
                    p0.push_back(x0[i + n]);
                }
                std::vector<T> q = q0, p = p0, dq = q0, dp = p0;

                for(int j = 0; j < m_order; j++){
                    m_dyn->DHp(ti, q0, p0, dp);
                    for(int i = 0; i < n; i++)
                        q[i] += m_c[j] * tau * dp[i];
                    m_dyn->DHq(ti, q, p0, dq);
                    for(int i = 0; i < n; i++)
                        p[i] -= m_d[j] * tau * dq[i];
                    q0 = q;
                    p0 = p;
                }

                xfinal.clear();
                for(int i = 0; i < n; i++)
                    xfinal.push_back(q[i]);
                for(int i = 0; i < n; i++)
                    xfinal.push_back(p[i]);
                               
                return 0;
            }
            
            /**
             * @brief integrate method to integrate between two given time steps, initial condition and number of steps (saving intermediate states)
             *
             * The method implements a fixed-step symplectic scheme to integrate with given initial time,
             * final time, initial state condition and number of steps (constant stepsize)
             * @param[in] ti initial time instant
             * @param[in] tend final time instant
             * @param[in] nsteps number of integration steps
             * @param[in] x0 vector of initial states
             * @param[out] x_history vector of intermediate state vector (including final one)
             * @param[out] t_history vector of intermediate times (including final one)
             * @return
             */
            int integrate(const double &ti, const double &tend, const int &nsteps, const std::vector<T> &x0, std::vector<std::vector<T> > &x_history, std::vector<double> &t_history) const{

                t_history.clear();
                x_history.clear();

                std::vector<T> dx = x0, x = x0, x_temp = x0;

                double t = ti, h = (tend-ti) / nsteps;

                for(int i = 0; i < nsteps; i++){
                    integration_step(t, h, x, x_temp);
                    t += h;
                    x = x_temp;
                    t_history.push_back(t);
                    x_history.push_back(x);
                }

                return 0;
            }
            
            /**
             * @brief integrate method to integrate bewteen two given time steps, initial condition and step lenght
             *
             * The method implements the corresponding integration scheme with given initial time,
             * final time, initial state condition and number of steps (constant stepsize)
             * @param[in] ti initial time instant
             * @param[in] tend final time instant
             * @param[in] nsteps number of integration steps
             * @param[in] x0 vector of initial states
             * @param[out] xfinal vector of final states
             * @return
             */
            int integrate(const double &ti, const double &tend, const int &nsteps, const std::vector<T> &x0, std::vector<T> &xfinal) const{

                std::vector<std::vector<T> > x_history;
                std::vector<double> t_history;

                integrate(ti,tend,nsteps,x0,x_history,t_history);

                xfinal=x_history.back();

                return 0;
            }

        protected:

            std::string m_name;
            const dynamics::base_hamiltonian<T> *m_dyn;
            int m_order;
            std::vector<double> m_c, m_d; // coefficients for integration step

        };
    }
}

#endif // SMARTMATH_BASE_SYMPLECTIC_H
