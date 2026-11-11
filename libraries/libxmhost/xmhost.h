/* xmhost.h -- libxmhost's own calls (the rest it gives are the MPI's).
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef XMHOST_H
#define XMHOST_H

/* Bring the system up for the ISP as majestic does, in its order: stop the
 * 3D noise reduction a previous program left running (and wait 200 ms for
 * it to let go of its buffers), disable VI channels 1 and 0 and shrink them
 * to 64x64, then XM_MPI_SYS_Init (the MMZ reset, VI and regions). Then
 * libispapp's ispApp, then the VI channels. 0, or the failing step's error. */
int xmhost_start(void);

#endif
