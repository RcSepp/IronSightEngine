/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include "GameUtils.h"

/*#include <Common/Base/Reflection/hkClass.h>
#include <Common/Base/System/Io/IStream/hkIStream.h>
#include <Common/Base/System/Io/OStream/hkOStream.h>
#include <Common/Base/System/Io/Reader/hkStreamReader.h>

#include <Common/Serialize/Util/hkSerializeUtil.h>

#include <Common/Base/Reflection/Registry/hkTypeInfoRegistry.h>
#include <Common/Base/Reflection/Registry/hkVtableClassRegistry.h>*/

// We've got two type of geometries here, spheres and boxes
#include <Common/Base/Types/Geometry/Sphere/hkSphere.h>
#include <Common/Base/Types/Geometry/hkGeometry.h>

// This makes sure that we get a set of verticies that define a convex hull
#include <Common/Internal/ConvexHull/hkGeometryUtility.h>

#include <Common/Base/Algorithm/PseudoRandom/hkPseudoRandomGenerator.h>

#include <Common/SceneData/Scene/hkxScene.h>
#include <Common/SceneData/SceneDataToGeometryConverter/hkxSceneDataToGeometryConverter.h>

#include <Physics/Collide/Filter/Group/hkpGroupFilter.h>
#include <Physics/Collide/Query/CastUtil/hkpWorldRayCastInput.h>
#include <Physics/Collide/Query/CastUtil/hkpWorldRayCastOutput.h>

// Stuff to create MOPP from TK file.
#include <Physics/Collide/Shape/Compound/Tree/Mopp/hkpMoppCompilerInput.h>
#include <Physics/Internal/Collide/Mopp/Code/hkpMoppCode.h>
#include <Physics/Collide/Shape/Compound/Tree/Mopp/hkpMoppUtility.h>
#include <Physics/Collide/Shape/Compound/Tree/Mopp/hkpMoppBvTreeShape.h>
#include <Physics/Collide/Shape/Compound/Collection/SimpleMesh/hkpSimpleMeshShape.h>
#include <Physics/Collide/Shape/Convex/Capsule/hkpCapsuleShape.h>
#include <Physics/Collide/Shape/Convex/Cylinder/hkpCylinderShape.h>
#include <Physics/Collide/Shape/Convex/ConvexVertices/hkpConvexVerticesShape.h>
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics/Collide/Shape/Convex/Sphere/hkpSphereShape.h>
#include <Physics/Collide/Shape/Compound/Collection/StorageExtendedMesh/hkpStorageExtendedMeshShape.h>
#include <Physics/Collide/Shape/Compound/Collection/CompressedMesh/hkpCompressedMeshShapeBuilder.h>
#include <Physics/Dynamics/Phantom/hkpPhantomListener.h>
#include <Physics/Dynamics/World/Listener/hkpWorldDeletionListener.h>

#include <Physics/Dynamics/Constraint/Bilateral/Ragdoll/hkpRagdollConstraintData.h>
#include <Physics/Dynamics/Constraint/Bilateral/LimitedHinge/hkpLimitedHingeConstraintData.h>
#include <Physics/Dynamics/Entity/hkpRigidBody.h>
#include <Physics/Dynamics/World/hkpPhysicsSystem.h>
#include <Physics/Dynamics/Entity/hkpEntityListener.h>

#include <Physics/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>
#include <Physics/Utilities/Serialize/hkpHavokSnapshot.h>
#include <Physics/Utilities/Serialize/hkpPhysicsData.h>

#include <Physics/Utilities/Collide/Filter/GroupFilter/hkpGroupFilterUtil.h>

hkpRigidBody*	HK_CALL GameUtils::createRagdollPart( const hkVector4& extents, hkReal mass, const hkVector4& position, int type, const char* filename)
{
	hkpRigidBody* rb = HK_NULL;

	switch( type )
	{
	case GameUtils::RPT_BOX:
		{
			rb = GameUtils::createBox( extents, mass, position );
		}
		break;
	case GameUtils::RPT_CONVEX:
		{
		}
		break;
	case GameUtils::RPT_SPHERE:
		{
			hkReal radius = extents.length3();
			rb = GameUtils::createSphere( 0.25f * radius, mass, position );
		}
		break;
	case GameUtils::RPT_CAPSULE:
		{
			rb = GameUtils::createCapsuleFromBox( extents, mass, position );
		}
		break;
	default:
		{
			HK_ASSERT2(0x58b3164b,  type < GameUtils::RPT_COUNT, "Unknown ragdoll part type!" );
		}
		break;
	}

	rb->setLinearDamping( 0.25f );
	rb->setAngularDamping( 0.25f );
	rb->setQualityType( HK_COLLIDABLE_QUALITY_MOVING );

	return rb;
}

hkpRagdollConstraintData* HK_CALL GameUtils::createRagdollContraint(
	hkpRigidBody* rbReference,
	hkpRigidBody* rbAttached,
	hkReal coneMin,
	hkReal coneMax,
	hkReal planeMin,
	hkReal planeMax,
	hkReal twistMin,
	hkReal twistMax,
	const hkVector4 &pivot,
	const hkVector4 &twistAxis,
	const hkVector4 &planeAxis )
{
	hkVector4 pivotReference;
	hkVector4 twistReference;
	hkVector4 planeReference;
	pivotReference.setTransformedInversePos( rbReference->getTransform(), pivot );
	twistReference.setRotatedInverseDir( rbReference->getRotation(), twistAxis ); twistReference.normalize3();
	planeReference.setRotatedInverseDir( rbReference->getRotation(), planeAxis ); planeReference.normalize3();

	hkVector4 pivotAttached;
	hkVector4 twistAttached;
	hkVector4 planeAttached;
	pivotAttached.setTransformedInversePos( rbAttached->getTransform(), pivot );
	twistAttached.setRotatedInverseDir( rbAttached->getRotation(), twistAxis );	twistAttached.normalize3();
	planeAttached.setRotatedInverseDir( rbAttached->getRotation(), planeAxis ); planeAttached.normalize3();

	// check for unsymmetrical cone axes
	hkReal centerCone	= (coneMax + coneMin) * 0.5f;
	hkReal diffCone		= (coneMax - coneMin) * 0.5f;

	if( hkMath::fabs( centerCone ) > HK_REAL_EPSILON )
	{
		twistReference._setRotatedDir( hkQuaternion( planeReference, -centerCone ), twistReference );
	}

	hkpRagdollConstraintData* bob = new hkpRagdollConstraintData( );
	bob->setInBodySpace(	pivotReference,	pivotAttached,
		planeReference,	planeAttached,
		twistReference,	twistAttached );

	bob->setConeAngularLimit( diffCone );
	bob->setPlaneMinAngularLimit( planeMin );
	bob->setPlaneMaxAngularLimit( planeMax );
	bob->setTwistMinAngularLimit( twistMin );
	bob->setTwistMaxAngularLimit( twistMax );
	bob->setMaxFrictionTorque( 0.7f );

	return bob;
}


hkpPhysicsSystem* HK_CALL GameUtils::createRagdoll( hkReal height, const hkVector4& ragdollPosition, const hkQuaternion& ragdollRotation, int systemNumber, RagdollPartType type )
{

	RagdollCinfo info;
	info.m_height = height;
	info.m_position = ragdollPosition;
	info.m_rotation = ragdollRotation;
	info.m_systemNumber = systemNumber;
	info.m_boneShapeType = type;
	return createRagdoll( info );
}

GameUtils::RagdollCinfo::RagdollCinfo()
{
	m_height = 2;
	m_position.setZero4();
	m_rotation.setIdentity();
	m_systemNumber = 1;
	m_numVertebrae = 1;
	m_wantOptimizeInertias = true;
	m_boneShapeType = RPT_CAPSULE;
	m_wantHandsAndFeet = NO_HANDS_AND_FEET;
	m_ragdollInterBoneCollisions = DISABLE_ALL_BONE_COLLISIONS;
}

