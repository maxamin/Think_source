#include <stdio.h>
#include <memory.h>

#pragma pack( push, before_include1 )
#pragma pack(1)

//============================================================================
//                      FUNDAMENTAL DATA TYPES
//============================================================================
#ifndef BYTE
	#define BYTE unsigned char
#endif

#ifndef WORD
	#define WORD short unsigned int
#endif

#ifndef DWORD
	#define DWORD unsigned int
#endif
//----------------------------------------------------------------------------



// relative LBA
#ifndef RLBA
	#define RLBA signed int
#endif

// unsigned LBA
#ifndef ULBA
	#define ULBA unsigned int
#endif

//							CONSTANTS
//===========================================================================
//
#ifndef RAW_SECTOR_SIZE
	#define RAW_SECTOR_SIZE	2352
#endif

#ifndef SYNC_SIZE
	#define SYNC_SIZE	12
#endif

#ifndef HEADER_SIZE
	#define HEADER_SIZE	4
#endif

#ifndef USER_DATA_SIZE
	#define USER_DATA_SIZE	2048
#endif

#ifndef EDC_SIZE
	#define EDC_SIZE 4
#endif

#ifndef ZERO_SIZE
	#define ZERO_SIZE	8
#endif

#ifndef P_SIZE
	#define P_SIZE 172
#endif

#ifndef Q_SIZE
	#define Q_SIZE 104
#endif


//=[MSF]======================================================================
// 
// 		MSF	-	structure storing minutes, seconds and frames
//			    (mainly used for addressing)
#ifndef  MSF
	struct MSF
	{
			BYTE M;		// MIN
			BYTE S;		// SEC
			BYTE F;		// FRAME
	};
#endif

#ifndef BCD
	struct BCD{
			BYTE M;		// MIN
			BYTE S;		// SEC
			BYTE F;		// FRAME

	};
#endif


#ifndef RAW_SECTOR_MODE1
	struct RAW_SECTOR_MODE1
	{
			BYTE		SYNC[SYNC_SIZE];
			struct BCD  ADDR;
			BYTE		MODE;
			BYTE		USER_DATA[USER_DATA_SIZE];
			BYTE		EDC[EDC_SIZE];
			BYTE		ZERO[ZERO_SIZE];
			BYTE		P[P_SIZE];
			BYTE		Q[Q_SIZE];
	};
#endif

#ifndef RAW_SECTOR_MODE2FORM1
	struct RAW_SECTOR_MODE2FORM1
	{
		BYTE	RAW[RAW_SECTOR_SIZE];
	};
#endif

#ifndef RAW_SECTOR_MODE2FORM2
	struct RAW_SECTOR_MODE2FORM2
	{
		BYTE	RAW[RAW_SECTOR_SIZE];
	};
#endif


#ifndef RAW_SECTOR
	struct RAW_SECTOR
	{
		BYTE	RAW[RAW_SECTOR_SIZE];
	};
#endif

//										ФУНКЦИИ
//============================================================================
// 


#define ElBy_TEST_ONLY		0
#define	ElBy_REPAIR			1
#define ElBy_SECTOR_OK		1
#define ElBy_SECTOR_ERROR	0
#define ElBy_NEED_REPAIR	0

#define NOT_SUPPORT 		-1

#define TRACK_MODE0			3
#define TRACK_MODE1			0
#define TRACK_MODE2			NOT_SUPPORT
#define TRACK_MODE2FORM1	1
#define TRACK_MODE2FORM2	2
#define TRACK_ERR			4
#define TRACK_TEXT(a)		((a==TRACK_MODE0)?"MODE0":(a==TRACK_MODE1)?"MODE1":\
							(a==TRACK_MODE2)?"MODE2":(a==TRACK_MODE2FORM1)?"MODE2FORM1":\
							(a==TRACK_MODE2FORM2)?"MODE2FORM2":"UNKNOWN MODE")



//=[BCDtoMSF]=================================================================
// 		преобразует BCD-число, в котором представлены абсолютные адреса
// в заголовках секторов, в MSF-формат (минуты:секунды:фреймы)
//
extern BCDtoMSF(struct BCD *bcd, struct MSF *msf);

