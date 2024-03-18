/*----------------------------------------------------------------------------
 *
 *				«ј√ќЋќ¬ќ„Ќџ… ‘ј…Ћ Ѕ»ЅЋ»ќ“≈ » ElbyECC
 *			составленный на основе дизассемблировани€ оной
 *			==============================================
 *
 * Build 0x001 @ 02.07.2003
----------------------------------------------------------------------------*/


// расчитать EDC/ECC пол€ дл€ user data и добавить к нему пол€ SYNC+HEADER
// -----------------------------------------------------------------------
// userdata - входные данные
// header - заголовок (Sector address/mode)
// sector  - сгенерированный сектор
// int __stdcall GenECCAndEDC_Mode1(char *userdata, int *header, char *sector);

// проверить EDC/ECC и при необходимости выполнить восстановление userdata
// -----------------------------------------------------------------------
// rawsector - указатель на сектор который провер€ть/лечить (src/dst)
// DO : 1 - лечить, 0 - только проверка
// ret: 1 - сектор хорош/вылечен успешно, 0 - сектор плох или не вылечен
//CheckECCAndEDC_Mode1(char *rawsector , int DO);

// расчитать новые EDC/ECC дл€ userdata
//-------------------------------------
// rawsector - указатель на сектор
// ReGenECCAndEDC_Mode1(char *rawsector)


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
