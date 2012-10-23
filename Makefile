
# SSIM Makefile

# Makefile is here for developers

# Targets:
#
# depend - generate files.inc and dependencies for makefile
# s_sim.hex - compile 
# program - build and programs greencard on PRG_DEVICE with picprog

PRG_DEVICE="/dev/ttyUSB0"
VERSION=$(shell cat src/version.inc | grep "define	Version" | awk -F \" '{ print ($$2) }')

# Handle the presence of s_sim.dep
ifeq (s_sim.dep,$(wildcard s_sim.dep))
include s_sim.dep
else
.PHONY: s_sim.hex
s_sim.hex:
	@echo
	@echo Run make depend first !!
	@echo
	@exit -1
endif

program: s_sim.hex
	picprog --erase --burn --input s_sim.hex --pic $(PRG_DEVICE) --device pic16f876

.PHONY: files depend

files:
	tools/genfiles >src/files/files.inc

depend: files
	tools/makedep >s_sim.dep

clean:
	-rm -f s_sim.cod s_sim.lst s_sim.hex

distclean: clean
	-rm config.inc s_sim.dep src/files/files.inc
	cp config config.inc

private: distclean
	-rm config.inc
	ln -s ../private.inc config.inc

dist: distclean files
	mv ./../ssim ./../ssim-$(VERSION)
	tar -cvvf ../ssim.tar ./../ssim-$(VERSION)
	gzip -c ../ssim.tar > ../ssim-$(VERSION).tar.gz
	-rm ../ssim.tar
	mv ./../ssim-$(VERSION) ./../ssim

backup: clean
	tar -cvvf ../ssim.tar ./
	gzip -c ../ssim.tar > ../ssim-$(shell date +%y-%m-%d-%H-%M).tar.gz
	-rm ../ssim.tar
