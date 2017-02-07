/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/*
------------Copyright (C) 2016 University of Strathclyde--------------
------------ e-mail: annalisa.riccardi@strath.ac.uk ------------------
------------ e-mail: carlos.ortega@strath.ac.uk ----------------------
--------- Author: Annalisa Riccardi and Carlos Ortega Absil ----------
*/


#ifndef SMARTMATH_TWOBODY_H
#define SMARTMATH_TWOBODY_H

#include "base_dynamics.h"
#include "../exception.h"

namespace smartmath
{
    namespace dynamics {

        /**
         * @brief The Two-body problem
         *
         * The Two-body problem models the dynamics of an object of mass \f$m\f$ orbiting around Earth is deifned as
         * \f{eqnarray*}{
           \ddot{\mathbf{x}} &=& -\frac{\mu}{r^3}\mathbf{x} + \frac{\mathbf{T}}{m} + \frac{1}{2}\rho \frac{C_D A}{m} \|\mathbf{v}_{rel}\| \mathbf{v}_{rel} + \mathbf{\epsilon}
           \f}
         * where \f$r\f$ is the distance from the Earth,  \f$\mathbf{v}_{rel}\f$ is the Earth relative velocity and the mass of the spacecraft varies as
            \f{eqnarray*}{\dot{m} = - \alpha \|\mathbf{T}\|\f}
         */
        template < class T >
        class twobody: public base_dynamics<T>
        {

        private:
            using base_dynamics<T>::m_name;

        public:

            /**
             * @brief twobody constructor
             *
             * The constructor initialize the problem parameters and scaling factors to the value supplied by the user.
             * The model parameters (10 in total) are supplied in order as:
             * - param 1-3: thrust value alng each direction
             * - param 4: \f$\alpha\f$
             * - param 5-6: \f$(\rho_0,H)\f$ atmospheric parameters (exponential atmospheric model is considered)
             * - param 7: \f$C_DA\f$ produc between aerodynamic coefficient and surface area
             * - param 8-10: unknown acceleration (3 components)
             * Default values for scaling factors are 1 and parameters is a vector of zero values or zero polynomials.
             * @param param vector of parameters (constants value or polynomials)
             * @param t_scale time scaling factor
             * @param r_scale position scaling factor
             */
            twobody(const std::vector<T> &param = std::vector<T>(10), const double &t_scale=1, const double &r_scale=1) : base_dynamics<T>("Two Body Problem"),
				m_param(param), m_t_scale(t_scale), m_r_scale(r_scale)
			{
				if(m_param.size()!=10)
					smartmath_throw(m_name+": the parameters list need to be of size 10");

			}


            /**
              * @brief ~twobody deconstructor
              */
            ~twobody(){}

            /**
             * @brief evaluate evaluate the dinamics of the Two-body problem at a given instant of time and a given state.
             *
             * Function to evaluate the dinamics of the Two-body problem at a given instant of time and a given state. It is a virtual function so any class that inherites from base_dynamics need to implement it.
             * @param[in] t time
             * @param[in] state state values at time t
             * @param[out] dstate derivative of the states at time t
             * @return
             */
            int evaluate(const double &t, const std::vector<T> &state, std::vector<T> &dstate) const
			{
				//sanity checks
				if(t<0)
					smartmath_throw(m_name+": negative time supplied in evaluation of the dynamical system");
				if(state.size()!=7)
					smartmath_throw(m_name+": the state dimension needs to be 7");

				dstate.clear();

				//constant parameters
				double radius_earth = 6378.0*pow(10,3) / m_r_scale;
				double mu_earth = 398600.4415*pow(10,9) / (pow(m_r_scale,3)/pow(m_t_scale,2));
				double omega_earth = 7.2921150*pow(10,-5) * m_t_scale;
				double H0_atmosphere = 900000.0 / m_r_scale;

				//precomputations
				T r = sqrt(state[0]*state[0]+state[1]*state[1]+state[2]*state[2]);
				T tmp_3D =  mu_earth/pow(r,3);

				//atmospheric model
				T rho = m_param[4]*exp(-(r-radius_earth-H0_atmosphere)/m_param[5]);

				//relative velocity
				T rel_v_x = state[3]-omega_earth*state[1];
				T rel_v_y = state[4]+omega_earth*state[0];
				T mod_rel_v = sqrt(rel_v_x*rel_v_x+rel_v_y*rel_v_y+state[5]*state[5]);

				//drag computation
				T tmp_drag = 0.5*rho*m_param[6]*mod_rel_v/state[6];

				dstate.push_back(state[3]); //dx/dt
				dstate.push_back(state[4]); //dy/dt
				dstate.push_back(state[5]); //dz/dt
				dstate.push_back(-tmp_3D*state[0]+m_param[0]/state[6]+m_param[7]-tmp_drag*rel_v_x); //dv_x/dt
				dstate.push_back(-tmp_3D*state[1]+m_param[1]/state[6]+m_param[8]-tmp_drag*rel_v_y); //dv_y/dt
				dstate.push_back(-tmp_3D*state[2]+m_param[2]/state[6]+m_param[9]-tmp_drag*state[5]); //dv_z/dt
				dstate.push_back(-m_param[3]*sqrt(m_param[0]*m_param[0]+m_param[1]*m_param[1]+m_param[2]*m_param[2])); //dm/dt

				return 0;

			}            

        private:
            mutable std::vector<T> m_param;
            double m_t_scale;
            double m_r_scale;
            double m_m_scale;

        };

    }
}

#endif // SMARTMATH_TWOBODY_H
