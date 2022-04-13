
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <bangtal.h>
#include <stdio.h>

SoundID practice_bgm, stage1_bgm, count[7], kick_sound;
SceneID main_scene, practice_scene, stage1_scene;
ObjectID puang, bar[7], button_practice, button_go, message1, message2;
TimerID beat, delay, beat2, beat_bar;

int perfecrt_range = 50, good_range = 100, bad_range = 150;
int practice_bpm = 125, num_play, beat_ongo, score = 0, prepare_beat, now_bar = 1;
int stage1_num_play = 40, stage1_prepare_beat = 4;
int now_stage = 0;
clock_t onebeat_time, sixbeat_time;
float stage1_bpm = 120, second_per_beat = 1, practice_second_delay = 0.67f;
float stage1_second_delay = 13.95f;
float sum_score = 100;
bool on_game = false, judged = false;

void scoreJudge(bool clicked)
{
	clock_t time_dif = clock() - sixbeat_time;
	if (time_dif <= second_per_beat * 1000 + perfecrt_range && time_dif >= second_per_beat * 1000 - perfecrt_range)
	{
		judged = true;
		showMessage("perfect");
		setObjectImage(puang, "resource/puang_perfect.png");
	}
	else if (time_dif <= second_per_beat * 1000 + good_range && time_dif >= second_per_beat * 1000 - good_range)
	{
		judged = true;
		showMessage("good");
		setObjectImage(puang, "resource/puang_good.png");

		if (now_stage == 2)
			sum_score -= 1.96f;
	}
	else if (time_dif <= second_per_beat * 1000 + bad_range && time_dif >= second_per_beat * 1000 - bad_range)
	{
		judged = true;
		showMessage("bad");
		setObjectImage(puang, "resource/puang_bad.png");

		if (now_stage == 2)
			sum_score -= 3.92f;
	}
	else if (time_dif > second_per_beat * 1000 + bad_range || time_dif < second_per_beat * 1000 - bad_range || not clicked)
	{
		judged = true;
		showMessage("miss");
		setObjectImage(puang, "resource/puang_miss.png");
		if (now_stage == 2)
			sum_score -= 5.88f;
	}
}

ObjectID createObject(const char* image, SceneID scene, int x, int y, bool shown)
{
	ObjectID object = createObject(image);
	locateObject(object, scene, x, y);

	if (shown)
	{
		showObject(object);
	}
	return object;
}

void setBar(SceneID scene)
{
	for (int i = 0; i < 7; i++)
	{
		setObjectImage(bar[i], "resource/bar.png");
		locateObject(bar[i], scene, 250 + 141 * i, 400);
		showObject(bar[i]);
	}
	setObjectImage(bar[6], "resource/bar7.png");
}

const char* countName(int num)
{
	static char buff[30];
	sprintf_s(buff, sizeof(buff), "resource/Count%d.mp3", num);
	return buff;
}

void keyboardCallback(KeyCode keycode, KeyState keystate)
{
	if (keycode == KeyCode::KEY_SPACE)
	{
		if (not judged)
		{
			if (beat_ongo >= 6 && beat_ongo <= 8)
			{
				scoreJudge(true);
			}
		}
	}

	if (keycode == KeyCode::KEY_ESCAPE && keystate == KeyState::KEY_PRESSED)
	{
		if (now_stage != 0)
		{
			enterScene(main_scene);
			stopTimer(beat); stopTimer(delay); stopTimer(beat2); stopTimer(beat_bar);
			stopSound(practice_bgm); stopSound(stage1_bgm);
			now_stage = 0;
			beat_ongo = 0;
			locateObject(puang, stage1_scene, 70, 350);
			setObjectImage(puang, "resource/puang.png");
			on_game = false;

		}
		else
		{
			endGame();
		}
	}
}

void mouseCallback(ObjectID object, int x, int y, MouseAction action)
{
	if (object == button_practice)
	{
		enterScene(practice_scene);


		second_per_beat = 60.0f / practice_bpm;

		locateObject(puang, practice_scene, 70, 350);
		showObject(puang);


		setBar(practice_scene);

		now_stage = 1;
		prepare_beat = 0;
	}
	if (object == button_go)
	{
		enterScene(stage1_scene);

		second_per_beat = 60.0f / stage1_bpm;

		locateObject(puang, stage1_scene, 70, 350);
		showObject(puang);

		setBar(stage1_scene);

		now_stage = 2;
		prepare_beat = 4;
	}
	if (object == puang)
	{
		if (not on_game)
		{
			if (now_stage == 1)
			{
				playSound(practice_bgm, false);

				setTimer(delay, practice_second_delay - second_per_beat);
				startTimer(delay);
				on_game = true;
			}
			if (now_stage == 2)
			{
				playSound(stage1_bgm, false);

				setTimer(delay, stage1_second_delay - second_per_beat);
				startTimer(delay);
				on_game = true;

				sum_score = 100;
			}
		}
	}
}

