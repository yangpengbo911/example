#pragma once

#include <QWidget>
#include "ui_LeftShutupWidget.h"

class LeftShutupWidget : public QWidget
{
	Q_OBJECT

public:
	LeftShutupWidget(QWidget *parent = Q_NULLPTR);
	~LeftShutupWidget();
///��Ϊ�����࣬�������Ӧ���ſ�
public:
	Ui::LeftShutupWidget ui;
};
