#include <stdio.h>
#include "windows.h"
#include "webrtc_vad.h"
#include "malloc.h"
#include "string.h"
#include "stdlib.h"

#define FRAME_SIZE 160

int main(int argc, char** argv)
{
    WebRtc_Word16 rs = -1;
    WebRtc_Word8 version[1024];
    WebRtc_Word32 channelSizeCnt;
    WebRtc_Word32 handler;
    WebRtc_Word32 *pHandler = &handler;
    VadInst **ppHandler = (VadInst **)&pHandler; 
    WebRtc_Word8 vad_inst_addr_tmp;
    WebRtc_Word8* vad_inst_addr = (WebRtc_Word8*) &vad_inst_addr_tmp;
    WebRtc_Word16 mode = 3;
    WebRtc_Word16 speech_frame = 0;
    WebRtc_Word16 frame_buffer[FRAME_SIZE];
    FILE* fpIn = NULL;
    FILE* fpOut = NULL;
    WebRtc_Word8 inputFileName[1024];
    WebRtc_Word8 outputFileName[1024];
    WebRtc_Word8 outputMode;
    WebRtc_Word16 vad_flag;
    WebRtc_Word16 vad_flag_buffer[FRAME_SIZE];
    WebRtc_Word32 cnt = 0;
    WebRtc_Word32 i;
    HMODULE hDLL = LoadLibrary("lib_vad.dll"); 

    if (NULL != hDLL)
    {
        if (5 != argc)
        {
            printf("the number of arguments is error!\n");
            return -1;
        }
        strcpy(inputFileName, argv[1]);
        strcpy(outputFileName, argv[2]);
        mode=atoi(argv[3]);
        outputMode=atoi(argv[4]);

        rs = WebRtcVad_get_version(version, 1024);
        if (0 != rs)
        {
            printf("error while getting the software version!\n");
            return -1;
        }
        else
        {
            printf("current software version is %s\n", version);
        }

        rs = WebRtcVad_AssignSize(&channelSizeCnt);
        if (0 != rs)
        {
            printf("error while getting size!\n");
            return -1;
        }
        else
        {
            printf("size is %d\n", &channelSizeCnt);
        }

        rs = WebRtcVad_Assign(ppHandler, (void *)vad_inst_addr);
        if (0 != rs)
        {
            printf("error while assigning space!\n");
            return -1;
        }
        else
        {
            printf("success to assign space\n");
        }

        rs = WebRtcVad_Create(ppHandler);
        if (0 != rs)
        {
            printf("error while creating!\n");
            return -1;
        }
        else
        {
            printf("success to create\n");
        }

        rs = WebRtcVad_Init(*ppHandler);
        if (0 != rs)
        {
            printf("error while initial!\n");
            return -1;
        }
        else
        {
            printf("success to init\n");
        }

        rs = WebRtcVad_set_mode(*ppHandler, mode);
        if (0 != rs)
        {
            printf("error while setting mode!\n");
            return -1;
        }
        else
        {
            printf("success to set mode to %d\n", mode);
        }

        fpIn =fopen(inputFileName,"rb");
        if(fpIn == NULL)
        {
            printf("input file open error\n");
            return -1;
        }

        fpOut =fopen(outputFileName,"wb");
        if(fpOut == NULL)
        {
            printf("output file open error\n");
            return -1;
        }

        while (FRAME_SIZE == fread(frame_buffer, sizeof(WebRtc_Word16), FRAME_SIZE, fpIn))
        {
            cnt++;
            vad_flag = (WebRtc_Word16) WebRtcVad_Process(*ppHandler,
                16000,
                frame_buffer,
                FRAME_SIZE);
            printf("frame: %d  vad flag: %d\n", cnt, vad_flag);
            for (i = 0; i < FRAME_SIZE; i++)
            {
                vad_flag_buffer[i] = vad_flag*10000;
            }
            if (1 == outputMode)
            {
                if (1 == vad_flag)
                {
                    fwrite(frame_buffer, sizeof(WebRtc_Word16), FRAME_SIZE, fpOut);
                }
            }
            else
            {
                fwrite(vad_flag_buffer, sizeof(WebRtc_Word16), FRAME_SIZE, fpOut);
            }
        }

        fclose(fpIn);
        fclose(fpOut);

        rs = WebRtcVad_Free(*ppHandler);
        if (0 != rs)
        {
            printf("error while freeing!\n");
            return -1;
        }
        else
        {
            printf("success to free\n");
        }

        free(hDLL);
    }

	return 0;
}