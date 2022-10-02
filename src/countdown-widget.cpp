#include "countdown-widget.hpp"

CountdownDockWidget::CountdownDockWidget(QWidget *parent)
	: QDockWidget("Countdown Timer", parent)
{
	blog(LOG_INFO, "Starting loading of widget!");
	countdownTimerData = new CountdownWidgetStruct;
	countdownTimerData->countdownTimerUI = new QWidget();
	countdownTimerData->countdownTimerUI->setLayout(
		SetupCountdownWidgetUI(countdownTimerData));

	// countdownTimerData->countdownTimerUI->setMinimumSize(200, 200);
	// countdownTimerData->countdownTimerUI->setVisible(true);

	setWidget(countdownTimerData->countdownTimerUI);

	ConnectObsSignalHandlers(countdownTimerData);

	LoadSavedSettings();

	InitialiseTimerTime(countdownTimerData);
	hide();
}

CountdownDockWidget::~CountdownDockWidget()
{
	this->destroy();
}

QVBoxLayout *CountdownDockWidget::SetupCountdownWidgetUI(
	CountdownWidgetStruct *countdownStruct)
{

	CountdownWidgetStruct *context = countdownStruct;
	context->timerDisplay = new QLCDNumber(8);
	context->timerDisplay->display("00:00:00");

	context->timerHours = new QLineEdit("0");
	context->timerHours->setAlignment(Qt::AlignCenter);
	context->timerHours->setMaxLength(2);
	context->timerHours->setValidator(new QRegularExpressionValidator(
		QRegularExpression("[0-9]{1,2}"), this));
	// QObject::connect(context->timerHours, SIGNAL(textEdited(QString text)),
	// 		 SLOT(updateHours()));

	context->timerMinutes = new QLineEdit("0");
	context->timerMinutes->setAlignment(Qt::AlignCenter);
	context->timerMinutes->setMaxLength(2);
	context->timerMinutes->setValidator(new QRegularExpressionValidator(
		QRegularExpression("^[1-5]?[0-9]"), this));
	// QObject::connect(context->timerMinutes,
	// 		 SIGNAL(textEdited(QString text)),
	// 		 SLOT(updateMinutes()));

	context->timerSeconds = new QLineEdit("0");
	context->timerSeconds->setAlignment(Qt::AlignCenter);
	context->timerSeconds->setMaxLength(2);
	context->timerSeconds->setValidator(new QRegularExpressionValidator(
		QRegularExpression("^[1-5]?[0-9]"), this));
	// QObject::connect(context->timerSeconds,
	// 		 SIGNAL(textEdited(QString text)), SLOT(UpdateTimer()));

	context->textSourceDropdownList = new QComboBox();
	// QObject::connect(context->textSourceDropdownList,
	// 		 SIGNAL(currentTextChanged(const QString &)),
	// 		 SLOT(SetSelectedSource(const QString &)));

	context->switchSceneCheckBox = new QCheckBox();
	QObject::connect(context->switchSceneCheckBox,
			 SIGNAL(stateChanged(int)),
			 SLOT(SceneSwitchCheckBoxSelected(int)));
	context->switchSceneCheckBox->setCheckState(Qt::Unchecked);

	context->sceneSourceDropdownList = new QComboBox();
	context->sceneSourceDropdownList->setEnabled(false);
	// QObject::connect(context->textSourceDropdownList, SIGNAL(currentTextChanged(const QString&)),
	//  SLOT(SetSelectedSource(const QString&)));

	context->playButton = new QPushButton(this);
	context->playButton->setProperty("themeID", "playIcon");
	context->playButton->setEnabled(true);
	QObject::connect(context->playButton, SIGNAL(clicked()),
			 SLOT(PlayButtonClicked()));

	context->pauseButton = new QPushButton(this);
	context->pauseButton->setProperty("themeID", "pauseIcon");
	context->pauseButton->setEnabled(false);
	QObject::connect(context->pauseButton, SIGNAL(clicked()),
			 SLOT(PauseButtonClicked()));

	context->resetButton = new QPushButton(this);
	context->resetButton->setProperty("themeID", "restartIcon");
	QObject::connect(context->resetButton, SIGNAL(clicked()),
			 SLOT(ResetButtonClicked()));

	context->isPlaying = false;

	QHBoxLayout *timerLayout = new QHBoxLayout();

	timerLayout->addWidget(context->timerHours);
	timerLayout->addWidget(new QLabel("h"));

	timerLayout->addWidget(context->timerMinutes);
	timerLayout->addWidget(new QLabel("m"));

	timerLayout->addWidget(context->timerSeconds);
	timerLayout->addWidget(new QLabel("s"));

	QHBoxLayout *buttonLayout = new QHBoxLayout();

	buttonLayout->addWidget(context->resetButton);
	buttonLayout->addWidget(context->pauseButton);
	buttonLayout->addWidget(context->playButton);

	QHBoxLayout *sourceDropDownLayout = new QHBoxLayout();
	sourceDropDownLayout->addWidget(new QLabel("Text Source"));
	sourceDropDownLayout->addWidget(context->textSourceDropdownList);

	// QGroupBox *optionsGroupBox = new QGroupBox("Options");
	// QVBoxLayout *optionsVerticalLayout = new QVBoxLayout();

	QHBoxLayout *endMessageLayout = new QHBoxLayout();
	context->endMessageCheckBox = new QCheckBox();
	QObject::connect(context->endMessageCheckBox, SIGNAL(stateChanged(int)),
			 SLOT(EndMessageCheckBoxSelected(int)));
	context->endMessageCheckBox->setCheckState(Qt::Unchecked);

	context->timerEndMessage = new QLineEdit();
	context->timerEndMessage->setEnabled(false);
	endMessageLayout->addWidget(context->endMessageCheckBox);
	endMessageLayout->addWidget(new QLabel("End Message"));
	endMessageLayout->addWidget(context->timerEndMessage);

	QHBoxLayout *sceneDropDownLayout = new QHBoxLayout();
	sceneDropDownLayout->addWidget(context->switchSceneCheckBox);
	sceneDropDownLayout->addWidget(new QLabel("Switch Scene"));
	sceneDropDownLayout->addWidget(context->sceneSourceDropdownList);

	// optionsVerticalLayout->addLayout(endMessageLayout);
	// optionsVerticalLayout->addLayout(sceneDropDownLayout);
	// optionsGroupBox->setLayout(optionsVerticalLayout);

	QVBoxLayout *timeLayout = new QVBoxLayout();

	timeLayout->addWidget(context->timerDisplay);
	timeLayout->addLayout(timerLayout);

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->addLayout(timeLayout);
	mainLayout->addLayout(sourceDropDownLayout);
	mainLayout->addLayout(endMessageLayout);
	mainLayout->addLayout(sceneDropDownLayout);
	// mainLayout->addWidget(optionsGroupBox);
	mainLayout->addLayout(buttonLayout);

	return mainLayout;
}

