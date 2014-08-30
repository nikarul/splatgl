import os

env = Environment(ENV = {'PATH' : os.environ['PATH'],
                         'TERM' : os.environ['TERM'],
                         'HOME' : os.environ['HOME']})

baseflags = "-fvisibility=internal -fvisibility-inlines-hidden"
CFLAGS = "-fno-common -Wall {}".format(baseflags).split()
CCFLAGS = CFLAGS + [ "-std=c++11" ]
LINKFLAGS = "{} -lGL -lGLU -lSDL2".format(baseflags).split()
CPPPATH = [ "#include",
			"/usr/include/SDL2" ]

env.Append(CFLAGS=CFLAGS)
env.Append(CCFLAGS=CCFLAGS)
env.Append(LINKFLAGS=LINKFLAGS)
env.Append(CPPPATH=CPPPATH)

splat_sources = []
for root, dirs, files in os.walk("./src"):
	sources = filter(lambda filename: filename.lower().endswith(".cpp"), files)
	splat_sources += map(lambda filename: os.path.join(root, filename), sources)

env.SharedLibrary(target = "splat", source = splat_sources)

