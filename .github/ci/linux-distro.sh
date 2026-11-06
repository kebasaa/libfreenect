#!/bin/sh
# Build, test and package libfreenect in a Debian or Ubuntu container (CI).
# Runs as root inside the container, with the source tree as the working
# directory. RPI=1 adds Raspberry Pi's own repository first, which turns a
# Debian image into the package set of Raspberry Pi OS of the same release.
set -eu
export DEBIAN_FRONTEND=noninteractive
SRC=$(pwd)
BUILD=/tmp/build

apt-get update
apt-get install -y --no-install-recommends ca-certificates curl

if [ "${RPI:-0}" = 1 ]; then
	. /etc/os-release
	mkdir -p /etc/apt/keyrings
	curl -fsSL https://archive.raspberrypi.com/debian/raspberrypi.gpg.key -o /etc/apt/keyrings/raspberrypi.asc
	echo "deb [signed-by=/etc/apt/keyrings/raspberrypi.asc] http://archive.raspberrypi.com/debian/ $VERSION_CODENAME main" \
		> /etc/apt/sources.list.d/raspi.list
	apt-get update
	apt-get -y full-upgrade
fi

apt-get install -y --no-install-recommends \
	cmake build-essential pkg-config python3 file \
	libusb-1.0-0-dev libglfw3-dev libgl-dev libglu1-mesa-dev \
	xvfb xauth libgl1-mesa-dri udev

. /etc/os-release
echo "== $PRETTY_NAME ($(uname -m)), $(gcc --version | head -1), $(cmake --version | head -1)"
dpkg -s libusb-1.0-0-dev libglfw3-dev 2>/dev/null | grep -E '^(Package|Version):'

cmake -S "$SRC" -B "$BUILD" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr \
	-DBUILD_EXAMPLES=ON -DBUILD_C_SYNC=ON -DBUILD_CPP=ON -DBUILD_FAKENECT=ON \
	-DFREENECT_GL_VIEWERS=GLFW -DBUILD_CPACK_DEB=ON
cmake --build "$BUILD" --parallel "$(nproc)"

echo "== unit tests (the window tests under Xvfb)"
xvfb-run -a ctest --test-dir "$BUILD" --output-on-failure

echo "== smoke tests (no Kinect attached)"
set +e
"$BUILD/bin/freenect-fwloader"; code=$?
set -e
test "$code" -eq 2 || { echo "freenect-fwloader should report no Kinect (exit 2), got $code"; exit 1; }
LIBFREENECT_HOTPLUG_POLL=1 "$BUILD/bin/freenect-hotplug" 2 --no-stream
for v in glview regview hiview chunkview micview glpclview cppview cpp_pcview; do
	test -x "$BUILD/bin/freenect-$v" || { echo "freenect-$v was not built"; exit 1; }
done

echo "== udev rules"
udevadm verify --no-style "$SRC/platform/linux/udev/51-kinect.rules"

echo "== .deb: build, install, remove"
(cd "$BUILD" && cpack -G DEB)
deb=$(ls "$BUILD"/*.deb)
dpkg-deb -c "$deb" | grep -E 'udev/rules.d/51-kinect.rules|systemd/system/freenect-fwloader.service|bin/freenect-fwloader$'
dpkg -i "$deb"
test -f /usr/lib/udev/rules.d/51-kinect.rules
test -f /usr/lib/systemd/system/freenect-fwloader.service
/usr/bin/freenect-fwloader --help > /dev/null
dpkg -r libfreenect-dev
test ! -e /usr/lib/udev/rules.d/51-kinect.rules
echo "== done: $PRETTY_NAME ($(uname -m))"
