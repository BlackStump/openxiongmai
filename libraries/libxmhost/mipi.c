/* mipi.c -- the XM530/XM550 MIPI receiver, as majestic sets it up.
 *
 * libispapp_XM530 calls XM_MPI_MIPI_SetDevAttr with the sensor's timing
 * (MIPI_SET_DEV_ATTR) and again to start it (MIPI_SET_RUN); the sensor
 * libraries call XM_MPI_MIPI_RefreshFV on a frame-length change. None of
 * the three is in the open MPI: majestic carries them. These do what
 * majestic's do (read from its xm530 build, master+6eb8e2c, 2026-11-10),
 * the same writes in the same order:
 *
 *   0x300c0008  the lane and depth word, from a table (below)
 *   0x300c000c  MipiCtrl as given
 *   0x300c0000  0xf882222a
 *   0x300c0004  3
 *   0x300c0010  delay as given
 *   0x300c0034  (active pixels + 16) << 16 | 16
 *   0x300c0038  1
 *   0x300c003c  active lines + 1
 *   0x300c0030  all lines | the ISP's line length in its clock << 16
 *
 * and to run: 0x300c002c to 0 then 4, 0x300f0010 to 1. A DVP sensor takes
 * the twelve pins at 0x10020120-0x1002014c to their function 2 instead.
 *
 * The ISP's line length, when the caller leaves bAllPixs 0, is the sensor's
 * line length in the ISP's clock: snsAllPixs * bMpDvpclk / pixel rate, the
 * pixel rate snsMpOutclk * 2 * lanes / bits; both divisions must be exact,
 * and the pixel rate no faster than the ISP's clock, as majestic refuses.
 *
 * SPDX-License-Identifier: MIT
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "mpi_phyvi.h"

/* libisp_XM530's, the open one's or the vendor's: the ISP's registers
 * through /dev/isp */
XM_S32 Write_IspReg(XM_U32 u32Addr, XM_U32 u32Value);
XM_S32 Read_IspReg(XM_U32 u32Addr);

#define ERR(s) fputs("\033[0;32;31mERR: " s "\n\033[m", stderr)

#define MIPI_CFG0 0x300c0000
#define MIPI_CFG1 0x300c0004
#define MIPI_LANES 0x300c0008
#define MIPI_CTRL 0x300c000c
#define MIPI_DELAY 0x300c0010
#define MIPI_RUN 0x300c002c
#define MIPI_LINES 0x300c0030
#define MIPI_HACT 0x300c0034
#define MIPI_VEN 0x300c0038
#define MIPI_VACT 0x300c003c
#define VI_INPUT 0x300f0010
#define DVP_PINS 0x10020120 /* twelve, to 0x1002014c */

/* By depth (8, 10, 12 bits) and lanes (1, 2, 4): the word for MIPI_LANES,
 * and the bits a pixel clock carries per MIPI clock (both edges, every
 * lane). majestic's tables, verbatim. */
static const XM_U32 lane_word[3][3] = {
    {0x2a010024, 0x2a010028, 0x2a010030},
    {0x2b010044, 0x2b010048, 0x2b010050},
    {0x2c010084, 0x2c010088, 0x2c010090},
};
static const XM_U8 lane_bits[3] = {2, 4, 8};
static const XM_U32 depth_bits[3] = {8, 10, 12};

static COMBO_DEV_ATTR_S kept;

