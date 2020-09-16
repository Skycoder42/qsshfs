#!/usr/bin/env bash

TMPDIR="/var/tmp/qsshfs-installer"
QSSHFS_DIR="$TMPDIR/qsshfs"
QPM_DIR="$TMPDIR/qpm"
QSSHFS="https://github.com/Skycoder42/qsshfs.git"
QSSHFS_VER="1.1.0"
QPM_VER="0.11.0"
export GOPATH="$QPM_DIR/gopath"
QPM="$GOPATH/bin/qpm"

src_fetch() {
	git clone $QSSHFS $QSSHFS_DIR
}

src_prepare() {
	cd $QSSHFS_DIR
	git reset --hard $QSSHFS_VER
}

make_dirs() {
	mkdir -p $TMPDIR
	mkdir -p $QSSHFS_DIR
	mkdir -p $QPM_DIR
	mkdir -p $GOPATH
}

clean() {
	rm -Rf $TMPDIR
}

make_qpm() {
	echo "This is the Gopath: $GOPATH"
	go get qpm.io/qpm
	cd $GOPATH/src/qpm.io
	git submodule init
	git submodule update
	go install qpm.io/qpm
	unset GOPATH
}

make_qsshfs() {
	cd $QSSHFS_DIR
	$QPM install
	qmake -r "./"
	make -j"$(nproc --all)"
}

install_qsshfs() {
	cd $QSSHFS_DIR
	sudo make install
	sudo ln -s /usr/lib64/qt5/bin/qsshfs /usr/bin/qsshfs
	sudo install -D -m644 de.skycoder42.qsshfs.desktop "/usr/share/applications/de.skycoder42.qsshfs.desktop"
	sudo install -D -m644 qsshfs_32.png "/usr/share/icons/hicolor/32x32/apps/qsshfs.png"
	sudo install -D -m644 qsshfs_16.png "/usr/share/icons/hicolor/16x16/apps/qsshfs.png"
	sudo install -D -m755 $0 "/usr/bin/qsshfs-installer"
}

installer() {
	
	clean
	src_fetch
	src_prepare
	make_qpm
	make_qsshfs
	install_qsshfs
}

uninstaller() {
	sudo rm -f /usr/lib64/qt5/bin/qsshfs
	sudo rm -f /usr/share/applications/de.skycoder42.qsshfs.desktop
	sudo rm -f /usr/share/icons/hicolor/32x32/apps/qsshfs.png
	sudo rm -f /usr/share/icons/hicolor/16x16/apps/qsshfs.png
}

help() {
	echo "This is the installer script for qsshfs. This will fetch, compile and install qsshfs by itself."
	echo "	"install": Will compile and install qsshfs."
	echo "	"uninstall": Will remove the installed version of qsshfs."
}

for i in "$@"
do
case $i in
	install)
	installer
	;;
	uninstall)
	uninstaller
	;;
	-h | --help | help)
	help
	;;
esac
done
