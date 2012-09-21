/* 
 * File:   functionalcomputation_kdltypes.hpp
 * Author: azamat
 *
 * Created on September 7, 2012, 3:27 PM
 */

#ifndef FUNCTIONALCOMPUTATION_KDLTYPES_HPP
#define	FUNCTIONALCOMPUTATION_KDLTYPES_HPP

#include <kdl/tree.hpp>
#include <kdl_extensions/treeid_vereshchagin_composable.hpp>
#include <kdl_extensions/functionalcomputation.hpp>

namespace kdl_extensions
{
//operation tags

class poseOperationTag
{
};

class twistOperationTag : public poseOperationTag
{
};

class accelerationTwistOperationTag : public twistOperationTag
{
};

class wrenchOperationTag : public accelerationTwistOperationTag
{
};

class inertiaOperationTag
{
};

typedef poseOperationTag pose;
typedef twistOperationTag twist;
typedef accelerationTwistOperationTag accTwist;
typedef wrenchOperationTag wrench;
typedef inertiaOperationTag inertia;

typedef std::map<std::string, KDL::TreeElement >::const_iterator tree_iterator;
typedef std::vector<KDL::Segment>::const_iterator chain_iterator;

template<typename Iterator, typename OperationTagT>
class transform;

template<>
class transform<tree_iterator, pose>
{
public:

    enum
    {
        NumberOfParams = 3
    };
    typedef KDL::SegmentState ReturnType;
    typedef tree_iterator Param1T;
    typedef KDL::JointState Param2T;
    typedef KDL::SegmentState Param3T;

    inline ReturnType operator()(Param1T segmentId, Param2T p_jointState, Param3T p_segmentState)
    {
        //check for joint type None should be tree serialization function.
        //a_segmentState.X =  a_p3.X * segmentId->second.segment.pose(p_jointState.q); //in base coordinates
        a_segmentState.Xdot = p_segmentState.Xdot;
        a_segmentState.Xdotdot = p_segmentState.Xdotdot;
        a_segmentState.X = segmentId->second.segment.pose(p_jointState.q);
        a_segmentState.jointIndex = p_jointState.jointIndex;
        a_segmentState.jointName = p_jointState.jointName;
        a_segmentState.segmentName = segmentId->first;
        std::cout << "Inside transformPose 0" << a_segmentState.X << std::endl;
        return a_segmentState;

    };
private:
    ReturnType a_segmentState;

};

template<>
class transform<chain_iterator, pose>
{
public:

    enum
    {
        NumberOfParams = 3
    };
    typedef KDL::SegmentState ReturnType;
    typedef chain_iterator Param1T;
    typedef KDL::JointState Param2T;
    typedef KDL::SegmentState Param3T;

    inline ReturnType operator()(Param1T segmentId, Param2T p_jointState, Param3T p_segmentState)
    {
        //check for joint type None should be tree serialization function.
        //a_segmentState.X =  a_p3.X * segmentId->second.segment.pose(p_jointState.q); //in base coordinates
        a_segmentState.Xdot = p_segmentState.Xdot;
        a_segmentState.Xdotdot = p_segmentState.Xdotdot;
        a_segmentState.X = segmentId->pose(p_jointState.q);
        a_segmentState.jointIndex = p_jointState.jointIndex;
        a_segmentState.jointName = p_jointState.jointName;
        a_segmentState.segmentName = segmentId->getName();
        std::cout << "Inside transformPose 0" << a_segmentState.X << std::endl;
        return a_segmentState;

    };
private:
    ReturnType a_segmentState;

};

template<>
class transform<tree_iterator, twist>
{
public:

    enum
    {
        NumberOfParams = 3
    };
    typedef KDL::SegmentState ReturnType;
    typedef tree_iterator Param1T;
    typedef KDL::JointState Param2T;
    typedef KDL::SegmentState Param3T;

    inline ReturnType operator()(Param1T segmentId, Param2T p_jointState, Param3T p_segmentState)
    {
        a_segmentState.X = p_segmentState.X;
        a_segmentState.Xdotdot = p_segmentState.Xdotdot;
        a_segmentState.Z = a_segmentState.X.M.Inverse(segmentId->second.segment.twist(p_jointState.q, 1.0));
        //a_segmentState.Z = a_jointUnitTwist;
        a_segmentState.Vj = a_segmentState.X.M.Inverse(segmentId->second.segment.twist(p_jointState.q, p_jointState.qdot));
        //a_segmentState.Vj = a_jointTwistVelocity;

        //do we check here for the index of a joint (whether the joint is first in the chain)
        a_segmentState.Xdot = a_segmentState.X.Inverse(p_segmentState.Xdot) + a_segmentState.Vj;

        return a_segmentState;
    };
private:
    ReturnType a_segmentState;
};

template<>
class transform<chain_iterator, twist>
{
public:

    enum
    {
        NumberOfParams = 3
    };
    typedef KDL::SegmentState ReturnType;
    typedef chain_iterator Param1T;
    typedef KDL::JointState Param2T;
    typedef KDL::SegmentState Param3T;

    inline ReturnType operator()(Param1T segmentId, Param2T p_jointState, Param3T p_segmentState)
    {
        a_segmentState.X = p_segmentState.X;
        a_segmentState.Xdotdot = p_segmentState.Xdotdot;
        a_segmentState.Z = a_segmentState.X.M.Inverse(segmentId->twist(p_jointState.q, 1.0));
        //a_segmentState.Z = a_jointUnitTwist;
        a_segmentState.Vj = a_segmentState.X.M.Inverse(segmentId->twist(p_jointState.q, p_jointState.qdot));
        //a_segmentState.Vj = a_jointTwistVelocity;

        //do we check here for the index of a joint (whether the joint is first in the chain)
        a_segmentState.Xdot = a_segmentState.X.Inverse(p_segmentState.Xdot) + a_segmentState.Vj;

        return a_segmentState;
    };
private:
    ReturnType a_segmentState;
};

template<>
class transform<tree_iterator, accTwist>
{
public:

