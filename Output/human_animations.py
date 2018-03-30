from Havok import *
from Direct3D import Vector3, Quaternion
from math import *

def CreateAnimations(ragdoll):
	#print(ani)
	#print(ani.foo())
	#f = Foo()
	#print(f.foo())
	#print(ani.enabled)

	ani = ragdoll.CreateAnimation(1.0)

	frames = []
	for i in range(3):
		frames.append(ragdoll.CreatePose())

	for frame in frames:
		ani.AddFrame(frame)
	ani.enabled = True
	return ani




def SET_BONE_ROTATION(pose, boneidx, rotx, roty, rotz):
	rotX = Quaternion.RotationYawPitchRoll(rotx * pi / 180.0, 0.0, 0.0)
	rotY = Quaternion.RotationYawPitchRoll(0.0, roty * pi / 180.0, 00.0)
	rotZ = Quaternion.RotationYawPitchRoll(0.0, 0.0, rotz * pi / 180.0)
	pose.SetRotation(rotY * rotX * rotZ, boneidx)

def TORSOR_POS(time):
	return 0.0148147555844855 * sin(1.3996930828088 * (time) - 0.92081890941623) - 0.0149605824201595

def CreateAnimation(ragdoll, name):
	print("Creating animation \"" + name + "\"")

	if name == "Idle":
		ani = ragdoll.CreateAnimation(1.0)

		frames = []
		for i in range(2):
			frames.append(ragdoll.CreatePose())

		for frame in frames:
			SET_BONE_ROTATION(frame, 1, 0.0, 0.0, 70.0) # Upper left arm
			SET_BONE_ROTATION(frame, 2, 0.0, 0.0, 20.0) # Lower left arm
			SET_BONE_ROTATION(frame, 3, 0.0, 0.0, -70.0) # Upper right arm
			SET_BONE_ROTATION(frame, 4, 0.0, 0.0, -20.0) # Lower right arm

			ani.AddFrame(frame)
		ani.enabled = True

	elif name == "Walking":
		ani = ragdoll.CreateAnimation(2.0)

		frames = []
		for i in range(9):
			frames.append(ragdoll.CreatePose())

		# Frame 0
		SET_BONE_ROTATION(frames[0], 5, 0.0, -30.0, 0.0) # Upper left leg
		SET_BONE_ROTATION(frames[0], 6, 0.0, 25.0, 0.0) # Lower left leg
		SET_BONE_ROTATION(frames[0], 7, 0.0, 15.0, 0.0) # Upper right leg
		SET_BONE_ROTATION(frames[0], 8, 0.0, 20.0, 0.0) # Lower right leg

		SET_BONE_ROTATION(frames[0], 1, -15.0, 0.0, 70.0) # Upper left arm
		SET_BONE_ROTATION(frames[0], 2, 0.0, 0.0, 20.0) # Lower left arm
		SET_BONE_ROTATION(frames[0], 3, -30.0, 0.0, -70.0) # Upper right arm
		SET_BONE_ROTATION(frames[0], 4, 0.0, 0.0, -20.0) # Lower right arm

		# Frame 1
		SET_BONE_ROTATION(frames[1], 5, 0.0, -15.0, 0.0) # Upper left leg
		SET_BONE_ROTATION(frames[1], 6, 0.0, 15.0, 0.0) # Lower left leg
		SET_BONE_ROTATION(frames[1], 7, 0.0, 0.0, 0.0) # Upper right leg
		SET_BONE_ROTATION(frames[1], 8, 0.0, 40.0, 0.0) # Lower right leg

		SET_BONE_ROTATION(frames[1], 1, 0.0, 0.0, 70.0) # Upper left arm
		SET_BONE_ROTATION(frames[1], 2, 0.0, 0.0, 20.0) # Lower left arm
		SET_BONE_ROTATION(frames[1], 3, -15.0, 0.0, -70.0) # Upper right arm
		SET_BONE_ROTATION(frames[1], 4, 0.0, 0.0, -20.0) # Lower right arm

		# Frame 2
		SET_BONE_ROTATION(frames[2], 5, 0.0, 0.0, 0.0) # Upper left leg
		SET_BONE_ROTATION(frames[2], 6, 0.0, 0.0, 0.0) # Lower left leg
		SET_BONE_ROTATION(frames[2], 7, 0.0, -15.0, 0.0) # Upper right leg
		SET_BONE_ROTATION(frames[2], 8, 0.0, 65.0, 0.0) # Lower right leg

		SET_BONE_ROTATION(frames[2], 1, 15.0, 0.0, 70.0) # Upper left arm
		SET_BONE_ROTATION(frames[2], 2, 0.0, 0.0, 20.0) # Lower left arm
		SET_BONE_ROTATION(frames[2], 3, 0.0, 0.0, -70.0) # Upper right arm
		SET_BONE_ROTATION(frames[2], 4, 0.0, 0.0, -20.0) # Lower right arm

		# Frame 3
		SET_BONE_ROTATION(frames[3], 5, 0.0, 15.0, 0.0) # Upper left leg
		SET_BONE_ROTATION(frames[3], 6, 0.0, 5.0, 0.0) # Lower left leg
		SET_BONE_ROTATION(frames[3], 7, 0.0, -35.0, 0.0) # Upper right leg
		SET_BONE_ROTATION(frames[3], 8, 0.0, 40.0, 0.0) # Lower right leg

		SET_BONE_ROTATION(frames[3], 1, 35.0, 0.0, 70.0) # Upper left arm
		SET_BONE_ROTATION(frames[3], 2, 0.0, 0.0, 20.0) # Lower left arm
		SET_BONE_ROTATION(frames[3], 3, 15.0, 0.0, -70.0) # Upper right arm
		SET_BONE_ROTATION(frames[3], 4, 0.0, 0.0, -20.0) # Lower right arm

		# Frame 4
		SET_BONE_ROTATION(frames[4], 5, 0.0, 25.0, 0.0) # Upper left leg
		SET_BONE_ROTATION(frames[4], 6, 0.0, 0.0, 0.0) # Lower left leg
		SET_BONE_ROTATION(frames[4], 7, 0.0, -30.0, 0.0) # Upper right leg
		SET_BONE_ROTATION(frames[4], 8, 0.0, 25.0, 0.0) # Lower right leg

		SET_BONE_ROTATION(frames[4], 1, 30.0, 0.0, 70.0) # Upper left arm
		SET_BONE_ROTATION(frames[4], 2, 0.0, 0.0, 20.0) # Lower left arm
		SET_BONE_ROTATION(frames[4], 3, 25.0, 0.0, -70.0) # Upper right arm
		SET_BONE_ROTATION(frames[4], 4, 0.0, 0.0, -20.0) # Lower right arm

		# Frame 5
		SET_BONE_ROTATION(frames[5], 5, 0.0, 15.0, 0.0) # Upper left leg
		SET_BONE_ROTATION(frames[5], 6, 0.0, 20.0, 0.0) # Lower left leg
		SET_BONE_ROTATION(frames[5], 7, 0.0, -15.0, 0.0) # Upper right leg
		SET_BONE_ROTATION(frames[5], 8, 0.0, 15.0, 0.0) # Lower right leg

		SET_BONE_ROTATION(frames[5], 1, 15.0, 0.0, 70.0) # Upper left arm
		SET_BONE_ROTATION(frames[5], 2, 0.0, 0.0, 20.0) # Lower left arm
		SET_BONE_ROTATION(frames[5], 3, 15.0, 0.0, -70.0) # Upper right arm
		SET_BONE_ROTATION(frames[5], 4, 0.0, 0.0, -20.0) # Lower right arm

		# Frame 6
		SET_BONE_ROTATION(frames[6], 5, 0.0, 0.0, 0.0) # Upper left leg
		SET_BONE_ROTATION(frames[6], 6, 0.0, 40.0, 0.0) # Lower left leg
		SET_BONE_ROTATION(frames[6], 7, 0.0, 0.0, 0.0) # Upper right leg
		SET_BONE_ROTATION(frames[6], 8, 0.0, 0.0, 0.0) # Lower right leg

		SET_BONE_ROTATION(frames[6], 1, 0.0, 0.0, 70.0) # Upper left arm
		SET_BONE_ROTATION(frames[6], 2, 0.0, 0.0, 20.0) # Lower left arm
		SET_BONE_ROTATION(frames[6], 3, 0.0, 0.0, -70.0) # Upper right arm
		SET_BONE_ROTATION(frames[6], 4, 0.0, 0.0, -20.0) # Lower right arm

		# Frame 7
		SET_BONE_ROTATION(frames[7], 5, 0.0, -15.0, 0.0) # Upper left leg
		SET_BONE_ROTATION(frames[7], 6, 0.0, 65.0, 0.0) # Lower left leg
		SET_BONE_ROTATION(frames[7], 7, 0.0, 15.0, 0.0) # Upper right leg
		SET_BONE_ROTATION(frames[7], 8, 0.0, 5.0, 0.0) # Lower right leg

		SET_BONE_ROTATION(frames[7], 1, -15.0, 0.0, 70.0) # Upper left arm
		SET_BONE_ROTATION(frames[7], 2, 0.0, 0.0, 20.0) # Lower left arm
		SET_BONE_ROTATION(frames[7], 3, -15.0, 0.0, -70.0) # Upper right arm
		SET_BONE_ROTATION(frames[7], 4, 0.0, 0.0, -20.0) # Lower right arm

		# Frame 8
		SET_BONE_ROTATION(frames[8], 5, 0.0, -35.0, 0.0) # Upper left leg
		SET_BONE_ROTATION(frames[8], 6, 0.0, 40.0, 0.0) # Lower left leg
		SET_BONE_ROTATION(frames[8], 7, 0.0, 25.0, 0.0) # Upper right leg
		SET_BONE_ROTATION(frames[8], 8, 0.0, 0.0, 0.0) # Lower right leg

		SET_BONE_ROTATION(frames[8], 1, -35.0, 0.0, 70.0) # Upper left arm
		SET_BONE_ROTATION(frames[8], 2, 0.0, 0.0, 20.0) # Lower left arm
		SET_BONE_ROTATION(frames[8], 3, -25.0, 0.0, -70.0) # Upper right arm
		SET_BONE_ROTATION(frames[8], 4, 0.0, 0.0, -20.0) # Lower right arm

		# Torsor movement
		for i in range(len(frames)):
			frames[i].SetPosition(Vector3(0.0, max(TORSOR_POS(i / 9.0), TORSOR_POS(((i + 4) % 9) / 9.0)), 0.0), 0)
		frames[0].SetPosition(Vector3(0.0, TORSOR_POS(0.0), 0.0), 0)

		for frame in frames:
			ani.AddFrame(frame)
		ani.AddFrame(frames[0])
		ani.enabled = True

	else:
		ani = None

	return ani