#include <Windows.h>

#include "EyeClock.h"
#include <string>
#include <QTime>

#include <QtMultimedia/QAudio>

#include <mmsystem.h>

using namespace std;

EyeClock::EyeClock(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

#ifdef NDEBUG
	ui.menuBar->setVisible(false);
	ui.debugLabel->hide();
#endif
	
	playIcon = QIcon(":/EyeClock/PlayIcon");
	pauseIcon = QIcon(":/EyeClock/PauseIcon");

	restingPicture = QPixmap(":/EyeClock/RestingPicture");
	workingPicture = QPixmap(":/EyeClock/WorkingPicture");

	breakSound.setSource(QUrl::fromLocalFile(":/EyeClock/BreakSound"));
	resumeSound.setSource(QUrl::fromLocalFile(":/EyeClock/ResumeSound"));

	timer = new QTimer(this);

	timerRunning = false;
	SetResting(false);
	tickCount = 0;

	workTimeLimit = ui.workTimeBox->value() * 60;
	restTimeLimit = ui.restTimeBox->value();

	QTime time = QTime(0, 0, 0);
	ui.clockLCD->display(time.toString("m:ss"));
	
	SetupSignals();

	// argument takes milliseconds, this is one second
	timer->start(1000);
}

void EyeClock::SetupSignals() {
	connect(ui.startButton, SIGNAL(clicked()), this, SLOT(PlayButtonClicked()));
	connect(ui.resetButton, SIGNAL(clicked()), this, SLOT(ResetButtonClicked()));

	connect(ui.workTimeBox, SIGNAL(valueChanged(int)), this, SLOT(WorkTimeChanged(int)));
	connect(ui.restTimeBox, SIGNAL(valueChanged(int)), this, SLOT(RestTimeChanged(int)));

	connect(timer, SIGNAL(timeout()), this, SLOT(TickUpdate()));

	connect(ui.volumeSlider, SIGNAL(valueChanged(int)), ui.sliderLabel, SLOT(setNum(int)));
	connect(ui.volumeSlider, SIGNAL(valueChanged(int)), this, SLOT(SetVolume(int)));

	connect(ui.dAlarmTime, SIGNAL(triggered()), this, SLOT(dAlarmTime()));
	connect(ui.dBellTime, SIGNAL(triggered()), this, SLOT(dBellTime()));
	connect(ui.dRingAlarm, SIGNAL(triggered()), this, SLOT(dRingAlarm()));
	connect(ui.dRingBell, SIGNAL(triggered()), this, SLOT(dRingBell()));
}

void EyeClock::PlayButtonClicked() {
	if (timerRunning) {
		timerRunning = false;
		ui.startButton->setIcon(playIcon);
	}
	else {
		timerRunning = true;
		ui.startButton->setIcon(pauseIcon);
	}
}

void EyeClock::ResetButtonClicked() {
	tickCount = 0;
	SetResting(false);

	QTime time = QTime(0, 0, 0);
	ui.clockLCD->display(time.toString("m:ss"));
}

void EyeClock::WorkTimeChanged(int minutes) {
	workTimeLimit = 60 * minutes;
}

void EyeClock::RestTimeChanged(int seconds) {
	restTimeLimit = seconds;
}

void EyeClock::TickUpdate() {
	if (timerRunning)
		tickCount++;

	if (!resting) {
		if (tickCount >= workTimeLimit) {
			tickCount = 0;
			SetResting(true);
			breakSound.play();
		}
	}
	else {
		if (tickCount >= restTimeLimit) {
			tickCount = 0;
			SetResting(false);
			resumeSound.play();
		}
	}

	int hours = tickCount / 3600;
	int minutes = (tickCount % 3600) / 60;
	int seconds = tickCount % 60;

	QTime time = QTime(hours, minutes, seconds);

	ui.clockLCD->display(time.toString("m:ss"));
}

void EyeClock::SetResting(bool restingInput)
{
	if (restingInput) {
		resting = true;
		ui.statusBox->setPixmap(restingPicture);
	}
	else {
		resting = false;
		ui.statusBox->setPixmap(workingPicture);
	}
}

void EyeClock::SetVolume(int volume) {
	float linearVolume = QAudio::convertVolume(volume / qreal(volumeScale),
		QAudio::LogarithmicVolumeScale,
		QAudio::LinearVolumeScale);

	resumeSound.setVolume(linearVolume);
	breakSound.setVolume(linearVolume);

	ui.debugLabel->setNum(linearVolume);
}

void EyeClock::dRingBell() {
	resumeSound.play();
}

void EyeClock::dRingAlarm() {
	breakSound.play();
}

void EyeClock::dBellTime() {
	tickCount = restTimeLimit - 3;
}

void EyeClock::dAlarmTime() {
	tickCount = workTimeLimit - 3;
}