hkpPhysicsSystem* HK_CALL GameUtils::createRagdoll( RagdollCinfo& info )
{
	const hkReal degreesToRadians  = HK_REAL_PI / 180.0f;

	hkpPhysicsSystem* ragdoll = new hkpPhysicsSystem;
	hkArray<hkTransform>	offsets; // not in the physics system (extra data?)

	struct _parts {
		hkArray<hkpRigidBody*>	spine;
		hkArray<hkpRigidBody*>	leftLeg;
		hkArray<hkpRigidBody*>	rightLeg;
		hkArray<hkpRigidBody*>	leftArm;
		hkArray<hkpRigidBody*>	rightArm;
	}	parts;

	hkBool	wantFeet	= false;
	hkBool	wantHands	= false;

	if (info.m_wantHandsAndFeet == WANT_HANDS_AND_FEET)
	{
		wantFeet = true;
		wantHands = true;
	}


	int	numVertebra	= info.m_numVertebrae;

	const hkReal head = info.m_height / 8.0f;
	const hkReal hand = head * 2.0f / 3.0f;

	// create rigid bodies
	{
		hkpRigidBody*	rb;
		hkVector4		extents;
		hkVector4		position;
		hkRotation		rotation;
		hkTransform		offset;
		hkReal			mass;

		hkStringBuf		filename;
		const hkReal	scale = info.m_height / 2.0f;

		filename = "Resources/Common/Tk/GameUtils/DisplayRagdoll_Pelvis.hkt";
		{
			extents.set( 0.65f * head * 1.75f, 0.7f * head, head + (numVertebra ? 0 : 2.25f * head), info.m_height );
			position.set( 0, 0, 4.25f * head );
			rotation.setRows( hkVector4( 0, 0, 1 ), hkVector4( 0, -1, 0 ), hkVector4( 1, 0, 0 ) );
			offset.getRotation() = rotation;
			offset.getTranslation().set( (numVertebra ? 0 : 1.125f * head), 0, 0 );
			mass = 30.0f * scale;

			hkVector4 p; p.setRotatedDir( offset.getRotation(), offset.getTranslation() ); p.add4( position );
			rb = createRagdollPart( extents, mass, p, info.m_boneShapeType, (0 == filename.getLength()) ? HK_NULL : filename );

			ragdoll->addRigidBody( rb );
			rb->removeReference(); // we don't want the ref anymore, leave with the system.

			offsets.pushBack( offset );

			parts.spine.pushBack( rb );
		}

		filename = "Resources/Common/Tk/GameUtils/DisplayRagdoll_Spine.hkt";
		for( int i = 0; i < numVertebra; i++ )
		{
			extents.set( 0.65f * head * 2.0f, 0.6f * head, 2.25f * head / numVertebra, info.m_height );
			position.set( 0, 0, (4.75f * head) + (i * (2.0f * head / numVertebra)) );
			rotation.setRows( hkVector4( 0, 0, 1 ), hkVector4( 0, -1, 0 ), hkVector4( 1, 0, 0 ) );
			offset.getTranslation().set( 0.5f * extents(2), 0, 0 );
			offset.getRotation() = rotation;
			mass = (20.0f * scale) / numVertebra;

			hkVector4 p; p.setRotatedDir( offset.getRotation(), offset.getTranslation() ); p.add4( position );
			rb = createRagdollPart( extents, mass, p, info.m_boneShapeType, (0 == filename.getLength()) ? HK_NULL : filename );

			ragdoll->addRigidBody( rb );
			rb->removeReference(); // we don't want the ref anymore, leave with the system.

			offsets.pushBack( offset );

			parts.spine.pushBack( rb );

			filename = "";
		}

		filename = "Resources/Common/Tk/GameUtils/DisplayRagdoll_Head.hkt";
		{
			extents.set( hand, head, head, info.m_height );
			position.set( 0, 0, 7.0f * head );
			rotation.setRows( hkVector4( 0, 0, 1 ), hkVector4( 0, -1, 0 ), hkVector4( 1, 0, 0 ) );
			offset.getTranslation().set( 0.5f * extents(2), 0.1f * extents(1), 0 );
			offset.getRotation() = rotation;
			mass = 5.0f * scale;

			hkVector4 p; p.setRotatedDir( offset.getRotation(), offset.getTranslation() ); p.add4( position );
			rb = createRagdollPart( extents, mass, p, info.m_boneShapeType, (0 == filename.getLength()) ? HK_NULL : filename );

			ragdoll->addRigidBody( rb );
			rb->removeReference(); // we don't want the ref anymore, leave with the system.

			offsets.pushBack( offset );

			parts.spine.pushBack( rb );
		}

		filename = "Resources/Common/Tk/GameUtils/DisplayRagdoll_L_Thigh.hkt";
		{
			extents.set( 0.5f * head, 0.75f * head, 2.0f * head, info.m_height );
			position.set( 0.75f * extents(0), 0, 4.25f * head );
			rotation.setRows( hkVector4( 0, 0, -1 ), hkVector4( 0, -1, 0 ), hkVector4( -1, 0, 0 ) );
			offset.getTranslation().set( 0.5f * extents(2), 0, 0 );
			offset.getRotation() = rotation;
			mass = 5.0f * scale;

			hkVector4 p; p.setRotatedDir( offset.getRotation(), offset.getTranslation() ); p.add4( position );
			rb = createRagdollPart( extents, mass, p, info.m_boneShapeType, (0 == filename.getLength()) ? HK_NULL : filename );

			ragdoll->addRigidBody( rb );
			rb->removeReference(); // we don't want the ref anymore, leave with the system.

			offsets.pushBack( offset );

			parts.leftLeg.pushBack( rb );
		}

		filename = "Resources/Common/Tk/GameUtils/DisplayRagdoll_L_Calf.hkt";
		{
			extents.set( 0.5f * head, 0.6f * head, (1.75f * head) + (wantFeet ? 0 : (0.5f * head)), info.m_height );
			position.set( 0.75f * extents(0), 0, 2.25f * head );
			rotation.setRows( hkVector4( 0, 0, -1 ), hkVector4( 0, -1, 0 ), hkVector4( -1, 0, 0 ) );
			offset.getTranslation().set( 0.5f * extents(2), 0, 0 );
			offset.getRotation() = rotation;
			mass = 5.0f * scale;

			hkVector4 p; p.setRotatedDir( offset.getRotation(), offset.getTranslation() ); p.add4( position );
			rb = createRagdollPart( extents, mass, p, info.m_boneShapeType, (0 == filename.getLength()) ? HK_NULL : filename );

			ragdoll->addRigidBody( rb );
			rb->removeReference(); // we don't want the ref anymore, leave with the system.

			offsets.pushBack( offset );

			parts.leftLeg.pushBack( rb );
		}

		filename = "Resources/Common/Tk/GameUtils/DisplayRagdoll_R_Thigh.hkt";
		{
			extents.set( 0.5f * head, 0.75f * head, 2.0f * head, info.m_height );
			position.set( -0.75f * extents(0), 0, 4.25f * head );
			rotation.setRows( hkVector4( 0, 0, -1 ), hkVector4( 0, -1, 0 ), hkVector4( -1, 0, 0 ) );
			offset.getTranslation().set( 0.5f * extents(2), 0, 0 );
			offset.getRotation() = rotation;
			mass = 5.0f * scale;

			hkVector4 p; p.setRotatedDir( offset.getRotation(), offset.getTranslation() ); p.add4( position );
			rb = createRagdollPart( extents, mass, p, info.m_boneShapeType, (0 == filename.getLength()) ? HK_NULL : filename );

			ragdoll->addRigidBody( rb );
			rb->removeReference(); // we don't want the ref anymore, leave with the system.

			offsets.pushBack( offset );

			parts.rightLeg.pushBack( rb );
		}

		filename = "Resources/Common/Tk/GameUtils/DisplayRagdoll_R_Calf.hkt";
		{
			extents.set( 0.5f * head, 0.6f * head, (1.75f * head) + (wantFeet ? 0 : (0.5f * head)), info.m_height );
			position.set( -0.75f * extents(0), 0, 2.25f * head );
			rotation.setRows( hkVector4( 0, 0, -1 ), hkVector4( 0, -1, 0 ), hkVector4( -1, 0, 0 ) );
			offset.getTranslation().set( 0.5f * extents(2), 0, 0 );
			offset.getRotation() = rotation;
			mass = 5.0f * scale;

			hkVector4 p; p.setRotatedDir( offset.getRotation(), offset.getTranslation() ); p.add4( position );
			rb = createRagdollPart( extents, mass, p, info.m_boneShapeType, (0 == filename.getLength()) ? HK_NULL : filename );

			ragdoll->addRigidBody( rb );
			rb->removeReference(); // we don't want the ref anymore, leave with the system.

			offsets.pushBack( offset );

			parts.rightLeg.pushBack( rb );
		}

		if( wantFeet )
		{
			filename = "";//"Resources/Common/Tk/GameUtils/DisplayRagdoll_L_Foot.hkt";
			{
				extents.set( 0.5f * head, 1.0f * head, 0.5f * head, info.m_height );
				position.set( 0.75f * extents(0), 0, extents(2) );
				rotation.setRows( hkVector4( 0, 0, -1 ), hkVector4( 0, -1, 0 ), hkVector4( -1, 0, 0 ) );
				offset.getTranslation().set( 0.5f * extents(2), 0.25f * extents(1), 0 );
				offset.getRotation() = rotation;
				mass = 1.0f * scale;

				hkVector4 p; p.setRotatedDir( offset.getRotation(), offset.getTranslation() ); p.add4( position );
				rb = createRagdollPart( extents, mass, p, info.m_boneShapeType, (0 == filename.getLength()) ? HK_NULL : filename );

				ragdoll->addRigidBody( rb );
				rb->removeReference(); // we don't want the ref anymore, leave with the system.

				offsets.pushBack( offset );

				parts.leftLeg.pushBack( rb );
			}

			filename = "";//"Resources/Common/Tk/GameUtils/DisplayRagdoll_R_Foot.hkt";
			{
				extents.set( 0.5f * head, 1.0f * head, 0.5f * head, info.m_height );
				position.set( -0.75f * extents(0), 0, extents(2) );
				rotation.setRows( hkVector4( 0, 0, -1 ), hkVector4( 0, -1, 0 ), hkVector4( -1, 0, 0 ) );
				offset.getTranslation().set( 0.5f * extents(2), 0.25f * extents(1), 0 );
				offset.getRotation() = rotation;
				mass = 1.0f * scale;

				hkVector4 p; p.setRotatedDir( offset.getRotation(), offset.getTranslation() ); p.add4( position );
				rb = createRagdollPart( extents, mass, p, info.m_boneShapeType, (0 == filename.getLength()) ? HK_NULL : filename );

				ragdoll->addRigidBody( rb );
				rb->removeReference(); // we don't want the ref anymore, leave with the system.

				offsets.pushBack( offset );

				parts.rightLeg.pushBack( rb );
			}
		}

		filename = "Resources/Common/Tk/GameUtils/DisplayRagdoll_L_UpperArm.hkt";
		{
			extents.set( 1.25f * head, 0.5f * head, 0.3f * head, info.m_height );
			position.set( 0.75f * head, 0, 6.75f * head );
			rotation.setRows( hkVector4( 1, 0, 0 ), hkVector4( 0, 1, 0 ), hkVector4( 0, 0, 1 ) );
			offset.getTranslation().set( 0.5f * extents(0), 0, 0 );
			offset.getRotation() = rotation;
			mass = 2.5f * scale;

			hkVector4 p; p.setRotatedDir( offset.getRotation(), offset.getTranslation() ); p.add4( position );
			rb = createRagdollPart( extents, mass, p, info.m_boneShapeType, (0 == filename.getLength()) ? HK_NULL : filename );

			ragdoll->addRigidBody( rb );
			rb->removeReference(); // we don't want the ref anymore, leave with the system.

			offsets.pushBack( offset );

			parts.leftArm.pushBack( rb );
		}

		filename = "Resources/Common/Tk/GameUtils/DisplayRagdoll_L_Forearm.hkt";
		{
			extents.set( (1.25f * head) + (wantHands ? 0 : hand), 0.4f * head, 0.3f * head, info.m_height );
			position.set( 2.0f * head, 0, 6.75f * head );
			rotation.setRows( hkVector4( 1, 0, 0 ), hkVector4( 0, 1, 0 ), hkVector4( 0, 0, 1 ) );
			offset.getTranslation().set( 0.5f * extents(0), 0, 0 );
			offset.getRotation() = rotation;
			mass = 2.5f * scale;

			hkVector4 p; p.setRotatedDir( offset.getRotation(), offset.getTranslation() ); p.add4( position );
			rb = createRagdollPart( extents, mass, p, info.m_boneShapeType, (0 == filename.getLength()) ? HK_NULL : filename );

			ragdoll->addRigidBody( rb );
			rb->removeReference(); // we don't want the ref anymore, leave with the system.

			offsets.pushBack( offset );

			parts.leftArm.pushBack( rb );
		}

		filename = "Resources/Common/Tk/GameUtils/DisplayRagdoll_R_UpperArm.hkt";
		{
			extents.set( 1.25f * head, 0.5f * head, 0.3f * head, info.m_height );
			position.set( -0.75f * head, 0, 6.75f * head );
			rotation.setRows( hkVector4( -1, 0, 0 ), hkVector4( 0, 1, 0 ), hkVector4( 0, 0, -1 ) );
			offset.getTranslation().set( 0.5f * extents(0), 0, 0 );
			offset.getRotation() = rotation;
			mass = 2.5f * scale;

			hkVector4 p; p.setRotatedDir( offset.getRotation(), offset.getTranslation() ); p.add4( position );
			rb = createRagdollPart( extents, mass, p, info.m_boneShapeType, (0 == filename.getLength()) ? HK_NULL : filename );

			ragdoll->addRigidBody( rb );
			rb->removeReference(); // we don't want the ref anymore, leave with the system.

			offsets.pushBack( offset );

			parts.rightArm.pushBack( rb );
		}

		filename = "Resources/Common/Tk/GameUtils/DisplayRagdoll_R_Forearm.hkt";
		{
			extents.set( (1.25f * head) + (wantHands ? 0 : hand), 0.4f * head, 0.3f * head, info.m_height );
			position.set( -2.0f * head, 0, 6.75f * head );
			rotation.setRows( hkVector4( -1, 0, 0 ), hkVector4( 0, 1, 0 ), hkVector4( 0, 0, -1 ) );
			offset.getTranslation().set( 0.5f * extents(0), 0, 0 );
			offset.getRotation() = rotation;
			mass = 2.5f * scale;

			hkVector4 p; p.setRotatedDir( offset.getRotation(), offset.getTranslation() ); p.add4( position );
			rb = createRagdollPart( extents, mass, p, info.m_boneShapeType, (0 == filename.getLength()) ? HK_NULL : filename );

			ragdoll->addRigidBody( rb );
			rb->removeReference(); // we don't want the ref anymore, leave with the system.

			offsets.pushBack( offset );

			parts.rightArm.pushBack( rb );
		}

		if( wantHands )
		{
			filename = "";//"Resources/Common/Tk/GameUtils/DisplayRagdoll_L_Hand.hkt";
			{
				extents.set( hand, 0.4f * head, 0.2f * head, info.m_height );
				position.set( 3.25f * head, 0, 6.75f * head );
				rotation.setRows( hkVector4( 1, 0, 0 ), hkVector4( 0, 0, -1 ), hkVector4( 0, 1, 0 ) );
				offset.getTranslation().set( 0.5f * extents(0), 0, 0 );
				offset.getRotation() = rotation;
				mass = 0.5f * scale;

				hkVector4 p; p.setRotatedDir( offset.getRotation(), offset.getTranslation() ); p.add4( position );
				rb = createRagdollPart( extents, mass, p, info.m_boneShapeType, (0 == filename.getLength()) ? HK_NULL : filename );

				ragdoll->addRigidBody( rb );
				rb->removeReference(); // we don't want the ref anymore, leave with the system.

				offsets.pushBack( offset );

				parts.leftArm.pushBack( rb );
			}

			filename = "";//"Resources/Common/Tk/GameUtils/DisplayRagdoll_R_Hand.hkt";
			{
				extents.set( hand, 0.4f * head, 0.2f * head, info.m_height );
				position.set( -3.25f * head, 0, 6.75f * head );
				rotation.setRows( hkVector4( -1, 0, 0 ), hkVector4( 0, 0, -1 ), hkVector4( 0, -1, 0 ) );
				offset.getTranslation().set( 0.5f * extents(0), 0, 0 );
				offset.getRotation() = rotation;
				mass = 0.5f * scale;

				hkVector4 p; p.setRotatedDir( offset.getRotation(), offset.getTranslation() ); p.add4( position );
				rb = createRagdollPart( extents, mass, p, info.m_boneShapeType, (0 == filename.getLength()) ? HK_NULL : filename );

				ragdoll->addRigidBody( rb );
				rb->removeReference(); // we don't want the ref anymore, leave with the system.

				offsets.pushBack( offset );

				parts.rightArm.pushBack( rb );
			}
		}
	}

	// create ragdoll->m_constraints
	{
		// Neck
		{
			hkpRagdollConstraintData* rdc = HK_NULL;

			hkpRigidBody* rbAttached		= parts.spine[parts.spine.getSize() - 1];
			hkpRigidBody* rbReference	= parts.spine[parts.spine.getSize() - 2];

			hkVector4 twistAxis( 0.0f, 0.0f, 1.0f );
			hkVector4 planeAxis( 1.0f, 0.0f, 0.0f );
			hkVector4 pivot;

			hkTransform	offset = offsets[ragdoll->getRigidBodies().indexOf( rbAttached )];
			pivot.setRotatedDir( offset.getRotation(), offset.getTranslation() );
			pivot.setNeg4( pivot );
			pivot.add4( rbAttached->getPosition() );

			rdc = createRagdollContraint(	rbReference,
				rbAttached,
				// cone
				-45.0f	* degreesToRadians,
				10.0f	* degreesToRadians,
				// plane
				-15.0f	* degreesToRadians,
				15.0f	* degreesToRadians,
				// twist
				-5.0f	* degreesToRadians,
				5.0f	* degreesToRadians,
				pivot,
				twistAxis,
				planeAxis );

			hkpConstraintInstance* ci = new hkpConstraintInstance( rbReference, rbAttached, rdc);
			ragdoll->addConstraint( ci );
			ci->removeReference();

			rdc->removeReference();
		}

		// Vertebra
		{
			hkpRagdollConstraintData* rdc = HK_NULL;

			for( int i = 1; i < (parts.spine.getSize() - 1); i++ )
			{

				hkpRigidBody* rbAttached		= parts.spine[i];
				hkpRigidBody* rbReference	= parts.spine[i - 1];
				//			hkTransform	offset = offsets[ragdoll->getRigidBodies().indexOf( rbAttached )];
				//			pivot.setRotatedDir( offset.getRotation(), offset.getTranslation() );
				//			pivot.setNeg4( pivot );
				//			pivot.add4( rbAttached->getPosition() );
				if ( !rdc )
				{
					hkVector4 twistAxis( 0.0f, 0.0f, 1.0f );
					hkVector4 planeAxis( 0.0f, -1.0f, 0.0f );
					hkVector4 pivot;

					hkTransform	offset = offsets[ragdoll->getRigidBodies().indexOf( rbAttached )];
					pivot.setRotatedDir( offset.getRotation(), offset.getTranslation() );
					pivot.setNeg4( pivot );
					pivot.add4( rbAttached->getPosition() );

					rdc = createRagdollContraint(	rbReference, rbAttached,
						// cone
						(-25.0f / numVertebra) * degreesToRadians,
						( 25.0f / numVertebra) * degreesToRadians,
						// plane
						(-20.0f / numVertebra) * degreesToRadians,
						( 10.0f / numVertebra) * degreesToRadians,
						// twist
						( -5.0f / numVertebra) * degreesToRadians,
						(  5.0f / numVertebra) * degreesToRadians,
						pivot,		twistAxis,		planeAxis );
				}
				hkpConstraintInstance* ci = new hkpConstraintInstance( rbReference, rbAttached, rdc);
				ragdoll->addConstraint( ci );
				ci->removeReference();
			}
			rdc->removeReference();
		}

		// left hip
		{
			hkpRagdollConstraintData* rdc = HK_NULL;

			hkpRigidBody* rbAttached		= parts.leftLeg[0];
			hkpRigidBody* rbReference	= parts.spine[0];

			hkVector4 twistAxis( 0.0f, 0.0f, -1.0f );
			hkVector4 planeAxis( 1.0f, 0.0f, 0.0f );
			hkVector4 pivot;

			hkTransform	offset = offsets[ragdoll->getRigidBodies().indexOf( rbAttached )];
			pivot.setRotatedDir( offset.getRotation(), offset.getTranslation() );
			pivot.setNeg4( pivot );
			pivot.add4( rbAttached->getPosition() );

			rdc = createRagdollContraint(	rbReference,
				rbAttached,
				// cone
				-25.0f	* degreesToRadians,
				85.0f	* degreesToRadians,
				// plane
				-45.0f	* degreesToRadians,
				2.5f	* degreesToRadians,
				// twist
				-5.0f	* degreesToRadians,
				5.0f	* degreesToRadians,
				pivot,
				twistAxis,
				planeAxis );

			hkpConstraintInstance* ci = new hkpConstraintInstance( rbReference, rbAttached, rdc);
			ragdoll->addConstraint( ci );
			ci->removeReference();
			rdc->removeReference();
		}

		// right hip
		{
			hkpRagdollConstraintData* rdc = HK_NULL;

			hkpRigidBody* rbAttached		= parts.rightLeg[0];
			hkpRigidBody* rbReference	= parts.spine[0];

			hkVector4 twistAxis( 0.0f, 0.0f, -1.0f );
			hkVector4 planeAxis( -1.0f, 0.0f, 0.0f );
			hkVector4 pivot;

			hkTransform	offset = offsets[ragdoll->getRigidBodies().indexOf( rbAttached )];
			pivot.setRotatedDir( offset.getRotation(), offset.getTranslation() );
			pivot.setNeg4( pivot );
			pivot.add4( rbAttached->getPosition() );

			rdc = createRagdollContraint(	rbReference,
				rbAttached,
				// cone
				-85.0f	* degreesToRadians,
				25.0f	* degreesToRadians,
				// plane
				-45.0f	* degreesToRadians,
				2.5f	* degreesToRadians,
				// twist
				-5.0f	* degreesToRadians,
				5.0f	* degreesToRadians,
				pivot,
				twistAxis,
				planeAxis );

			hkpConstraintInstance* ci = new hkpConstraintInstance( rbReference, rbAttached, rdc);
			ragdoll->addConstraint( ci );
			ci->removeReference();
			rdc->removeReference();

		}

		// left shoulder
		{
			hkpRagdollConstraintData* rdc = HK_NULL;

			hkpRigidBody* rbAttached		= parts.leftArm[0];
			hkpRigidBody* rbReference	= parts.spine[parts.spine.getSize()-2];

			hkVector4 twistAxis( 1.0f, 0.0f, 0.0f );
			hkVector4 planeAxis( 0.0f, -1.0f, 0.0f );
			hkVector4 pivot;

			hkTransform	offset = offsets[ragdoll->getRigidBodies().indexOf( rbAttached )];
			pivot.setRotatedDir( offset.getRotation(), offset.getTranslation() );
			pivot.setNeg4( pivot );
			pivot.add4( rbAttached->getPosition() );

			rdc = createRagdollContraint(	rbReference,
				rbAttached,
				// cone
				-85.0f	* degreesToRadians,
				75.0f	* degreesToRadians,
				// plane
				-65.0f	* degreesToRadians,
				5.0f	* degreesToRadians,
				// twist
				-5.0f	* degreesToRadians,
				5.0f	* degreesToRadians,
				pivot,
				twistAxis,
				planeAxis );

			hkpConstraintInstance* ci = new hkpConstraintInstance( rbReference, rbAttached, rdc);
			ragdoll->addConstraint( ci );
			ci->removeReference();
			rdc->removeReference();

		}

		// right shoulder
		{
			hkpRagdollConstraintData* rdc = HK_NULL;

			hkpRigidBody* rbAttached		= parts.rightArm[0];
			hkpRigidBody* rbReference	= parts.spine[parts.spine.getSize()-2];

			hkVector4 twistAxis( -1.0f, 0.0f, 0.0f );
			hkVector4 planeAxis( 0.0f, -1.0f, 0.0f );
			hkVector4 pivot;

			hkTransform	offset = offsets[ragdoll->getRigidBodies().indexOf( rbAttached )];
			pivot.setRotatedDir( offset.getRotation(), offset.getTranslation() );
			pivot.setNeg4( pivot );
			pivot.add4( rbAttached->getPosition() );

			rdc = createRagdollContraint(	rbReference,
				rbAttached,
				// cone
				-75.0f	* degreesToRadians,
				85.0f	* degreesToRadians,
				// plane
				-65.0f	* degreesToRadians,
				5.0f	* degreesToRadians,
				// twist
				-5.0f	* degreesToRadians,
				5.0f	* degreesToRadians,
				pivot,
				twistAxis,
				planeAxis );

			hkpConstraintInstance* ci = new hkpConstraintInstance( rbReference, rbAttached, rdc);
			ragdoll->addConstraint( ci );
			ci->removeReference();
			rdc->removeReference();

		}

		// left elbow
		{
			hkpRigidBody* rbAttached		= parts.leftArm[1];
			hkpRigidBody* rbReference	= parts.leftArm[0];

			hkpLimitedHingeConstraintData* bob = new hkpLimitedHingeConstraintData( );

			bob->setMinAngularLimit(  10.0f	* degreesToRadians );
			bob->setMaxAngularLimit(  80.0f	* degreesToRadians );

			hkVector4 axis( 0.0f, 0.0f, -1.0f );
			hkVector4 pivot;

			hkTransform	offset = offsets[ragdoll->getRigidBodies().indexOf( rbAttached )];
			pivot.setRotatedDir( offset.getRotation(), offset.getTranslation() );
			pivot.setNeg4( pivot );
			pivot.add4( rbAttached->getPosition() );

			bob->setInWorldSpace( rbAttached->getTransform(), rbReference->getTransform(), pivot, axis );

			hkpConstraintInstance* ci = new hkpConstraintInstance( rbAttached, rbReference, bob );
			ragdoll->addConstraint( ci );
			ci->removeReference();
			bob->removeReference();

		}

		// right elbow
		{
			hkpRigidBody* rbAttached		= parts.rightArm[1];
			hkpRigidBody* rbReference	= parts.rightArm[0];

			hkpLimitedHingeConstraintData* bob = new hkpLimitedHingeConstraintData( );

			bob->setMinAngularLimit(  10.0f	* degreesToRadians );
			bob->setMaxAngularLimit(  80.0f	* degreesToRadians );

			hkVector4 axis( 0.0f, 0.0f, 1.0f );
			hkVector4 pivot;

			hkTransform	offset = offsets[ragdoll->getRigidBodies().indexOf( rbAttached )];
			pivot.setRotatedDir( offset.getRotation(), offset.getTranslation() );
			pivot.setNeg4( pivot );
			pivot.add4( rbAttached->getPosition() );

			bob->setInWorldSpace( rbAttached->getTransform(), rbReference->getTransform(), pivot, axis );

			hkpConstraintInstance* ci = new hkpConstraintInstance( rbAttached, rbReference, bob );
			ragdoll->addConstraint( ci );
			ci->removeReference();
			bob->removeReference();
		}

		// left knee
		{
			hkpRigidBody* rbAttached		= parts.leftLeg[1];
			hkpRigidBody* rbReference	= parts.leftLeg[0];

			hkpLimitedHingeConstraintData* bob = new hkpLimitedHingeConstraintData( );

			bob->setMinAngularLimit(  10.0f	* degreesToRadians );
			bob->setMaxAngularLimit(  80.0f	* degreesToRadians );

			hkVector4 axis( 1.0f, 0.0f, 0.0f );
			hkVector4 pivot;

			hkTransform	offset = offsets[ragdoll->getRigidBodies().indexOf( rbAttached )];
			pivot.setRotatedDir( offset.getRotation(), offset.getTranslation() );
			pivot.setNeg4( pivot );
			pivot.add4( rbAttached->getPosition() );

			bob->setInWorldSpace( rbAttached->getTransform(), rbReference->getTransform(), pivot, axis );

			hkpConstraintInstance* ci = new hkpConstraintInstance( rbAttached, rbReference, bob );
			ragdoll->addConstraint( ci );
			ci->removeReference();
			bob->removeReference();
		}

		// right knee
		{
			hkpRigidBody* rbAttached		= parts.rightLeg[1];
			hkpRigidBody* rbReference	= parts.rightLeg[0];

			hkpLimitedHingeConstraintData* bob = new hkpLimitedHingeConstraintData();

			bob->setMinAngularLimit(  10.0f	* degreesToRadians );
			bob->setMaxAngularLimit(  80.0f	* degreesToRadians );

			hkVector4 axis( 1.0f, 0.0f, 0.0f );
			hkVector4 pivot;

			hkTransform	offset = offsets[ragdoll->getRigidBodies().indexOf( rbAttached )];
			pivot.setRotatedDir( offset.getRotation(), offset.getTranslation() );
			pivot.setNeg4( pivot );
			pivot.add4( rbAttached->getPosition() );

			bob->setInWorldSpace( rbAttached->getTransform(), rbReference->getTransform(), pivot, axis );

			hkpConstraintInstance* ci = new hkpConstraintInstance( rbAttached, rbReference, bob );
			ragdoll->addConstraint( ci );
			ci->removeReference();
			bob->removeReference();
		}

		if( wantHands )
		{
			// left wrist
			{
				hkpRigidBody* rbAttached		= parts.leftArm[2];
				hkpRigidBody* rbReference	= parts.leftArm[1];

				hkpLimitedHingeConstraintData* bob = new hkpLimitedHingeConstraintData();

				bob->setMinAngularLimit(  -10.0f	* degreesToRadians );
				bob->setMaxAngularLimit(   20.0f	* degreesToRadians );

				hkVector4 axis( 0.0f, 1.0f, 0.0f );
				hkVector4 pivot;

				hkTransform	offset = offsets[ragdoll->getRigidBodies().indexOf( rbAttached )];
				pivot.setRotatedDir( offset.getRotation(), offset.getTranslation() );
				pivot.setNeg4( pivot );
				pivot.add4( rbAttached->getPosition() );

				bob->setInWorldSpace( rbAttached->getTransform(), rbReference->getTransform(), pivot, axis );

				hkpConstraintInstance* ci = new hkpConstraintInstance( rbAttached, rbReference, bob );
				ragdoll->addConstraint( ci );
				ci->removeReference();
				bob->removeReference();
			}

			// right wrist
			{
				hkpRigidBody* rbAttached		= parts.rightArm[2];
				hkpRigidBody* rbReference	= parts.rightArm[1];

				hkpLimitedHingeConstraintData* bob = new hkpLimitedHingeConstraintData();

				bob->setMinAngularLimit(  -10.0f	* degreesToRadians );
				bob->setMaxAngularLimit(   20.0f	* degreesToRadians );

				hkVector4 axis( 0.0f, -1.0f, 0.0f );
				hkVector4 pivot;

				hkTransform	offset = offsets[ragdoll->getRigidBodies().indexOf( rbAttached )];
				pivot.setRotatedDir( offset.getRotation(), offset.getTranslation() );
				pivot.setNeg4( pivot );
				pivot.add4( rbAttached->getPosition() );

				bob->setInWorldSpace( rbAttached->getTransform(), rbReference->getTransform(), pivot, axis );

				hkpConstraintInstance* ci = new hkpConstraintInstance( rbAttached, rbReference, bob );
				ragdoll->addConstraint( ci );
				ci->removeReference();
				bob->removeReference();
			}
		}

		if( wantFeet )
		{
			// left ankle
			{
				hkpRigidBody* rbAttached		= parts.leftLeg[2];
				hkpRigidBody* rbReference	= parts.leftLeg[1];

				hkpLimitedHingeConstraintData* bob = new hkpLimitedHingeConstraintData();

				bob->setMinAngularLimit(  -15.0f	* degreesToRadians );
				bob->setMaxAngularLimit(   35.0f	* degreesToRadians );

				hkVector4 axis( 1.0f, 0.0f, 0.0f );
				hkVector4 pivot;

				hkTransform	offset = offsets[ragdoll->getRigidBodies().indexOf( rbAttached )];
				pivot.setRotatedDir( offset.getRotation(), offset.getTranslation() );
				pivot.setNeg4( pivot );
				pivot.add4( rbAttached->getPosition() );

				bob->setInWorldSpace( rbAttached->getTransform(), rbReference->getTransform(), pivot, axis );

				hkpConstraintInstance* ci = new hkpConstraintInstance( rbAttached, rbReference, bob );
				ragdoll->addConstraint( ci );
				ci->removeReference();
				bob->removeReference();
			}

			// right wrist
			{
				hkpRigidBody* rbAttached		= parts.rightLeg[2];
				hkpRigidBody* rbReference	= parts.rightLeg[1];

				hkpLimitedHingeConstraintData* bob = new hkpLimitedHingeConstraintData();

				bob->setMinAngularLimit(  -15.0f	* degreesToRadians );
				bob->setMaxAngularLimit(   35.0f	* degreesToRadians );

				hkVector4 axis( -1.0f, 0.0f, 0.0f );
				hkVector4 pivot;

				hkTransform	offset = offsets[ragdoll->getRigidBodies().indexOf( rbAttached )];
				pivot.setRotatedDir( offset.getRotation(), offset.getTranslation() );
				pivot.setNeg4( pivot );
				pivot.add4( rbAttached->getPosition() );

				bob->setInWorldSpace( rbAttached->getTransform(), rbReference->getTransform(), pivot, axis );

				hkpConstraintInstance* ci = new hkpConstraintInstance( rbAttached, rbReference, bob );
				ragdoll->addConstraint( ci );
				ci->removeReference();
				bob->removeReference();
			}
		}
	}

	// re-position ragdoll
	// set collision information
	{
		for( int i = ragdoll->getRigidBodies().getSize()-1; i >= 0; i-- )
		{
			hkpRigidBody* rbRoot	= ragdoll->getRigidBodies()[0];
			hkpRigidBody* rb		= ragdoll->getRigidBodies()[i];
			if ( info.m_ragdollInterBoneCollisions == DISABLE_ALL_BONE_COLLISIONS )
			{
				rb->setCollisionFilterInfo( hkpGroupFilter::calcFilterInfo( 0, info.m_systemNumber ) );
			}

			hkVector4 position;
			position.setSub4( rb->getPosition(), rbRoot->getPosition() );
			position._setRotatedDir( info.m_rotation, position );
			position.add4( info.m_position );

			rb->setPosition( position );
			rb->setRotation( info.m_rotation );
		}

		if ( info.m_ragdollInterBoneCollisions == DISABLE_ONLY_ADJOINING_BONE_COLLISIONS )
		{
			hkpGroupFilterUtil::disableCollisionsBetweenConstraintBodies( ragdoll->getConstraints().begin(), ragdoll->getConstraints().getSize(), info.m_systemNumber );
		}
		if ( info.m_wantOptimizeInertias)
		{
			hkpInertiaTensorComputer::optimizeInertiasOfConstraintTree( ragdoll->getConstraints().begin(), ragdoll->getConstraints().getSize(), ragdoll->getRigidBodies()[0] );
		}

	}

	// ragdoll system will keep the references.

	return ragdoll;
}


