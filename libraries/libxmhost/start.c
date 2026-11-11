/* start.c -- the system brought up for the ISP, as majestic brings it up.
 *
 * XM_MPI_SYS_Init on its own is not enough after another program ran the
 * ISP: the hardware keeps going when that program is gone. The 3D noise
 * reduction goes on writing into buffers the MMZ reset then hands out
 * again, and the VI channels stay on; started over that, the ISP's
 * statistics sat at full scale and every frame came out black (M1, SC5332,
 * after majestic). majestic (xm530, master+6eb8e2c) first stops the 3D NR
 * (0x30040000 to 0, then 200 ms), disables VI channels 1 and 0 and sets
 * them to 64x64, and only then resets MMZ. So does this.
 *
 * SPDX-License-Identifier: MIT
 */
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "re_sys.h"
#include "xmhost.h"

XM_S32 Write_IspReg(XM_U32 u32Addr, XM_U32 u32Value);
XM_S32 Read_IspReg(XM_U32 u32Addr);

#define NR3D_CTRL 0x30040000
#define VI_DISABLE 0x40045603u
#define VI_SIZE 0x400C5601u

__attribute__((visibility("default"))) int xmhost_start(void) {
  if (Read_IspReg(NR3D_CTRL)) {
    Write_IspReg(NR3D_CTRL, 0);
    usleep(200000);
  }
  const int fd = open("/dev/vi", O_RDWR);
  if (fd < 0) {
    perror("xmhost: /dev/vi");
    return -1;
  }
  for (int chn = 1; chn >= 0; chn--) {
    int c = chn;
    struct {
      int chn;
      unsigned width, height;
    } size = {chn, 64, 64};
    /* which call fails, and why: a channel nobody enabled since boot may
     * refuse the disable, which would be nothing to report */
    if (ioctl(fd, VI_DISABLE, &c))
      fprintf(stderr, "xmhost: VI channel %d disable: %s\n", chn,
              strerror(errno));
    if (ioctl(fd, VI_SIZE, &size))
      fprintf(stderr, "xmhost: VI channel %d size 64x64: %s\n", chn,
              strerror(errno));
  }
  close(fd);
  return XM_MPI_SYS_Init();
}
