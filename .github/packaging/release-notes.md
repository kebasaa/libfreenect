Kinect v1 drivers, tools and libraries for Windows 10/11, Linux and macOS, built from [kebasaa/libfreenect](https://github.com/kebasaa/libfreenect) at @VERSION@. This is a fork of [OpenKinect/libfreenect](https://github.com/OpenKinect/libfreenect), not an official OpenKinect release. The changes are proposed upstream in OpenKinect/libfreenect#703.

## Downloads

| System | File |
|---|---|
| Windows 10/11, 64-bit PC | `libfreenect-@VERSION@-windows-x64.zip` |
| Windows 11 on ARM | `libfreenect-@VERSION@-windows-ARM64.zip` |
| Ubuntu 24.04+, Debian 13 (PC) | `libfreenect-@VERSION@-linux-amd64.deb` |
| Ubuntu/Debian on ARM, 64-bit Raspberry Pi OS | `libfreenect-@VERSION@-linux-arm64.deb` |
| 32-bit Raspberry Pi OS | `libfreenect-@VERSION@-raspios-armhf.deb` |
| macOS 11+, Apple Silicon and Intel | `libfreenect-@VERSION@-macos-universal.tar.gz` |

`SHA256SUMS.txt` has the checksums. Each package has a `README-RELEASE.txt` with the installation steps.

## What's in it

- **Windows 11 with Memory integrity on:** `freenect-driver-install` binds the Kinect to Windows' own WinUSB driver, so Microsoft's old Kinect driver isn't needed.
- **Cameras:** depth, colour and IR. Depth in millimetres uses Microsoft's formula; near mode is included.
- **Motor, LED and IR emitter.**
- **Coordinate mapping** between depth, colour and 3-D space.
- **Microphone array:** the Kinect works as a normal microphone with Microsoft's audio firmware. There is also a beamformer and sound-source direction, and a single beamformed microphone on Linux (PipeWire) and macOS.
- **Skeleton, hand and gesture tracking** (`freenect-nuitest`, `libfreenect_nui`).
- **Tools:** viewers (`freenect-glview`, `-regview`, `-hiview`, …), `freenect-camtest`, `freenect-tiltdemo`, `freenect-micbeam`, `freenect-maptest`, `freenect-hotplug`, and `freenect-v4l2` on Linux.

**Microsoft's files are not included.** The audio firmware, beamformer weights and skeleton models are downloaded from Microsoft's official Kinect Runtime installer on your computer, verified, and extracted there. Microsoft's licence terms apply to them.

## Good to know

- **Tested hardware:** Xbox 360 Kinect model 1414 on Windows 11. The Kinect for Windows (1517) and the Xbox model 1473 are supported but untested. Reports are welcome in the issues.
- **Unsigned programs:** the Windows programs aren't code-signed, so SmartScreen may warn ("More info" > "Run anyway"). The macOS files aren't notarised; see the README for the one command that clears the quarantine.
- **Licences:** libfreenect is Apache 2.0 or GPL v2. Bundled: libusb (LGPL 2.1) and GLFW (zlib). The licence texts are in each package.
