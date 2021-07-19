# SM64GSW GNUMakeFile
version := 1.4

sm64gsw: clean
	gcc sm64gsw.c -o sm64gsw
	
all: linux-static64 linux-static32 cross-win32

mac-ppc: clean
	gcc -isysroot/Developer/SDKs/MacOSX10.4u.sdk -arch ppc -arch ppc750 -arch ppc7400 -arch ppc7450 -arch ppc970 -mmacosx-version-min=10.3 sm64gsw.c -o sm64gsw
	mkdir sm64gsw_$(version)_mac_os_x_powerpc
	cp -rv sm64gsw readme.html license.txt gs.txt known_working_codes.txt sm64gsw_$(version)_mac_os_x_powerpc/

linux-static64: clean
	gcc -static sm64gsw.c -o sm64gsw
	mkdir sm64gsw_$(version)_linux_x86_64
	cp -rv sm64gsw readme.html license.txt gs.txt known_working_codes.txt sm64gsw_$(version)_linux_x86_64/

linux-static32: clean
	gcc -static -m32 sm64gsw.c -o sm64gsw
	mkdir sm64gsw_$(version)_linux_x86
	cp -rv sm64gsw readme.html license.txt gs.txt known_working_codes.txt sm64gsw_$(version)_linux_x86/

cross-win32: clean
	i686-w64-mingw32-gcc sm64gsw.c -o sm64gsw.exe
	mkdir sm64gsw_$(version)_win32
	cp -rv sm64gsw.exe readme.html license.txt gs.txt known_working_codes.txt sm64gsw_$(version)_win32/
 
clean:
	rm -rf sm64gsw sm64gsw.exe sm64gsw_$(version)_linux_x86_64 sm64gsw_$(version)_linux_x86 sm64gsw_$(version)_win32 sm64gsw_$(version)_mac_os_x_powerpc
