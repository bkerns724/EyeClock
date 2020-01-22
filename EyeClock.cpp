#include <Windows.h>

#include "EyeClock.h"
#include <string>
#include <QTime>
#include <QtMultimedia/QSound>

#include <mmsystem.h>

using namespace std;

EyeClock::EyeClock(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	playIcon = QIcon(":/EyeClock/PlayIcon");
	pauseIcon = QIcon(":/EyeClock/PauseIcon");

	restingPicture = QPixmap(":/EyeClock/RestingPicture");
	workingPicture = QPixmap(":/EyeClock/WorkingPicture");

	breakSound = new QSound(":/EyeClock/BreakSound");
	resumeSound = new QSound(":/EyeClock/ResumeSound");

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
	// Try to use a functor below
	connect(ui.workTimeBox, SIGNAL(valueChanged(int)), this, SLOT(WorkTimeChanged(int)));
	connect(ui.restTimeBox, SIGNAL(valueChanged(int)), this, SLOT(RestTimeChanged(int)));
	connect(timer, SIGNAL(timeout()), this, SLOT(TickUpdate()));
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
			breakSound->play();
		}
	}
	else {
		if (tickCount >= restTimeLimit) {
			tickCount = 0;
			SetResting(false);
			resumeSound->play();
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