#include "stable.h"
#include "XDemux.h"

inline double XDemux::r2d(AVRational r)
{
	return r.den == 0 ? 0 : (double)r.num / r.den;
}

XDemux::XDemux()
	: m_pIC(NULL)
	, m_nVideoStream(0)
	, m_nAudioStream(1)
	, m_nTotalMs(0)
	, m_nWidth(0)
	, m_nHeight(0)
	, m_nSampleRate(0)
	, m_nChanels(0)
{
	static bool isFirst = true;
	static QMutex mut;
	///���μ��δ����ʱ���Ƿ��Ե�һ��ע������
	if (isFirst)
	{
		///���������Ƿ��Ѿ��������߳�ע������
		QMutexLocker locker(&mut);
		if (isFirst)
		{
			///��ʼ����װ��
			av_register_all();

			///��ʼ������⣨���Դ�rtsp rtmp httpЭ�����ý����Ƶ��;
			avformat_network_init();
			isFirst = false;
		}
	}
}

XDemux::~XDemux()
{

}

bool XDemux::isAudio(AVPacket* pkt)
{
	if (!pkt)
	{
		return false;
	}
	if (pkt->stream_index == m_nVideoStream)
	{
		return false;
	}
	return true;
}

bool XDemux::open(const std::string url)
{
	close();
	QMutexLocker locker(&m_mut);
	AVDictionary* opts = NULL;
	av_dict_set(&opts, "rtsp_transport", "tcp", 0);
	av_dict_set(&opts, "max_delay", "2000", 0);
	///���װ������
	int re = avformat_open_input(&m_pIC,
		url.c_str(),
		0,		//0 �Զ�ѡ��
		&opts	//�������ã�����rtsp���ӳ�ʱ��
	);
	if (re != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		debug_out << buf << '\n';
		return false;
	}
	debug_out << "open " << url.c_str() << " sucessfull" << '\n';
	///��ȡ����Ϣ
	re = avformat_find_stream_info(m_pIC, 0);

	///��ʱ��
	m_nTotalMs = m_pIC->duration / (AV_TIME_BASE / 1000);
	debug_out << "time lenght" << "\t" << m_nTotalMs << '\n';

	///��Ƶ��Ƶ��������ȡʱ����������Ƶ

	av_dump_format(m_pIC, 0, url.c_str(), 0);

	///��ȡ��Ƶ����Ϣ
	m_nVideoStream = av_find_best_stream(m_pIC, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	AVStream* as = m_pIC->streams[m_nVideoStream];
	debug_out << "/*************************************************************/" << '\n';
	debug_out << "codec_id��" << as->codecpar->codec_id << '\n';
	debug_out << "format�� " << as->codecpar->format << '\n';
	debug_out << "��Ƶ��Ϣ ��" << m_nVideoStream <<  '\n';
	debug_out << "width ��" << as->codecpar->width << '\n';
	debug_out << "height��" << as->codecpar->height << '\n';
	m_nWidth = as->codecpar->width;
	m_nHeight = as->codecpar->height;
	///֡�� fps
	debug_out << "video fps ��" << r2d(as->avg_frame_rate) << '\n';
	debug_out << "**************************************************************/" << "\n\n";

	///��ȡ��Ƶ����Ϣ
	m_nAudioStream = av_find_best_stream(m_pIC, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
	as = m_pIC->streams[m_nAudioStream];
	debug_out << "/*************************************************************/" << '\n';
	debug_out << "��Ƶ��Ϣ ��" << m_nAudioStream << '\n';
	m_nSampleRate = as->codecpar->sample_rate;
	debug_out << "sample_rate��" << as->codecpar->sample_rate << '\n';
	m_nChanels = as->codecpar->channels;
	debug_out << "channals��" << as->codecpar->channels << '\n';
	///һ֡���� ��ͨ����������
	debug_out << "frame_size��" << as->codecpar->frame_size << '\n';
	///frame_size * ͨ�� * ������ʽn����λ = һ֡��С��fps = sample_rate/frame_size
	debug_out << "**************************************************************/" << "\n";

	return true;
}

AVPacket* XDemux::read()
{
	QMutexLocker locker(&m_mut);
	if (!m_pIC)
	{
		return nullptr;
	}
	AVPacket* pkt = av_packet_alloc();
	int re = av_read_frame(m_pIC, pkt);
	if (re != 0)
	{
		av_packet_free(&pkt);
		if (pkt)
		{
			debug_out << "AVPacket* resource free failed" << '\n';
		}
		return nullptr;
	}
	///ptsת������ms
	pkt->pts = pkt->pts * r2d(m_pIC->streams[pkt->stream_index]->time_base) * 1000;
	pkt->dts = pkt->dts * r2d(m_pIC->streams[pkt->stream_index]->time_base) * 1000;
	//m_mut.unlock();
	//debug_out << "pkt=>pts �� " << pkt->pts << '\n';
	return pkt;
}

bool XDemux::seek(double pos)
{
	QMutexLocker locker(&m_mut);
	int64_t seekPos = 0;
	if (!m_pIC)
	{
		return false;
	}
	//int ms = 3000; ///�������3���ʱ��
	//long long pos = (int64_t)((double)ms / (double)1000 * r2d(ic->streams[pkt->stream_index]->time_base));
	avformat_flush(m_pIC);
	if (m_pIC->streams[m_nVideoStream]->duration <= 0)
	{
		seekPos = m_pIC->streams[m_nVideoStream]->time_base.den == 0 ||
			m_pIC->streams[m_nVideoStream]->time_base.num == 0 ? 0 : (m_pIC->streams[m_nVideoStream]->time_base.den / m_pIC->streams[m_nVideoStream]->time_base.num) * pos;
	}
	else
	{
		seekPos = m_pIC->streams[m_nVideoStream]->duration * pos;
	}
	int re = av_seek_frame(m_pIC, m_nVideoStream, seekPos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
	if (re < 0)
	{
		char buf[512] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		debug_out << "seek to failed" << '\n';
		debug_out << buf << '\n';
		return false;
	}
	return true;
}

void XDemux::clear()
{
	QMutexLocker locker(&m_mut);
	if (!m_pIC)
	{
		return;
	}
	avformat_flush(m_pIC);
}

void XDemux::close()
{
	QMutexLocker locker(&m_mut);
	if (!m_pIC)
	{
		return;
	}
	avformat_close_input(&m_pIC);
	m_nTotalMs = 0;
}

AVCodecParameters* XDemux::copyVParam()
{
	QMutexLocker mut(&m_mut);
	if (!m_pIC)
	{
		return nullptr;
	}
	AVCodecParameters* pa = avcodec_parameters_alloc();
	int re = avcodec_parameters_copy(pa, m_pIC->streams[m_nVideoStream]->codecpar);
	if (re)
	{
		debug_out << "copy video paramters failed" << '\n';
		return nullptr;
	}
	return pa;
}

AVCodecParameters* XDemux::copyAParam()
{
	QMutexLocker locker(&m_mut);
	if (!m_pIC)
	{
		return nullptr;
	}
	AVCodecParameters* pa = avcodec_parameters_alloc();
	int re = avcodec_parameters_copy(pa, m_pIC->streams[m_nAudioStream]->codecpar);
	if (re)
	{
		debug_out << "copy audio paramters failed" << '\n';
		return nullptr;
	}
	return pa;
}

int XDemux::videoWidth() const
{
	return m_nWidth;
}

int XDemux::videoHeitht() const
{
	return m_nHeight;
}

 int XDemux::audioSampleRate() const
{
	return m_nSampleRate;
}

int XDemux::audioChanels() const
{
	return m_nChanels;
}
