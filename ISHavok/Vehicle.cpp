#include "Havok.h"


#include <Physics/Vehicle/AeroDynamics/Default/hkpVehicleDefaultAerodynamics.h>
#include <Physics/Vehicle/DriverInput/Default/hkpVehicleDefaultAnalogDriverInput.h>
#include <Physics/Vehicle/Brake/Default/hkpVehicleDefaultBrake.h>
#include <Physics/Vehicle/Engine/Default/hkpVehicleDefaultEngine.h>
#include <Physics/Vehicle/VelocityDamper/Default/hkpVehicleDefaultVelocityDamper.h>
#include <Physics/Vehicle/Steering/Default/hkpVehicleDefaultSteering.h>
#include <Physics/Vehicle/Suspension/Default/hkpVehicleDefaultSuspension.h>
#include <Physics/Vehicle/Transmission/Default/hkpVehicleDefaultTransmission.h>
#include <Physics/Vehicle/WheelCollide/RayCast/hkpVehicleRayCastWheelCollide.h>
#include <Physics/Vehicle/TyreMarks/hkpTyremarksInfo.h>

class TankSteering: public hkpVehicleDefaultSteering
{
	public:
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE);
		/// Calculates information about the effects of steering on the vehicle.
		virtual void calcSteering(const hkReal deltaTime, const hkpVehicleInstance* vehicle, const hkpVehicleDriverInput::FilteredDriverInputOutput& filteredInfoOutput, SteeringAnglesOutput& steeringOutput );
};

void TankSteering::calcSteering(const hkReal deltaTime, const hkpVehicleInstance* vehicle, const hkpVehicleDriverInput::FilteredDriverInputOutput& filteredInfoOutput, SteeringAnglesOutput& steeringOutput )
{
	hkpVehicleDefaultSteering::calcMainSteeringAngle( deltaTime, vehicle, filteredInfoOutput, steeringOutput );

	// Wheels.
	for (int w_it = 0; w_it < m_doesWheelSteer.getSize(); w_it++)
	{
		if ( m_doesWheelSteer[w_it] )
		{
			steeringOutput.m_wheelsSteeringAngle [w_it] = steeringOutput.m_mainSteeringAngle;
		}
		else
		{
			// Steer with front and back wheels to simulate a tank.
			steeringOutput.m_wheelsSteeringAngle [w_it] = -steeringOutput.m_mainSteeringAngle;
		}
	}
}

void setupComponent( const hkpVehicleData& data, hkpVehicleDefaultAnalogDriverInput& driverInput )
{
// We also use an analog "driver input" class to help converting user input to vehicle behavior.

driverInput.m_slopeChangePointX = 0.8f;
driverInput.m_initialSlope = 0.7f;
driverInput.m_deadZone = 0.0f;
driverInput.m_autoReverse = true;
}

void setupComponent( const hkpVehicleData& data, hkpVehicleDefaultSteering& steering )
{   
steering.m_doesWheelSteer.setSize( data.m_numWheels );

// degrees
steering.m_maxSteeringAngle = 35 * ( HK_REAL_PI / 180 );//PI / 2.0f;

// [mph/h] The steering angle decreases linearly 
// based on your overall max speed of the vehicle. 
steering.m_maxSpeedFullSteeringAngle = 70.0f * (1.605f / 3.6f);
steering.m_doesWheelSteer[0] = true;
steering.m_doesWheelSteer[1] = true;
steering.m_doesWheelSteer[2] = false;
steering.m_doesWheelSteer[3] = false;
}

void setupComponent( const hkpVehicleData& data, hkpVehicleDefaultTransmission& transmission )
{
int numGears = 4;

transmission.m_gearsRatio.setSize( numGears );
transmission.m_wheelsTorqueRatio.setSize( data.m_numWheels );

transmission.m_downshiftRPM = 3500.0f;
transmission.m_upshiftRPM = 6500.0f;

transmission.m_clutchDelayTime = 0.0f;
transmission.m_reverseGearRatio = 1.0f;
transmission.m_gearsRatio[0] = 2.0f;
transmission.m_gearsRatio[1] = 1.5f;
transmission.m_gearsRatio[2] = 1.0f;
transmission.m_gearsRatio[3] = 0.75f;
transmission.m_wheelsTorqueRatio[0] = 0.2f;
transmission.m_wheelsTorqueRatio[1] = 0.2f;
transmission.m_wheelsTorqueRatio[2] = 0.3f;
transmission.m_wheelsTorqueRatio[3] = 0.3f;

const hkReal vehicleTopSpeed = 130.0f; 	 
const hkReal wheelRadius = 0.4f;
const hkReal maxEngineRpm = 7500.0f;
transmission.m_primaryTransmissionRatio = hkpVehicleDefaultTransmission::calculatePrimaryTransmissionRatio( vehicleTopSpeed,
																											wheelRadius,
																											maxEngineRpm,
																											transmission.m_gearsRatio[ numGears - 1 ] );
}

