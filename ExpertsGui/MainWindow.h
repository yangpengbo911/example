#pragma once

#include <QWidget>
#include "ui_MainWindow.h"

class MainWindow : public QWidget
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = Q_NULLPTR);
	~MainWindow();
///��Ϊ�����࣬���е�ui���Ӧ���ſ���������
public:
	Ui::MainWindow ui;
};
