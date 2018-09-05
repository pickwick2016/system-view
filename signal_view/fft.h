#ifndef FFT_H
#define FFT_H

#include <complex>
#include <math.h>

#include <fftw/fftw3.h>
#pragma comment(lib, "fftw/libfftw3-3.lib")

/**
 * Fft计算.
 */
class Fft
{
public:
	Fft(unsigned int count = 1024, int type = FFTW_FORWARD);
	~Fft();

public:
	static void cleanup();

public:
	// FFT 点数（2^N）
	int length() { return m_fftlen; }

	// FFT 类型（正变换、逆变换）
	int type() { return m_type; }

public:
	// 计算fft数值. 错误返回0.
	int calculate(void * in, std::complex<double> * out, int fft, int datatype);

	// 计算fft数值. 错误返回0.
	template <class T>
	int calculate(const T * in, std::complex<double> * out, int fftlen);

	// 计算fft数值. 错误返回0.
	template <class T>
	int calculate(const std::complex<T> * in, std::complex<double> * out, int fftlen);

public:
	// 计算功率谱.
	int power(void * in, float * out, int fftlen, int datatype);

	template <class T>
	int power(const T * in, float * out, int fftlen);

	template <class T>
	int power(const std::complex<T> * in, float * out, int fftlen);

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
int Fft::calculate(const T * in, std::complex<double> * out, int fftlen)
{
	if (!in || fftlen <= 0) {
		return 0;
	}

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

	return fftlen;
}

template <class T>
int Fft::calculate(const std::complex<T> * in, std::complex<double> * out, int fftlen)
{
	if (!in || fftlen <= 0) {
		return 0;
	}

	reset(fftlen, FFTW_FORWARD);

	for (unsigned int i = 0; i < fftlen; i++) {
		m_input[i][0] = in[i].real();
		m_input[i][1] = in[i].imag();
	}

	fftw_execute(m_plan);

	if (out != nullptr) {
		for (unsigned int i = 0; i < fftlen; i++) {
			out[i].real(m_output[i][0]);
			out[i].imag(m_output[i][1]);
		}
	}

	return fftlen;
}

template <class T>
int Fft::power(const T * in, float * out, int fftlen)
{
	int ret = calculate(in, nullptr, fftlen);
	if (ret <= 0)
		return 0;

	for (int i = 0; i < fftlen / 2; i++) {
		out[i] = 5.0 * std::log10f(m_output[i][0] * m_output[i][0] + m_output[i][1] * m_output[i][1]);
	}

	return fftlen / 2;
}

template <class T>
int Fft::power(const std::complex<T> * in, float * out, int fftlen)
{
	int ret = calculate(in, nullptr, fftlen);
	if (ret <= 0)
		return 0;

	for (int i = 0; i < fftlen; i++) {
		out[i] = 5.0 * std::log10f(m_output[i][0] * m_output[i][0] + m_output[i][1] * m_output[i][1]);
	}

	return fftlen;
}

#endif //FFT_H