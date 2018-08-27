#include "fft.h"

Fft::Fft(unsigned int count, int type)
	: m_type(0), m_plan(nullptr), m_input(nullptr), m_output(nullptr)
{
	reset(count, type);
}

Fft::~Fft()
{
	if (m_plan) {
		fftw_destroy_plan(m_plan);
		//fftw_cleanup();
	}
	
	if (m_input != nullptr) {
		fftw_free(m_input);
	}
	
	if (m_output != nullptr) {
		fftw_free(m_output);
	}
}

void Fft::cleanup()
{
	fftw_cleanup();
}


void Fft::reset(int fftlen, int type)
{
	bool remalloc = (fftlen != m_fftlen);
	bool replan = (fftlen != m_fftlen) || (type != m_type);

	m_fftlen = fftlen;
	m_type = type;

	if (remalloc) {
		if (m_input) {
			fftw_free(m_input);
		}

		if (m_output)
			fftw_free(m_output);

		m_input = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * m_fftlen);
		m_output = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * m_fftlen);
	}
	
	if (replan) {
		if (m_plan) {
			fftw_destroy_plan(m_plan);
			//fftw_cleanup();

			m_plan = nullptr;
		}

		m_plan = fftw_plan_dft_1d(fftlen, m_input, m_output, m_type, FFTW_PATIENT/*FFTW_ESTIMATE*/);
	}
}