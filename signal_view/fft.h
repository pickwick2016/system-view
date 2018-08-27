#pragma once

#include <complex>
#include <math.h>

#include <fftw/fftw3.h>
#pragma comment(lib, "fftw/libfftw3-3.lib")

/**
 * Fftº∆À„.
 */
class Fft
{
public:
	Fft(unsigned int count = 1024, int type = FFTW_FORWARD);
	~Fft();

public:
	int length() { return m_fftlen; }
	int type() { return m_type; }

public:
	template <class T>
	void calculate(const T * in, std::complex<double> * out, int fftlen);

	template <class T>
	void calculate(const std::complex<T> * in, std::complex<double> * out, int fftlen);

	template <class T>
	void power(const T * in, float * out, int fftlen);

	template <class T>
	void power(const std::complex<T> * in, float * out, int fftlen);

private:
	void reset(int fftlen, int type);

private:
	int m_type;
	int m_fftlen;
	fftw_plan m_plan;
	fftw_complex * m_input;
	fftw_complex *m_output;
};


template <class T>
void Fft::calculate(const T * in, std::complex<double> * out, int fftlen)
{
	reset(fftlen, FFTW_FORWARD);

	for (unsigned int i = 0; i < fftlen; i++) {
		m_input[i][0] = in[i];
		m_input[i][1] = 0;
	}

	fftw_execute(m_plan);

	if (out != nullptr) {
		for (unsigned int i = 0; i < fftlen; i++) {
			out[i].real(m_output[i][0]);
			out[i].imag(m_output[i][1]);
		}
	}
}

template <class T>
void Fft::calculate(const std::complex<T> * in, std::complex<double> * out, int fftlen)
{
	reset(fftlen, FFTW_FORWARD);

	for (unsigned int i = 0; i < fftlen; i++) {
		m_input[i][0] = in[i].real();
		m_input[i][1] = in[i].imag();
	}

	fftw_execute(m_plan);

	if (out != nullptr) {
		//memcpy(out, m_output, sizeof(double) * 2 * fftlen);
		for (unsigned int i = 0; i < fftlen; i++) {
			out[i].real(m_output[i][0]);
			out[i].imag(m_output[i][1]);
		}
	}
}

template <class T>
void Fft::power(const T * in, float * out, int fftlen)
{
	//std::vector<std::complex<double>> temp(fftlen);
	//calculate(in, temp.data(), fftlen);
	calculate(in, nullptr, fftlen);

	for (int i = 0; i < fftlen / 2; i++) {
		//out[i] = std::log10f(std::abs(temp[i]));
		out[i] = 5.0 * std::log10f(m_output[i][0] * m_output[i][0] + m_output[i][1] * m_output[i][1]);
	}
}

template <class T>
void Fft::power(const std::complex<T> * in, float * out, int fftlen)
{
	calculate(in, nullptr, fftlen);
	for (int i = 0; i < fftlen; i++) {
		out[i] = 5.0 * std::log10f(m_output[i][0] * m_output[i][0] + m_output[i][1] * m_output[i][1]);
	}
}