hkpRigidBody* HK_CALL GameUtils::createRigidBody(hkpShape* shape, const hkReal mass, const hkVector4 &position)
{
	//
	// Create a rigid body construction template
	//
	hkpRigidBodyCinfo rbInfo;

	if(mass != 0.0f)
	{
		rbInfo.m_mass = mass;
		hkpInertiaTensorComputer::setShapeVolumeMassProperties(shape, mass, rbInfo);
	}
	else
	{
		rbInfo.m_motionType = hkpMotion::MOTION_FIXED;
	}

	rbInfo.m_rotation.setIdentity();
	rbInfo.m_shape = shape;
	rbInfo.m_position = position;

	hkpRigidBody* rigidBody= new hkpRigidBody(rbInfo);

	return rigidBody;

}


void HK_CALL GameUtils::createStaticBox( hkpWorld* world, float centerX, float centerY, float centerZ, float radiusX, float radiusY, float radiusZ )
{
	hkVector4 radii ( radiusX, radiusY, radiusZ );

	hkpShape* boxShape = new hkpBoxShape( radii , 0 );

	hkpRigidBodyCinfo boxInfo;

	boxInfo.m_motionType = hkpMotion::MOTION_FIXED;
	boxInfo.m_shape = boxShape;
	boxInfo.m_position.set( centerX, centerY, centerZ );

	hkpRigidBody* boxRigidBody = new hkpRigidBody(boxInfo);
	world->addEntity( boxRigidBody );
	boxRigidBody->removeReference();
	boxShape->removeReference();
}



