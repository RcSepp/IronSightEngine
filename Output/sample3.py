from Engine import *
from Forms import *

# >>> Init Forms

def Form_MouseDown(sender, args, user):
	print(args.ldown)

def Form_MouseMove(sender, args, user):
	print(args.mousepos.x)

frmsettings = FormSettings()
frmsettings.hashscroll = False
frmsettings.hasvscroll = False
frmsettings.fbstyle = FormBorderStyle.Default
frmsettings.clientsize = SizeI(800, 600)
frmsettings.caption = "Iron Sight Engine - Window 2"
frmsettings.windowpos = PointI(0, 0)
frmsettings.options = 0
frmsettings.parent = None
frmsettings.mdiwindowmenu = None

form = fms.CreateForm(frmsettings)
form.SetMouseDownCallback(Form_MouseDown, None)
form.SetMouseMoveCallback(Form_MouseMove, None)