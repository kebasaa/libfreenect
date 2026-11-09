libfreenect
===========

libfreenect is a userspace driver for the Microsoft Kinect.
It runs on Linux, OSX, and Windows and supports

- RGB and Depth Images
- Motors
- Accelerometer
- LED
- Audio

Notice: If you have the newer Kinect v2 (XBox One), use [OpenKinect/libfreenect2](https://github.com/OpenKinect/libfreenect2) instead.


# Build Instructions

To build libfreenect, you'll need

- [libusb](http://libusb.info) >= 1.0.18 (Windows needs >= 1.0.26)
- [CMake](http://cmake.org) >= 3.12.4
- [python](http://python.org) >= 2.7 or >= 3.3 (only if BUILD_PYTHON=ON or BUILD_PYTHON2=ON or BUILD_PYTHON3=ON or BUILD_REDIST_PACKAGE=OFF)

For the examples, you'll need

- OpenGL   (included with OSX)
- [GLFW](https://www.glfw.org) >= 3.3 for the viewers' windows; it runs natively on Wayland, X11, macOS and Windows.
  GLUT (freeglut) still works instead: `-DFREENECT_GL_VIEWERS=GLUT`. `AUTO` (the default) takes GLFW if it's there, else GLUT; `OFF` skips the viewers.
- [pthreads-win32](http://sourceforge.net/projects/pthreads4w/) (Windows, optional: without it, a built-in header-only pthread subset over native Win32 threads is used)

## <a name="fetch-build"></a>Fetch & Build

    git clone https://github.com/OpenKinect/libfreenect
    cd libfreenect
    mkdir build
    cd build
    cmake -L .. # -L lists all the project options
    make

    # if you don't have `make` or don't want color output
    # cmake --build .

Use CMake options to control what gets built.
For example, to build the Python wrapper (defaults to system Python):

    cmake .. -DBUILD_PYTHON=ON
    make

Or the Python 3 wrapper:

    cmake .. -DBUILD_PYTHON3=ON
    make


You can specify a build with debug symbols:

    cmake .. -DCMAKE_BUILD_TYPE=debug
    # or with optimizations
    # cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo

You can build .deb, .rpm, and/or .tgz packages with `cpack`:

    cmake .. -DBUILD_CPACK_DEB=ON -DBUILD_CPACK_RPM=ON -DBUILD_CPACK_TGZ=ON
    cpack

For audio support, you must upload firmware to the device.
Newer Kinect models may require audio firmware for motor and LED support.
The best method is to [insert firmware at runtime](https://github.com/OpenKinect/libfreenect/issues/376#issuecomment-41211251) just after calling `freenect_init()`.

Alternately, firmware for Kinect model 1414 can be downloaded automatically by specifying:

    cmake .. -DBUILD_REDIST_PACKAGE=OFF

Note that firmware may not be legal to redistribute in your jurisdiction!

Microsoft's own audio firmware (`UAC.BIN`) can also be used. With it, the microphones become a standard USB Audio device for the operating system (ALSA/PipeWire, Core Audio, Windows), instead of being read through libfreenect's audio API. To fetch it from Microsoft's Kinect for Windows Runtime installer:

    python3 src/fwfetcher.py --uac [--from KinectRuntime-v1.8-Setup.exe] [--user | OUTPUT]

This downloads the installer from download.microsoft.com, extracts only the firmware, verifies its SHA-256, and writes `UACFirmware.bin`, without running the installer. OUTPUT is a `.bin` file or a directory; `--user` writes to `$XDG_DATA_HOME/libfreenect` (`~/.local/share/libfreenect`). Put the file where libfreenect looks for firmware, for example `~/.local/share/libfreenect/`, or `/usr/local/share/libfreenect/` so that the Linux plug-in service finds it too. On Linux and macOS libfreenect prefers `audios.bin`; set `LIBFREENECT_AUDIO_FIRMWARE=uac` to use `UACFirmware.bin`. On Windows, `freenect-driver-install` fetches it for you. Microsoft's licence terms for the Kinect for Windows Runtime apply to this firmware.

## Depth in millimetres

`FREENECT_DEPTH_MM` and `FREENECT_DEPTH_REGISTERED` convert depth to millimetres with the formula of Microsoft's Kinect for Windows runtime, from each device's own calibration. Unlike the runtime, libfreenect doesn't round the device's reference distance to whole centimetres. By default every distance the sensor reports is kept; beyond 4 m depth gets noisier. To keep only a range, as the runtime does, call `freenect_set_depth_range()`:

| Range | Keeps |
|---|---|
| `FREENECT_DEPTH_RANGE_FULL` (default) | every distance. `FREENECT_DEPTH_MM` saturates at `FREENECT_DEPTH_MM_MAX_VALUE` (10 m), as before |
| `FREENECT_DEPTH_RANGE_SDK` | 800–4000 mm, the runtime's default |
| `FREENECT_DEPTH_RANGE_NEAR` | 400–3000 mm, as in the runtime's near mode. With `FREENECT_DEPTH_RANGE_SDK` set, switching `FREENECT_NEAR_MODE` on (Kinect for Windows) selects it |

Distances outside the range are delivered as 0. Earlier versions of libfreenect used a slightly different formula, with a 0.375-pixel offset, which gave shorter distances: with the reference sample calibration about 0.5 % at 1.2 m, 1.7 % at 4 m and 3 % at 7 m.

## Environment variables

These change libfreenect's behaviour for every application:

| Variable | Effect |
|---|---|
| `LIBFREENECT_LED=off` | Keeps the Kinect's LED off, for example at night. Every request to set the LED turns it off instead, and it's switched off when the Kinect is opened |
| `LIBFREENECT_TILT_RATE_LIMIT=0` | Turns off the tilt rate limit (at least 1 s between moves, at most 15 moves per 20 s), which protects the motor as in Microsoft's SDK |
| `LIBFREENECT_FIRMWARE_PATH=DIR` | Looks for the audio firmware in `DIR` first |
| `LIBFREENECT_AUDIO_FIRMWARE=uac` or `=xbox` | Loads only Microsoft's `UACFirmware.bin`, or only the Xbox `audios.bin` |
| `LIBFREENECT_USBDK=1` | Windows: uses libusb's UsbDk backend instead of WinUSB |
| `LIBFREENECT_GLFW_PLATFORM=x11` or `=wayland` | Linux: the window system the viewers use (GLFW 3.4 and newer prefer Wayland) |
| `LIBFREENECT_HOTPLUG_POLL=1` | Finds Kinects being plugged in and out by scanning once a second, instead of through libusb's hotplug events (for example in a VM or container where those don't arrive) |
| `LIBFREENECT_INCOMPLETE_FRAMES=1` | Delivers depth and video frames that are missing packets, as libfreenect did before. By default such frames are dropped, as in Microsoft's driver, because the missing parts would hold data from an earlier frame |
| `LIBFREENECT_LOG=info` (or `error`, `warning`, `notice`, `debug`, `spew`) | Sets libfreenect's log level whatever the program asks for, and puts a time stamp on each line. Useful for bug reports |
| `LIBFREENECT_WINDOWS_AUDIO_UNSAFE=1` | Windows: allows libfreenect's own audio streaming. It's disabled because it hung a test machine; only use it on a machine you can hard-reset |

## Troubleshooting

### No depth or video frames

The Kinect streams depth, video and audio as USB *isochronous* transfers, which reserve bandwidth on the bus every 125 µs. Control requests (tilt, LED, opening the device) use ordinary transfers, so a Kinect can be found, tilt and blink, and still deliver no frames when the isochronous path doesn't work.

libfreenect watches every stream. When, a few seconds after a stream starts, at least 95% of its packets arrive empty or failed, it logs a warning saying that the stream delivers no data; at 20% it notes that the stream is losing data. Each is reported once per stream start, with a hint for the situation:

- **Power and cabling.** The Kinect needs its own power supply (the Xbox 360 model needs the USB/power adapter cable). Plug it into a port directly on the computer, not into a hub, a monitor's ports or an extension cable.
- **USB controller.** Some USB 3 (xHCI) controllers handle the Kinect's high-bandwidth isochronous streams badly. If frames are missing or partial, try a port on another controller (on many PCs the front and back ports, or the USB 2 and USB 3 ports, are on different controllers), and keep other high-bandwidth devices such as webcams off the same controller.
- **Virtual machines and containers.** USB passthrough of isochronous endpoints is often unreliable or unsupported, see [OpenKinect/libfreenect#696](https://github.com/OpenKinect/libfreenect/issues/696). libfreenect detects common hypervisors (QEMU/KVM, VMware, VirtualBox, Parallels, Xen, Hyper-V guests, macOS VMs) and WSL2, and says so in the warning.
  - **WSL2 with usbipd:** usbip doesn't carry the Kinect's isochronous streams well. Use libfreenect on Windows itself instead (it uses the in-box WinUSB driver, see [Windows](#windows)).
  - **Other VMs:** use the Kinect on the host, or pass a whole USB controller through to the VM (PCI passthrough, for example VFIO with QEMU/KVM), so that the guest drives real hardware rather than an emulated controller.
  - Inside containers and VMs, libusb's hotplug events may not arrive; `LIBFREENECT_HOTPLUG_POLL=1` makes libfreenect scan for Kinects instead.

## OSX

If you don't have a package manager, install [Homebrew](http://brew.sh/).
For a manual build, see [the wiki](http://openkinect.org/wiki/Getting_Started#Manual_Build_under_OSX).
[platform/macos](platform/macos/README.md) covers universal builds, the audio firmware and the launchd agent that loads it whenever the Kinect is plugged in.

### Homebrew

    brew install libfreenect
    # or get the very latest:
    # brew install --HEAD libfreenect

### MacPorts

    sudo port install git-core cmake libusb libtool

Continue with [Fetch & Build](#fetch-build).


## Linux

`cmake --install` installs the udev rules, which give the logged-in user (and the `plugdev` group) access to the Kinect without root, and a systemd unit that uploads the audio firmware when the Kinect is plugged in. See [platform/linux](platform/linux/README.md) for details, manual installation and the `gspca_kinect` kernel driver.
For a manual build, see [the wiki](http://openkinect.org/wiki/Getting_Started#Manual_Build_on_Linux).

### Ubuntu/Debian/Mint

The version packaged in Ubuntu may be very old.
To install newer packaged builds, see [the wiki](http://openkinect.org/wiki/Getting_Started#Ubuntu.2FDebian).
Continue with this section for a manual build.

    sudo apt-get install git cmake build-essential libusb-1.0-0-dev

    # only if you are building the examples:
    sudo apt-get install libglfw3-dev libgl-dev libglu1-mesa-dev

Continue with [Fetch & Build](#fetch-build).

There is also a [debian branch](https://github.com/OpenKinect/libfreenect/tree/debian) for packaging purposes.

### Gentoo Linux

There are [dev-libs/libfreenect](https://github.com/piedar/overboard/tree/master/dev-libs/libfreenect) ebuilds in the [overboard repo](https://github.com/piedar/overboard).

### Arch Linux

There is a [libfreenect](https://aur.archlinux.org/packages/libfreenect) PKGBUILD in the AUR.
Alternately, the [libfreenect-git](https://aur.archlinux.org/packages/libfreenect-git) PKGBUILD builds the very latest.


## Windows

libfreenect talks to the Kinect from user mode through libusb, using Microsoft's in-box WinUSB driver.
No kernel driver of its own is needed, so it works on Windows 11 with Memory integrity (core isolation) turned on, unlike the old Kinect for Windows SDK driver.
Use libusb 1.0.26 or newer on Windows 8.1 or newer. Older libusb versions lack WinUSB isochronous support or have a known regression in it.

Bind the Kinect to WinUSB by running `freenect-driver-install --install` from an Administrator terminal; `--list` shows the current drivers and `--uninstall` undoes it.
See [platform/windows/README.md](platform/windows/README.md) for what it changes.
[Zadig](http://zadig.akeo.ie/) also works: install **WinUSB** (or libusbK) for the camera, the motor and the audio device (enable *Options > List All Devices*).
The old libusb-win32 (libusb0) driver cannot stream video or depth.
If you have [UsbDk](https://github.com/daynix/UsbDk) installed instead, set the environment variable `LIBFREENECT_USBDK=1` to make libusb use it.

Follow [Fetch & Build](#fetch-build) or use Github and CMake GUI tools.
pthreads-win32 is optional (see above), so MSVC builds of the wrappers and examples need only libusb and, for the viewers, GLFW (the vcpkg manifest's `examples` feature brings both).
Remember to supply paths to CMake so it can find dependencies.
For example:

    cmake .. -DLIBUSB_1_INCLUDE_DIR="C:\path\to\libusb\include" -DLIBUSB_1_LIBRARY="C:\path\to\libusb\libusb.lib"


# Wrappers

Interfaces to various languages are provided in [wrappers/](https://github.com/OpenKinect/libfreenect/tree/master/wrappers).
Wrappers are not guaranteed to be API stable or up to date.

- C (using a synchronous API)
- C++
- C#
- python
- ruby
- actionscript
- Java (JNA)

# Fakenect

Using fakenect, you can record a session to a directory and play it back later.

    mkdir session
    fakenect-record ./session

To use a fakenect recorded stream, just provide the fakenect lib as a pre loaded library with `LD_PRELOAD` and indicates the recorded files directory with `FAKENECT_PATH`.

- Sample with python wrappers :
```shell
    LD_PRELOAD="/usr/local/lib/fakenect/libfakenect.so" FAKENECT_PATH="./session" python ./wrappers/python/demo_cv_sync.py
```
- Sample with C bin :
```shell
    LD_PRELOAD="/usr/local/lib/fakenect/libfakenect.so" FAKENECT_PATH="./session" freenect-glview
```

# Code Contributions

In order of importance:

- Make sure to sign commits: `git commit -s`
- Use a [feature branch](https://www.atlassian.com/git/workflows#!workflow-feature-branch) in your own fork and target master with pull requests
- Tab indentation, no trailing whitespace


# Maintainers

Ongoing Development and Maintenance by the OpenKinect Community

http://www.openkinect.org

- Original Code and Engineering: Hector Martin (marcan)
- Community Lead: Josh Blake (JoshB)
- Integration: Kyle Machulis (qDot)


# License

The libfreenect project is covered under a dual Apache v2/GPL v2
license. The licensing criteria are listed below, as well as at the
top of each source file in the repo.

```
This file is part of the OpenKinect Project. http://www.openkinect.org

Copyright (c) 2010 individual OpenKinect contributors. See the CONTRIB
file for details.

This code is licensed to you under the terms of the Apache License,
version 2.0, or, at your option, the terms of the GNU General Public
License, version 2.0. See the APACHE20 and GPL2 files for the text of
the licenses, or the following URLs:
http://www.apache.org/licenses/LICENSE-2.0
http://www.gnu.org/licenses/gpl-2.0.txt

If you redistribute this file in source form, modified or unmodified,
you may:

- Leave this header intact and distribute it under the same terms,
  accompanying it with the APACHE20 and GPL2 files, or
- Delete the Apache 2.0 clause and accompany it with the GPL2 file, or
- Delete the GPL v2 clause and accompany it with the APACHE20 file

In all cases you must keep the copyright notice intact and include a
copy of the CONTRIB file.

Binary distributions must follow the binary distribution requirements
of either License.
```


# More Information

Information about the OpenKinect project can be found at http://www.openkinect.org

For questions, support, and discussion, check out the google groups mailing list at http://groups.google.com/group/openkinect

Or the IRC channel at \#openkinect on [Freenode](http://freenode.net/)

We are also on twitter at http://twitter.com/openkinect
