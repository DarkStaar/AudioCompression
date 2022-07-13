/*
 * DSPHeader.h
 *
 *  Created on: 22.12.2020.
 *      Author: Dejan
 */

#ifndef DSPHEADER_H_
#define DSPHEADER_H_

typedef struct DSP_HEADER_
{
		char		ID[4];		// "DSP1"
		Uint16		quantBL;
		Uint16		quantBR;
		Uint16		MS;
		Uint32		size;
} DSP_HEADER;



#endif /* DSPHEADER_H_ */