    enum
    {
        NumberOfParams = 3
    };
    typedef KDL::SegmentState ReturnType;
    typedef tree_iterator Param1T;
    typedef KDL::JointState Param2T;
    typedef KDL::SegmentState Param3T;

    inline ReturnType operator()(Param1T segmentId, Param2T p_jointState, Param3T p_segmentState)
    {
        a_segmentState = p_segmentState;
        a_segmentState.Xdotdot = p_segmentState.X.Inverse(p_segmentState.Xdotdot) + p_segmentState.Z * p_jointState.qdotdot + p_segmentState.Xdot * p_segmentState.Vj;
        return a_segmentState;
    };
private:
    ReturnType a_segmentState;

};

template<>
class transform<chain_iterator, accTwist>
{
public:

    enum
    {
        NumberOfParams = 3
    };
    typedef KDL::SegmentState ReturnType;
    typedef chain_iterator Param1T;
    typedef KDL::JointState Param2T;
    typedef KDL::SegmentState Param3T;

    inline ReturnType operator()(Param1T segmentId, Param2T p_jointState, Param3T p_segmentState)
    {
        a_segmentState = p_segmentState;
        a_segmentState.Xdotdot = p_segmentState.X.Inverse(p_segmentState.Xdotdot) + p_segmentState.Z * p_jointState.qdotdot + p_segmentState.Xdot * p_segmentState.Vj;
        return a_segmentState;
    };
private:
    ReturnType a_segmentState;

};

template<typename Iterator, typename OperationTagT>
class project;

template<>
class project<chain_iterator, wrench>
{
public:


};

template<>
class project<tree_iterator, wrench>
{
public:
enum
    {
        NumberOfParams = 3
    };
    typedef KDL::SegmentState ReturnType;
    typedef tree_iterator Param1T;
    typedef KDL::JointState Param2T;
    typedef KDL::SegmentState Param3T;

    inline ReturnType operator()(Param1T segmentId, Param2T p_jointState, Param3T p_segmentState)
    {
       // return ;
    };


};

template<>
class project<chain_iterator, inertia>
{
public:
    enum
    {
        NumberOfParams = 3
    };
    typedef KDL::RigidBodyInertia ReturnType;
    typedef chain_iterator Param1T;
    typedef KDL::JointState Param2T;
    typedef KDL::SegmentState Param3T;

    inline ReturnType operator()(Param1T segmentId, Param2T p_jointState, Param3T p_segmentState)
    {
        return segmentId->getInertia();
    };

};

template<>
class project<tree_iterator, inertia>
{
public:

    enum
    {
        NumberOfParams = 3
    };
    typedef KDL::RigidBodyInertia ReturnType;
    typedef tree_iterator Param1T;
    typedef KDL::JointState Param2T;
    typedef KDL::SegmentState Param3T;

    inline ReturnType operator()(Param1T segmentId, Param2T p_jointState, Param3T p_segmentState)
    {
        return segmentId->second.segment.getInertia();
    };
};

template<>
class DFSPolicy<KDL::Chain>
{
public:

    DFSPolicy()
    {
    };

    ~DFSPolicy()
    {
    };

    template <typename OP>
    inline static bool walk(KDL::Chain a_topology, std::vector<typename OP::Param2T>& a_jointStateVectorIn, std::vector<typename OP::Param3T>& a_linkStateVectorIn,
                            std::vector<typename OP::Param3T>& a_linkStateVectorOut, OP a_op)
    {
        return true;
    };

};

template<>
class DFSPolicy<KDL::Tree>
{
public:

    DFSPolicy()
    {
    };

    ~DFSPolicy()
    {
    };

    template <typename OP>
    inline static bool walk(KDL::Tree a_topology, std::vector<typename OP::Param2T> a_jointStateVectorIn, std::vector<typename OP::Param3T> a_linkStateVectorIn,
                            std::vector<typename OP::Param3T> a_linkStateVectorOut, OP a_op)
    {
        //just a simple test, will implement DFS algorithm
        for (KDL::SegmentMap::const_iterator iter = a_topology.getSegments().begin(); iter != a_topology.getSegments().end(); ++iter)
        {
            a_op(iter, a_jointStateVectorIn[0], a_linkStateVectorIn[0]);
        };
        return true;
    };

};

template<>
class BFSPolicy<KDL::Tree>
{
public:

    BFSPolicy()
    {
    };

    ~BFSPolicy()
    {
    };

    template <typename OP>
    inline static bool walk(KDL::Tree a_topology, std::vector<typename OP::Param2T> a_jointStateVectorIn, std::vector<typename OP::Param3T> a_linkStateVectorIn,
                            std::vector<typename OP::Param3T> a_linkStateVectorOut, OP a_op)
    {
        //just a simple test, will implement DFS algorithm
        for (KDL::SegmentMap::const_iterator iter = a_topology.getSegments().begin(); iter != a_topology.getSegments().end(); ++iter)
        {
            a_op(iter, a_jointStateVectorIn[0], a_linkStateVectorIn[0]);
        };
        return true;
    };

};

};

#include "../../src/functionalcomputation_kdltypes.inl"
#endif	/* FUNCTIONALCOMPUTATION_KDLTYPES_HPP */
