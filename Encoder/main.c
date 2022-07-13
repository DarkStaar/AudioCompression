//////////////////////////////////////////////////////////////////////////////
// * File name: main.c
// *
// *
// * Description: Audio Encoder
// * Course: OAiS DSP1
// * Year: 2020
// *
// * Author: Milan Kapetanovic RA 184/2018
// * Faculty of Technical Sciences, Novi Sad
// *
//////////////////////////////////////////////////////////////////////////////

#include "stdio.h"
#include "ezdsp5535.h"
#include "ezdsp5535_i2c.h"
#include "aic3204.h"
#include "ezdsp5535_aic3204_dma.h"
#include "ezdsp5535_i2s.h"
#include "Dsplib.h"
#include "encode.h"
#include "WAVheader.h"
#include "DSPHeader.h"
#include "quant.h"
#include "encode.h"

static WAV_HEADER inputWAVhdr;
static DSP_HEADER outputHeader = {{'D','S','P','1'}, 0, 8, 1, 0};  //quantBL 16; quantBR 16; MS 0; size 0;

#define SAMPLE_RATE 48000L
#define GAIN 0


#pragma DATA_ALIGN(InputBufferL,4)
Int16 InputBufferL[AUDIO_IO_SIZE];
#pragma DATA_ALIGN(InputBufferR,4)
Int16 InputBufferR[AUDIO_IO_SIZE];

#pragma DATA_ALIGN(CompressedBuffer,4)
Int16 CompressedBuffer[FRAME_SIZE];

/*
 *
 *  main( )
 *
 */
void main( void )

{
	Int32 i;
	Int32 j;
	Int32 number_of_blocks;

    /* Initialize BSL */
    EZDSP5535_init( );

    printf("\n OAiS DSP 1 - Audio Encoder \n");
		
    /* Initialise hardware interface and I2C for code */
    aic3204_hardware_init();

    aic3204_set_input_filename("../63.wav");
	aic3204_set_output_filename("../out_encoder_0.dsp1");
	
    /* Initialise the AIC3204 codec */
	aic3204_init();
          
    aic3204_dma_init();
    
    aic3204_read_wav_header(&inputWAVhdr);

    number_of_blocks = inputWAVhdr.data.SubChunk2Size/
    		(inputWAVhdr.fmt.NumChannels*inputWAVhdr.fmt.BitsPerSample*AUDIO_IO_SIZE/8);

    outputHeader.size = number_of_blocks*FRAME_SIZE*2;

    //TODO Postaviti preostale parametre zaglavlja
    Uint16 BL = outputHeader.quantBL;
    Uint16 BR = outputHeader.quantBR;


    aic3204_write_dsp1_header(&outputHeader);

    init_vorbis_window(AUDIO_IO_SIZE);

    for(i = 0; i < number_of_blocks; i++)
	{
		aic3204_read_block(InputBufferL, InputBufferR);
		
		for(j = 0; j < AUDIO_IO_SIZE; j++)
		{
			Int16 M = (InputBufferL[j] + InputBufferR[j])>>2;
			Int16 S = (InputBufferL[j] - InputBufferR[j])>>2;
			InputBufferL[j] = M;
			InputBufferR[j] = S;
		}

		// TODO: Pozvati funkciju za enkodovanje
		encode(InputBufferL, InputBufferR, CompressedBuffer, AUDIO_IO_SIZE, BL, BR);

		aic3204_write_data(CompressedBuffer, FRAME_SIZE);
	}

	/* Disable I2S and put codec into reset */ 
    aic3204_disable();

    printf( "\n***Program has Terminated***\n" );
	SW_BREAKPOINT;
}

