#pragma once

#include <QtWidgets/QMainWindow>
#include <QTimer>
#include <QtMultimedia/QSoundEffect>
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

	QSoundEffect breakSound;
	QSoundEffect resumeSound;

	QPixmap restingPicture;
	QPixmap workingPicture;

	QTimer *timer;

	const int volumeScale = 100;

private slots:
	void PlayButtonClicked();
	void ResetButtonClicked();
	void WorkTimeChanged(int minutes);
	void RestTimeChanged(int seconds);
	void TickUpdate();
	void SetResting(bool restingInput);
	void SetVolume(int volume);

	void dRingBell();
	void dRingAlarm();
	void dBellTime();
	void dAlarmTime();
};