void CountdownDockWidget::PlayButtonClicked()
{
	// blog(LOG_INFO, "Play Button Clicked");
	CountdownWidgetStruct *context = countdownTimerData;
	if (IsSetTimeZero(context))
		return;

	context->timerDisplay->display(
		ConvertTimeToDisplayString(context->time));
	StartTimerCounting(context);
}

void CountdownDockWidget::PauseButtonClicked()
{
	// blog(LOG_INFO, "Pause Button Clicked");
	CountdownWidgetStruct *context = countdownTimerData;
	StopTimerCounting(context);
}

void CountdownDockWidget::ResetButtonClicked()
{
	// blog(LOG_INFO, "Reset Button Clicked");

	CountdownWidgetStruct *context = countdownTimerData;
	int hours = context->timerHours->text().toInt();
	int minutes = context->timerMinutes->text().toInt();
	int seconds = context->timerSeconds->text().toInt();

	StopTimerCounting(context);

	context->time->setHMS(hours, minutes, seconds, 0);

	UpdateTimeDisplay(context, context->time);
}

void CountdownDockWidget::StartTimerCounting(CountdownWidgetStruct *context)
{
	context->isPlaying = true;
	context->timer->start(COUNTDOWNPERIOD);
	context->playButton->setEnabled(false);
	context->pauseButton->setEnabled(true);
	context->resetButton->setEnabled(false);

	context->timerHours->setEnabled(false);
	context->timerMinutes->setEnabled(false);
	context->timerSeconds->setEnabled(false);

	context->textSourceDropdownList->setEnabled(false);
	context->timerEndMessage->setEnabled(false);
	context->sceneSourceDropdownList->setEnabled(false);
	context->endMessageCheckBox->setEnabled(false);
	context->switchSceneCheckBox->setEnabled(false);
	blog(LOG_INFO, "Timer STARTED counting!");
}

