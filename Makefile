SIM_ROOT ?= $(shell readlink -f "$(CURDIR)")

CLEAN=$(findstring clean,$(MAKECMDGOALS))

STANDALONE=$(SIM_ROOT)/lib/sniper
PIN_FRONTEND=$(SIM_ROOT)/frontend/pin-frontend/obj-intel64/pin_frontend
LIB_CARBON=$(SIM_ROOT)/lib/libcarbon_sim.a
LIB_PIN_SIM=$(SIM_ROOT)/pin/../lib/pin_sim.so
LIB_FOLLOW=$(SIM_ROOT)/pin/../lib/follow_execv.so
LIB_SIFT=$(SIM_ROOT)/sift/libsift.a
LIB_DECODER=$(SIM_ROOT)/decoder_lib/libdecoder.a
SIM_TARGETS=$(LIB_DECODER) $(LIB_CARBON) $(LIB_SIFT) $(LIB_PIN_SIM) $(LIB_FOLLOW) $(STANDALONE) $(PIN_FRONTEND)

.PHONY: all message dependencies compile_simulator configscripts package_deps pin python linux builddir showdebugstatus distclean mbuild xed_install xed
# Remake LIB_CARBON on each make invocation, as only its Makefile knows if it needs to be rebuilt
.PHONY: $(LIB_CARBON)

all: message dependencies $(SIM_TARGETS) configscripts

include common/Makefile.common

dependencies: package_deps sde_kit pin_kit pin xed python mcpat linux builddir showdebugstatus
ifeq ($(BUILD_ARM),1)
dependencies: capstone
.PHONY: capstone
endif

$(SIM_TARGETS): dependencies

message:
	@echo -n Building for x86 \($(SNIPER_TARGET_ARCH)\)
ifeq ($(BUILD_RISCV),1)
	@echo -n " and RISCV"
endif
ifeq ($(BUILD_ARM),1)
	@echo -n " and arm64"
endif
	@echo ""

$(STANDALONE): $(LIB_CARBON) $(LIB_SIFT) $(LIB_DECODER)
	@$(MAKE) $(MAKE_QUIET) -C $(SIM_ROOT)/standalone

$(PIN_FRONTEND):
	@$(MAKE) $(MAKE_QUIET) -C $(SIM_ROOT)/frontend/pin-frontend

# Disable original frontend

#$(LIB_PIN_SIM): $(LIB_CARBON) $(LIB_SIFT) $(LIB_DECODER)
#	@$(MAKE) $(MAKE_QUIET) -C $(SIM_ROOT)/pin $@

#$(LIB_FOLLOW):
#	@$(MAKE) $(MAKE_QUIET) -C $(SIM_ROOT)/pin $@

$(LIB_CARBON): 
	@$(MAKE) $(MAKE_QUIET) -C $(SIM_ROOT)/common

$(LIB_SIFT): $(LIB_CARBON)
	@$(MAKE) $(MAKE_QUIET) -C $(SIM_ROOT)/sift

$(LIB_DECODER): $(LIB_CARBON)
	@$(MAKE) $(MAKE_QUIET) -C $(SIM_ROOT)/decoder_lib 

CAPSTONE_GITID=f9c6a90489be7b3637ff1c7298e45efafe7cf1b9
CAPSTONE_INSTALL=$(SIM_ROOT)/capstone
CAPSTONE_INSTALL_DEP=$(CAPSTONE_INSTALL)/arch/AArch64/ARMMappingInsnOp.inc
$(CAPSTONE_INSTALL_DEP):
	$(_MSG) '[DOWNLO] capstone'
	$(_CMD) git clone --quiet https://github.com/aquynh/capstone.git $(CAPSTONE_INSTALL)
	$(_CMD) git -C $(CAPSTONE_INSTALL) reset --quiet --hard $(CAPSTONE_GITID)

CAPSTONE_BUILD_DEP=$(CAPSTONE_INSTALL)/libcapstone.so.4
capstone: $(CAPSTONE_BUILD_DEP)
$(CAPSTONE_BUILD_DEP): $(CAPSTONE_INSTALL_DEP)
	$(_MSG) '[INSTAL] capstone'
	$(_CMD) cd $(CAPSTONE_INSTALL) ; ./make.sh


MBUILD_GITID=1651029643b2adf139a8d283db51b42c3c884513
MBUILD_INSTALL=$(SIM_ROOT)/mbuild
MBUILD_INSTALL_DEP=$(MBUILD_INSTALL)/mbuild/arar.py
mbuild: $(MBUILD_INSTALL_DEP)
$(MBUILD_INSTALL_DEP):
	$(_MSG) '[DOWNLO] mbuild'
	$(_CMD) git clone --quiet https://github.com/intelxed/mbuild.git $(MBUILD_INSTALL)
	$(_CMD) git -C $(MBUILD_INSTALL) reset --quiet --hard $(MBUILD_GITID)