void setupComponent( const hkpVehicleData& data, hkpVehicleDefaultBrake& brake )
{
brake.m_wheelBrakingProperties.setSize( data.m_numWheels );

const float bt = 1500.0f;
brake.m_wheelBrakingProperties[0].m_maxBreakingTorque = bt;
brake.m_wheelBrakingProperties[1].m_maxBreakingTorque = bt;
brake.m_wheelBrakingProperties[2].m_maxBreakingTorque = bt;
brake.m_wheelBrakingProperties[3].m_maxBreakingTorque = bt;

// Handbrake is attached to rear wheels only.
brake.m_wheelBrakingProperties[0].m_isConnectedToHandbrake = false;
brake.m_wheelBrakingProperties[1].m_isConnectedToHandbrake = false;
brake.m_wheelBrakingProperties[2].m_isConnectedToHandbrake = true;
brake.m_wheelBrakingProperties[3].m_isConnectedToHandbrake = true;
brake.m_wheelBrakingProperties[0].m_minPedalInputToBlock = 0.9f;
brake.m_wheelBrakingProperties[1].m_minPedalInputToBlock = 0.9f;
brake.m_wheelBrakingProperties[2].m_minPedalInputToBlock = 0.9f;
brake.m_wheelBrakingProperties[3].m_minPedalInputToBlock = 0.9f;
brake.m_wheelsMinTimeToBlock = 1000.0f;
}

void setupComponent( const hkpVehicleData& data, hkpVehicleDefaultSuspension& suspension ) 
{
suspension.m_wheelParams.setSize( data.m_numWheels );
suspension.m_wheelSpringParams.setSize( data.m_numWheels );

suspension.m_wheelParams[0].m_length = 0.35f;
suspension.m_wheelParams[1].m_length = 0.35f;
suspension.m_wheelParams[2].m_length = 0.35f;
suspension.m_wheelParams[3].m_length = 0.35f;

const float str = 50.0f;
suspension.m_wheelSpringParams[0].m_strength = str;
suspension.m_wheelSpringParams[1].m_strength = str;
suspension.m_wheelSpringParams[2].m_strength = str;
suspension.m_wheelSpringParams[3].m_strength = str;

const float wd = 3.0f; 
suspension.m_wheelSpringParams[0].m_dampingCompression = wd;
suspension.m_wheelSpringParams[1].m_dampingCompression = wd;
suspension.m_wheelSpringParams[2].m_dampingCompression = wd;
suspension.m_wheelSpringParams[3].m_dampingCompression = wd;

suspension.m_wheelSpringParams[0].m_dampingRelaxation = wd;
suspension.m_wheelSpringParams[1].m_dampingRelaxation = wd;
suspension.m_wheelSpringParams[2].m_dampingRelaxation = wd;
suspension.m_wheelSpringParams[3].m_dampingRelaxation = wd;
	
//
// NB: The hardpoints MUST be positioned INSIDE the chassis.
//
{
	/*const hkReal hardPointFrontZ = 1.3f;
	const hkReal hardPointBackZ = -1.1f;
	const hkReal hardPointY = -0.5f;//-0.05f;
	const hkReal hardPointX = 1.1f;*/
	const hkReal hardPointFrontZ = 0.9f;
	const hkReal hardPointBackZ = -1.05f;
	const hkReal hardPointY = 0.6f;
	const hkReal hardPointX = 0.75f;

	suspension.m_wheelParams[0].m_hardpointChassisSpace.set(-hardPointX, hardPointY, hardPointFrontZ); 
	suspension.m_wheelParams[1].m_hardpointChassisSpace.set( hardPointX, hardPointY, hardPointFrontZ); 
	suspension.m_wheelParams[2].m_hardpointChassisSpace.set(-hardPointX, hardPointY, hardPointBackZ); 
	suspension.m_wheelParams[3].m_hardpointChassisSpace.set( hardPointX, hardPointY, hardPointBackZ); 
}

const hkVector4 downDirection( 0.0f, -1.0f, 0.0f );
suspension.m_wheelParams[0].m_directionChassisSpace = downDirection;
suspension.m_wheelParams[1].m_directionChassisSpace = downDirection;
suspension.m_wheelParams[2].m_directionChassisSpace = downDirection;
suspension.m_wheelParams[3].m_directionChassisSpace = downDirection;
}

