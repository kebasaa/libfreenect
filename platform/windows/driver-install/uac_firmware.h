/*
 * This file is part of the OpenKinect Project. http://www.openkinect.org
 *
 * Copyright (c) 2026 individual OpenKinect contributors. See the CONTRIB file
 * for details.
 *
 * This code is licensed to you under the terms of the Apache License, version
 * 2.0, or, at your option, the terms of the GNU General Public License,
 * version 2.0. See the APACHE20 and GPL2 files for the text of the licenses,
 * or the following URLs:
 * http://www.apache.org/licenses/LICENSE-2.0
 * http://www.gnu.org/licenses/gpl-2.0.txt
 *
 * If you redistribute this file in source form, modified or unmodified, you
 * may:
 *   1) Leave this header intact and distribute it under the same terms,
 *      accompanying it with the APACHE20 and GPL20 files, or
 *   2) Delete the Apache 2.0 clause and accompany it with the GPL2 file, or
 *   3) Delete the GPL v2 clause and accompany it with the APACHE20 file
 * In all cases you must keep the copyright notice intact and include a copy
 * of the CONTRIB file.
 *
 * Binary distributions must follow the binary distribution requirements of
 * either License.
 */

#pragma once

#include <windows.h>

/* File name of the installed firmware. */
#define UAC_FIRMWARE_NAME L"UACFirmware.bin"

/*
 * Download Microsoft's Kinect for Windows Runtime v1.8 installer from
 * download.microsoft.com, check its Authenticode signature, extract the
 * audio/motor controller firmware (UACFirmware.bin) from it without running
 * it, verify the firmware, and write it to dest_dir\UACFirmware.bin.
 * Nothing of Microsoft's is redistributed with libfreenect. Temporary files
 * are deleted. Returns 0 on success.
 *
 * If local_installer is not NULL, that file is used instead of downloading
 * (for offline installs and tests). It is checked in the same way.
 */
int uac_fetch_firmware(const wchar_t *dest_dir, const wchar_t *local_installer);

/* Default destination: %ProgramData%\libfreenect. */
void uac_default_dir(wchar_t *dir, size_t len);
