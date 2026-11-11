# libxmhost

What majestic gives the Xiongmai XM530/XM550 ISP libraries, for any other
program that runs them. `libispapp_XM530.so`, `libisp_XM530.so` and the
sensor libraries take about 85 functions from the program that loads them;
with this library loaded too, that program needn't be majestic:

- the open MPI's system, VI and region calls (`../libmpi`, built in);
- the MIPI receiver (`XM_MPI_MIPI_SetDevAttr`, `GetDevAttr`, `RefreshFV`,
  `mipi.c`), majestic's register writes reimplemented;
- the 3A library's hooks (`XM_MPI_ISP_AELibRegCallBack`,
  `AWBLibRegCallBack`, `XM_MPI_AE/AWB_SensorRegCallBack`) and the exposure,
  white balance and colour matrix attributes as a store (`alg.c`). The AE
  and AWB themselves are the open ones in `libisp_XM530`
  (`LIBISP_OPEN_AE=1`, `LIBISP_OPEN_AWB=1`), which this library expects;
- libispapp's `camera_*` and `Camera*` calls, weak and doing nothing
  (`camera.c`), for the program to replace where it wants them to act.

The encoder is not here: `../libxmvenc` is the open one.

`test/xmhost_test` brings the sensor up through the ISP into VI channel 0
and reports the frames, with majestic stopped.