void CountdownDockWidget::StopTimerCounting(CountdownWidgetStruct *context)
{
	context->isPlaying = false;
	context->timer->stop();
	context->playButton->setEnabled(true);
	context->pauseButton->setEnabled(false);
	context->resetButton->setEnabled(true);

	context->timerHours->setEnabled(true);
	context->timerMinutes->setEnabled(true);
	context->timerSeconds->setEnabled(true);

	context->textSourceDropdownList->setEnabled(true);

	context->endMessageCheckBox->setEnabled(true);
	if(context->endMessageCheckBox->isChecked()) {
		context->timerEndMessage->setEnabled(true);
	}
	context->switchSceneCheckBox->setEnabled(true);
	if(context->switchSceneCheckBox->isChecked()){
		context->sceneSourceDropdownList->setEnabled(true);
	}

	blog(LOG_INFO, "Timer STOPPED counting!");
}

void CountdownDockWidget::InitialiseTimerTime(CountdownWidgetStruct *context)
{
	context->timer = new QTimer();
	QObject::connect(context->timer, SIGNAL(timeout()),
			 SLOT(TimerDecrement()));
	context->time = new QTime(context->timerHours->text().toInt(),
				  context->timerMinutes->text().toInt(),
				  context->timerSeconds->text().toInt());
}

void CountdownDockWidget::TimerDecrement()
{
	CountdownWidgetStruct *context = countdownTimerData;

	QTime *currentTime = context->time;

	currentTime->setHMS(currentTime->addMSecs(-COUNTDOWNPERIOD).hour(),
			    currentTime->addMSecs(-COUNTDOWNPERIOD).minute(),
			    currentTime->addMSecs(-COUNTDOWNPERIOD).second());

	UpdateTimeDisplay(context, currentTime);

	if (currentTime->hour() == 0 && currentTime->minute() == 0 &&
	    currentTime->second() == 0) {
		QString endMessageText = context->timerEndMessage->text();
		if (context->endMessageCheckBox->isChecked()) {
			SetSourceText(context,
				      endMessageText.toStdString().c_str());
		}
		if(context->switchSceneCheckBox->isChecked()){
			SetCurrentScene();
		}
		context->timerDisplay->display("00:00:00");
		currentTime->setHMS(0, 0, 0, 0);
		StopTimerCounting(context);
		blog(LOG_INFO, "Timer reached zero");
		return;
	}

	// blog(LOG_INFO, "One second down!");
}

QString CountdownDockWidget::ConvertTimeToDisplayString(QTime *timeToConvert)
{
	return timeToConvert->toString("hh:mm:ss");
}

bool CountdownDockWidget::IsSetTimeZero(CountdownWidgetStruct *context)
{
	bool isZero = false;

	if (context->time->hour() == 0 && context->time->minute() == 0 &&
	    context->time->second() == 0) {
		isZero = true;
	} else if (context->timerHours->text().toInt() == 0 &&
		   context->timerMinutes->text().toInt() == 0 &&
		   context->timerSeconds->text().toInt() == 0) {
		isZero = true;
	}

	return isZero;
}

void CountdownDockWidget::ConnectObsSignalHandlers(
	CountdownWidgetStruct *context)
{
	// Source Signals
	signal_handler_connect(obs_get_signal_handler(), "source_create",
			       OBSSourceCreated, context);

	signal_handler_connect(obs_get_signal_handler(), "source_destroy",
			       OBSSourceDeleted, context);

	signal_handler_connect(obs_get_signal_handler(), "source_rename",
			       OBSSourceRenamed, context);
}