static XM_S32 dev_attr(const COMBO_DEV_ATTR_S *a) {
  if (a->input_mode != SENSCONT_MIPI) {
    for (XM_U32 r = DVP_PINS; r <= DVP_PINS + 0x2c; r += 4)
      Write_IspReg(r, 2);
    memcpy(&kept, a, sizeof(kept));
    return XM_SUCCESS;
  }
  const MIPI_DEV_ATTR_S *m = &a->mipi_attr;
  if ((XM_U32)m->depth > 2 || (XM_U32)m->lane > 2) {
    ERR("mipimode_Err!");
    return XM_FAILURE;
  }
  const XM_U32 word = lane_word[m->depth][m->lane];
  uint64_t isp_line = m->bAllPixs;
  if (!isp_line) {
    const XM_U32 bits = depth_bits[m->depth];
    const XM_U32 per = m->snsMpOutclk * lane_bits[m->lane];
    if (per % bits) {
      ERR("mipioutclk1_Err!");
      return XM_FAILURE;
    }
    const XM_U32 pix = per / bits;
    if (!pix || pix > m->bMpDvpclk) {
      ERR("mipioutclk2_Err!");
      return XM_FAILURE;
    }
    const uint64_t n = (uint64_t)m->snsAllPixs * m->bMpDvpclk;
    if (n % pix) {
      ERR("mipioutclk3_Err!");
      return XM_FAILURE;
    }
    isp_line = n / pix;
  }
  Write_IspReg(MIPI_LANES, word);
  Write_IspReg(MIPI_CTRL, m->MipiCtrl);
  Write_IspReg(MIPI_CFG0, 0xf882222a);
  Write_IspReg(MIPI_CFG1, 3);
  Write_IspReg(MIPI_DELAY, m->delay);
  Write_IspReg(MIPI_HACT, (m->snsActivePixs + 16) << 16 | 16);
  Write_IspReg(MIPI_VEN, 1);
  Write_IspReg(MIPI_VACT, m->snsActiveLine + 1);
  Write_IspReg(MIPI_LINES, m->snsAllLine | (XM_U32)isp_line << 16);
  memcpy(&kept, a, sizeof(kept));
  return XM_SUCCESS;
}

__attribute__((visibility("default"))) XM_S32
XM_MPI_MIPI_SetDevAttr(ISP_DEV IspDev, MIPI_CMD enCmd,
                       const COMBO_DEV_ATTR_S *pstComboDevAttr) {
  if (IspDev != 0) {
    ERR("just support IspDev[0]!");
    return XM_FAILURE;
  }
  switch (enCmd) {
  case MIPI_SET_DEV_ATTR:
    if (!pstComboDevAttr) {
      ERR("when [MIPI_SET_DEV_ATTR] cmd, pstComboDevAttr can't NULL");
      return XM_FAILURE;
    }
    return dev_attr(pstComboDevAttr);
  case MIPI_SET_RUN:
    if (kept.input_mode == SENSCONT_MIPI) {
      Write_IspReg(MIPI_RUN, 0);
      Write_IspReg(MIPI_RUN, MIPI_SET_RUN);
      Write_IspReg(VI_INPUT, 1);
    } else {
      Write_IspReg(VI_INPUT, 0);
    }
    return XM_SUCCESS;
  default:
    /* the clock edge, reset and unreset: nothing in majestic either */
    return XM_SUCCESS;
  }
}

__attribute__((visibility("default"))) XM_S32
XM_MPI_MIPI_GetDevAttr(ISP_DEV IspDev, COMBO_DEV_ATTR_S *pstComboDevAttr) {
  if (IspDev != 0) {
    ERR("just support IspDev[0]!");
    return XM_FAILURE;
  }
  if (!pstComboDevAttr) {
    ERR("pstComboDevAttr can't NULL!");
    return XM_FAILURE;
  }
  memcpy(pstComboDevAttr, &kept, sizeof(kept));
  return XM_SUCCESS;
}

/* A new frame length (all lines) taken without stopping: the receiver
 * held for u32DelayMs (400 for 0), the lines set, run again. */
__attribute__((visibility("default"))) XM_S32
XM_MPI_MIPI_RefreshFV(XM_U32 u32DelayMs, XM_U32 u32TotalSizeV) {
  const XM_U16 lines = (XM_U16)u32TotalSizeV;
  Write_IspReg(MIPI_RUN, 0);
  if (lines)
    Write_IspReg(MIPI_LINES,
                 ((XM_U32)Read_IspReg(MIPI_LINES) & 0xffff0000) | lines);
  usleep((u32DelayMs ? u32DelayMs : 400) * 1000);
  Write_IspReg(MIPI_RUN, MIPI_SET_RUN);
  kept.mipi_attr.snsAllLine = lines;
  return XM_SUCCESS;
}
