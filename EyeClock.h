#pragma once

#include <QtWidgets/QMainWindow>
#include <QTimer>
#include "ui_EyeClock.h"

#include <Windows.h>

#include <string>

class EyeClock : public QMainWindow
{
	Q_OBJECT

public:
	EyeClock(QWidget *parent = Q_NULLPTR);

private:
	Ui::EyeClockClass ui;
	void SetupSignals();

	bool timerRunning;
	bool resting;

	unsigned int tickCount;
	unsigned int workTimeLimit;
	unsigned int restTimeLimit;

	QIcon playIcon;
	QIcon pauseIcon;

	LPCWSTR alarmFilename;
	LPCWSTR bellFilename;

	QPixmap restingPicture;
	QPixmap workingPicture;

	QTimer *timer;

private slots:
	void PlayButtonClicked();
	void ResetButtonClicked();
	void WorkTimeChanged(int minutes);
	void RestTimeChanged(int seconds);
	void TickUpdate();
	void SetResting(bool restingInput);
};