void CountdownDockWidget::OBSSourceCreated(void *param, calldata_t *calldata)
{
	auto context =
		static_cast<CountdownDockWidget::CountdownWidgetStruct *>(
			param);

	obs_source_t *source;

	calldata_get_ptr(calldata, "source", &source);

	// blog(LOG_INFO, "Source Create Signal Triggered!");

	if (!source)
		return;
	int sourceType = CheckSourceType(source);
	// If not sourceType we need;
	if (!sourceType)
		return;

	const char *name = obs_source_get_name(source);

	if (sourceType == TEXT_SOURCE) {
		context->textSourceDropdownList->addItem(name);
	} else if (sourceType == SCENE_SOURCE) {
		context->sceneSourceDropdownList->addItem(name);
	}
};

void CountdownDockWidget::OBSSourceDeleted(void *param, calldata_t *calldata)
{
	auto context =
		static_cast<CountdownDockWidget::CountdownWidgetStruct *>(
			param);

	obs_source_t *source;

	calldata_get_ptr(calldata, "source", &source);

	if (!source)
		return;
	int sourceType = CheckSourceType(source);
	// If not sourceType we need;
	if (!sourceType)
		return;
	// blog(LOG_INFO, "Text Source Deleted!");

	const char *name = obs_source_get_name(source);

	if (sourceType == TEXT_SOURCE) {
		int textIndexToRemove =
			context->textSourceDropdownList->findText(name);
		context->textSourceDropdownList->removeItem(textIndexToRemove);
	} else if (sourceType == SCENE_SOURCE) {
		int sceneIndexToRemove =
			context->sceneSourceDropdownList->findText(name);
		context->sceneSourceDropdownList->removeItem(
			sceneIndexToRemove);
	}

	obs_source_release(source);
};

void CountdownDockWidget::OBSSourceRenamed(void *param, calldata_t *calldata)
{
	auto context =
		static_cast<CountdownDockWidget::CountdownWidgetStruct *>(
			param);

	obs_source_t *source;
	calldata_get_ptr(calldata, "source", &source);

	if (!source)
		return;
	int sourceType = CheckSourceType(source);
	// If not sourceType we need;
	if (!sourceType)
		return;

	const char *newName = calldata_string(calldata, "new_name");
	const char *oldName = calldata_string(calldata, "prev_name");

	if (sourceType == TEXT_SOURCE) {
		int textListIndex =
			context->textSourceDropdownList->findText(oldName);
		if (textListIndex == -1)
			return;
		context->textSourceDropdownList->setItemText(textListIndex,
							     newName);
		// blog(LOG_INFO, "Text Source Renamed!");
	} else if (sourceType == SCENE_SOURCE) {
		int sceneListIndex =
			context->sceneSourceDropdownList->findText(oldName);
		if (sceneListIndex == -1)
			return;
		context->sceneSourceDropdownList->setItemText(sceneListIndex,
							      newName);
		// blog(LOG_INFO, "Scene Source Renamed!");
	}

	// blog(LOG_INFO, "Source Rename Triggered!");
	// blog(LOG_INFO, "Old name: %s", oldName);
	// blog(LOG_INFO, "New name: %s", newName);
};

int CountdownDockWidget::CheckSourceType(obs_source_t *source)
{
	blog(LOG_INFO, "Checking if Text Source");
	const char *source_id = obs_source_get_unversioned_id(source);
	blog(LOG_INFO, "source_id: %s", source_id);
	if (strcmp(source_id, "text_ft2_source") == 0 ||
	    strcmp(source_id, "text_gdiplus") == 0) {
		return TEXT_SOURCE;
	} else if (strcmp(source_id, "scene") == 0) {
		return SCENE_SOURCE;
	}
	return 0;
}