XED_GITID=2be2d282939f6eb84e03e1fed9ba82f32c8bac2d
XED_INSTALL_DEP=$(XED_INSTALL)/src/common/xed-init.c
xed_install: $(XED_INSTALL_DEP)
$(XED_INSTALL_DEP):
	$(_MSG) '[DOWNLO] xed'
	$(_CMD) git clone --quiet https://github.com/intelxed/xed.git $(XED_INSTALL)
	$(_CMD) git -C $(XED_INSTALL) reset --quiet --hard $(XED_GITID)

XED_DEP=$(XED_HOME)/include/xed/xed-iclass-enum.h
xed: mbuild xed_install $(XED_DEP)
$(XED_DEP): $(XED_INSTALL_DEP)
	$(_MSG) '[INSTAL] xed'
	$(_CMD) cd $(XED_INSTALL) ; ./mfile.py --silent --extra-flags=-fPIC --shared --install-dir $(XED_HOME) install

SDE_DOWNLOAD=https://downloadmirror.intel.com/684899/sde-external-9.0.0-2021-11-07-lin.tar.xz
SDE_DEP=$(SDE_HOME)/intel64/pin_lib/libpin3dwarf.so
sde_kit: $(SDE_DEP)
$(SDE_DEP):
	$(_MSG) '[DOWNLO] SDE 9.0.0'
	$(_CMD) mkdir -p $(SDE_HOME)
	$(_CMD) wget -O - --no-verbose --quiet $(SDE_DOWNLOAD) | tar -x -J --strip-components 1 -C $(SDE_HOME)
	$(_CMD) touch $(SDE_HOME)/.autodownloaded
	$(_MSG) '[DOWNLO] pinplay-scripts'
	$(_CMD) svn checkout --quiet https://github.com/intel/pinplay-tools/trunk/pinplay-scripts $(SDE_HOME)/pinplay-scripts

PIN_DOWNLOAD=https://software.intel.com/sites/landingpage/pintool/downloads/pin-3.18-98332-gaebd7b1e6-gcc-linux.tar.gz
PIN_DEP=$(PIN_HOME)/intel64/lib-ext/libpin3dwarf.so
pin_kit: $(PIN_DEP)
$(PIN_DEP):
	$(_MSG) '[DOWNLO] Pin 3.18-98332'
	$(_CMD) mkdir -p $(PIN_HOME)
	$(_CMD) wget -O - --no-verbose --quiet $(PIN_DOWNLOAD) | tar -x -z --strip-components 1 -C $(PIN_HOME)
	$(_CMD) touch $(PIN_HOME)/.autodownloaded

ifneq ($(USE_PINPLAY), 1)
$(info Using SDE kit)
else
$(info Using Pinplay kit)
endif

ifneq ($(NO_PIN_CHECK),1)
PIN_REV_MINIMUM=71313
pin: $(PIN_HOME)/intel64/bin/pinbin $(PIN_HOME)/source/tools/Config/makefile.config package_deps
	@if [ "$$(tools/pinversion.py $(PIN_HOME) | cut -d. -f3)" -lt "$(PIN_REV_MINIMUM)" ]; then echo; echo "Found Pin version $$(tools/pinversion.py $(PIN_HOME)) in $(PIN_HOME)"; echo "but at least revision $(PIN_REV_MINIMUM) is required."; echo; false; fi
$(PIN_HOME)/source/tools/Config/makefile.config:
	@echo
	@echo "Old Pin version found in $(PIN_HOME), Sniper requires Pin version $(PIN_REV_MINIMUM) or newer."
	@echo
	@false
$(PIN_HOME)/intel64/bin/pinbin:
	@echo
	@echo "Cannot find Pin in $(PIN_HOME). Please download and extract Pin version $(PIN_REV_MINIMUM)"
	@echo "from http://www.pintool.org/downloads.html into $(PIN_HOME), or set the PIN_HOME environment variable."
	@echo
	@false
endif

ifneq ($(NO_PYTHON_DOWNLOAD),1)
PYTHON_DEP=python_kit/$(SNIPER_TARGET_ARCH)/lib/python2.7/lib-dynload/_sqlite3.so
python: $(PYTHON_DEP)
$(PYTHON_DEP):
	$(_MSG) '[DOWNLO] Python $(SNIPER_TARGET_ARCH)'
	$(_CMD) mkdir -p python_kit/$(SNIPER_TARGET_ARCH)
	$(_CMD) wget -O - --no-verbose --quiet "http://snipersim.org/packages/sniper-python27-$(SNIPER_TARGET_ARCH).tgz" | tar xz --strip-components 1 -C python_kit/$(SNIPER_TARGET_ARCH)
