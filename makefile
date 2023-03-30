# sm64gsw GNUMakefile by Alex Free
CC=gcc
CFLAGS=-Wall -Werror
VER=2.0

sm64gsw: clean
	$(CC) $(CFLAGS) sm64gsw.c -o sm64gsw

clean:
	rm -rf sm64gsw.exe sm64gsw

linux-x86:
	make sm64gsw CFLAGS="-m32 -static -Wall -Werror -Ofast"

linux-x86_64:
	make sm64gsw CFLAGS="-static -Wall -Werror -Ofast"

windows-x86:
	make sm64gsw CC="i686-w64-mingw32-gcc"

windows-x86_64:
	make sm64gsw CC="x86_64-w64-mingw32-gcc"

linux-release:
	rm -rf sm64gsw-$(VER)-$(PLATFORM) sm64gsw-$(VER)-$(PLATFORM).zip
	mkdir sm64gsw-$(VER)-$(PLATFORM)
	cp -rv sm64gsw images readme.md license.txt sm64gsw-$(VER)-$(PLATFORM)
	chmod -R 777 sm64gsw-$(VER)-$(PLATFORM)
	zip -r sm64gsw-$(VER)-$(PLATFORM).zip sm64gsw-$(VER)-$(PLATFORM)
	rm -rf sm64gsw-$(VER)-$(PLATFORM)

windows-release:
	rm -rf sm64gsw-$(VER)-$(PLATFORM) sm64gsw-$(VER)-$(PLATFORM).zip
	mkdir sm64gsw-$(VER)-$(PLATFORM)
	cp -rv sm64gsw.exe images readme.md license.txt sm64gsw-$(VER)-$(PLATFORM)
	chmod -R 777 sm64gsw-$(VER)-$(PLATFORM)
	zip -r sm64gsw-$(VER)-$(PLATFORM).zip sm64gsw-$(VER)-$(PLATFORM)
	rm -rf sm64gsw-$(VER)-$(PLATFORM)

linux-x86-release: linux-x86
	make linux-release PLATFORM=linux_x86_static

linux-x86_64-release: linux-x86_64
	make linux-release PLATFORM=linux_x86_64_static

windows-x86-release: windows-x86
	make windows-release PLATFORM=windows_x86

windows-x86_64-release: windows-x86_64
	make windows-release PLATFORM=windows_x86_64

clean-zip: clean
	rm -rf *.zip

all: linux-x86-release linux-x86_64-release windows-x86-release windows-x86_64-release