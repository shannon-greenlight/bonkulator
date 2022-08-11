#include <Wire.h>
// Keyboard functions

void select_button_down(void);
void scan_keyboard(void);

// #define KEYBOARD_ROW_0 0
#define KEYBOARD_ROW_1 A0
#define KEYBOARD_ROW_2 A1

#define SELECT_BUTTON_PIN 4

#define KEYBOARD_COLUMN_0 A2
#define KEYBOARD_COLUMN_1 A3
#define KEYBOARD_COLUMN_2 A6
#define KEYBOARD_COLUMN_3 A7

//
// "class" vars
volatile int keypress = 0; // set by several sources
volatile unsigned long select_button_down_time = 0;
volatile boolean select_button_pushed;
volatile uint8_t keys_row_0;
volatile uint8_t keys_row_1;
volatile uint8_t keys_row_2;

boolean esc_mode = false;
boolean select_button_available;

void reset_keyboard()
{
	// set the rows LOW so that any keypress will trigger interrupt
	// Row 0 is wired high
	digitalWrite(KEYBOARD_ROW_1, LOW);
	digitalWrite(KEYBOARD_ROW_2, LOW);
	keys_row_0 = 0;
	keys_row_1 = 0;
	keys_row_2 = 0;
	esc_mode = false;
	keypress = 0;
	select_button_pushed = false;
	select_button_available = false;
}

uint8_t crunch_key()
{
	uint8_t keys_row;
	keys_row = !digitalRead(KEYBOARD_COLUMN_0);
	keys_row |= !digitalRead(KEYBOARD_COLUMN_1) << 1;
	keys_row |= !digitalRead(KEYBOARD_COLUMN_2) << 2;
	keys_row |= !digitalRead(KEYBOARD_COLUMN_3) << 3;
	return keys_row;
}

bool key_available()
{
	return (select_button_available) || keys_row_0 || keys_row_1 || keys_row_2;
}

void wait_all_keys_up()
{
	while (key_available())
	{
		delay(10);
		scan_keyboard();
	}
}

void scan_keyboard()
{
	uint8_t same_cnt = 0;
	uint8_t sanity_cnt = 0;
	int last_val = 0;
	int current_val;
	while (sanity_cnt++ < 100 && same_cnt < 5)
	{
		// go row by row
		digitalWrite(KEYBOARD_ROW_1, HIGH);
		digitalWrite(KEYBOARD_ROW_2, HIGH);
		keys_row_0 = crunch_key();

		// digitalWrite(KEYBOARD_ROW_0, HIGH);
		digitalWrite(KEYBOARD_ROW_1, LOW);
		digitalWrite(KEYBOARD_ROW_2, HIGH);
		delay(1);
		keys_row_1 = crunch_key();

		// digitalWrite(KEYBOARD_ROW_0, HIGH);
		digitalWrite(KEYBOARD_ROW_1, HIGH);
		digitalWrite(KEYBOARD_ROW_2, LOW);
		delay(1);
		keys_row_2 = crunch_key();

		current_val = keys_row_0 << 8 | keys_row_1 << 4 | keys_row_2;
		if (current_val == last_val)
		{
			// hooray
			same_cnt++;
		}
		else
		{
			same_cnt = 0;
		}
		last_val = current_val;
		// delay(20);
	}
	// now check rotary push button
	unsigned long now = millis();
	if (select_button_pushed)
	{
		bool being_held = !digitalRead(SELECT_BUTTON_PIN);
		if (being_held)
		{
			if (now > select_button_down_time + 2000)
			{
				keypress = '*';
				select_button_available = true;
				select_button_pushed = false;
			}
		}
		else
		{
			keypress = '!';
			select_button_available = true;
			select_button_pushed = false;
		}
	}
	else
	{
		select_button_available = false;
	}
	if (key_available() && !ui.display_is_on)
	{
		restore_display();
		wait_all_keys_up();
		reset_keyboard();
	}
	// Serial.println("Sanity: " + String(sanity_cnt));
}