void CountdownDockWidget::UpdateTimeDisplay(CountdownWidgetStruct *context,
					    QTime *time)
{

	QString formattedTime = time->toString("hh:mm:ss");
	context->timerDisplay->display(formattedTime);
	// blog(LOG_INFO, "Update Time Function String is: %s",
	//  formattedTime.toLocal8Bit().data());
	SetSourceText(context, formattedTime);
}

void CountdownDockWidget::SetSourceText(CountdownWidgetStruct *context,
					QString newText)
{

	QString currentSourceNameString =
		context->textSourceDropdownList->currentText();

	obs_source_t *selectedSource = obs_get_source_by_name(
		currentSourceNameString.toStdString().c_str());

	if (selectedSource != NULL) {
		// blog(LOG_INFO, "Updating Source Text With Text: %s!", newText.toStdString().c_str());
		obs_data_t *sourceSettings =
			obs_source_get_settings(selectedSource);
		obs_data_set_string(sourceSettings, "text",
				    newText.toStdString().c_str());
		obs_source_update(selectedSource, sourceSettings);
		obs_data_release(sourceSettings);
		obs_source_release(selectedSource);
	}
}

void CountdownDockWidget::EndMessageCheckBoxSelected(int state)
{
	if (state) {
		countdownTimerData->timerEndMessage->setEnabled(true);
	} else {
		countdownTimerData->timerEndMessage->setEnabled(false);
	}
	blog(LOG_INFO, "End Message Check Box Clicked!");
	blog(LOG_INFO, "State Set To: %i", state);
}

void CountdownDockWidget::SceneSwitchCheckBoxSelected(int state)
{
	if (state) {
		countdownTimerData->sceneSourceDropdownList->setEnabled(true);
	} else {
		countdownTimerData->sceneSourceDropdownList->setEnabled(false);
	}
	blog(LOG_INFO, "Scene Switch Check Box Clicked!");
	blog(LOG_INFO, "State Set To: %i", state);
}

void CountdownDockWidget::SetCurrentScene() {
	QString selectedScene = countdownTimerData->sceneSourceDropdownList->currentText();
	if(selectedScene.length()){
		obs_source_t *source = obs_get_source_by_name(selectedScene.toStdString().c_str());
		if(source != NULL){
			obs_frontend_set_current_scene(source);
			obs_source_release(source);
		}
	}
}

void CountdownDockWidget::LoadSavedSettings() {
	char *file = obs_module_config_path("config.json");
	obs_data_t *data = nullptr;
	if (file) {
		data = obs_data_create_from_json_file(file);
		bfree(file);
	}
	if (data) {
		// Things to get:
		// Hours, Minutes and Seconds setting
		// Selected Text Source
		// Check status for end message
		// String for end message
		// Check status for scene switch
		// Selected Scene Source

		// Get Save Data
		int hours = obs_data_get_int(data, "hours");
		int minutes = obs_data_get_int(data, "minutes");
		int seconds = obs_data_get_int(data, "seconds");

		blog(LOG_INFO, "Saved Hours: %i", hours);

		const char* selectedTextSource = obs_data_get_string(data, "selectedTextSource");

		int endMessageCheckBoxStatus = obs_data_get_int(data, "endMessageCheckBoxStatus");

		const char* endMessageText = obs_data_get_string(data, "endMessageText");

		int switchSceneCheckBoxStatus =
			obs_data_get_int(data, "switchSceneCheckBoxStatus");
	
		const char* selectedSceneSource = obs_data_get_string(data, "selectedSceneSource");

		UNUSED_PARAMETER(selectedTextSource);
		UNUSED_PARAMETER(endMessageCheckBoxStatus);
		UNUSED_PARAMETER(endMessageText);
		UNUSED_PARAMETER(switchSceneCheckBoxStatus);
		UNUSED_PARAMETER(selectedSceneSource);

		// Apply saved data to plugin
		countdownTimerData->timerHours->setText(QString::number(hours));
		countdownTimerData->timerMinutes->setText(QString::number(minutes));
		countdownTimerData->timerSeconds->setText(QString::number(seconds));

		
	}
}

void CountdownDockWidget::SaveSettings() {

}