// This is just a user function to help create a box in one line
hkpRigidBody* HK_CALL GameUtils::createBox(const hkVector4 &size, const hkReal mass, const hkVector4 &position, hkReal radius )
{
	hkVector4 halfExtents(size(0) * 0.5f, size(1) * 0.5f, size(2) * 0.5f);
	hkpBoxShape* cube = new hkpBoxShape(halfExtents, radius );	// Note: We use HALF-extents for boxes

	//
	// Create a rigid body construction template
	//
	hkpRigidBodyCinfo boxInfo;

	if(mass != 0.0f)
	{
		boxInfo.m_mass = mass;
		hkpMassProperties massProperties;
		hkpInertiaTensorComputer::computeBoxVolumeMassProperties(halfExtents, mass, massProperties);
		boxInfo.m_inertiaTensor = massProperties.m_inertiaTensor;
		boxInfo.m_motionType = hkpMotion::MOTION_BOX_INERTIA;
	}
	else
	{
		boxInfo.m_motionType = hkpMotion::MOTION_FIXED;
	}
	boxInfo.m_rotation.setIdentity();
	boxInfo.m_shape = cube;
	boxInfo.m_position = position;

	//
	// create a rigid body (using the template above)
	//

	hkpRigidBody* boxRigidBody = new hkpRigidBody(boxInfo);

	cube->removeReference();

	return boxRigidBody;
}