void calc_keypress()
{
	esc_mode = false;
	if (keys_row_0 == 1)
	{
		esc_mode = true;
		keypress = 'A'; // up 65
		return;
	}
	if (keys_row_0 == 2)
	{
		esc_mode = true;
		keypress = 'B'; // down
		return;
	}
	if (keys_row_0 == 4)
	{
		esc_mode = true;
		keypress = 'D'; // left
		return;
	}
	if (keys_row_0 == 8)
	{
		esc_mode = true;
		keypress = 'C'; // right
		return;
	}

	if (keys_row_1 == 1)
	{
		// keypress = '0';
		select_fxn(OUTPUT_0_FXN);
		return;
	}
	if (keys_row_1 == 2)
	{
		select_fxn(OUTPUT_1_FXN);
		return;
	}
	if (keys_row_1 == 4)
	{
		select_fxn(OUTPUT_2_FXN);
		return;
	}
	if (keys_row_1 == 8)
	{
		select_fxn(OUTPUT_3_FXN);
		return;
	}

	if (keys_row_2 == 1)
	{
		select_fxn(OUTPUT_4_FXN);
		return;
	}
	if (keys_row_2 == 2)
	{
		select_fxn(OUTPUT_5_FXN);
		return;
	}
	if (keys_row_2 == 4)
	{
		select_fxn(OUTPUT_6_FXN);
		return;
	}
	if (keys_row_2 == 8)
	{
		select_fxn(OUTPUT_7_FXN);
		return;
	}
}

void process_key()
{
	// ui.terminal_debug("Process key: " + String(keypress));
	switch (keypress)
	{
	case '~':
		// hardware_begin();
		// output_device.begin(AD5328_LDAC_PASSTHRU, AD5328_VDD_NONE, AD5328_BUFFERED_NONE, AD5328_GAIN_NONE);
		// digitalWrite(T3_LED, HIGH);
		// timer_debug();
		// add_waveform();
		// input_corrections_init();
		// ui.terminal_debug("Server status: " + String(server.status()));
		// input_cal_init();
		// ui.terminal_debug(String(cv_time_inc(2048, 128.)));
		// Serial.println(String(cv_time_inc(1023, 128.)));
		// Serial.println(String(cv_time_inc(512, 128.)));
		ui.terminal_debug("Data len: " + String(outputs[selected_output.get()].data_len));
		// printWifiStatus();
		// Serial.println("Server Status: " + String((server.status())));
		// dump_waveform(selected_output.get(), true);
		// dump_waveform(selected_output.get(), false);
		// output_debug();
		// selected_trigger->do_cmd('!');
		// (*settings_update_fxns[2])();
		break;
	case 'Z': // disable display and terminal
		ui.all_off();
		break;
	case 'z':
		restore_display();
		break;
	case '!':
		activate();
		break;
	case 'u':
		adjust_param(1);
		break;
	case 'd':
		adjust_param(-1);
		break;
	case '*':
		select_fxn(SETTINGS_FXN);
		break;
	// case '+':
	// 	select_fxn(remembered_fxn.get() + 1);
	// 	break;
	// case '-':
	// 	select_fxn(remembered_fxn.get() - 1);
	// 	break;
	case 'A': // up arrow in esc mode A
		if (esc_mode)
		{
			dec_param_num();
			// selected_fxn->inc_param_num_by(-1);
		}
		break;
	case 'B': // dn arrow B
		if (esc_mode)
		{
			inc_param_num();
			// selected_fxn->inc_param_num_by(1);
		}
		break;
	case 'C': // right arrow in esc mode C
		if (esc_mode)
		{
			// selected_fxn->inc_dig_num_by(1);
			inc_dig_num();
		}
		break;
	case 'D': // left arrow D
		if (esc_mode)
		{
			// selected_fxn->inc_dig_num_by(-1);
			dec_dig_num();
		}
		break;
	}
	esc_mode = keypress == 27 || keypress == '[';
}

