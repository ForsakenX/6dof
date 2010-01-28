debug = 0
debuglvl = 0
debugmask = 0

-- Video configuration
fullscreen = false
width = 1024
height = 768
bpp = 24
vsync = true
gamma = 0 -- if positive, set the screen gamma value
drawnormals = false
drawbasis = false
stereo = false
stereorightcolor = 0 -- 0 = green, 1 = blue, 2 = cyan
stereoeyesep = 20
stereofocaldist = 750
-- Temporary - materials make things really slow right now
materials = false

-- Audio configuration
audiofreq = 44100
audiobufsize = 1024

-- Input configuration
mousesensx = 0.5 -- Negative values will result in an inverted axis
mousesensy = 0.5
joystick = true

-- Level to load
level = 'level/fsknmx.lua'

-- Camera physics (temporary)
accel = 3000
drag = 50
turnaccel = 500
turndrag = 30

-- Temporary options for level/model to load and texture directory
texdir = '/home/pim/src/forsaken/skeleton/Data/Textures'
lvlfile = '/home/pim/src/forsaken/skeleton/Data/Levels/ship/SHIP.MXV'
lvltexdir = '/home/pim/src/forsaken/skeleton/Data/Levels/ship/textures'
