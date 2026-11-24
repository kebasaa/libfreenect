libfreenect (kebasaa fork): Kinect v1 drivers, tools and libraries
=====================================================================

This is a build of https://github.com/kebasaa/libfreenect, a fork of
OpenKinect's libfreenect (https://github.com/OpenKinect/libfreenect). It is
not an official OpenKinect release.

It supports the Xbox 360 Kinect (models 1414 and 1473) and the Kinect for
Windows (1517). Only the 1414 has been tested on hardware.

Microsoft's files are not included
----------------------------------
The audio firmware (UAC.BIN), Microsoft's beamformer weights and the skeleton
tracker's models are not part of this package. The installer downloads
Microsoft's official Kinect for Windows Runtime 1.8 on your computer, checks
its signature and checksums, and extracts only what is needed. Microsoft's
licence terms for the Runtime apply to those files.

Windows 10 and 11
-----------------
1. Unpack the zip anywhere.
2. Plug in the Kinect (with its power adapter).
3. In bin\, run as administrator:
       freenect-driver-install --install
   This binds the Kinect to Windows' own WinUSB driver (Memory integrity can
   stay on) and fetches Microsoft's audio firmware. Undo it with --uninstall.
4. Try it:   freenect-glview      (depth and colour)
             freenect-camtest 10  (frame rates)
             freenect-tiltdemo    (motor and LED)
   For the skeleton tracker, fetch its models once:
       freenect-driver-install --fetch-nui-models
   then run freenect-nuitest.

Windows may warn that the programs are from an unknown publisher
("Windows protected your PC"): they are not code-signed. Choose
"More info", then "Run anyway".

Linux (Ubuntu 24.04 or newer, Debian 13, Raspberry Pi OS)
--------------------------------------------------------
    sudo apt install ./libfreenect-*-linux-amd64.deb   (or -arm64, or -raspios-armhf)
    sudo python3 /usr/share/fwfetcher.py --uac /usr/share/libfreenect   (audio firmware, optional)
    python3 /usr/share/fwfetcher.py --nui --user    (skeleton tracker models, optional)

The package installs udev rules, so no root is needed to use the Kinect, and
loads the audio firmware when the Kinect is plugged in. Then run, for
example, freenect-glview.

macOS 11 or newer (Apple Silicon and Intel)
-------------------------------------------
    tar -xzf libfreenect-*-macos-universal.tar.gz
    cd libfreenect-*-macos-universal
    xattr -dr com.apple.quarantine .      (the files are not notarised)
    bin/freenect-glview

Audio firmware and skeleton models (optional):
    python3 share/fwfetcher.py --uac --user
    python3 share/fwfetcher.py --nui --user

To install for all users, copy bin, lib, include and share to /usr/local
(the launchd agent and the microphone plug-in expect it there).

Licences
--------
libfreenect is available under the Apache License 2.0 or the GNU GPL v2, at
your choice. The licence texts (APACHE20, GPL2, and CONTRIB for the
contributors) are in the licenses folder on Windows, and in
share/doc/libfreenect/licenses on Linux (/usr/share/...) and macOS.
The source code of this build is at https://github.com/kebasaa/libfreenect.
Third-party libraries included in the Windows and macOS packages (Linux
uses the system's own), with their licences in the same folder:
  - libusb (LGPL 2.1): libusb.txt, source at https://github.com/libusb/libusb
  - GLFW (zlib licence): glfw.txt (Windows: glfw3.txt)