hkpConvexVerticesShape* HK_CALL GameUtils::createConvexVerticesBoxShape( const hkVector4& center, const hkVector4& halfExtents, hkReal radius )
{
	hkVector4 verts[8];
	int d = 0;
	for ( int x = -1; x <= 1; x+=2 )
	{
		for ( int y = -1; y <= 1; y+=2 )
		{
			for ( int z = -1; z <= 1; z+=2 )
			{
				verts[d].set( hkReal(x), hkReal(y), hkReal(z) );
				verts[d].mul4( halfExtents );
				verts[d].add4( center );
				d++;
			}
		}
	}

	hkInplaceArrayAligned16<hkVector4, 6> planeEquations;
	planeEquations.setSizeUnchecked(6);
	planeEquations[0].set( 0, 0, 1, -center(2) -halfExtents(2) );
	planeEquations[1].set( 0, 0,-1, +center(2) -halfExtents(2) );
	planeEquations[2].set( 0, 1, 0, -center(1) -halfExtents(1) );
	planeEquations[3].set( 0,-1, 0, +center(1) -halfExtents(1) );
	planeEquations[4].set( 1, 0, 0, -center(0) -halfExtents(0) );
	planeEquations[5].set(-1, 0, 0, +center(0) -halfExtents(0) );

	hkStridedVertices sverts;
	{
		sverts.m_numVertices = 8;
		sverts.m_striding = sizeof(verts[0]);
		sverts.m_vertices = &verts[0](0);
	}
	hkpConvexVerticesShape* shape = new hkpConvexVerticesShape( sverts, planeEquations, radius );

	return shape;
}


