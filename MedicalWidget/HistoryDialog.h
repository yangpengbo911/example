#pragma once

#include <QDialog>

#ifndef GLOBAL_DEFINE //���û�а���ȫ��ͷ�ļ�����һ������
struct ListContent
{
	std::string                content;             //����
	std::string                patient_id;          //����id
	std::string                doctor_id;           //ר��id
	std::string                datetime;            //����ʱ���ʽ:YY-mm-dd HH:MM:SS
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & content;
		ar & patient_id;
		ar & doctor_id;
		ar & datetime;
	}
};
namespace boost
{
	namespace serialization
	{
		template<class Archive>
		void serialize(Archive & ar, ListContent & g, const unsigned int version)//���������boost::serialization���ֿռ���
		{
			ar & g.content;
			ar & g.patient_id;
			ar & g.doctor_id;
			ar & g.datetime;
		}
	}
}
//��ʷ��Ϣ��Ӧ
struct IllHistoryResponse
{
	std::list <ListContent>		drug;				 //ҩ�����ʷ
	std::list <ListContent>		past;                //����ʷ
	std::list <ListContent>		family;              //����ʷ
	std::list <ListContent>		genetic;             //�Ŵ���ʷ
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & drug;
		ar & past;
		ar & family;
		ar & genetic;
	}
};
namespace boost
{
	namespace serialization
	{
		template<class Archive>
		void serialize(Archive & ar, IllHistoryResponse & g, const unsigned int version)//���������boost::serialization���ֿռ���
		{
			ar & g.drug;
			ar & g.past;
			ar & g.family;
			ar & g.genetic;
		}
	}
}
#endif
namespace Ui
{
	class HistoryDialog;
};

class MEDICALWIDGET_EXPORT HistoryDialog : public QDialog
{
	Q_OBJECT

public:
	enum WindowIndex
	{
		Drug = 0,
		Past = 1,
		Family = 2,
		Genetic = 3
	};
	enum HistoryDialog::WindowIndex :int;
public:
	HistoryDialog(QWidget *parent = Q_NULLPTR);
	~HistoryDialog();
	void setData(IllHistoryResponse const& hisData);
	void setIndex(WindowIndex index);
	void setVisiableAppend(bool bVisiable);
signals:
	void addDrugClicked();
	void addPastClecked();
	void addFamilyClicked();
	void addGeneticClicked();
private:
	Ui::HistoryDialog* ui;
	QStandardItemModel* drupModel;
	QStandardItemModel* pastModel;
	QStandardItemModel* familyModel;
	QStandardItemModel* geneticModel;

};
