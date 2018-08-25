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
	int length() { return fftlen; }
	int type() { return ffttype; }

public:
	template <class T>
	void calculate(const T * in, std::complex<double> * out);

	template <class T>
	void calculate(const std::complex<T> * in, std::complex<double> * out);

	template <class T>
	void power(const T * in, float * out);

private:
	int ffttype;
	int fftlen;
	fftw_plan plan;
	fftw_complex * input, *output;
};


template <class T>
void Fft::calculate(const T * in, std::complex<double> * out)
{
	for (unsigned int i = 0; i < fftlen; i++) {
		input[i][0] = in[i];
		input[i][1] = 0;
	}

	fftw_execute(plan);

	if (out != nullptr) {
		for (unsigned int i = 0; i < fftlen; i++) {
			out[i].real(output[i][0]);
			out[i].imag(output[i][1]);
		}
	}
}

template <class T>
void Fft::calculate(const std::complex<T> * in, std::complex<double> * out)
{
	for (unsigned int i = 0; i < fftlen; i++) {
		input[i][0] = in[i].real();
		input[i][1] = in[i].imag();
	}

	fftw_execute(plan);

	if (out != nullptr) {
		//memcpy(out, output, sizeof(double) * 2 * fftlen);
		for (unsigned int i = 0; i < fftlen; i++) {
			out[i].real(output[i][0]);
			out[i].imag(output[i][1]);
		}
	}
}

template <class T>
void Fft::power(const T * in, float * out)
{
	calculate(in, nullptr);

	for (int i = 0; i < fftlen / 2; i++) {
		out[i] = std::log10f(output[i][0] * output[i][0] + output[i][1] * output[i][1]) / 20.0;
	}
}