hkpRigidBody* HK_CALL GameUtils::createConvexVerticesBox( const hkVector4& size, const hkReal mass, const hkVector4& position, hkReal radius )
{
	hkVector4 halfExtents( ( size(0) * 0.5f ), ( size(1) * 0.5f ), ( size(2) * 0.5f ) );

	hkpConvexVerticesShape* shape = createConvexVerticesBoxShape( hkVector4::getZero(), halfExtents, radius);
	
	//
	// Create a rigid body construction template
	//
	hkpRigidBodyCinfo boxInfo;

	if( mass != 0.0f )
	{
		boxInfo.m_mass = mass;
		hkpMassProperties massProperties;
		hkpInertiaTensorComputer::computeBoxVolumeMassProperties( halfExtents, mass, massProperties );
		boxInfo.m_inertiaTensor = massProperties.m_inertiaTensor;
		boxInfo.m_motionType = hkpMotion::MOTION_BOX_INERTIA;
	}

	else
	{
		boxInfo.m_motionType = hkpMotion::MOTION_FIXED;
	}

	boxInfo.m_rotation.setIdentity();
	boxInfo.m_shape = shape;
	boxInfo.m_position = position;

	//
	// create a rigid body (using the template above)
	//

	hkpRigidBody* boxRigidBody = new hkpRigidBody( boxInfo );

	shape->removeReference();

	return boxRigidBody;
}


// This is just a user function to help create a sphere in one line
hkpRigidBody* HK_CALL GameUtils::createSphere(const hkReal radius, const hkReal mass, const hkVector4& position)
{
	hkpSphereShape* sphere = new hkpSphereShape(radius);

	//
	// Create a rigid body construction template
	//
	hkpRigidBodyCinfo sphereInfo;

	if(mass != 0.0f)
	{
		// Haven't yet added sphere functionality to hkpInertiaTensorComputer, so here's explicit code.
		sphereInfo.m_mass = mass;
		hkpMassProperties massProperties;
		hkpInertiaTensorComputer::computeSphereVolumeMassProperties(radius, mass, massProperties);
		sphereInfo.m_inertiaTensor = massProperties.m_inertiaTensor;
		sphereInfo.m_motionType = hkpMotion::MOTION_SPHERE_INERTIA;
	}
	else
	{
		sphereInfo.m_motionType = hkpMotion::MOTION_FIXED;
	}
	sphereInfo.m_rotation.setIdentity();
	sphereInfo.m_shape = sphere;
	sphereInfo.m_position = position;

	//
	// create a rigid body (using the template above)
	//

	hkpRigidBody* sphereRigidBody = new hkpRigidBody(sphereInfo);

	sphere->removeReference();

	return sphereRigidBody;
}

hkpRigidBody* HK_CALL GameUtils::createCapsuleFromBox(const hkVector4& size, const hkReal mass, const hkVector4& position)
{
	int maxIndex = size.getMajorAxis3();
	int minIndex = (maxIndex + 1) % 3;
	int midIndex = (minIndex + 1) % 3;
	if( size(minIndex) > size(midIndex) )
	{
		// swap
		minIndex = minIndex ^ midIndex;
		midIndex = minIndex ^ midIndex;
		minIndex = minIndex ^ midIndex;
	}

	hkReal		radius = 0.5f * size(midIndex);
	hkReal      length = hkMath::max2( radius * 0.01f ,( 0.5f * size(maxIndex)) - radius);

	hkVector4	start;	start.setZero4();	start(maxIndex) = length;
	hkVector4	end;	end.setNeg4( start );

	hkpRigidBodyCinfo bob;

	bob.m_shape			= new hkpCapsuleShape( start, end, radius );
	bob.m_mass			= mass;

	if( 0.0f == bob.m_mass )
	{
		bob.m_motionType	= hkpMotion::MOTION_FIXED;
	}
	else
	{
		bob.m_motionType	= hkpMotion::MOTION_BOX_INERTIA;

		hkpMassProperties massProperties;
		hkpInertiaTensorComputer::setShapeVolumeMassProperties( bob.m_shape, bob.m_mass, bob );
	}

	bob.m_rotation.setIdentity();
	bob.m_position = position;

	hkpRigidBody* rb = new hkpRigidBody( bob );

	bob.m_shape->removeReference();

	return rb;
}

