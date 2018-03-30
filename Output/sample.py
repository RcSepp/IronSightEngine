from Engine import *
from DirectInput import *
from Direct3D import *
#from Havok import *

abc = 0

def Render(): #d3dwnd
	print("Render()")
	#global abc
	#abc += 1
	#if abc < 10:
	#	sdesc = D3dSphereShapeDesc()
	#	sdesc.radius = 1.0
	#	sdesc.slices = sdesc.stacks = 10
	#	obj = d3dwnd.CreateObject(sdesc)
	#	import random
	#	obj.pos = Vector3(0, 0, 0)
	#	obj.x = random.randint(-100, 100) / 20.0
	#	print(obj.x)
	#	d3dwnd.RegisterClassForRendering(obj, RenderType.Default)
	#	obj.SetShader(sdr)

def OnSpace(key, user):
	d3dwnd.RegisterFunctionForRendering(Render, RenderType.Default)

	## Create object from file
	#obj2_clone = d3dwnd.CreateObject(obj2, False)
	##obj2_clone.pos = Vector3(0, 5, 5)
	#obj2_clone.SetShader(sdrTex)
	#hvk.CreateConvexCollisionHull(obj2_clone)
	#d3dwnd.RegisterClassForRendering(obj2_clone, RenderType.Default)

def OnEnter(key, user):
	sdesc = SphereShapeDesc()
	sdesc.radius = 1.0
	sdesc.slices = sdesc.stacks = 10
	obj = d3dwnd.CreateObject(sdesc)
	d3dwnd.RegisterClassForRendering(obj, RenderType.Default)
	import random
	obj.pos = Vector3(random.randint(-10, 10), random.randint(-10, 10), random.randint(0, 20))
	obj.SetShader(sdr)


# >>> Init DirectInput

dip.Init()
dip.EnableMouse(True, False)
dip.EnableKeyboard(True, False)
dip.AddKeyHandler(Key.Space, OnSpace, None)
dip.AddKeyHandler(Key.Return, OnEnter, None)


# >>> Init Direct3D

settings = OutputWindowSettings()
settings.backbuffersize.width = 640
settings.backbuffersize.height = 480
settings.screenidx = 0
settings.wnd = MAIN_WINDOW
d3dwnd = d3d.CreateOutputWindow(settings)

d3dwnd.MakeConsoleTarget(11, 11)
d3dwnd.ShowFps()

# Create camera
cam = d3d.CreateCamera(0.0, 0.001, 200.0)
cam.pos = Vector3(0.0, 0.0, -2.0)
cam.viewmode = ViewMode.Fly
d3dwnd.SetCamera(cam)

# Create shader
sdr = d3dwnd.CreateRenderShader("shader withblending.fx")
sdrTex = d3dwnd.CreateRenderShader("K:\\Resources\\Shader\\Default.fx")

# Create sphere
sdesc = D3dSphereShapeDesc()
sdesc.radius = 1.0
sdesc.slices = sdesc.stacks = 10
obj = d3dwnd.CreateObject(sdesc)
d3dwnd.RegisterClassForRendering(obj, RenderType.Default)
obj.SetShader(sdr)

# Create weapon (without registering it)
obj2 = d3dwnd.CreateObject("K:\\Resources\\Objects\\G36C.x", True)
obj2.ComputeNormals()
obj2.CommitChanges()
obj2.pos = Vector3(0, 5, 5)

# Create level
#objLevel = d3dwnd.CreateObject("Level\\Level.x", True)
#d3dwnd.RegisterClassForRendering(objLevel, RenderType.Default)
#objLevel.SetShader(sdr)

# Create SkyBox
skybox = d3dwnd.CreateSkyBox()
skybox.CreateLayer("SkyBox\\North_Sky.bmp", "SkyBox\\East_Sky.bmp", "SkyBox\\South_Sky.bmp", "SkyBox\\West_Sky.bmp", "SkyBox\\Top.bmp", "SkyBox\\Bottom.bmp", 0)
skybox.CreateLayer("SkyBox\\North_Scene.dds", "SkyBox\\East_Scene.dds", "SkyBox\\South_Scene.dds", "SkyBox\\West_Scene.dds", "", "", 8)
d3dwnd.RegisterClassForRendering(skybox, RenderType.Background)
#d3dwnd.SetBackColor(false, (DWORD)0) Disable screen clearing

# Create HUD
tex = d3dwnd.CreateTexture("Pause.png", TextureUsage.Skin_NoMipmap, True)
hud = d3dwnd.CreateHUD()
hud.CreateElement(tex, 20, 20, DockType.TopLeft, Color(0xFFFFFFFF))
d3dwnd.RegisterClassForRendering(hud, RenderType.Foreground)


# >>> Init Havok

# sdrPhysics = d3dwnd.CreateRenderShader("shader physics.fx")
# if False:
	# hvk.Init(VisualizationType.Local, d3dwnd, sdrPhysics)
	# hvk.EnableViewer(HvkViewer.BroadPhase)
	# hvk.EnableViewer(HvkViewer.Constraint)
	# hvk.EnableViewer(HvkViewer.ActiveContactPoint)
	# hvk.EnableViewer(HvkViewer.InactiveContactPoint)
	# hvk.EnableViewer(HvkViewer.ConvexRadius)
	# hvk.EnableViewer(HvkViewer.InconsistendWinding)
	# hvk.EnableViewer(HvkViewer.MidPhase)
	# hvk.EnableViewer(HvkViewer.MousePicking)
	# hvk.EnableViewer(HvkViewer.PhantomDisplay)
	# hvk.EnableViewer(HvkViewer.RigidBodyCenterOfMass)
	# hvk.EnableViewer(HvkViewer.RigidBodyInertia)
	# hvk.EnableViewer(HvkViewer.ShapeDisplay)
	# hvk.EnableViewer(HvkViewer.SimulationIsland)
	# hvk.EnableViewer(HvkViewer.SingleBodyConstraint)
	# hvk.EnableViewer(HvkViewer.SweptTransformDisplay)
	# hvk.EnableViewer(HvkViewer.ToiContactPoint)
	# hvk.EnableViewer(HvkViewer.ToiCount)
	# hvk.EnableViewer(HvkViewer.Vehicle)
	# hvk.EnableViewer(HvkViewer.Welding)
# else:
	# hvk.Init(VisualizationType.Disabled, None, None)
# hvk.InitDone()

# desc = HvkPlaneShapeDesc()
# desc.mtype = MotionType.Fixed
# desc.vcenter = Vector3(0.0, 0.0, 0.0);
# desc.vdir = Vector3(0.0, 1.0, 0.0);
# desc.vhalfext = Vector3(50.0, 50.0, 50.0);
# entityFloor = hvk.CreateRegularEntity(desc, Layer.Landscape, Vector3(0.0, 0.0, 0.0), Quaternion(0.0, 0.0, 0.0, 1.0))