import os

env = Environment(ENV = {'PATH' : os.environ['PATH'],
#                         'TERM' : os.environ['TERM'],
                         'HOME' : os.environ['HOME']})

TARGET = ARGUMENTS.get("TARGET", "debug")
if TARGET not in [ "debug", "release" ]:
	print("Invalid TARGET")
	Exit(1)

baseflags = "-fvisibility=internal"
CFLAGS = "-std=c99 -fno-common -Wall {}".format(baseflags).split()
LINKFLAGS = "{} -lGL -lGLU -lSDL2".format(baseflags).split()

if TARGET == "debug":
	CFLAGS += ["-O0", "-g"]
	LINKFLAGS += [ "-g" ]
else:
	CFLAGS += [ "-O3" ]

CPPPATH = [ "#include",
			"/usr/include/SDL2" ]

env.Append(CFLAGS=CFLAGS)
env.Append(LINKFLAGS=LINKFLAGS)
env.Append(CPPPATH=CPPPATH)

if "CC" in os.environ:
    env.Replace(CC = os.environ["CC"])

splat_sources = []
for root, dirs, files in os.walk("./src"):
	sources = filter(lambda filename: filename.lower().endswith(".c"), files)
	splat_sources += map(lambda filename: os.path.join(root, filename), sources)

env.SharedLibrary(target = "splat", source = splat_sources)

