// Copyright  (C)  2009

// Version: 1.0
// Author:
// Maintainer:
// URL: http://www.orocos.org/kdl

// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef KDL_CHAIN_IDSOLVER_VERESHCHAGIN_HPP
#define KDL_CHAIN_IDSOLVER_VERESHCHAGIN_HPP

#include "chainidsolver.hpp"
#include "frames.hpp"
#include "articulatedbodyinertia.hpp"

namespace KDL {
    /**
     * \brief Dynamics calculations by constraints based on Vereshchagin 1989.
     *
     * For a chain
     */
    typedef std::vector<Twist> Twists;
    typedef std::vector<Frame> Frames;

    typedef Eigen::Matrix<double, 6, 1 > Vector6d;
    typedef Eigen::Matrix<double, 6, 6 > Matrix6d;
    typedef Eigen::Matrix<double, 6, Eigen::Dynamic> Matrix6Xd;

    class ChainIdSolver_Constraint_Vereshchagin {
    public:
        /**
         * Constructor for the solver, it will allocate all the necessary memory
         * \param chain The kinematic chain to calculate the inverse dynamics for, an internal copy will be made.
         * \param root_acc The acceleration vector of the root to use during the calculation.(most likely contains gravity)
         *
         */
        ChainIdSolver_Constraint_Vereshchagin(const Chain& chain, Twist root_acc, unsigned int nc);

        ~ChainIdSolver_Constraint_Vereshchagin() {
        };

        /**
         * Function to calculate from Cartesian forces to joint torques.
         * Input parameters;
         * \param q The current joint positions
         * \param q_dot The current joint velocities
         * \param f_ext The external forces (no gravity) on the segments
         * Output parameters:
         * \param q_dotdot The joint accelerations
         * \param torques the resulting torques for the joints
         */
        int CartToJnt(const JntArray &q, const JntArray &q_dot, JntArray &q_dotdot, Twists& x_dotdot, Jacobian& alfa, const JntArray& beta, const Wrenches& f_ext, JntArray &torques);
        //Returns cartesian positions of links in base coordinates
        void getLinkCartesianPose(Frames& x_base);
        //Returns cartesian postions of links in link coordinates
        void getLinkPose(Frames& x_local);
        //Returns cartesian velocities of links in link coordinates
        void getLinkCartesianVelocity(Twists& xDot_base);
        //Acceleration energy due to unit constraint forces at the end-effector
        void getLinkUnitForceAccelerationEnergy(Eigen::MatrixXd& M);
        //Acceleration energy due to arm configuration: bias force plus input joint torques
        void getLinkBiasForceAcceleratoinEnergy(Eigen::VectorXd& G);

        void getLinkUnitForceMatrix(Matrix6Xd& E_tilde);

        void getLinkBiasForceMatrix(Wrenches& R_tilde);

        void getJointBiasAcceleration(JntArray &bias_q_dotdot);

        void getJointConstraintAcceleration(JntArray &constraint_q_dotdot);

        void getJointNullSpaceAcceleration(JntArray &nullspace_q_dotdot);


    private:
        //Functions to calculate velocity, propagated inertia, propagated bias forces, constraint forces and accelerations
        void initial_upwards_sweep(const JntArray &q, const JntArray &q_dot, const JntArray &q_dotdot, const Wrenches& f_ext);
        void downwards_sweep(Jacobian& alfa, JntArray& torques);
        void constraint_calculation(const JntArray& beta);
        void final_upwards_sweep(JntArray &q_dotdot, Twists& x_dotdot, JntArray &torques);

        Chain chain;
        unsigned int nj;
        unsigned int ns;
        unsigned int nc;
        Twist acc_root;
        Jacobian alfa_N;
        Jacobian alfa_N2;
        Eigen::MatrixXd M_0_inverse;
        Eigen::MatrixXd Um;
        Eigen::MatrixXd Vm;
        JntArray beta_N;
        Eigen::VectorXd nu;
        Eigen::VectorXd nu_sum;
        Eigen::VectorXd Sm;
        Eigen::VectorXd tmpm;
        Wrench qdotdot_sum;
        Frame F_total;

        struct segment_info {
            Frame F; //local pose with respect to previous link in segments coordinates
            Frame F_base; // pose of a segment in root coordinates
            Twist Z; //Unit twist
            Twist v; //twist
            Twist acc; //acceleration twist
            Wrench U; //wrench p of the bias forces (in cartesian space)
            Wrench R; //wrench p of the bias forces
            Wrench R_tilde; //vector of wrench p of the bias forces (new) in matrix form
            Twist C; //constraint
            Twist A; //constraint
            ArticulatedBodyInertia H; //I (expressed in 6*6 matrix)
            ArticulatedBodyInertia P; //I (expressed in 6*6 matrix)
            ArticulatedBodyInertia P_tilde; //I (expressed in 6*6 matrix)
            Wrench PZ; //vector U[i] = I_A[i]*S[i]
            Wrench PC; //vector E[i] = I_A[i]*c[i]
            double D; //vector D[i] = S[i]^T*U[i]
            Matrix6Xd E; //matrix with virtual unit constraint force due to acceleration constraints
            Matrix6Xd E_tilde;
            Eigen::MatrixXd M; //acceleration energy already generated at link i
            Eigen::VectorXd G; //magnitude of the constraint forces already generated at link i
            Eigen::VectorXd EZ; //K[i] = Etiltde'*Z


            double nullspaceAccComp; //Azamat: constribution of joint space u[i] forces to joint space acceleration
            double constAccComp; //Azamat: constribution of joint space constraint forces to joint space acceleration
            double biasAccComp; //Azamat: constribution of joint space bias forces to joint space acceleration

            double totalBias; //Azamat: R+PC (centrepital+coriolis) in joint subspace
            double u; //vector u[i] = torques(i) - S[i]^T*(p_A[i] + I_A[i]*C[i]) in joint subspace. Azamat: In code u[i] = torques(i) - s[i].totalBias

            segment_info(unsigned int nc) {
                E.resize(6, nc);
                E_tilde.resize(6, nc);
                G.resize(nc);
                M.resize(nc, nc);
                EZ.resize(nc);
                E.setZero();
                E_tilde.setZero();
                M.setZero();
                G.setZero();
                EZ.setZero();
            };
        };

        std::vector<segment_info> results;

    };
}

#endif