void process_cmd(String in_str)
{
	// Serial.println("Process cmd: " + in_str);
	// ui.terminal_debug("Process cmd: " + in_str);

	char cmd = in_str.charAt(0);
	int int_param = in_str.substring(1).toInt();
	int dig1 = in_str.substring(1, 2).toInt();
	int dig2 = in_str.substring(2, 3).toInt();
	int selected_trig_num = OUTPUT_ENABLE_T0 + selected_trigger->trig_num;
	int selected_output_temp = selected_output.get();

	// ui.terminal_debug("Process cmd: " + in_str + " int_param: " + String(in_str.substring(0).toInt()) + " cmd: " + String(cmd));

	switch (cmd)
	{
	case '[':
		esc_mode = true;
		cmd = in_str.charAt(1);
		break;
	case ':':
		selected_fxn->put_dig_num(int_param);
		selected_fxn->printParams();
		break;
	case 'f':
		select_fxn(int_param);
		break;
	case 'p':
		selected_fxn->put_param_num(int_param);
		selected_fxn->printParams();
		break;
	case 'c':
		// clear trigger
		select_trigger(int_param);
		selected_trigger->clear();
		break;
	case 't':
		// select trigger
		// ui.terminal_debug("process_cmd: " + in_str.substring(1, 2));
		select_trigger(in_str.substring(1, 2).toInt());
		selected_fxn->printParams();
		if (in_str.charAt(2) == '!')
		{
			selected_trigger->trigger();
		}
		break;
	case 'T':
		switch (dig1)
		{
		case 0:
			// ui.terminal_debug("Disable Trigger! Output: " + String(dig2));
			select_fxn(dig2);
			(the_output)().param_put(0, selected_trig_num);
			output_update_trigger();
			select_fxn(selected_output_temp);
			break;
		case 1:
			// ui.terminal_debug("Enable Trigger! Output: " + String(dig2));
			select_fxn(dig2);
			(the_output)().param_put(1, selected_trig_num);
			output_update_trigger();
			select_fxn(selected_output_temp);
			break;
		case 2:
			// ui.terminal_debug("Toggle Trigger! Output: " + String(dig2));
			select_fxn(dig2);
			(the_output)().param_put((the_output)().get_param(selected_trig_num) == 1 ? 0 : 1, selected_trig_num);
			output_update_trigger();
			select_fxn(selected_output_temp);
			break;
		case 3:
			// ui.terminal_debug("Disable All Triggers!");
			for (int i = 0; i < NUM_OUTPUTS; i++)
			{
				for (int j = 0; j < NUM_TRIGGERS; j++)
				{
					bonk_output(i).put_param(0, OUTPUT_ENABLE_T0 + j);
					if (i == 0) // we only need to do this once
					{
						(*triggers[j]).disable_all();
					}
				}
			}
			break;
		case 4:
			// ui.terminal_debug("Trigger All Triggers!");
			for (int i = 0; i < NUM_TRIGGERS; i++)
			{
				select_trigger(i);
				selected_trigger->trigger();
			}
			break;
		case 5:
			// ui.terminal_debug("Clear All Triggers!");
			clear_all_triggers();
			break;
		case 6:
			// ui.terminal_debug("Display All Triggers!");
			trigger_report();
			break;
		}
		break;
	case 'J':
		switch (int_param)
		{
		case 0:
			ui.display_off();
			break;
		case 1:
			ui.display_on();
			break;
		}
		break;
	case 'M':
		switch (int_param)
		{
		case 1:
			input_cal_params_macro();
			break;
		case 2:
			output_cal_params_macro();
			break;
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
			user_waveform_params_macro(int_param - 10);
			break;
		default:
			gen_params_macro(selected_fxn);
		}
		break;
	case 'D':
		ui.clearTerminal();
		dump_waveform(dig1, dig2 > 0);
		Serial.println("Time_inc: " + String(outputs[dig1].time_inc));
		break;
	case 'O':
		(the_output)().put_param_w_offset(int_param, OUTPUT_OFFSET);
		break;
	case 'Q':
		(the_output)().put_param(int_param, OUTPUT_QUANTIZE);
		break;
	case 'S':
		(the_output)().put_param(int_param, OUTPUT_SCALE);
		break;
	case 'R':
		(the_output)().put_param(int_param, OUTPUT_RANDOMNESS);
		break;
	case 'V':
		(the_output)().put_param(int_param, OUTPUT_IDLE_VALUE);
		break;
	case 'W':
		ui.clearTerminal();
		printWifiStatus();
		break;
	case '$':
		// String sval = urlDecode(in_str.substring(1));
		selected_fxn->put_string_var(urlDecode(in_str.substring(1)));
		break;
	case '/':
		// comment character. Do nothing
		break;
	case '#':
		selected_fxn->put_param_w_offset(int_param);
		if (in_output_fxn())
		{
			switch (selected_fxn->param_num)
			{
			case OUTPUT_SCALE:
			case OUTPUT_OFFSET:
			case OUTPUT_RANDOMNESS:
			case OUTPUT_QUANTIZE:
				apply_params_to_waveform();
				graph_waveform(selected_output.get());
				break;

			default:
				break;
			}
		}
		break;
	}

	// Serial.println("Process cmd: " + in_str + " int_param: " + String(in_str.substring(0).toInt()) + " cmd: " + String(cmd));
	if (cmd >= '-' && cmd <= '9' && cmd != '/')
	{
		// selected_fxn->put_param(in_str.toInt());
		selected_fxn->put_param_w_offset(in_str.substring(0).toInt());
		if (in_output_fxn())
		{
			switch (selected_fxn->param_num)
			{
			case OUTPUT_SCALE:
			case OUTPUT_OFFSET:
			case OUTPUT_RANDOMNESS:
			case OUTPUT_QUANTIZE:
				apply_params_to_waveform();
				graph_waveform(selected_output.get());
				break;

			default:
				break;
			}
		}
	}

	if (String(cmd) != "\n" && cmd != 'p')
	{
		if (String(cmd) == "%")
		{
			keypress = 27;
		}
		else
		{
			keypress = cmd;
		}

		process_key();
	}
}

