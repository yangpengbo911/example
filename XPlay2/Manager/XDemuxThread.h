#pragma once

#include <QObject>


class XDemux;
class XDecode;
class XResample;
class XVideoThread;
class XAudioThread;
class XVideoWidget;
class IVideoCall;
class WorkerController;

class XDemuxThread : public QObject
{
	Q_OBJECT

public:
	XDemuxThread(QObject *parent = 0);
	virtual ~XDemuxThread();

	bool open(char const* url, IVideoCall* pCall);

	void start();

	XVideoThread* videoThread() const
	{
		return m_pVideoThread;
	}

	XAudioThread* audioThread() const
	{
		return m_pAudioThread;
	}

	int64_t getTotalMs() const;

	int64_t getPts() const;
public slots:
	void doWork();
	void close();
private:
	bool				m_isExit;		///�߳��˳���ʶ
	int64_t				m_llPts;
	int64_t				m_lltotalMs;		///��Ƶ�ܳ���
	QMutex				m_mut;			///�������
	XDemux*				m_pxdem;		///���װ��
	XVideoThread*		m_pVideoThread;	///��Ƶ�����߳�
	XAudioThread*		m_pAudioThread;	///��Ƶ�����߳�
	WorkerController*	m_pController;	///�߳̿�����
};