extern MSFtoBCD(struct MSF *msf, struct BCD *bcd);

extern LBAtoMSF(RLBA LBA, struct MSF *msf);

extern ULBAtoMSF(ULBA LBA, struct MSF *msf);

extern RLBA MSFtoLBA(struct MSF *msf);

extern ULBA MSFtoULBA(struct MSF *msf);

extern LBAtoBCD(RLBA lba, struct BCD *bcd);

extern ULBAtoBCD(ULBA lba, struct BCD *bcd);

extern RLBA BCDtoLBA(struct BCD *bcd);

extern ULBA BCDtoULBA(struct BCD *bcd);

// проверить EDC/ECC и при необходимости выполнить восстановление userdata
// -----------------------------------------------------------------------
// rawsector - указатель на сектор который проверять/лечить (src/dst)
// DO : 1 - лечить, 0 - только проверка
// ret: 1 - сектор хорош/вылечен успешно, 0 - сектор плох или не вылечен
// (для MODE2 FORM2 возможна только проверка, т.к. избыточных кодов там нет)
extern CheckSector(struct RAW_SECTOR *sector, int DO);
extern CheckSectors(struct RAW_SECTOR *sector, int raw_sector_size, int N_SEC, int DO);
extern CheckECCAndEDC_Mode1(struct RAW_SECTOR_MODE1 *sector , int DO);
extern CheckECCAndEDC_Mode2Form1(struct RAW_SECTOR_MODE2FORM1 *sector , int DO);
extern CheckECCAndEDC_Mode2Form2(struct RAW_SECTOR_MODE2FORM1 *sector);


// генерация различных секторов
// ---------------------------------------------------------------------------------
//  raw_sector_dst - сектор-приемник, 
//					внимание USER_DATA копируется не всегда! читайте комментарии к DO
//  DO				- что делать-то?
//					 == 0: построить SYNC и HEADER_ADDR на основе переданного адреса
//					       с MODE = 0 и забить USER_DATA_dst нулями
//                   == 1: построить сектор в MODE1, расчитать EDC/ECC по USER_DATA
//                         саму USER_DATA при этом не копировать в сектор
//                   == 2: построить SYNC и HEADER_ADDR в MODE 2, остальное ничего
//                         не трогать
//                   == 3: построить сектор в MODE2FORM1, рассчитать EDC/ECC по USER
//                         но саму USER_DATA при этом не копировать в сектор назнач.
//                   == 4: построить сектор в MODE2FORM, рассчитать EDC по USER,
//                         но саму USER не копировать
//                   == 5: постоить BAD сектор в MODE1 (с SYNC & HEADER & UUUUUUU)
//                   == 6: построить BAD сектор в MODE2 (с SYNC & HEADER & UUUUUU)
//	
// LBA  			- LBA адрес сектора
//
// USER_DATA		- указатель на USER_DATA по которой выполняется расчет EDC/ECC
//                   внимание! если младший байт адреса != 0, то поле M ареса
//                   OR'ится с A0h!
//
extern GenerateSector(struct RAW_SECTOR *raw_sector_dst, int DO, RLBA lba,  char *user_data);


// определелить тип трека
// -------------------------------------------------------------------------
// 
// ret: 0 - MODE1
//      1 - MODE2FORM1
//      2 - MODE2FORM2
//		3 - MODE0
//      4 - NO SYNC (NOT VALID SECTOR)
//
// REM:
//		1) CloneCD  полагает, что MODE2 всегда имеет FORM1/FORM2
//		2) в MODE0 если SubMode и 4 последних байта != 0 - ошибка
//						
extern IdentifyTrack(struct RAW_SECTOR *sector);

// расчитать EDC/ECC поля для user data и добавить к нему поля SYNC+HEADER
// -----------------------------------------------------------------------
// userdata - входные данные
// header   - заголовок (Sector address/mode)
// sector   - сгенерированный сектор
// RET:
//		 0 - err
//    != 0 - ok
extern GenECCAndEDC_Mode1(char *userdata_src, int *header_src,
						struct RAW_SECTOR_MODE1 *raw_sector_mode1_dst);