void setupComponent( const hkpVehicleData& data, hkpVehicleDefaultAerodynamics& aerodynamics ) 
{
aerodynamics.m_airDensity = 1.3f;
// In m^2.
aerodynamics.m_frontalArea = 1.0f;	

aerodynamics.m_dragCoefficient = 0.7f;
aerodynamics.m_liftCoefficient = -0.3f;

// Extra gavity applies in world space (independent of m_chassisCoordinateSystem).
aerodynamics.m_extraGravityws.set( 0.0f, -5.0f, 0.0f); 
}

void setupComponent( const hkpVehicleData& data, hkpVehicleDefaultVelocityDamper& velocityDamper )
{
// Caution: setting negative damping values will add energy to system. 
// Setting the value to 0 will not affect the angular velocity. 

// Damping the change of the chassis angular velocity when below m_collisionThreshold. 
// This will affect turning radius and steering.
velocityDamper.m_normalSpinDamping    = 0.0f; 

// Positive numbers dampen the rotation of the chassis and 
// reduce the reaction of the chassis in a collision. 
velocityDamper.m_collisionSpinDamping = 4.0f; 

// The threshold in m/s at which the algorithm switches from 
// using the normalSpinDamping to the collisionSpinDamping. 	
velocityDamper.m_collisionThreshold   = 1.0f; 
}

void setupWheelCollide( const hkpWorld* world, const hkpVehicleInstance& vehicle, hkpVehicleRayCastWheelCollide& wheelCollide )
{
// Set the wheels to have the same collision filter info as the chassis.
wheelCollide.m_wheelCollisionFilterInfo = vehicle.getChassis()->getCollisionFilterInfo();
}

void setupTyremarks( const hkpVehicleData& data, hkpTyremarksInfo& tyreMarks ) 
{
tyreMarks.m_minTyremarkEnergy = 100.0f;
tyreMarks.m_maxTyremarkEnergy = 1000.0f;
}

void buildVehicle( const hkpWorld* world, hkpVehicleInstance& vehicle )
{
//
// All memory allocations are made here.
//

vehicle.m_driverInput	 = new hkpVehicleDefaultAnalogDriverInput;
vehicle.m_steering		 = new hkpVehicleDefaultSteering;//TankSteering;
vehicle.m_transmission	 = new hkpVehicleDefaultTransmission;
vehicle.m_brake			 = new hkpVehicleDefaultBrake;
vehicle.m_suspension	 = new hkpVehicleDefaultSuspension;
vehicle.m_aerodynamics	 = new hkpVehicleDefaultAerodynamics;
vehicle.m_velocityDamper = new hkpVehicleDefaultVelocityDamper;
vehicle.m_deviceStatus	 = new hkpVehicleDriverInputAnalogStatus;

// For illustrative purposes we use a custom hkpVehicleRayCastWheelCollide
// which implements varying 'ground' friction in a very simple way.
vehicle.m_wheelCollide		= new hkpVehicleRayCastWheelCollide;//FrictionMapVehicleRaycastWheelCollide;

// initialise the tyremarks controller with 128 tyremark points.
vehicle.m_tyreMarks		= new hkpTyremarksInfo( *vehicle.m_data, 128 );

setupComponent( *vehicle.m_data, *static_cast< hkpVehicleDefaultAnalogDriverInput* >(vehicle.m_driverInput) );
setupComponent( *vehicle.m_data, *static_cast< hkpVehicleDefaultSteering*>(vehicle.m_steering));
setupComponent( *vehicle.m_data, *static_cast< hkpVehicleDefaultTransmission*>(vehicle.m_transmission) );
setupComponent( *vehicle.m_data, *static_cast< hkpVehicleDefaultBrake*>(vehicle.m_brake) );
setupComponent( *vehicle.m_data, *static_cast< hkpVehicleDefaultSuspension*>(vehicle.m_suspension) );
setupComponent( *vehicle.m_data, *static_cast< hkpVehicleDefaultAerodynamics*>(vehicle.m_aerodynamics) );
setupComponent( *vehicle.m_data, *static_cast< hkpVehicleDefaultVelocityDamper*>(vehicle.m_velocityDamper) );

setupWheelCollide( world, vehicle, *static_cast< hkpVehicleRayCastWheelCollide*>(vehicle.m_wheelCollide) );

setupTyremarks( *vehicle.m_data, *static_cast< hkpTyremarksInfo*>(vehicle.m_tyreMarks) );
}

