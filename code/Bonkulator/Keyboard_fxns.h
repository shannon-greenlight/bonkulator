#include <Wire.h>
// Keyboard functions

// real definitions
void select_button_down(void);
// #define KEYBOARD_ROW_0 0
#define KEYBOARD_ROW_1 A0
#define KEYBOARD_ROW_2 A1

#define SELECT_BUTTON_PIN 4

#define KEYBOARD_COLUMN_0 A2
#define KEYBOARD_COLUMN_1 A3
#define KEYBOARD_COLUMN_2 A6
#define KEYBOARD_COLUMN_3 A7

//
// "private" vars
volatile uint8_t keys_row_0;
volatile uint8_t keys_row_1;
volatile uint8_t keys_row_2;

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

void scan_keyboard()
{
	uint8_t same_cnt = 0;
	uint8_t sanity_cnt = 0;
	int last_val;
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
			keypress = '\n';
			select_button_available = true;
			select_button_pushed = false;
		}
	}
	else
	{
		select_button_available = false;
	}
	// Serial.println("Sanity: " + String(sanity_cnt));
}

bool key_available()
{
	return (select_button_available) || keys_row_0 || keys_row_1 || keys_row_2;
}

void wait_all_keys_up()
{
	while (key_available())
	{
		scan_keyboard();
	}
}

void calc_keypress()
{
	esc_mode = false;
	output_mode = false;
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
		output_mode = true;
		keypress = '0';
		return;
	}
	if (keys_row_1 == 2)
	{
		output_mode = true;
		keypress = '1';
		return;
	}
	if (keys_row_1 == 4)
	{
		output_mode = true;
		keypress = '2';
		return;
	}
	if (keys_row_1 == 8)
	{
		output_mode = true;
		keypress = '3';
		return;
	}

	if (keys_row_2 == 1)
	{
		output_mode = true;
		keypress = '4';
		return;
	}
	if (keys_row_2 == 2)
	{
		output_mode = true;
		keypress = '5';
		return;
	}
	if (keys_row_2 == 4)
	{
		output_mode = true;
		keypress = '6';
		return;
	}
	if (keys_row_2 == 8)
	{
		output_mode = true;
		keypress = '7';
		return;
	}
}

void check_serial()
{
	static bool entering_string = false;

	if (Serial.available() > 0)
	{
		char c = Serial.read();
		bool is_esc_char = esc_mode && (c == 'A' || c == 'B' || c == 'C' || c == 'D');
		// Serial.println(in_str + " .. " + String(c));
		if (c == '$')
		{
			entering_string = true;
		}
		if (!entering_string && (c == '~' || c == '*' || c == '!' || c == 'u' || c == 'd' || c == '+' || c == '-' || c == 'z' || c == 'Z' || is_esc_char))
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
				}
			}
		}
	}
}

void process_cmd(String in_str)
{
	// Serial.println("Process cmd: " + in_str);
	// ui.terminal_debug("Process cmd: " + in_str);
	// noInterrupts();
	char cmd = in_str.charAt(0);
	int int_param = in_str.substring(1).toInt();
	int val;
	remote_adjusting = false;

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
	case 'p':
		selected_fxn->put_param_num(int_param);
		selected_fxn->printParams();
		break;
	case '#':
		selected_fxn->put_param(int_param);
		selected_fxn->printParam();
		break;
	case '$':
		String sval = urlDecode(in_str.substring(1));
		selected_fxn->put_string_var(sval);
		selected_fxn->printParam();
		break;
	}

	keypress = cmd;
}

void process_key()
{
	// ui.terminal_debug("Process key: " + String(keypress));
	switch (keypress)
	{
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
		selected_output.put(keypress - '0');
		in_wifi = false;
		break;
	case '*':
		in_wifi = false;
		break;
	}

	switch (keypress)
	{
	case '!':
		activate();
		break;
	case 'u':
		adjust_param(1);
		break;
	case 'd':
		adjust_param(-1);
		break;
	case '0':
		selected_fxn = &output0;
		selected_fxn->display();
		break;
	case '1':
		selected_fxn = &output1;
		selected_fxn->display();
		break;
	case '2':
		selected_fxn = &output2;
		selected_fxn->display();
		break;
	case '3':
		selected_fxn = &output3;
		selected_fxn->display();
		break;
	case '4':
		selected_fxn = &output4;
		selected_fxn->display();
		break;
	case '5':
		selected_fxn = &output5;
		selected_fxn->display();
		break;
	case '6':
		selected_fxn = &output6;
		selected_fxn->display();
		break;
	case '7':
		selected_fxn = &output7;
		selected_fxn->display();
		break;
	case '*':
		selected_fxn = &settings_fxn;
		selected_fxn->display();
		break;
	case 'A': // up arrow in esc mode A
		if (esc_mode)
		{
			selected_fxn->inc_param_num_by(-1);
		}
		break;
	case 'B': // dn arrow B
		if (esc_mode)
		{
			selected_fxn->inc_param_num_by(1);
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
