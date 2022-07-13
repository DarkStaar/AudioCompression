//////////////////////////////////////////////////////////////////////////////
// * File name: decode.c
// *
// *
// * Description: Audio Decoder
// * Course: OAiS DSP1
// * Year: 2020
// *
// * Author: Milan Kapetanovic RA 184/20180
// * Faculty of Technical Sciences, Novi Sad
// *
//////////////////////////////////////////////////////////////////////////////

#include "decode.h"

#include "Dsplib.h"
#include "ezdsp5535_aic3204_dma.h"
#include <math.h>
#include "quant.h"


#define PI 3.1415926535
#define FFT_SIZE (2*AUDIO_IO_SIZE)

//Niz koji ce biti prosledjen funkciji rifft mora biti poravnat na 4.
#pragma DATA_ALIGN(fft_bufferL,4)
Int16 fft_bufferL[FFT_SIZE*2];
#pragma DATA_ALIGN(fft_bufferR,4)
Int16 fft_bufferR[FFT_SIZE*2];

static Int16 out_delayL[FFT_SIZE/2];
static Int16 out_delayR[FFT_SIZE/2];

Int16 M[FFT_SIZE];
Int16 S[FFT_SIZE];

Int16 window[FFT_SIZE];

void init_vorbis_window(Int16 N)
{
  int i;
  double t;

  for (i = 0; i < N; i++) {
    t = sin(0.5 * PI * (i + 0.5) / N);
    window[i] = 32767 * sin(0.5 * PI * t * t);
    window[2*N-1-i] = window[i];
  }
}

int sign(Int16 x)
{
    if (x > 0)
    {
    	return 1;
    }
    else if (x < 0)
    {
    	return -1;
    }
    else return 0;
}

// TODO: Realizovati funkciju za dekodovanje i unutar nje uraditi vracanje u opseg i sintezu za svaki kanal ponaosob.
void decode(Int16* in, Int16* outL, Int16* outR, Int16 N, Uint32 size, Uint16 BL, Uint16 BR)
{
	int i;
	int ogranicenje1 = (256/48000) * 100;
	int ogranicenje2 = (256/48000) * 512;
	int ogranicenje3 = (256/48000) * 4096;
	int ogranicenje4 = (256/48000) * 14336;

	if(BL == 0)
	{
		//Zadatak 4
		for(i = 0; i < 2*N; i++)
		{
			if(i <= ogranicenje1)
			{
				fft_bufferL[i] = 0;
			}else if(i <= ogranicenje2)
			{
				fft_bufferL[i] = reconstructB(fft_bufferL[i], 8);
			}else if(i <= ogranicenje3)
			{
				fft_bufferL[i] = reconstructB(fft_bufferL[i], 12);
			}else if(i <= ogranicenje4)
			{
				fft_bufferL[i] = reconstructB(fft_bufferL[i], 6);
			}else
			{
				fft_bufferL[i] = 0;
			}
		}
	}else
	{
		//Vracanje u opseg
		for(i = 0; i < 2*N; i++)
		{
			fft_bufferL[i] = reconstructB(in[2*i], BL);
		}
	}

	//Vracanje u opseg -- stavljeno ovde zbog Zadatka 3

	if(BR == 0)
	{
		//Zadatak 4
		for(i = 0; i < 2*N; i++)
		{
			if(i <= ogranicenje1)
			{
				fft_bufferR[i] = 0;
			}else if(i <= ogranicenje2)
			{
				fft_bufferR[i] = reconstructB(fft_bufferR[i], 8);
			}else if(i <= ogranicenje3)
			{
				fft_bufferR[i] = reconstructB(fft_bufferR[i], 12);
			}else if(i <= ogranicenje4)
			{
				fft_bufferR[i] = reconstructB(fft_bufferR[i], 6);
			}else
			{
				fft_bufferR[i] = 0;
			}
		}
	}else
	{
		for(i = 0; i < 2*N; i++)
		{
			fft_bufferR[i] = reconstructB(in[2*i+1], BR);
		}
	}

	//Zadatak 3
	for(i = 0; i < 2*N; i++)
	{
		M[i] = fft_bufferL[i];
		S[i] = fft_bufferR[i];
		fft_bufferL[i] = M[i] + S[i];
		fft_bufferR[i] = M[i] - S[i];
	}

	//Zadatak 2
    for (i = 0; i < 2*N; i++)
    {
        fft_bufferL[i] = sign(fft_bufferL[i]) * _smpy(fft_bufferL[i], fft_bufferL[i]);
    }
	//Sinteza
	rifft(fft_bufferL, 2*N, NOSCALE);

	for(i = 0; i < N; i++)
	{
		outL[i] = _smpy(fft_bufferL[i], window[i]) + _smpy(out_delayL[i], window[i + N]);
		outL[i] *= 4;
		out_delayL[i] = fft_bufferL[N+i];
	}

	//Zadatak2
	//Za desni kanal
    for (i = 0; i < 2*N; i++)
    {
        fft_bufferR[i] = sign(fft_bufferR[i]) * _smpy(fft_bufferR[i], fft_bufferR[i]);
    }

	//Sinteza
	rifft(fft_bufferR, 2*N, NOSCALE);
	for(i = 0; i < N; i++)
	{
		outR[i] = _smpy(fft_bufferR[i], window[i]) + _smpy(out_delayR[i], window[N+i]);
		outR[i] *= 4;
		out_delayR[i] = fft_bufferR[N+i];
	}
}