void SetupVehicleData(hkpVehicleData& data, const VehicleDesc& vehicledesc, const hkpWorld* world);
void SetupEngine(const hkpVehicleData& data, const VehicleDesc& vehicledesc, hkpVehicleDefaultEngine& engine);

//-----------------------------------------------------------------------------
// Name: Vehicle()
// Desc: Constructor
//-----------------------------------------------------------------------------
Vehicle::Vehicle(Havok* parent) : parent(parent)
{
	acceleration = 0.0f;
	steering = 0.0f;
	handbrakepulled = false;
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Create the vehicle
//-----------------------------------------------------------------------------
Result Vehicle::Init(const VehicleDesc* vehicledesc, Vector3* vpos, Quaternion* qrot)
{
	Result rlt;

	this->vpos = vpos;
	this->qrot = qrot;

	// Create chassis shape
	hkpShape* chassisshape;
	CHKRESULT(Havok::CreateShape(vehicledesc->chassis, &chassisshape));

	// Create chassis rigid body info
	hkpRigidBody* chassisrigidbody;

	hkpRigidBodyCinfo binfo;
	binfo.m_mass = 750.0f;
	binfo.m_shape = chassisshape;
	binfo.m_friction = 0.8f;
	binfo.m_motionType = hkpMotion::MOTION_BOX_INERTIA;

	// Set initial position and rotation
	hkVector4 hkvpos = *vpos;
	binfo.m_position = hkvpos;
	hkQuaternion hkqrot = *qrot;
	binfo.m_rotation = hkqrot;

	binfo.m_inertiaTensor.setDiagonal(1.0f, 1.0f, 1.0f);
	binfo.m_centerOfMass.set(0.0f, 0.143f, -0.037f);
	binfo.m_collisionFilterInfo = hkpGroupFilter::calcFilterInfo(IHavok::LAYER_MOVABLE_ENVIRONMENT, 0);

	// Create chassis rigid body
	chassisrigidbody = new hkpRigidBody(binfo);
	chassisshape->removeReference();

	// Create vehicle
	vehicle = new hkpVehicleInstance(chassisrigidbody);
	vehicle->m_data = new hkpVehicleData;
	SetupVehicleData(*vehicle->m_data, *vehicledesc, parent->world);
	vehicle->m_engine = new hkpVehicleDefaultEngine;
	SetupEngine(*vehicle->m_data, *vehicledesc, *(hkpVehicleDefaultEngine*)vehicle->m_engine);
	buildVehicle(parent->world, *vehicle);
	vehicle->init();

	// Add vehicle to world
	parent->world->lock();
	vehicle->addToWorld(parent->world);
	parent->world->addAction(vehicle);
	chassisrigidbody->removeReference();
	parent->world->unlock();

	return R_OK;
}

#pragma region Setup functions

void SetupVehicleData(hkpVehicleData& data, const VehicleDesc& vehicledesc, const hkpWorld* world)
{
	data.m_gravity = world->getGravity();
	// Set chassis coordinate system: up				  forward			  right
	data.m_chassisOrientation.setCols(hkVector4(0, 1, 0), hkVector4(0, 0, 1), hkVector4(-1, 0, 0));

	data.m_frictionEqualizer = 0.5f; 

	// The inertia tensor for each axis is calculated by (1 / chassis_mass) * (torque_axis_factor / chassis_unit_inertia)
	data.m_torqueRollFactor = 0.625f;
	data.m_torquePitchFactor = 0.5f;
	data.m_torqueYawFactor = 0.35f;

	data.m_chassisUnitInertiaYaw = 1.0f; 
	data.m_chassisUnitInertiaRoll = 1.0f;
	data.m_chassisUnitInertiaPitch = 1.0f; 

	// Add or remove torque around the yaw axis based on the current steering angle (affecting steering)
	data.m_extraTorqueFactor = -0.5f; 
	data.m_maxVelocityForPositionalFriction = 0.0f; 

	// >>> Wheel specifications

	data.m_wheelParams.setSize(data.m_numWheels = vehicledesc.wheels.size());

	for(int i = 0; i < data.m_numWheels; i++)
	{
		data.m_wheelParams[i].m_axle = vehicledesc.wheels[i].axle;
		data.m_wheelParams[i].m_friction = vehicledesc.wheels[i].friction;
		data.m_wheelParams[i].m_slipAngle = vehicledesc.wheels[i].slipangle;

		data.m_wheelParams[i].m_radius = vehicledesc.wheels[i].radius; // This value is also used to calculate m_primaryTransmissionRatio
		data.m_wheelParams[i].m_width = vehicledesc.wheels[i].width;
		data.m_wheelParams[i].m_mass = vehicledesc.wheels[i].mass;

		data.m_wheelParams[i].m_viscosityFriction = vehicledesc.wheels[i].viscosityfriction;
		data.m_wheelParams[i].m_maxFriction = vehicledesc.wheels[i].maxfriction;
		data.m_wheelParams[i].m_forceFeedbackMultiplier = vehicledesc.wheels[i].forcefeedbackmultiplier;
		data.m_wheelParams[i].m_maxContactBodyAcceleration = (hkReal)data.m_gravity.length3() * 2.0f;
	}
}

void SetupEngine(const hkpVehicleData& data, const VehicleDesc& vehicledesc, hkpVehicleDefaultEngine& engine)
{
	engine.m_maxTorque = vehicledesc.engine.maxtorque;

	engine.m_minRPM = vehicledesc.engine.minrpm;
	engine.m_optRPM = vehicledesc.engine.optrpm;

	engine.m_maxRPM = vehicledesc.engine.maxrpm; // This value is also used to calculate m_primaryTransmissionRatio

	engine.m_torqueFactorAtMinRPM = vehicledesc.engine.torquefactor_minrpm;
	engine.m_torqueFactorAtMaxRPM = vehicledesc.engine.torquefactor_maxrpm;

	engine.m_resistanceFactorAtMinRPM = vehicledesc.engine.resistencefactor_minrpm;
	engine.m_resistanceFactorAtOptRPM = vehicledesc.engine.resistencefactor_optrpm;
	engine.m_resistanceFactorAtMaxRPM = vehicledesc.engine.resistencefactor_maxrpm;
}

#pragma endregion

//-----------------------------------------------------------------------------
// Name: PreUpdate()
// Desc: ?
//-----------------------------------------------------------------------------
void Vehicle::PreUpdate()
{
	parent->world->markForWrite();

	if(acceleration || steering)
		vehicle->getChassis()->activate();
	hkpVehicleDriverInputAnalogStatus* deviceStatus = (hkpVehicleDriverInputAnalogStatus*)vehicle->m_deviceStatus;
	deviceStatus->m_positionY = acceleration;
	deviceStatus->m_positionX = steering;
	deviceStatus->m_reverseButtonPressed = false;
	deviceStatus->m_handbrakeButtonPressed = handbrakepulled;

	parent->world->unmarkForWrite();
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: ?
//-----------------------------------------------------------------------------
void Vehicle::Update()
{
	hkVector4 pos = vehicle->getChassis()->getPosition();
	vpos->x = pos.getComponent(0);
	vpos->y = pos.getComponent(1);
	vpos->z = pos.getComponent(2);

	hkQuaternion rot = vehicle->getChassis()->getRotation();
	qrot->x = rot(0);
	qrot->y = rot(1);
	qrot->z = rot(2);
	qrot->w = rot(3);
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void Vehicle::Release()
{
	parent->vehicles.remove(this);

	parent->world->lock();
	parent->world->removeAction(vehicle);
	/*vehicle->removeFromWorld();*/ //EDIT: Fails when the entity already left the broadphase
	vehicle->removeReference();
	parent->world->unlock();

	delete this;
}