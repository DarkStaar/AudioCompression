//////////////////////////////////////////////////////////////////////////////
// * File name: encode.c
// *
// *
// * Description: Audio Decoder
// * Course: OAiS DSP1
// * Year: 2020
// *
// * Author: Milan Kapetanovic RA 184/2018
// * Faculty of Technical Sciences, Novi Sad
// *
//////////////////////////////////////////////////////////////////////////////
#include "encode.h"

#include "Dsplib.h"
#include "ezdsp5535_aic3204_dma.h"
#include <math.h>
#include "quant.h"


#define PI 3.1415926535
#define FFT_SIZE (2*AUDIO_IO_SIZE)

#pragma DATA_ALIGN(fft_bufferL,4)
#pragma DATA_ALIGN(fft_bufferR,4)
static Int16 fft_bufferL[FFT_SIZE];
static Int16 fft_bufferR[FFT_SIZE];

DATA *x = fft_bufferL;
DATA *r = fft_bufferR;
short nx = FFT_SIZE;
DATA *x1 = fft_bufferL;
DATA *r1 = fft_bufferR;

static Int16 in_delayL[FFT_SIZE/2];
static Int16 in_delayR[FFT_SIZE/2];

Int16 sign1[FFT_SIZE];
Int16 sign2[FFT_SIZE];

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


Int16 znak[FFT_SIZE];

// TODO: Realizovati funkciju za enkodovanje i unutar nje uraditi analizu i kvantizaciju za svaki kanal ponaosob

void encode(Int16* inL, Int16* inR, Int16* out, Int16 N, Uint16 BL, Uint16 BR)
{
	int i;
	int ogranicenje1 = (256/48000) * 100;
	int ogranicenje2 = (256/48000) * 512;
	int ogranicenje3 = (256/48000) * 4096;
	int ogranicenje4 = (256/48000) * 14336;
	//Analiza za levi kanal
	for(i = 0; i < N; i++)
	{
		fft_bufferL[i] = in_delayL[i];
		fft_bufferL[N+i] = inL[i];
		in_delayL[i] = inL[i];
	}

	for(i = 0; i < 2*N; i++)
	{
		fft_bufferL[i] = _smpy(fft_bufferL[i], window[i]);
	}

	rfft(fft_bufferL, FFT_SIZE, SCALE);

	//Zadatak 2
	for(i = 0; i < 2*N; i++)
	{
		if (fft_bufferL[i] > 0)
		{
			sign1[i] = 1;
		}
		else if(fft_bufferL[i] < 0)
		{
			sign1[i] = -1;
		}
		else sign1[i] = 0;
	}

	sqrt_16(x, r, nx);

	for(i = 0; i < 2*N; i++)
	{
		fft_bufferL[i] = sign1[i] * fft_bufferL[i];
	}

	if(BL == 0)
	{
		//Zadatak 4
		for(i = 0; i < 2*N; i++)
		{
			if(i <= ogranicenje1)
			{
				out[2*i] = 0;
			}else if(i <= ogranicenje2)
			{
				out[2*i] = quantB(fft_bufferL[i], 8);
			}else if(i <= ogranicenje3)
			{
				out[2*i] = quantB(fft_bufferL[i], 12);
			}else if(i <= ogranicenje4)
			{
				out[2*i] = quantB(fft_bufferL[i], 6);
			}else
			{
				out[2*i] = 0;
			}
		}
	}else
	{
		//Kvantizacija za levi kanal
		for(i =0; i < 2*N; i++)
		{
			out[2*i] = quantB(fft_bufferL[i], BL);
		}

	}


	//Analiza za desni kanal
	for(i = 0; i < N; i++)
	{
		fft_bufferR[i] = in_delayR[i];
		fft_bufferR[N+i] = inR[i];
		in_delayR[i] = inR[i];
	}

	for(i = 0; i < 2*N; i++)
	{
		fft_bufferR[i] = _smpy( fft_bufferR[i], window[i]);
	}

	rfft(fft_bufferR, FFT_SIZE, SCALE);

	//Zadatak 2
	for(i = 0; i < 2*N; i++)
	{
		if (fft_bufferR[i] > 0)
		{
			sign2[i] = 1;
		}
		else if(fft_bufferR[i] < 0)
		{
			sign2[i] = -1;
		}
		else sign2[i] = 0;
	}

	sqrt_16(x1, r1, nx);

	for(i = 0; i < 2*N; i++)
	{
		fft_bufferR[i] = sign2[i] * fft_bufferR[i];
	}

	if(BR == 0)
	{
		//Zadatak 4
		for(i = 0; i < 2*N; i++)
		{
			if(i <= ogranicenje1)
			{
				out[2*i] = 0;
			}else if(i <= ogranicenje2)
			{
				out[2*i] = quantB(fft_bufferR[i], 8);
			}else if(i <= ogranicenje3)
			{
				out[2*i] = quantB(fft_bufferR[i], 12);
			}else if(i <= ogranicenje4)
			{
				out[2*i] = quantB(fft_bufferR[i], 6);
			}else
			{
				out[2*i] = 0;
			}
		}
	}else
	{
		//Kvantizacija za desni kanal
		for(i = 0; i < 2*N; i++)
		{
			out[2*i+1] = quantB(fft_bufferR[i], BR);
		}
	}


}

