#pragma once
class XDecode
{
public:
	///�򿪽����������ͷ�AVCodecParameters* para;
	virtual bool open(AVCodecParameters* para);

	///���͵������̣߳�����Ҫʵʱ�ͷ�pkt�Ŀռ�
	virtual bool send(AVPacket* pkt);

	///��ȡ���������ݣ�һ�ε�send���ܶ�Ӧ���recv,��ȡ��󻺳�ʱ��Ҫ����NULL��ȡ��󻺳�
	///ÿ�θ���һ�ݣ��ɵ�����ȥ�ͷ�av_frame_free
	virtual AVFrame* recv();

	///�رս���
	virtual void close();

	///������뻺��
	virtual void clear();

	int64_t pts() const;
	
	XDecode();
	virtual ~XDecode();

	static void XFreeAvpacket(AVPacket** pkt);
private:
	bool m_isAudio;
	int32_t  m_llPts;				///��ǰ�����pts
	AVCodecContext* m_codec;
	QMutex m_mut;
};

