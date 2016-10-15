#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

#define KEY_LEN 128
#define MLF_LEN 256

typedef struct tagSTRU_wav
{
	char key[KEY_LEN];
	char mlf[MLF_LEN];
	int wavLen;
}STRU_wav, *PST_wav;

#endif //_TYPEDEF_H_