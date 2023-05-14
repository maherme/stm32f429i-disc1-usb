from waflib.Configure import conf

# Add misc gcc flags

common_flags = [
    "-g",
    "-O0",
    "-ffunction-sections",
    "-fdata-sections",
    "-Wall",
    "-Wunused-parameter",
]

@conf
def add_gcc_flags(conf):
    conf.env.CPPFLAGS = common_flags
    conf.env.ASFLAGS = common_flags
    conf.env.CFLAGS = ["-std=c11"]
    conf.env.CXXFLAGS = ["-std=c++17"]

def configure(conf):
    conf.add_gcc_flags()