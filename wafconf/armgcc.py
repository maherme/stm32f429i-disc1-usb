from waflib.Configure import conf

@conf
def find_armgcc(conf):
    conf.find_program("arm-none-eabi-gcc", var="AS")
    conf.find_program("arm-none-eabi-ar", var="AR")
    conf.find_program("arm-none-eabi-gcc", var="LINK_CC")
    conf.find_program("arm-none-eabi-objcopy", var="OBJCOPY")
    conf.find_program("arm-none-eabi-gcc", var="CC")
    conf.find_program("arm-none-eabi-g++", var="CXX")
    conf.find_program("arm-none-eabi-c++", var="LINK_CXX")
    conf.env.CC_NAME = "armgcc"

    conf.get_cc_version(conf.env.CC, gcc=True)

def configure(conf):
 #   libs       = ["c", "nosys", "m"]
    c_flags    = [""]
    linker_script = conf.path.abspath() + '/lnk/STM32F429ZITX.ld'
    link_flags = [
        "-Wl,--gc-sections",
        "-Wl,--print-memory-usage",
        # To avoid a warning in Binutils 2.39 "elf has a LOAD segment with RWX permissions"
        "-Wl,--no-warn-rwx-segments",
        "--specs=nosys.specs",
        "--specs=nano.specs",
        "-static",
        "-Wl,-Map=stm32f429i-disc1.map",
        "-T" + linker_script
    ]

    # configure cc for arm
    conf.load("c_config")
    conf.find_armgcc()
    conf.cc_add_flags()
    conf.link_add_flags()
    conf.load("asm")

    conf.env.ARFLAGS = ['rcs']

    conf.env.CC_TGT_F            = ['-c', '-o']
    conf.env.AS_TGT_F            = ["-c", "-o"]
    conf.env.ASLNK_TGT_F         = ["-o"]
    conf.env.CCLNK_TGT_F         = ['-o']
    conf.env.CPPPATH_ST          = '-I%s'
    conf.env.DEFINES_ST          = '-D%s'
    conf.env.LIB_ST              = '-l%s' # template for adding libs
    conf.env.LIBPATH_ST          = '-L%s' # template for adding libpaths

    conf.env.CPPFLAGS.extend(c_flags)
#    conf.env.CXXFLAGS.extend("-fno-exceptions")
#    conf.env.LIB          = libs
    conf.env.LINKFLAGS    = link_flags
    conf.env.CXXLNK_TGT_F = ['-o']
    conf.env.CXX_TGT_F    = ['-c', '-o']
    conf.env.INCLUDES     = []
    conf.env.DEFINES      = ["STM32F429xx"]
