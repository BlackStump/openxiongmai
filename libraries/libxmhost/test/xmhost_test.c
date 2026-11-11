/* xmhost_test -- the sensor through the ISP into VI channel 0 with no
 * majestic: libxmhost, the vendor's libispapp_XM530 and sensor library,
 * and a libisp_XM530 (the open one, for its AE and AWB).
 *
 *   xmhost_test [seconds]       (default 20)
 *
 * It sets the system up (XM_MPI_SYS_Init), has libispapp start the sensor
 * and the ISP in its mode 0 (the application owns VI), sets channel 0 up as
 * majestic does on the SC5332 (2600x1456 at 0,248 of the frame, to
 * 1920x1080), then takes a frame a second and prints its size and mean luma.
 * The frame at 10 s goes to /tmp/xmhost.nv12. Stop majestic first.
 *
 * SPDX-License-Identifier: MIT
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "re_sys.h"
#include "re_vi.h"

int ispApp(int argc, char *argv[]);

static int frame_luma(const VIDEO_FRAME_INFO_S *f, int save) {
  const XM_U32 w = f->stVFrame.u32Width, h = f->stVFrame.u32Height;
  const size_t ysize = (size_t)w * h, size = ysize * 3 / 2;
  const long page = sysconf(_SC_PAGESIZE);
  const XM_U32 phys = f->stVFrame.u32PhyAddr[0];
  const XM_U32 base = phys & ~(XM_U32)(page - 1), off = phys - base;
  int fd = open("/dev/mem", O_RDONLY | O_SYNC);
  if (fd < 0)
    return -1;
  unsigned char *m = mmap(NULL, size + off, PROT_READ, MAP_SHARED, fd, base);
  close(fd);
  if (m == MAP_FAILED)
    return -1;
  const unsigned char *y = m + off;
  unsigned long sum = 0, n = 0;
  for (size_t i = 0; i < ysize; i += 97, n++)
    sum += y[i];
  if (save) {
    FILE *o = fopen("/tmp/xmhost.nv12", "wb");
    if (o) {
      /* the chroma plane as the frame gives it, when it is not right after */
      fwrite(y, 1, ysize, o);
      const XM_U32 cphys = f->stVFrame.u32PhyAddr[1];
      if (cphys == phys + ysize)
        fwrite(y + ysize, 1, ysize / 2, o);
      fclose(o);
      printf("saved /tmp/xmhost.nv12 (%ux%u NV12%s)\n", w, h,
             cphys == phys + ysize ? "" : ", luma only");
    }
  }
  munmap(m, size + off);
  return n ? (int)(sum / n) : -1;
}

int main(int argc, char **argv) {
  const int seconds = argc > 1 ? atoi(argv[1]) : 20;
  setvbuf(stdout, NULL, _IOLBF, 0);

  if (XM_MPI_SYS_Init() != XM_SUCCESS) {
    fprintf(stderr, "XM_MPI_SYS_Init failed\n");
    return 1;
  }
  /* as majestic starts it: mode 0, PAL timing, resolution 0, the sensor's
   * own interface */
  char *isp_argv[] = {"ispApp", "-m", "0", "-v", "p", "-r", "0", NULL};
  if (ispApp(7, isp_argv) != 0)
    fprintf(stderr, "ispApp returned non-zero (going on)\n");

  VI_CHN_ATTR_S vi;
  memset(&vi, 0, sizeof(vi));
  vi.stCapRect.s32X = 0;
  vi.stCapRect.s32Y = 248;
  vi.stCapRect.u32Width = 2600;
  vi.stCapRect.u32Height = 1456;
  vi.stDestSize.u32Width = 1920;
  vi.stDestSize.u32Height = 1080;
  if (XM_MPI_VI_SetChnAttr(0, &vi) != XM_SUCCESS ||
      XM_MPI_VI_EnableChn(0) != XM_SUCCESS) {
    fprintf(stderr, "VI channel 0 failed\n");
    return 1;
  }

  int got = 0;
  for (int s = 1; s <= seconds; s++) {
    sleep(1);
    VIDEO_FRAME_INFO_S f;
    memset(&f, 0, sizeof(f));
    if (XM_MPI_VI_GetFrame(0, &f) != XM_SUCCESS) {
      printf("%2d s: no frame\n", s);
      continue;
    }
    got++;
    const int luma = frame_luma(&f, s == 10);
    printf("%2d s: %ux%u pts %llu, mean luma %d\n", s, f.stVFrame.u32Width,
           f.stVFrame.u32Height, (unsigned long long)f.stVFrame.u64pts, luma);
    XM_MPI_VI_ReleaseFrame(0, &f);
  }
  printf("%d frames of %d\n", got, seconds);
  return got ? 0 : 1;
}
