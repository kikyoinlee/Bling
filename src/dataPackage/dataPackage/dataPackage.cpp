#include <stdio.h>
#include <stdlib.h>
#include "typedef.h"
#include <vector>
#include <string>
#include <map>
using namespace std;

#define MAX_LEN 4096

//read mlf as a map
int readMlf(char *fileList, map <string, string> *mlf_map)
{
	FILE *fp_mlf = NULL;
	fp_mlf = fopen(fileList, "r");
	string onelineName;
	size_t found;
	char tmpStr[MAX_LEN];
	string wavKey;
	string wavValue;
	if(NULL == fp_mlf)
	{
		printf("Error : Can't open input mlf file %s!\n", fileList);
		return -1;
	}

	while(!feof(fp_mlf))
	{
		memset(tmpStr, 0, MAX_LEN);
		fgets(tmpStr, MAX_LEN, fp_mlf);
		if(strlen(tmpStr) < 2)
		{
			continue;
		}
		tmpStr[strlen(tmpStr) - 1] = '\0';
		onelineName = tmpStr;
		found = onelineName.find(" ");
		if (found !=  string::npos)
		{
			wavKey = onelineName.substr(0, found);
			wavValue = onelineName.substr(found + 1, MAX_LEN);
			mlf_map->insert(make_pair(wavKey, wavValue));
		}
		else
		{
			printf("WARNING : Can't find key and value in line : %s\n", tmpStr);
		}
	}
	fclose(fp_mlf);
	return 0;
}

//read file list as a map
int readFileList(char *fileList, map <string, string> *utt_key)
{
	FILE *fp_scp = NULL;
	fp_scp = fopen(fileList, "r");
	string onelineName;
	size_t found;
	char tmpStr[MAX_LEN];
	string wavKey;
	string wavValue;
	if(NULL == fp_scp)
	{
		printf("Error : Can't open input scp file %s!\n", fileList);
		return -1;
	}

	while(!feof(fp_scp))
	{
		memset(tmpStr, 0, MAX_LEN);
		fgets(tmpStr, MAX_LEN, fp_scp);
		if(strlen(tmpStr) < 2)
		{
			continue;
		}
		tmpStr[strlen(tmpStr) - 1] = '\0';
		onelineName = tmpStr;
		found = onelineName.find("=");
		if (found !=  string::npos)
		{
			wavKey = onelineName.substr(0, found);
			wavValue = onelineName.substr(found + 1, MAX_LEN);
			utt_key->insert(make_pair(wavKey, wavValue));
		}
		else
		{
			printf("WARNING : Can't find key and value in line : %s\n", tmpStr);
		}
	}
	fclose(fp_scp);
	return 0;
}

int main(int argc, char *argv[])
{
	if(argc != 5)
	{
		printf("usage : dataPackage.exe scp mlf split_part package_name\n");
		return -1;
	}

	FILE *fp_output = NULL;
	FILE *fp_split_scp = NULL;
	map <string, string> fileName_map;
	map <string, string> mlf_map;
	string wavName;
	int wavNum = 0;
	int splitNum = 0;
	string splitName = argv[4];
	string tmp;
	char tmpName[10];

	readFileList(argv[1], &fileName_map);
	wavNum = fileName_map.size();
	readMlf(argv[2], &mlf_map);
	splitNum = wavNum / atoi(argv[3]);

	map <string, string>::iterator it = fileName_map.begin();
	map <string, string>::iterator it_mlf;
	for(int i = 0; i < atoi(argv[3]); i++)
	{
		splitName = argv[4];
		itoa(i, tmpName, 10);
		splitName += tmpName;
		FILE *fp_part = fopen(splitName.c_str(), "wb");
		string wav_key;
		string wav_path;
		string mlf;
		STRU_wav wavHead;
		int fileLen;
		FILE *fp_wav = NULL;

		for(int j = 0; (j < splitNum) && (it != fileName_map.end()); j++, ++it)
		{
			wav_key = it->first;
			wav_path = it->second;
			strcpy(wavHead.key, wav_key.c_str());
			it_mlf = mlf_map.find(wav_key);
			if(it_mlf != mlf_map.end())
			{
				mlf = it_mlf->second;
				strcpy(wavHead.mlf, mlf.c_str());
			}
			fp_wav = NULL;
			fp_wav = fopen(wav_path.c_str(), "rb");
			if(NULL == fp_wav)
			{
				printf("Error : can't open file %s\n", wav_path.c_str());
				continue;
			}
			fseek(fp_wav, 0, SEEK_END);
			wavHead.wavLen = ftell(fp_wav);
			fseek(fp_wav, 0, SEEK_SET);
			char *buff = (char *)malloc(wavHead.wavLen);
			fwrite(&wavHead, sizeof(wavHead), 1, fp_part);
			fread(buff, 1, wavHead.wavLen, fp_wav);
			fwrite(buff, 1, wavHead.wavLen, fp_part);
			fclose(fp_wav);
			
			if(it == fileName_map.end())
			{
				break;
			}
		}
		fclose(fp_part);
	}
	return 0;
}
