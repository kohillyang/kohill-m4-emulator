
PRO   = emulator.pro
QMAKE = qmake

all:	QtMakefile
	$(MAKE) -f QtMakefile.Debug
	./Debug/emulator.exe
clean:
	rm -fr QtMakefile QtMakefile.debug QtMakefile.release debug release

QtMakefile:
	$(QMAKE) -o QtMakefile $(PRO) CONFIG+=debug_and_release

debug:	QtMakefile
	$(MAKE) -f QtMakefile debug

release:	QtMakefile
	$(MAKE) -f QtMakefile release

.PHONY: all clean debug clean-debug release clean-release