endif

ifneq ($(NO_MCPAT_DOWNLOAD),1)
mcpat: mcpat/mcpat-1.0
mcpat/mcpat-1.0:
	$(_MSG) '[DOWNLO] McPAT'
	$(_CMD) mkdir -p mcpat
	$(_CMD) wget -O - --no-verbose --quiet "http://snipersim.org/packages/mcpat-1.0.tgz" | tar xz -C mcpat
endif

linux: include/linux/perf_event.h
include/linux/perf_event.h:
	$(_MSG) '[INSTAL] perf_event.h'
	$(_CMD) if [ -e /usr/include/linux/perf_event.h ]; then cp /usr/include/linux/perf_event.h include/linux/perf_event.h; else cp include/linux/perf_event_2.6.32.h include/linux/perf_event.h; fi

builddir: lib
lib:
	@mkdir -p $(SIM_ROOT)/lib

showdebugstatus:
ifneq ($(DEBUG),)
	@echo Using flags: $(OPT_CFLAGS)
endif

configscripts: dependencies
	@mkdir -p config
	@> config/sniper.py
	@echo '# This file is auto-generated, changes made to it will be lost. Please edit Makefile instead.' >> config/sniper.py
	@echo "target=\"$(SNIPER_TARGET_ARCH)\"" >> config/sniper.py
	@./tools/makerelativepath.py sde_home "$(SIM_ROOT)" "$(SDE_HOME)" >> config/sniper.py
	@./tools/makerelativepath.py pin_home "$(SIM_ROOT)" "$(PIN_HOME)" >> config/sniper.py
	@./tools/makerelativepath.py xed_home "$(SIM_ROOT)" "$(XED_HOME)" >> config/sniper.py
	@./tools/makerelativepath.py dynamorio_home "$(SIM_ROOT)" "$(DR_HOME)" >> config/sniper.py
	@if [ $$(which git) ]; then if [ -e "$(SIM_ROOT)/.git" ]; then echo "git_revision=\"$$(git --git-dir='$(SIM_ROOT)/.git' rev-parse HEAD)\"" >> config/sniper.py; fi ; fi
	@./tools/makebuildscripts.py "$(SIM_ROOT)" "$(SDE_HOME)" "$(PIN_HOME)" "$(DR_HOME)" "$(CC)" "$(CXX)" "$(SNIPER_TARGET_ARCH)"

empty_config:
	$(_MSG) '[CLEAN ] config'
	$(_CMD) rm -f config/sniper.py config/buildconf.sh config/buildconf.makefile

clean: empty_config empty_deps
	$(_MSG) '[CLEAN ] standalone'
	$(_CMD) $(MAKE) $(MAKE_QUIET) -C standalone clean
	$(_MSG) '[CLEAN ] pin'
	$(_CMD) $(MAKE) $(MAKE_QUIET) -C pin clean
	$(_MSG) '[CLEAN ] common'
	$(_CMD) $(MAKE) $(MAKE_QUIET) -C common clean
	$(_MSG) '[CLEAN ] sift'
	$(_CMD) $(MAKE) $(MAKE_QUIET) -C sift clean
	$(_MSG) '[CLEAN ] tools'
	$(_CMD) $(MAKE) $(MAKE_QUIET) -C tools clean
	$(_MSG) '[CLEAN ] frontend/pin-frontend'
	$(_CMD) if [ -d "$(PIN_HOME)" ]; then $(MAKE) $(MAKE_QUIET) -C frontend/pin-frontend clean ; fi
	$(_CMD) rm -f .build_os

distclean: clean
	$(_MSG) '[DISTCL] Pin kit'
	$(_CMD) if [ -e "$(PIN_HOME)/.autodownloaded" ]; then rm -rf $(PIN_HOME); fi
	$(_MSG) '[DISTCL] SDE kit'
	$(_CMD) if [ -e "$(SDE_HOME)/.autodownloaded" ]; then rm -rf $(SDE_HOME); fi
	$(_MSG) '[DISTCL] python_kit'
	$(_CMD) rm -rf python_kit
	$(_MSG) '[DISTCL] McPAT'
	$(_CMD) rm -rf mcpat
	$(_MSG) '[DISTCL] Xed'
	$(_CMD) rm -rf xed xed_kit mbuild
	$(_MSG) '[DISTCL] perf_event.h'
	$(_CMD) rm -f include/linux/perf_event.h

regress_quick: regress_unit regress_apps

empty_deps:
	$(_MSG) '[CLEAN ] deps'
	$(_CMD) find . -name \*.d -exec rm {} \;

package_deps:
	@BOOST_INCLUDE=$(BOOST_INCLUDE) ./tools/checkdependencies.py
