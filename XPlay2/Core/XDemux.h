#pragma once
struct AVRational;
struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;

class XDemux
{
///��̬������
public:
	static double r2d(AVRational r);


	///���нӿ���
public:
	XDemux();
	virtual ~XDemux();

	bool isAudio(AVPacket* pkt);

	/// ����ý���ļ������ļ���������rtmp rtsp http
	virtual bool open(const std::string url);

	///�ռ���Ҫ�������ͷţ��ͷ�AVPacket����ռ䣬�����ݿռ� av_packet_free
	virtual AVPacket* read();

	///������Ƶ�����ƶ���ȡ��λ��,posΪ�ٷֱ�ֵ0.0-1.0֮��
	virtual bool seek(double pos);

	virtual void clear();

	virtual void close();

	///��ȡ��Ƶ����,���صĿռ���Ҫ�������avcodec_parameters_free()
	AVCodecParameters* copyVParam();

	///��ȡ��Ƶ����,���صĿռ���Ҫ�������avcodec_parameters_free()
	AVCodecParameters* copyAParam();

	///��ȡ������������Ƶ���ȣ����룩
	int64_t totalMs() const { return m_nTotalMs; }

	void setTotalMs(int val) { m_nTotalMs = val; }

	int32_t videoWidth() const;

	int32_t videoHeitht() const;
	
	int32_t audioSampleRate() const;

	int32_t audioChanels() const;
protected:
	///���װ������
	AVFormatContext* m_pIC;

	///����Ƶ��
	int m_nVideoStream;
	int m_nAudioStream;

private:
	///��Ƶ�Ŀ��
	int32_t m_nWidth;
	int32_t m_nHeight;

	///��Ƶ�Ĳ����������ʹ�С
	int32_t m_nSampleRate;

	int32_t m_nChanels;

	///ý����ʱ��(����)
	int32_t m_nTotalMs;
	///�߳���
	QMutex m_mut;
};