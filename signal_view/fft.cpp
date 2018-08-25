#include "fft.h"

Fft::Fft(unsigned int count, int type)
{
	fftlen = count;
	ffttype = type;

	input = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fftlen);
	output = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fftlen);

	plan = fftw_plan_dft_1d(fftlen, input, output, ffttype, FFTW_ESTIMATE);
}

Fft::~Fft()
{
	fftw_destroy_plan(plan);
	fftw_cleanup();

	if (input != NULL)
		fftw_free(input);

	if (output != NULL)
		fftw_free(output);
}

