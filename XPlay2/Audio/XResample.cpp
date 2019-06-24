#include "stable.h"
#include "XResample.h"

XResample::XResample()
	: m_format(AV_SAMPLE_FMT_S16)
	, m_pActx(nullptr)
{
}

XResample::~XResample()
{
}

bool XResample::open(AVCodecParameters* para,bool isClear)
{
	QMutexLocker locker(&m_mut);
	///��Ƶ�ز�����������
	//����ز�����nullptr���Զ�����alloc
#if 0
	if (!m_pActx)
	{
		m_pActx = swr_alloc();
	}
#endif
	///��Ƶ�ز��������ĳ�ʼ��
	m_pActx = swr_alloc_set_opts(
		m_pActx,
		av_get_default_channel_layout(2),	///�����ʽ2ͨ��
		m_format,							///���������ʽ
		para->sample_rate,					///���������
		av_get_default_channel_layout(para->channels),		///�����ʽ
		(AVSampleFormat)para->format,									///
		para->sample_rate,
		0, 0
	);
	isClear ? avcodec_parameters_free(&para) : 0;

	int re = swr_init(m_pActx);
	if (re != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		debug_out << "swr_init\t" << buf << endl;
		system("pause");
		return false;
	}
	return true;
}

void XResample::close()
{
	QMutexLocker locker(&m_mut);
	if (m_pActx)
	{
		swr_free(&m_pActx);
	}
}

int32_t XResample::resample(AVFrame* indata, uint8_t* d)
{
	if (!indata)
	{
		return 0;
	}
	if (!d)
	{
		av_frame_free(&indata);
	}
	uint8_t* data[2] = { 0 };
	data[0] = d;
	int re = swr_convert(m_pActx, data, indata->nb_samples,				///�������
		(const uint8_t**)indata->data, indata->nb_samples				///����
	);
	if (re < 0)
	{
		av_frame_free(&indata);
		return re;
	}
	int nSize = re * indata->channels * av_get_bytes_per_sample(m_format);
	av_frame_free(&indata);
	return nSize;
}

void XResample::setOutFormat(AVSampleFormat format)
{
	m_format = format;
}