extern GenECCAndEDC_Mode2Form1(char *userdata_src, char *header_src, char *subheader_src,
						struct RAW_SECTOR_MODE2FORM1 *raw_sector_mode2form1_dst);

extern GenECCAndEDC_Mode2Form2(char *userdata_src, char *header_src, char *subheader_src,
						struct RAW_SECTOR_MODE2FORM2 *raw_sector_mode2form2_dst);


// построить нулевой сектор для Lead-In области
//--------------------------------------------------------------------
// реально записыват лишь SYNC & ADDR в HEADER, переводя из LBA в M:S:F
// raw_sector_dst - сектор куда писать 
// f_build        - == 0,1: строить заголовок
//		 		  -  >= 2  : не строить заголовок
// lba            -  LBA адрес
// f_lba            == 0 : от lba следует отнять 150 секторов
//                  != 0 : брать lba таким какой он есть
extern GenHeadAndSyc_4_RawDatenLeadin_0Sect(struct RAW_SECTOR *raw_sector_dst,int f_build, ULBA lba, int f_lba);
	

// скремблировать сектор
//-------------------------------------------------------------
// скрамблит все от SYNC и до конца
// raw_sector		- сырой сектор (источник/приемник) 
// N_SEC			- кол-во секторов для скремблирования
//                    (внимание! в ElByECC.DLL ver 4.0.0.0 есть
//                     баг, пускающий функцию в разнос когда N_SEC == 0)
//                     ScrambleSectors отвечает на нулевой N_SEC корректной ошибкой
//
// raw_sec_size     - размер 1го сектора
extern RawScrambleSector(struct RAW_SECTOR *raw_sector);
extern RawScrambleSectors(struct RAW_SECTOR *raw_sector, int N_SEC, int raw_sec_size);

// RET: 0 - no SYNC, 1 - OK
extern ScrambleSector(struct RAW_SECTOR *raw_sector);
extern ScrambleSectors(struct RAW_SECTOR *raw_sector, int N_SEC, int raw_sector_size);

// RET: 0 - no SYNC, 1 - OK
externUnScrambleSector(struct RAW_SECTOR *raw_sector);
extern UnScrambleSectors(struct RAW_SECTOR *raw_sector, int N_SEC, int raw_sector_size);

// расчитать новые EDC/ECC для userdata
//-------------------------------------
// rawsector - указатель на сектор
// ReGenECCAndEDC_Mode1(char *rawsector)
extern ReGenECCAndEDC(struct RAW_SECTOR *raw_sector);
extern ReGenECCAndEDC_Mode1(struct RAW_SECTOR *raw_sector);
extern ReGenECCAndEDC_Mode2Form1(struct RAW_SECTOR *raw_sector);

// Sync-шаблок
//extern SyncByteTemplate;

#pragma pack( pop, before_include1 )

#define LEN_SCREEN	80
print_hex(unsigned char *p, int len)
{
	int a;
	for (a = 0; a < len; a++)
	{
		printf("%02X ", p[a]);
		if ((a+1) % ((LEN_SCREEN - 1)/3) == 0) printf("\n");
	}printf("\n");
}

#define SECHEADER_SIZE			4
#define RAWSECTOR_SIZE			2352

#define SYNC_SIZE				12
#define pSYNC					0

#define HEADER_SIZE				4
#define pHEADER					12

#define USER_DATA_SIZE			2048
#define pUSER_DATA				16

#define EDC_SIZE				4
#define pEDC					2064

#define INTERMEDIATE_SZ			8
#define pINTERMEDIATE			2068

#define P_party_SIZE			172
#define pP_party				2076

#define Q_party_SIZE			104
#define pQ_party				2248

#define CHECK_ONLY				0	
#define CHECK_AND_RECORVER		1

#define SECTOR_OK				1
#define NEED_RECORVER			0
#define SECTOR_ERROR			0


