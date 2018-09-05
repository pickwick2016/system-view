#include "fft.h"
#include "reader.h"

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

int Fft::calculate(void * in, std::complex<double> * out, int fft, int datatype)
{
	switch (datatype) {
	case DataType::Int8:
		return calculate<char>((const char *) in, out, fft);

	case DataType::Int16:
		return calculate<short>((const short *)in, out, fft);

	case DataType::Int32:
		return calculate<int>((const int *)in, out, fft);

	case DataType::Real32:
		return calculate<float>((const float *)in, out, fft);

	case DataType::Real64:
		return calculate<double>((const double *)in, out, fft);

	case DataType::Int8_2:
		return calculate<char>((const std::complex<char> *)in, out, fft);

	case DataType::Int16_2:
		return calculate<short>((const std::complex<short> *)in, out, fft);

	case DataType::Int32_2:
		return calculate<int>((const std::complex<int> *)in, out, fft);

	case DataType::Real32_2:
		return calculate<float>((const std::complex<float> *)in, out, fft);

	case DataType::Real64_2:
		return calculate<double>((const std::complex<double> *)in, out, fft);

	default:
		return 0;
	}
}

int Fft::power(void * in, float * out, int fftlen, int datatype)
{
	switch (datatype) {
	case DataType::Int8:
		return power<char>((const char *)in, out, fftlen);

	case DataType::Int16:
		return power<short>((const short *)in, out, fftlen);

	case DataType::Int32:
		return power<int>((const int *)in, out, fftlen);

	case DataType::Real32:
		return power<float>((const float *)in, out, fftlen);

	case DataType::Real64:
		return power<double>((const double *)in, out, fftlen);

	case DataType::Int8_2:
		return power<char>((const std::complex<char> *)in, out, fftlen);

	case DataType::Int16_2:
		return power<short>((const std::complex<short> *)in, out, fftlen);

	case DataType::Int32_2:
		return power<int>((const std::complex<int> *)in, out, fftlen);

	case DataType::Real32_2:
		return power<float>((const std::complex<float> *)in, out, fftlen);

	case DataType::Real64_2:
		return power<double>((const std::complex<double> *)in, out, fftlen);

	default:
		return 0;
	}
}