hkpRigidBody* HK_CALL GameUtils::createCylinder( hkReal radius, hkReal height, hkReal mass, const hkVector4& position )
{
	hkVector4 pointA( 0.0f, -height / 2.0f, 0.0f );
	hkVector4 pointB( 0.0f, height / 2.0f, 0.0f );

	hkpRigidBodyCinfo info;


	hkpCylinderShape* cylinderShape = new hkpCylinderShape( pointA, pointB, radius );

	info.m_shape = cylinderShape;
	if ( mass != 0.0f )
	{
		hkpMassProperties massProperties;
		hkpInertiaTensorComputer::computeCylinderVolumeMassProperties( pointA, pointB, radius, mass, massProperties );
		info.m_mass = mass;
		info.m_centerOfMass = massProperties.m_centerOfMass;
		info.m_inertiaTensor = massProperties.m_inertiaTensor;
		info.m_motionType = hkpMotion::MOTION_BOX_INERTIA;
	}
	else
	{
		info.m_motionType = hkpMotion::MOTION_FIXED;
	}

	info.m_rotation.setIdentity();
	info.m_position = position;

	hkpRigidBody* body = new hkpRigidBody(info);

	cylinderShape->removeReference();

	return body;
}

hkpRigidBody* HK_CALL GameUtils::createConvexGeometricPrism(const hkArray<hkVector4>& verts, const hkVector4& extrusionDirection, hkReal radius, const hkReal mass, const hkVector4& position)
{
	// Add verts and extruded verts
	hkArray<hkVector4> vertices;

	for(int i = 0; i < verts.getSize(); i++)
	{
		vertices.pushBack( verts[i]);
		hkVector4 v = verts[i];
		v.add4( extrusionDirection );
		vertices.pushBack( v );
	}

	// create a convexVerticesShape from this
	hkpConvexVerticesShape::BuildConfig config;
	config.m_convexRadius	=	radius;
	hkpConvexVerticesShape* cvs = new hkpConvexVerticesShape(vertices, config);
	
	hkpRigidBodyCinfo convexInfo;

	convexInfo.m_shape = cvs;
	if(mass != 0.0f)
	{
		convexInfo.m_mass = mass;
		hkpInertiaTensorComputer::setShapeVolumeMassProperties(convexInfo.m_shape, convexInfo.m_mass, convexInfo);
		convexInfo.m_motionType = hkpMotion::MOTION_BOX_INERTIA;
	}
	else
	{
		convexInfo.m_motionType = hkpMotion::MOTION_FIXED;
	}

	convexInfo.m_rotation.setIdentity();
	convexInfo.m_position = position;

	hkpRigidBody* convexRigidBody = new hkpRigidBody(convexInfo);

	cvs->removeReference();

	return convexRigidBody;
}



hkpRigidBody* HK_CALL GameUtils::createConvexGeometric( hkGeometry* geometryIn, const hkReal mass, const hkVector4& position )
{
	// convert it to a convex vertices shape
	hkpConvexVerticesShape* cvs = new hkpConvexVerticesShape(geometryIn->m_vertices);
	
	hkpRigidBodyCinfo bob;

	bob.m_shape = cvs;
	bob.m_mass = mass;

	if( 0.0f == bob.m_mass )
	{
		bob.m_motionType = hkpMotion::MOTION_FIXED;
	}
	else
	{
		hkpInertiaTensorComputer::setShapeVolumeMassProperties( bob.m_shape, bob.m_mass, bob );
		bob.m_motionType = hkpMotion::MOTION_BOX_INERTIA;
	}

	bob.m_rotation.setIdentity();
	bob.m_position = position;

	hkpRigidBody* rb = new hkpRigidBody( bob );

	cvs->removeReference();

	return rb;
}

/////////////////////////////////////////////////////////////////////////////////////////

/* Helper Functions */



void HK_CALL GameUtils::addShapeToWorld( hkpShape* shape, hkpWorld* world, hkpMassProperties* massProperties )
{
	HK_ASSERT2( 0x35c7996f, shape, "Null shape passed to addFixedShapeToWorld.");
	HK_ASSERT2( 0x602bae8f, world, "Null hkpWorld passed to addFixedShapeToWorld.");

	// Create static rigid body
	hkpRigidBodyCinfo cinfo;
	cinfo.m_position.setZero();
	cinfo.m_shape = shape;

	if (massProperties)
	{
		cinfo.m_motionType    = hkpMotion::MOTION_DYNAMIC;
		cinfo.m_mass          = massProperties->m_mass;
		cinfo.m_inertiaTensor = massProperties->m_inertiaTensor;
	}
	else
	{
		cinfo.m_motionType = hkpMotion::MOTION_FIXED;
	}
	hkpRigidBody* rigidBody = new hkpRigidBody( cinfo );

	// Rigid body owns the shape
	shape->removeReference();

	// Add to world
	world->addEntity( rigidBody );

	// World owns the rigid body
	rigidBody->removeReference();
}

hkpMoppBvTreeShape* HK_CALL GameUtils::createMoppFromGeometry( const hkGeometry& geometry, const hkTransform* globalTransform/* = HK_NULL */, hkpMassProperties* massPropertiesInOut )
{
	hkpCompressedMeshShape* meshShape = HK_NULL;
	{
		// instantiate a compressed mesh shape builder
		hkpCompressedMeshShapeBuilder builder;

		const hkReal quantizationError = 0.001f;
		meshShape = builder.createMeshShape( quantizationError, hkpCompressedMeshShape::MATERIAL_NONE );
		
		// Apply transform if supplied
		hkMatrix4 transform;
		if (globalTransform)
		{
			transform.set( *globalTransform );
		}
		else
		{
			transform.setIdentity();
		}

		// Create the CMS
		const int subpart = builder.beginSubpart( meshShape );
		builder.addGeometry( geometry, transform, meshShape );
		builder.endSubpart( meshShape );
		builder.addInstance( subpart, transform, meshShape );
	}

	// Need this if we plan to make the shape into a dynamic rigid body later
	if (massPropertiesInOut)
	{
		hkInertiaTensorComputer::computeGeometryVolumeMassProperties( &geometry, massPropertiesInOut->m_mass, *massPropertiesInOut );
	}

	if (!meshShape)
	{
		HK_WARN( 0x2beba4b2, "Failed to create Compressed Mesh Shape" );
		return HK_NULL;
	}

	// Build the MOPP
	hkpMoppBvTreeShape* moppShape = HK_NULL;
	{
		hkpMoppCompilerInput mfr;
		mfr.setAbsoluteFitToleranceOfAxisAlignedTriangles( hkVector4( 0.1f, 0.1f, 0.1f ) );
		hkpMoppCode* pCode = hkpMoppUtility::buildCode( meshShape, mfr );
		moppShape = new hkpMoppBvTreeShape( meshShape, pCode );

		// MOPP owns the meshShape
		meshShape->removeReference();

		// Done with MOPP code
		pCode->removeReference();
	}

	return moppShape;
}