void timerCallback(TimerID timer)
{
	if (timer == delay)
	{
		beat_ongo = 1;
		setTimer(beat, second_per_beat);
		startTimer(beat);
		if (now_stage == 1)
			setTimer(beat_bar, second_per_beat * 9);
		if (now_stage == 2)
			setTimer(beat_bar, second_per_beat * 13);
		startTimer(beat_bar);
	}
	if (timer == beat_bar)
	{
		setTimer(beat, second_per_beat);
		startTimer(beat);
		setTimer(beat_bar, second_per_beat * 8);
		startTimer(beat_bar);

		beat_ongo = 1;
		setObjectImage(puang, "resource/puang.png");
		setObjectImage(bar[0], "resource/bar_on.png");

		if (now_stage == 1)
			locateObject(puang, practice_scene, 70, 350);
		else if (now_stage == 2)
			locateObject(puang, stage1_scene, 70, 350);

		if (now_stage == 1)
			playSound(count[0]);
		if (now_stage == 2)
			playSound(kick_sound, false);
		beat_ongo++;
	}
	if (timer == beat)
	{
		if (on_game)
		{
			if (prepare_beat > 0)
			{
				playSound(count[prepare_beat - 1]);
				prepare_beat--;
				setTimer(beat, second_per_beat);
				startTimer(beat);
			}
			else if (beat_ongo < 8)
			{
				if (beat_ongo % 4 == 1)
				{
					if (now_stage == 1)
						locateObject(puang, practice_scene, 70, 350);
					else if (now_stage == 2)
						locateObject(puang, stage1_scene, 70, 350);
				}
				if (beat_ongo % 4 == 3)
				{
					if (now_stage == 1)
						locateObject(puang, practice_scene, 60, 300);
					else if (now_stage == 2)
						locateObject(puang, stage1_scene, 60, 300);
				}
				if (beat_ongo == 1)
				{
					setObjectImage(puang, "resource/puang.png");
					setObjectImage(bar[0], "resource/bar_on.png");
				}
				else if (beat_ongo == 7)
				{
					setObjectImage(bar[5], "resource/bar.png");
					setTimer(beat, second_per_beat);
					startTimer(beat);

				}
				else
				{
					setObjectImage(bar[beat_ongo - 1], "resource/bar_on.png");
					setObjectImage(bar[beat_ongo - 2], "resource/bar.png");

					if (beat_ongo == 6)
					{
						judged = false;
						sixbeat_time = clock();
					}
				}

				if (now_stage == 1)
					playSound(count[beat_ongo - 1], false);

				else if (now_stage == 2)
				{
					stopSound(kick_sound);
					playSound(kick_sound, false);
				}
				beat_ongo++;
				setTimer(beat, second_per_beat);
				startTimer(beat);
			}
			else if (beat_ongo == 8)
			{
				if (not judged)
					scoreJudge(false);
				beat_ongo = 1;
				if (now_stage == 1)
				{
					if (now_bar == 13)
					{
						stopTimer(beat_bar);
						stopTimer(beat);
						char buff[100];
						sprintf_s(buff, 100, "연습 종료! \nESC를 눌러 본게임을 시작하세요");
						showMessage(buff);
					}
				}
				if (now_stage == 2)
				{
					if (now_bar == 8)
						second_per_beat *= 4;
					else if (now_bar == 9)
						second_per_beat /= 8;
					else if (now_bar == 17)
					{
						stopTimer(beat_bar);
						stopTimer(beat);
						char buff[50];
						sprintf_s(buff, 50, "Score : %0.1f 점", sum_score);
						showMessage(buff);
					}
				}
				now_bar++;
			}
		}
	}
}

int main()
{
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);

	setMouseCallback(mouseCallback);
	setTimerCallback(timerCallback);
	setKeyboardCallback(keyboardCallback);

	main_scene = createScene("main", "resource/background.jpg");
	practice_scene = createScene("practice", "resource/background.jpg");
	stage1_scene = createScene("stage1", "resource/background.jpg");

	practice_bgm = createSound("resource/practice.mp3");
	stage1_bgm = createSound("resource/stage1_bgm.mp3");
	kick_sound = createSound("resource/drumkick.mp3");

	puang = createObject("resource/puang.png");

	button_practice = createObject("resource/button_practice.png", main_scene, 500, 110, true);
	button_go = createObject("resource/button_go.png", main_scene, 500, 10, true);

	message1 = createObject("resource/message_practice1.png", practice_scene, 10, 550, true);
	message2 = createObject("resource/message_stage1.png", stage1_scene, 10, 650, true);

	beat = createTimer(second_per_beat);
	delay = createTimer(practice_second_delay);
	beat2 = createTimer(second_per_beat / 2);
	beat_bar = createTimer(second_per_beat * 8);

	for (int i = 0; i < 6; i++)
	{
		bar[i] = createObject("resource/bar.png");
	}
	bar[6] = createObject("resource/bar7.png");

	for (int i = 0; i < 7; i++)
	{
		count[i] = createSound(countName(i + 1));
	}

	startGame(main_scene);
}