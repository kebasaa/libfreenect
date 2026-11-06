# libfreenect on macOS

Supported: the newest macOS and the one before it, on Apple Silicon and Intel.
libfreenect uses libusb's IOKit backend; no kernel extension, driver or
special permission is needed.

    brew install libusb glfw cmake
    cmake -S . -B build && cmake --build build

To build a universal library (arm64 and x86_64), libusb has to be universal
too (Homebrew's isn't): build it from source with
`CFLAGS="-arch arm64 -arch x86_64"` and configure libfreenect with
`-DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"`, `LIBUSB_1_INCLUDE_DIR` and
`LIBUSB_1_LIBRARY` pointing to it. CI does exactly this
(`.github/workflows/ci.yml`, "macOS universal").

## Audio firmware at plug-in

The Kinect's audio controller starts in a bootloader and needs firmware before
its microphones work (and, on the model 1473 and the Kinect for Windows, the
motor and LED). Microsoft's UAC firmware turns it into a standard USB audio
device, which Core Audio then offers as a 4-channel microphone (Audio MIDI
Setup, Sound settings), with no libfreenect application running.

Get the firmware (downloaded from Microsoft, checked and extracted; nothing of
Microsoft's is shipped with libfreenect):

    python3 fwfetcher.py --uac --user     # ~/Library/Application Support/libfreenect

libfreenect also looks in `/opt/homebrew/share/libfreenect` and
`/usr/local/share/libfreenect`. If `audios.bin` (the Xbox firmware) is there
too, libfreenect prefers it; set `LIBFREENECT_AUDIO_FIRMWARE=uac` to use the UAC
firmware.

To upload it automatically whenever the Kinect is plugged in, install the
launchd agent (`share/libfreenect/macos/org.openkinect.fwloader.plist` after
`cmake --install`, or `build/platform/macos/` in a build tree):

    cp org.openkinect.fwloader.plist ~/Library/LaunchAgents/
    launchctl bootstrap gui/$(id -u) ~/Library/LaunchAgents/org.openkinect.fwloader.plist

launchd starts `freenect-fwloader --launchd` when the audio bootloader
(USB 045e:02ad or 045e:02be) appears; it uploads the firmware and exits. To
remove it:

    launchctl bootout gui/$(id -u)/org.openkinect.fwloader
    rm ~/Library/LaunchAgents/org.openkinect.fwloader.plist

To use the agent with firmware in `~/Library/Application Support`, install it
as above (as your user, not in `/Library/LaunchAgents` for all users).

## Notes

- libfreenect claims only interface 0 of the audio controller; with the UAC
  firmware Core Audio keeps the microphone interfaces.
- The viewers use GLFW and OpenGL, which is deprecated on macOS but still
  present.
- Planned: a Core Audio plug-in for a beamformed microphone (with the audio DSP
  of Phase 4b) and a Camera Extension that serves depth and colour to other
  apps, both optional and needing Developer ID signing.