void HK_CALL GameUtils::createTower( hkpWorld* world, const hkVector4& position, const hkVector4& halfExtents, hkReal gapWidth, int size, int height )
{
	hkpBoxShape* box = new hkpBoxShape( halfExtents, 0.01f );

	hkVector4 cornerPos = position;

	// do a raycast to place the tower
	{
		hkpWorldRayCastInput ray;
		ray.m_from = cornerPos;
		ray.m_to = cornerPos;

		ray.m_from(1) += 20.0f;
		ray.m_to(1)   -= 20.0f;

		hkpWorldRayCastOutput result;
		world->castRay( ray, result );
		cornerPos.setInterpolate4( ray.m_from, ray.m_to, result.m_hitFraction );
	}

	// move the start point
	cornerPos(0) -= ( gapWidth + 2.0f * halfExtents(0) ) * size * 0.5f;
	cornerPos(2) -= ( gapWidth + 2.0f * halfExtents(0) ) * size * 0.5f;
	cornerPos(1) -= halfExtents(1) + box->getRadius();

	hkpRigidBodyCinfo boxInfo;
	{
		hkpInertiaTensorComputer::setShapeVolumeMassProperties(box, 10.0f, boxInfo);
		boxInfo.m_solverDeactivation = boxInfo.SOLVER_DEACTIVATION_MEDIUM;
		boxInfo.m_shape = box;
		boxInfo.m_qualityType = HK_COLLIDABLE_QUALITY_DEBRIS;
		boxInfo.m_motionType = hkpMotion::MOTION_BOX_INERTIA;
	}
	const hkReal sizem = -0.5f + size;
	{
		for (int z = 0; z < 2; z ++ )
		{
			for ( int x = 0; x < size; x ++ )		// along the ground
			{
				boxInfo.m_position = cornerPos;
				hkReal dx = halfExtents(0)* 2.0f + gapWidth;
				hkReal dy = halfExtents(1)* 2.0f;
				if ( z )
				{
					boxInfo.m_position(0) += dx * sizem;
					boxInfo.m_position(2) += dx * size;
					dx = -dx;
				}
				boxInfo.m_position(0) += x * dx;
				for( int ii = 0; ii < height; ii++ )
				{
					boxInfo.m_position(1) += dy;
					{
						hkpRigidBody* boxRigidBody = new hkpRigidBody(boxInfo);
						world->addEntity( boxRigidBody, HK_ENTITY_ACTIVATION_DO_NOT_ACTIVATE )->removeReference();
					}
					boxInfo.m_position(1) += dy;
					boxInfo.m_position(0) += dx * 0.5f;
					{
						hkpRigidBody* boxRigidBody = new hkpRigidBody(boxInfo);
						world->addEntity( boxRigidBody, HK_ENTITY_ACTIVATION_DO_NOT_ACTIVATE )->removeReference();
					}
					boxInfo.m_position(0) -= dx * 0.5f;
				}
			}
		}
	}
	box->removeReference();

	hkVector4 halfExtents2( halfExtents(2), halfExtents(1), halfExtents(0) );
	boxInfo.m_shape = new hkpBoxShape( halfExtents2, 0.01f );
	hkpInertiaTensorComputer::setShapeVolumeMassProperties(boxInfo.m_shape, 10.0f, boxInfo);
	{
		for (int z = 0; z < 2; z ++ )
		{
			for ( int x = 0; x < size; x ++ )		// along the ground
			{
				hkReal dx = halfExtents(0)* 2.0f + gapWidth;
				hkReal dy = halfExtents(1) * 2.0f;

				boxInfo.m_position = cornerPos;
				boxInfo.m_position(0) -= 0.25f * dx;
				boxInfo.m_position(2) += 0.25f * dx;
				if ( z )
				{
					boxInfo.m_position(2) += dx * sizem;
					dx = -dx;
				}
				else
				{
					boxInfo.m_position(0) += dx * size;
				}
				boxInfo.m_position(2) += x * dx;
				for( int ii = 0; ii < height; ii++ )
				{
					boxInfo.m_position(1) += dy;
					{
						hkpRigidBody* boxRigidBody = new hkpRigidBody(boxInfo);
						world->addEntity( boxRigidBody, HK_ENTITY_ACTIVATION_DO_NOT_ACTIVATE )->removeReference();
					}
					boxInfo.m_position(1) += dy;
					boxInfo.m_position(2) += dx * 0.5f;
					{
						hkpRigidBody* boxRigidBody = new hkpRigidBody(boxInfo);
						world->addEntity( boxRigidBody, HK_ENTITY_ACTIVATION_DO_NOT_ACTIVATE )->removeReference();
					}
					boxInfo.m_position(2) -= dx * 0.5f;
				}
			}
		}
	}
	boxInfo.m_shape->removeReference();
}


hkpMoppBvTreeShape* GameUtils::createMoppShape(int side, const hkVector4& position, hkReal scale)
{
	//
	//	We use a storage mesh in our example, which copies all data.
	//  If you want to share graphics and physics, use the hkpExtendedMeshShape instead
	//
	hkpSimpleMeshShape* meshShape = new hkpSimpleMeshShape( 0.05f );
	{
		meshShape->m_vertices.setSize( side * side );
		for(int i = 0; i < side; i++)
		{
			for (int j = 0; j < side; j++ )
			{
				hkVector4 vertex ( i * 1.0f - side * 0.5f,
					j * 1.0f - side * 0.5f,
					0.6f * hkMath::cos((hkReal)j + i) + 0.3f * hkMath::sin( 2.0f * i) );
				vertex.add4(position);
				vertex.mul4(scale);
				meshShape->m_vertices[i*side + j] = vertex ;
			}
		}

		meshShape->m_triangles.setSize( (side-1) * (side-1) * 2);
		int corner = 0;
		int curTri = 0;
		for(int i = 0; i < side - 1; i++)
		{
			for (int j = 0; j < side - 1; j++ )
			{
				meshShape->m_triangles[curTri].m_a = corner;
				meshShape->m_triangles[curTri].m_b = corner+side;
				meshShape->m_triangles[curTri].m_c = corner+1;
				curTri++;

				meshShape->m_triangles[curTri].m_a = corner+1;
				meshShape->m_triangles[curTri].m_b = corner+side;
				meshShape->m_triangles[curTri].m_c = corner+side+1;
				curTri++;
				corner++;
			}
			corner++;
		}
	}

	hkpStorageExtendedMeshShape* extendedMesh = new hkpStorageExtendedMeshShape();

	hkpExtendedMeshShape::TrianglesSubpart part;
	part.m_numTriangleShapes = meshShape->m_triangles.getSize();
	part.m_numVertices = meshShape->m_vertices.getSize();
	part.m_vertexBase = &meshShape->m_vertices.begin()[0](0);
	part.m_stridingType = hkpExtendedMeshShape::INDICES_INT32;
	part.m_vertexStriding = sizeof(hkVector4);
	part.m_indexBase = meshShape->m_triangles.begin();
	part.m_indexStriding = sizeof(hkpSimpleMeshShape::Triangle);
	extendedMesh->addTrianglesSubpart( part );

	hkpMoppCompilerInput mci;
	mci.m_enableChunkSubdivision = true;

	hkpMoppCode* code = hkpMoppUtility::buildCode( extendedMesh ,mci);

	hkpMoppBvTreeShape* moppShape = new hkpMoppBvTreeShape(extendedMesh, code);
	code->removeReference();
	extendedMesh->removeReference();
	meshShape->removeReference();

	return moppShape;
}

//
hkpMoppBvTreeShape* HK_CALL GameUtils::createMOPPFromGeometry(const hkGeometry& geom, const hkVector4& scale, hkReal radius)
{	
	hkpSimpleMeshShape* meshStorage = new hkpSimpleMeshShape(radius > 0 ? radius : hkConvexShapeDefaultRadius);

	meshStorage->m_vertices.reserveExactly(geom.m_vertices.getSize());
	for(int i=0;i<geom.m_vertices.getSize();++i)
	{
		meshStorage->m_vertices.expandOne().setMul4(geom.m_vertices[i],scale);
	}

	meshStorage->m_triangles.reserveExactly(geom.m_triangles.getSize());
	for(int i=0;i<geom.m_triangles.getSize();++i)
	{
		hkpSimpleMeshShape::Triangle& t = meshStorage->m_triangles.expandOne();
		t.m_weldingInfo=0;
		t.m_a = geom.m_triangles[i].m_a;
		t.m_b = geom.m_triangles[i].m_b;
		t.m_c = geom.m_triangles[i].m_c;
	}

	hkpMoppBvTreeShape* pShape = HK_NULL;	
	hkpMoppCompilerInput mfr;
	mfr.setAbsoluteFitToleranceOfAxisAlignedTriangles( hkVector4( 0.1f, 0.1f, 0.1f ) );
	hkpMoppCode* pCode = hkpMoppUtility::buildCode( meshStorage, mfr );
	pShape = new hkpMoppBvTreeShape( meshStorage, pCode );

	pCode->removeReference();
	meshStorage->removeReference();

	return pShape;
}



/*
class HK_CALL GameUtilesRemapIdListener: public hkpEntityListener, public hkpPhantomListener, hkpWorldDeletionListener
{
public:
GameUtilesRemapIdListener( hkpWorld* world)
{
m_id = 1;
world->addEntityListener( this );
world->addPhantomListener( this );
world->addWorldDeletionListener( this );
}

virtual void entityAddedCallback( hkpEntity* entity )
{
if ( !entity->hasProperty( HK_PROPERTY_DEBUG_DISPLAY_ID ))
{
entity->addProperty( HK_PROPERTY_DEBUG_DISPLAY_ID, m_id++);
}
}

virtual void entityRemovedCallback( hkpEntity* entity ){		}

virtual void phantomAddedCallback( hkpPhantom* phantom )
{
if ( !phantom->hasProperty( HK_PROPERTY_DEBUG_DISPLAY_ID ))
{
phantom->addProperty( HK_PROPERTY_DEBUG_DISPLAY_ID, m_id++);
}

}
/// Called when an phantom is removed from the hkpWorld.
virtual void phantomRemovedCallback( hkpPhantom*  ) {}

virtual void worldDeletedCallback( hkpWorld* world)
{
world->removeEntityListener( this );
world->removePhantomListener( this );
world->removeWorldDeletionListener( this );
delete this;
}

protected:
int m_id;
};

void HK_CALL GameUtils::createRemapDebugDisplayIdsListener( hkpWorld* world )
{
new GameUtilesRemapIdListener( world );
}
*/

/*
 * Havok SDK - NO SOURCE PC DOWNLOAD, BUILD(#20120405)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2012
 * Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
 * Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
 * rights, and intellectual property rights in the Havok software remain in
 * Havok and/or its suppliers.
 * 
 * Use of this software for evaluation purposes is subject to and indicates
 * acceptance of the End User licence Agreement for this product. A copy of
 * the license is included with this software and is also available at www.havok.com/tryhavok.
 * 
 */
