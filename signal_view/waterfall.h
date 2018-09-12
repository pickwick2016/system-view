#ifndef WATERFALL_H
#define WATERFALL_H

#include <vector>
#include <utility>
#include <memory>
#include <tuple>
#include <functional>

#include <QRect>
#include <QPixmap>

class Fft;
class Reader;

/**
 * �ٲ�ͼ����������.
 * 1�����ݿͻ���ָ��ʱ��ε����󣬹������ṩ�ٲ�ͼ���ݣ�׼��һ�������û���������ݼ��ɣ�.
 * 2���Զ������ڲ��������.
 */
class WaterfallLoader
{
	// �ڲ�״̬3Ԫ��<fft-len, pos-range, pos-step>
	typedef std::tuple<unsigned int, std::pair<unsigned int, unsigned>, unsigned int> State;

public:
	WaterfallLoader(unsigned int fftlen = 128);
	~WaterfallLoader();

public:
	// ��������.
	bool load(const std::string & filename, int type, double samplerate);

	void load(std::shared_ptr<Reader> reader);

	bool isLoaded();
	
	// �ر�����.
	void close();

public:
	/**
	 * ׼���ٲ�ͼ����.
	 * 1.��鵱ǰ�����Ƿ���ã���������ã��͸���.
	 * 2.���׼����ɣ����Ե���pixmap(), datamap(), ��ȡ��ǰ���ݣ�����pixmapSize()��datamapSize()��ȡͼ��ߴ�.
	 *
	 * @param visible ��ǰ���ӷ�Χ.
	 * @param sizeHint ����ֶε�����(segmentHint, freqHint).
	 * @return �������.
	 */
	bool prepare(QRectF visible, std::pair<int, int> sizeHint = { 0, 0 });

	// ������߼�ʱƵ��Χ.
	QRectF totalArea();

	// ��ǰƵ������.
	std::vector<float> & datamap() { return m_datamap; }

	// Ƶ�����ݴ�С (freq_count, segment_count)
	std::pair<int, int> dataSize();

	// ��ǰƵ��������ֵ��Χ.
	std::pair<float, float> currentValueRange() { return m_valueRange; }

	// ��ǰͼ������.
	QPixmap & pixmap() { return m_pixmap; }

	// ��ǰ���ݵ�ʱ���Ƶ�ʷ�Χ���߼�ֵ������ʱ�䣬����Ƶ�ף�.
	QRectF pixmapArea();

	// ��ǰɫ�ʷ�Χ.
	std::pair<float, float> colorRange() { return m_colorRange; }
	
	// ����ɫ�ʷ�Χ.
	void setColorRange(std::pair<float, float> rng);


	void setAutoFft(bool on, int fftlen = 256);
	bool isAutoFft() { return m_isAutoFft; }
		
private:
	// �������󣬼��㵱ǰ״̬.
	State calculateState(QRectF visible, std::pair<int, int> sizeHint = { 0, 0 });

	// ��ָ��״̬��������.������ɹ���ͬʱ�ı��ڲ�״̬��ʧ�ܣ��򲻸ı䣩
	bool reload(State state);

	// ���ݵ�ǰ״̬��������������.
	bool reloadValues();

	// ���ݵ�ǰ��ֵ���ع�λͼ.
	void reloadPixmap();
	
	// ���״̬.
	void clear();
	
	// ��ֵת��Ϊɫ��.
	uint32_t valueToColor(float val);

private:
	std::shared_ptr<Reader> m_reader;	// ��ȡ��.

	std::shared_ptr<Fft> m_fft; // fft ������.
	std::vector<unsigned int> m_fftHints;

	unsigned int m_stepAlign; // ����.

	QPixmap m_pixmap;
	std::vector<unsigned int> m_pixmapData; // ͼ�����ݣ�Ԫ��Ϊ uint32 �� RGBA ���ݣ�
	
	std::vector<float> m_datamap;
	std::pair<float, float> m_valueRange;

	std::pair<float, float> m_colorRange;
	std::pair<float, float> m_prevColorRange;

	std::function<std::tuple<int, int, int>(float, float, float)> m_colormap; // ɫ��ת������.

	State m_state;
	
	bool m_isAutoFft;
	unsigned int m_fixFft;
};

#endif //WATERFALL_H


