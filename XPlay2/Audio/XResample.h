#pragma once

class XResample
{
public:
	XResample();
	virtual ~XResample();
	///��һ��Ҫ�ز���������
	bool open(AVCodecParameters* para,bool isClear = false);

	///�ر��ز���
	void close();

	///�����ز����Ĵ�С,���۳ɹ����Ҫ�ͷ�indata�ռ�
	int32_t resample(AVFrame* indata, uint8_t* data);

	void setOutFormat(AVSampleFormat format);
private:
	AVSampleFormat m_format;
	QMutex m_mut;
	SwrContext* m_pActx;
};