void check_keyboard()
{
	scan_keyboard();
	if (key_available())
	{
		calc_keypress();
		process_key();
		wait_all_keys_up();
		delay(10);
		reset_keyboard();
	}
}

void check_serial()
{
	static String in_str = ""; // for serial input
	static boolean cmd_available = false;
	static bool entering_string = false; // or entering a number or comment

	if (Serial.available() > 0)
	{
		char c = Serial.read();
		bool is_esc_char = esc_mode && (c == 'A' || c == 'B' || c == 'C' || c == 'D');
		// Serial.println(in_str + " .. " + String(c));
		// ui.terminal_debug(in_str + " .. " + String(c));
		if (c == '$' || c == '-' || c == '+' || c == '/')
		{
			entering_string = true;
		}
		if (!entering_string && (in_str.length() < 2 || is_esc_char) && (c == '~' || c == '*' || c == '!' || c == 'u' || c == 'd' || c == 'z' || c == 'Z' || is_esc_char))
		{
			// process this immediately with process_keypress
			keypress = c;
		}
		else
		{
			if (c == '\r')
			{
				// process this with process_cmd
				cmd_available = true;
				entering_string = false;
			}
			else
			{
				if (c == 27 || c == '[')
				{
					esc_mode = true;
				}
				else
				{
					in_str += c;
					if (c == '!')
					{
						cmd_available = true;
						entering_string = false;
					}
				}
			}
		}
		if (keypress || cmd_available)
		{
			if (cmd_available)
			{
				process_cmd(in_str);
				in_str = "";
				keypress = 0;
				cmd_available = false;
			}
			else
			{
				process_key();
				reset_keyboard();
				in_str = "";
			}
		}
	}
}

void select_button_down()
{
	if (!select_button_pushed) // ignore falling noise
	{
		select_button_pushed = true;
		select_button_down_time = millis();
	}
}

void keyboard_begin()
{
	pinMode(KEYBOARD_ROW_1, OUTPUT);
	pinMode(KEYBOARD_ROW_2, OUTPUT);
	pinMode(KEYBOARD_COLUMN_0, INPUT_PULLUP);
	pinMode(KEYBOARD_COLUMN_1, INPUT_PULLUP);
	pinMode(KEYBOARD_COLUMN_2, INPUT_PULLUP);
	pinMode(KEYBOARD_COLUMN_3, INPUT_PULLUP);
	reset_keyboard();
	delay(200);
	attachInterrupt(digitalPinToInterrupt(SELECT_BUTTON_PIN), select_button_down, FALLING);
}