print_sector(char *rawsector)
{
	printf("\nSYNC:\n");      print_hex(&rawsector[pSYNC], SYNC_SIZE);
	printf("\nHEADER:\n");    print_hex(&rawsector[pHEADER], HEADER_SIZE);
	printf("\nUSER DATA:\n"); print_hex(&rawsector[pUSER_DATA], USER_DATA_SIZE);
	printf("\nEDC:\n");       print_hex(&rawsector[pEDC], EDC_SIZE);
	printf("\nINTERMEDIATE:\n");       print_hex(&rawsector[pINTERMEDIATE], INTERMEDIATE_SZ);
	printf("\nP Party:\n");       print_hex(&rawsector[pP_party], P_party_SIZE);
	printf("\nQ Party:\n");       print_hex(&rawsector[pQ_party], Q_party_SIZE);
}




#define FILE_SECTOR_MODE1		"MODE1.good.dat"
#define FILE_SECTOR_MODE2FORM1  "MODE2Form1.good.dat"

#define PUT_ERR_MIN	6
#define PUT_ERR_MAX 666
main()
{
	
    int		a;                   	
	RLBA	lba;
	ULBA	ulba;
    FILE	*sector_mode1;
    FILE	*sector_mode2form1;

	struct	BCD 	bcd;
	struct	MSF 	msf;

	struct	RAW_SECTOR_MODE1	raw_sector_mode1;
	struct	RAW_SECTOR_MODE1	raw_sector_mode1_damaged;
	struct	RAW_SECTOR_MODE1	raw_sector_mode1_new;

	struct	RAW_SECTOR_MODE2FORM1	raw_sector_mode2form1;
	struct	RAW_SECTOR_MODE2FORM1	raw_sector_mode2form1_damaged;
	struct	RAW_SECTOR_MODE2FORM1	raw_sector_mode2form1_new;
		
	printf("ElbyCloneCD KERNEL enginere gate by Kris Kaspersky\n");
	printf("ElbyECC library testing...\n\n");

	// иницилизация
	sector_mode1 = fopen(FILE_SECTOR_MODE1, "rb");
	if ( sector_mode1== 0)
		{ fprintf(stderr,"-ERR: open file %s\x7\n",FILE_SECTOR_MODE1); return -1;}
	else
		{ fprintf(stderr,"+OK: open file %s\n",FILE_SECTOR_MODE1);}
		
	a = fread(&raw_sector_mode1, 1, sizeof(raw_sector_mode1), sector_mode1);
	if (a != sizeof(raw_sector_mode1))
		{ fprintf(stderr, "-ERR: read file %s\x7\n",FILE_SECTOR_MODE1); return -1;}
	else
	    { fprintf(stderr,"+OK: read file %s\n",FILE_SECTOR_MODE1);}

    //
	sector_mode2form1 = fopen(FILE_SECTOR_MODE2FORM1, "rb");
	if ( sector_mode2form1== 0)
		{ fprintf(stderr,"-ERR: open file %s\x7\n",FILE_SECTOR_MODE2FORM1); return -1;}
	else
		{ fprintf(stderr,"+OK: open file %s\n",FILE_SECTOR_MODE2FORM1);}
		
	a = fread(&raw_sector_mode2form1, 1, sizeof(raw_sector_mode2form1), sector_mode2form1);
	if (a != sizeof(raw_sector_mode2form1))
		{ fprintf(stderr, "-ERR: read file %s\x7\n",FILE_SECTOR_MODE2FORM1); return -1;}
	else
	    { fprintf(stderr,"+OK: read file %s\n",FILE_SECTOR_MODE2FORM1);}


	//print_sector(&raw_sector_mode1);

	printf("\n");

	// +
	printf("= test BCDtoMSF   @ %02Xh:%02Xh:%02Xh -> ", bcd.M = 0x16, bcd.S = 0x11, bcd.F = 0x10);
	BCDtoMSF(&bcd, &msf);
	printf("%d:%d:%d\n",msf.M, msf.S, msf.F, bcd.F = bcd.S =bcd.M = 0);

	// +
	printf("= test MSFtoBCD   @ %d:%d:%d    -> ", msf.M, msf.S, msf.F); 
    MSFtoBCD(&msf, &bcd);
    printf("%02Xh:%02Xh:%02Xh\n",bcd.M, bcd.S, bcd.F);

    // +
    printf("= test LBAtoMSF   @ %d         -> ", lba = -75);
    LBAtoMSF(lba, &msf);
    printf("%02d:%02d:%02d\n",msf.M, msf.S, msf.F);

    // +
    printf("= test MSFtoLBA   @ %02d:%02d:%02d    -> ", msf.M, msf.S, msf.F, lba = 0);
    lba = MSFtoLBA(&msf);
    printf("%d\n",lba);

	// +
	printf("= test ULBAtoMSF  @ %d         -> ", ulba = 666);
	ULBAtoMSF(ulba, &msf);
	printf("%02d:%02d:%02d\n",msf.M, msf.S, msf.F);

    // +
    printf("= test MSFtoULBA  @ %02d:%02d:%02d    -> ", msf.M, msf.S, msf.F, lba = 0);
    ulba = MSFtoULBA(&msf);
    printf("%d\n",ulba);

    // +
    printf("= test LBAtoBCD   @ %d         -> ", lba = -10);
    LBAtoBCD(lba, &bcd);
    printf("%02Xh:%02Xh:%02Xh\n",bcd.M, bcd.S, bcd.F);

    // +
    printf("= test BCDtoLBA   @ %02Xh:%02Xh:%02Xh -> ", bcd.M, bcd.S, bcd.F, lba = 0);
    lba = BCDtoLBA(&bcd);
    printf("%d\n",lba);

    // +
    printf("= test ULBAtoBCD  @ %d         -> ", ulba = 666);
    ULBAtoBCD(ulba, &bcd);
    printf("%02Xh:%02Xh:%02Xh\n",bcd.M, bcd.S, bcd.F);

    // +
    printf("= test BCDtoULBA  @ %02Xh:%02Xh:%02Xh -> ", bcd.M, bcd.S, bcd.F, ulba = 0);
    ulba = BCDtoULBA(&bcd);
    printf("%d\n",ulba);

    printf("\n");

	// ++++
    printf("= test CheckECCAndEDC_Mode1 @ good (TEST_ONLY) -> ");
    a = CheckECCAndEDC_Mode1(&raw_sector_mode1, ElBy_TEST_ONLY);
    printf("%s\n",(a == ElBy_SECTOR_OK)?"SECTOR_OK":"SECTOR ERROR");

    // +
    printf("= test CheckECCAndEDC_Mode1 @ damaged (TEST_ONLY) -> ");
    memcpy(&raw_sector_mode1_damaged, &raw_sector_mode1, sizeof(raw_sector_mode1));
    for (a = 0; a < PUT_ERR_MIN; a++) raw_sector_mode1_damaged.USER_DATA[a] ^= 0xFF;
    a = CheckECCAndEDC_Mode1(&raw_sector_mode1_damaged, ElBy_TEST_ONLY);
    printf("%s\n",(a == ElBy_SECTOR_OK)?"SECTOR_OK":"SECTOR ERROR");

    // +
	printf("= test CheckECCAndEDC_Mode1 @ damaged (REPAIR) -> ");    
    a = CheckECCAndEDC_Mode1(&raw_sector_mode1_damaged, ElBy_REPAIR);
    printf("%s",(a == ElBy_SECTOR_OK)?"SECTOR REPAIR OK":"SECTOR REPAIR ERROR");
    printf(" (%s)\n",(memcmp(&raw_sector_mode1_damaged, &raw_sector_mode1,
    			sizeof(raw_sector_mode1)) == 0)?"REALLY REPAIRED":"NOT REPAIRED YET");

    	
	// +++
    printf("= test CheckECCAndEDC_Mode2Form1 @ good (TEST_ONLY) -> ");
    a = CheckECCAndEDC_Mode2Form1(&raw_sector_mode2form1, ElBy_TEST_ONLY);
    printf("%s\n",(a == ElBy_SECTOR_OK)?"SECTOR_OK":"SECTOR ERROR");

    // +
    printf("= test CheckECCAndEDC_Mode2Form1 @ damaged (TEST_ONLY) -> ");
    memcpy(&raw_sector_mode2form1_damaged, &raw_sector_mode2form1, sizeof(raw_sector_mode2form1));
    for (a = 0; a < PUT_ERR_MIN; a++) raw_sector_mode2form1_damaged.RAW[a + 33 /* fime */] ^= 0xFF;
    a = CheckECCAndEDC_Mode2Form1(&raw_sector_mode2form1_damaged, ElBy_TEST_ONLY);
    printf("%s\n",(a == ElBy_SECTOR_OK)?"SECTOR_OK":"SECTOR ERROR");

    // +
	printf("= test CheckECCAndEDC_Mode2Form1 @ damaged (REPAIR) -> ");    
    a = CheckECCAndEDC_Mode2Form1(&raw_sector_mode2form1_damaged, ElBy_REPAIR);
    printf("%s",(a == ElBy_SECTOR_OK)?"SECTOR REPAIR OK":"SECTOR REPAIR ERROR");
    printf(" (%s)\n",(memcmp(&raw_sector_mode2form1_damaged, &raw_sector_mode2form1,
    			sizeof(raw_sector_mode2form1)) == 0)?"REALLY REPAIRED":"NOT REPAIRED YET");


	// +++
    printf("= test CheckSector    @ good MODE1 (TEST_ONLY) -> ");
    a = CheckSector((struct RAW_SECTOR*) &raw_sector_mode1, ElBy_TEST_ONLY);
    printf("%s\n",(a == ElBy_SECTOR_OK)?"SECTOR_OK":"SECTOR ERROR");

    // +
    printf("= test CheckSector    @ good MODE2FORM1 (TEST_ONLY) -> ");
    a = CheckSector((struct RAW_SECTOR*) &raw_sector_mode2form1, ElBy_TEST_ONLY);
    printf("%s\n",(a == ElBy_SECTOR_OK)?"SECTOR_OK":"SECTOR ERROR");

    // +
    printf("= test CheckSector   @ damaged MODE1 (TEST_ONLY) -> ");
    memcpy(&raw_sector_mode1_damaged, &raw_sector_mode1, sizeof(raw_sector_mode1));
    for (a = 0; a < PUT_ERR_MIN; a++) raw_sector_mode1_damaged.USER_DATA[a] ^= 0xFF;
    a = CheckSector((struct RAW_SECTOR*)&raw_sector_mode1_damaged, ElBy_TEST_ONLY);
    printf("%s\n",(a == ElBy_SECTOR_OK)?"SECTOR_OK":"SECTOR ERROR");

    // +
    printf("= test CheckSector   @ damaged MODE2FORM1 (TEST_ONLY) -> ");
    memcpy(&raw_sector_mode2form1_damaged, &raw_sector_mode2form1, sizeof(raw_sector_mode2form1));
    for (a = 0; a < PUT_ERR_MIN; a++) raw_sector_mode2form1_damaged.RAW[a + 33 /* fixme */] ^= 0xFF;
    a = CheckSector((struct RAW_SECTOR*)&raw_sector_mode2form1_damaged, ElBy_TEST_ONLY);
    printf("%s\n",(a == ElBy_SECTOR_OK)?"SECTOR_OK":"SECTOR ERROR");

    // +
	printf("= test CheckSector @ damaged MODE1 (REPAIR) -> ");    
    a = CheckSector((struct RAW_SECTOR*)&raw_sector_mode1_damaged, ElBy_REPAIR);
    printf("%s",(a == ElBy_SECTOR_OK)?"SECTOR REPAIR OK":"SECTOR REPAIR ERROR");
    printf(" (%s)\n",(memcmp(&raw_sector_mode1_damaged, &raw_sector_mode1,
    			sizeof(raw_sector_mode1)) == 0)?"REALLY REPAIRED":"NOT REPAIRED YET");

	// +
	printf("= test CheckSector @ damaged MODE2FORM1 (REPAIR) -> ");    
    a = CheckSector((struct RAW_SECTOR*)&raw_sector_mode2form1_damaged, ElBy_REPAIR);
    printf("%s",(a == ElBy_SECTOR_OK)?"SECTOR REPAIR OK":"SECTOR REPAIR ERROR");
    printf(" (%s)\n",(memcmp(&raw_sector_mode2form1_damaged, &raw_sector_mode2form1,
    			sizeof(raw_sector_mode2form1)) == 0)?"REALLY REPAIRED":"NOT REPAIRED YET");

	// +++
    printf("= test CheckSectors    @ good MODE1 (TEST_ONLY) -> ");
    a = CheckSectors((struct RAW_SECTOR*) &raw_sector_mode1, RAW_SECTOR_SIZE, 1, ElBy_TEST_ONLY);
    printf("%s\n",(a == ElBy_SECTOR_OK)?"SECTOR_OK":"SECTOR ERROR");

    // +
    printf("= test CheckSectors   @ damaged MODE1 (TEST_ONLY) -> ");
    memcpy(&raw_sector_mode1_damaged, &raw_sector_mode1, sizeof(raw_sector_mode1));
    for (a = 0; a < PUT_ERR_MIN; a++) raw_sector_mode1_damaged.USER_DATA[a] ^= 0xFF;
    a = CheckSectors((struct RAW_SECTOR*)&raw_sector_mode1_damaged, RAW_SECTOR_SIZE, 1, ElBy_TEST_ONLY);
    printf("%s\n",(a == ElBy_SECTOR_OK)?"SECTOR_OK":"SECTOR ERROR");


	// +++
	printf("= test IdentifyTrack @ MODE1 -> ");
	a = IdentifyTrack((struct RAW_SECTOR*)&raw_sector_mode1);
	printf("%s\n",TRACK_TEXT(a));

	// +
	printf("= test IdentifyTrack @ MODE2FORM1 -> ");
	a = IdentifyTrack((struct RAW_SECTOR*)&raw_sector_mode2form1);
	printf("%s\n",TRACK_TEXT(a));


    // BUILD SECTOR (GET USER_DATA, HEADER AND GENERATE RS-CODE)
    // =======================================================================

	// +++
  	printf("= test GenECCAndEDC_Mode1 @ -> ");
	a = GenECCAndEDC_Mode1(&raw_sector_mode1.USER_DATA[0], &raw_sector_mode1.ADDR,&raw_sector_mode1_new);
	printf("%s",(a == ElBy_SECTOR_OK)?"SECTOR_OK":"SECTOR ERROR");
    printf(" (%s)\n",(memcmp(&raw_sector_mode1, &raw_sector_mode1_new,
    			sizeof(raw_sector_mode1)) == 0)?"REALLY OK":"NOT GOOD");

    // +
  	printf("= test GenECCAndEDC_Mode2Form1 @  -> ");
	a = GenECCAndEDC_Mode2Form1(&raw_sector_mode2form1.RAW[24], &raw_sector_mode2form1.RAW[12],
	&raw_sector_mode2form1.RAW[16], &raw_sector_mode2form1_new);
	printf("%s",(a == ElBy_SECTOR_OK)?"SECTOR_OK":"SECTOR ERROR");
    printf(" (%s)\n",(memcmp(&raw_sector_mode2form1, &raw_sector_mode2form1_new,
    			sizeof(raw_sector_mode2form1)) == 0)?"REALLY OK":"NOT GOOD");

    // ++++
    printf("= test  GenHeadAndSyc_4 @ BUILD %02d ->",ulba  = 10);
    memset(&raw_sector_mode1, 0, sizeof(raw_sector_mode1));
    GenHeadAndSyc_4_RawDatenLeadin_0Sect(&raw_sector_mode1,1, ulba, 1);
    printf(" ... ");for (a = 8; a < 16; a++) printf("%02X ",((unsigned char *)&raw_sector_mode1)[a]); printf("\n");
	

	// +++++
	printf(" = test GenerateSector @ ZERO ->");
	GenerateSector((struct RAW_SECTOR*)&raw_sector_mode1, 0, 0x10, &raw_sector_mode1.USER_DATA);

//extern GenerateSector(struct RAW_SECTOR *sector, int DO, RLBA lba,  char *user_data);

}

