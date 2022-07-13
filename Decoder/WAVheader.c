
#include "WAVheader.h"

// Type defs
//-----------------------------------------------------------------------------

const WAV_HEADER defaultWavHeader =
{
	{{'R', 'I', 'F', 'F'}, 0, {'W', 'A', 'V', 'E'}},
	{{'f', 'm', 't', ' '}, 16, 1, 2, 48000, 96000, 2, 16},
	{{'d', 'a', 't', 'a'}, 0},
	44
};
//-------------------------------------------------------------------------------
