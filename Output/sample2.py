from Engine import *
from XAudio2 import *

# >>> Init WinAudio

xa2.Init()
xa2.volume = 0.1
##formats = wad.GetSupportedOutputFormats(0)
#wadoutdev = wad.OpenOutputDevice(0, AudioFormat.FromChannelMask(14914, 8, 4))
#wadoutdev.Volume = 0xA000;
sound = xa2.CreateSound("H:\\Games\\Portable DX-Ball 2\\Sounds\\Bang.wav")
#str = AudioFormat.FromChannelMask(14914, 8, 4)
#print(str)
sound.Stop()
sound.Play()