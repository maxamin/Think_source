 0001:00001F21       ExecuteActions(CTaskActionArray &)
 0001:0000257A       CalcSubChannelCRC(uchar const *,ulong)
 0001:000025B5       GetCD_CRCTable(void)
 0001:00002940       CheckECCAndEDC_Mode1(uchar * const,int)
 0001:000029D0       CheckECCAndEDC_Mode2Form1(uchar * const,int)
 0001:00002A69       CheckECCAndEDC_Mode2Form2(uchar * const,int)
 0001:00002AD4       CheckECCAndEDC_Mode2Form1_2336(uchar * const,int)
 0001:00002BBA       CheckECCAndEDC_Mode2Form2_2336(uchar * const,int)
 0001:00002C29       GenECCAndEDC_Mode1(uchar const * const,SectHeaderMSF const *,uchar * const,SECT_TYPE)
 0001:00002CAD       Convert2kToMode2Form1(uchar const * const,uchar * const,uint,uchar const *)
 0001:00002D2E       Convert2kToMode2Form1With2332(uchar const * const,uchar * const,uchar const *)
 0001:00002D48       Convert2kToMode2Form1With2336(uchar const * const,uchar * const,uchar const *)
 0001:00002D62       GenECCAndEDC_Mode2Form1(uchar const * const,SectHeaderMSF const *,uchar const * const,uchar * const,SECT_TYPE)
 0001:00002DEC       GenECCAndEDC_Mode2Form2(uchar const * const,SectHeaderMSF const *,uchar const * const,uchar * const,SECT_TYPE)
 0001:00002E6B       GenEDC(uchar *,ulong,LittleEndian<ulong> *)
 0001:00003031       IdentifyTrack(uchar const * const)
 0001:000030A0       IdentifyTrackFor2336(uchar const * const)
 0001:000030B8       GenRawEmptySector(uchar * const,TRACKMODE,ulong,int)
 0001:00003184       ScrambleSector(uchar * const)
 0001:000031A5       ConvertMode2_2336_to_2352(uchar * const,SectHeaderMSF const *,uchar * const)
 0001:000031E0       ByteSwap2352(uchar *)
 0001:00003206       WordSwap2352(uchar *)
 0001:00003226       NormalizeRawSector(uchar *,ulong)
 0001:000032C7       GenEmptyAudio96Sector(uchar *,int,uchar,ulong,int)
 0001:000032DE       MergePQ16WithRWRaw(uchar *,uchar *,uchar const *)
 0001:0000339B       ExtractPQDataFromRaw(QDATAtag *,uchar *)
 0001:000034E0       ConvertPackedToRawSubChannel(uchar *,uchar *,uchar *,int)
