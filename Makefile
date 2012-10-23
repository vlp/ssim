# Targets:
#
# depend - generate files.inc and dependencies for makefile
# ssim.hex - compile 
# program - build and programs greencard on PRG_DEVICE with picprog

PRG_DEVICE="/dev/ttyUSB0"
VERSION=$(shell cat src/version.inc | grep "define	Version" | awk -F \" '{ print ($$2) }')

# Handle the presence of ssim.dep
ifeq (ssim.dep,$(wildcard ssim.dep))
include ssim.dep
else
.PHONY: ssim.hex
ssim.hex:
	@echo
	@echo Run make depend first !!
	@echo
	@exit -1
endif

program: ssim.hex
	picprog --erase --burn --input ssim.hex --pic $(PRG_DEVICE) --device pic16f876

.PHONY: files depend

files:
	tools/genfiles >src/files/files.inc

depend: files
	tools/makedep >ssim.dep

clean:
	-rm -f ssim.cod ssim.lst ssim.hex

distclean: clean
	-rm config.inc ssim.dep src/files/files.inc
	cp config config.inc

private: distclean
	-rm config.inc
	ln -s ../private.inc config.inc

backup: clean
	tar -cvvf ../ssim.tar ./
	gzip -c ../ssim.tar > ../ssim-$(shell date +%y-%m-%d-%H-%M).tar.gz
	-rm ../ssim.tar
