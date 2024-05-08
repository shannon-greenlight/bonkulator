#include "Arduino.h"
#include <TerminalVT100.h>
#include <Greenface_EEPROM.h>
#include <Greenface_ui.h>

#include <Greenface_gadget.h>

extern uint16_t get_chars_per_line();
extern void noop();

// vector<string> split (const string &s, char delim) {
//     vector<string> result;
//     stringstream ss (s);
//     string item;

//     while (getline (ss, item, delim)) {
//         result.push_back (item);
//     }

//     return result;
// }

extern String csv_elem(String s, char delim, int instance);

Greenface_gadget::Greenface_gadget(String _name, String *_labels, uint16_t **_params, uint16_t _num_params) : params(*_params, _num_params)
{
    name = _name;
    labels = _labels;
    num_params = _num_params;
    param_num = 0;
    digit_num = 0;
    params = EEPROM_Arr16(_params[0], _num_params);
    mins = _params[1];
    maxs = _params[2];
    init_vals = _params[3];
    display_fxn = &noop;
}

void Greenface_gadget::begin()
{
    params.begin(false);
    params.xfer();
    update_fxns = nullptr;
    triggered = false;

    // EEPROM_String *string_var = &string_vars[get_param(p_num)];
    // Serial.println("Spank Begin: " + name);
    // Serial.println("Size: " + String(sizeof(string_vars)));

    // Serial.println("");
    // Serial.print(name + " Beginning "+labels[0]);
    // Serial.println(params.get(0));
}

int Greenface_gadget::offset()
{
    return params.offset;
}

uint8_t Greenface_gadget::get_param_type(int p_num)
{
    if (string_params)
    {
        if (string_params[p_num].length())
        {
            if (string_params[p_num] == STRING_TOKEN)
            {
                return SPANK_STRING_VAR_TYPE;
            }
            else
            {
                return SPANK_STRING_PARAM_TYPE;
            }
        }
    }
    if (decimal_places)
    {
        if (decimal_places[p_num] > 0)
        {
            return SPANK_FIXED_POINT_TYPE;
        }
    }
    return SPANK_INT_PARAM_TYPE;
}

uint16_t Greenface_gadget::get_param(int16_t indx)
{
    if (indx == -1)
        indx = param_num;
    uint16_t param = params.get(indx);
    param = min(get_max(indx), param);
    param = max(get_min(indx), param);
    return param;
}

bool Greenface_gadget::get_param_active(int16_t indx)
{
    if (indx == -1)
        indx = param_num;
    if (active_params)
    {
        return active_params[indx];
    }
    else
    {
        return false;
    }
}

int Greenface_gadget::get_param_w_offset(int16_t indx)
{
    if (indx == -1)
        indx = param_num;
    int param = (int)params.get(indx);
    if (offsets)
    {
        param += offsets[indx];
    }
    // hmmm... no constraints?
    return param;
}

float Greenface_gadget::get_param_as_float(int16_t indx)
{
    float p = float(get_param_w_offset(indx));
    double divisor = pow(10, decimal_places[indx]);
    return p / divisor;
}

String Greenface_gadget::get_param_as_string(int16_t indx)
{
    // uint16_t param = get_param(indx);
    return csv_elem(string_params[indx], ',', get_param(indx));
}

String Greenface_gadget::get_param_as_string_var(int16_t indx)
{
    EEPROM_String *string_var = &string_vars[get_param(indx)];
    return (*string_var).get();
}

void Greenface_gadget::put_string_var(String val, int16_t indx)
{
    if (indx == -1)
        indx = param_num;
    uint8_t param_type = get_param_type(indx);
    if (param_type == SPANK_STRING_VAR_TYPE)
    {
        string_vars[get_param(indx)].put(val);
        printParam();
        exe_update_fxn();
        // Serial.println("indx: " + String(indx) + " param: " + String(get_param(indx)) + " val: " + val);
    }
}

uint16_t Greenface_gadget::get_min(int16_t indx)
{
    if (indx == -1)
        indx = param_num;
    return mins[indx];
}

uint16_t Greenface_gadget::get_max(int16_t indx)
{
    if (indx == -1)
        indx = param_num;
    return maxs[indx];
}

int Greenface_gadget::get_min_w_offset(int16_t indx)
{
    if (indx == -1)
        indx = param_num;
    int16_t min = mins[indx];
    if (offsets)
    {
        min += offsets[indx];
    }
    return min;
}

int Greenface_gadget::get_max_w_offset(int16_t indx)
{
    if (indx == -1)
        indx = param_num;
    int max = maxs[indx];
    if (offsets)
    {
        max += offsets[indx];
    }
    return max;
}

void Greenface_gadget::adjust_param(int e, unsigned long delta)
{
    char string_var_char;
    EEPROM_String *string_var = &string_vars[get_param(param_num)];
    switch (get_param_type(param_num))
    {
    case SPANK_STRING_VAR_TYPE:
        string_var_char = (*string_var).charAt(digit_num);
        // Serial.println("string: " + string_var.get());
        string_var_char += e;
        if (string_var_char > 0x7b)
        {
            string_var_char = 0x20;
        }
        if (string_var_char < 0x20)
        {
            string_var_char = 0x7b;
        }

        // ui->terminal_debug("varchar: " + String(string_var_char) + " strlen: " + String((*string_var).length()));
        if (digit_num >= (*string_var).length())
        {
            (*string_var).append(String(string_var_char));
        }
        else
        {
            (*string_var).putCharAt((string_var_char), digit_num);
        }
        printParam();
        break;
    default:
        int the_param = get_param();
        int multiplier = 1;
        uint8_t ndigs = get_num_digits(param_num);
        digit_num = min(ndigs - 1, digit_num);
        int dig = (ndigs - digit_num) - 1;
        if (decimal_places)
        {
            if (dig > decimal_places[param_num] && decimal_places[param_num] > 0)
            {
                dig--;
            }
        }
        multiplier = pow(10, dig);
        multiplier = max(1, multiplier);
        the_param += e * multiplier;
        // Serial.println("dig: " + String(dig) + " ndigs: " + String(ndigs) + " multiplier: " + String(multiplier) + " the_param: " + String(the_param) + " dir: " + String(e));
        // Serial.println("dignum: " + String(digit_num));
        the_param = constrain(the_param, 0, 0xFFFF);
        put_param(the_param);
        break;
    }
}

int Greenface_gadget::check_param(int param)
{
    // ui.terminal_debug("Param OK param#: " + String(param_num) + " Offset: " + String(display_offset));
    switch (param_num)
    {
    case LONGEST_PULSE:
        param = max(param, get_param(SHORTEST_PULSE) + 1);
        break;
    case SHORTEST_PULSE:
        param = min(param, get_param(LONGEST_PULSE) - 1);
        break;
    }
    return param;
}

void Greenface_gadget::print_string_param(uint16_t p_num, uint16_t line_num)
{
    String s = csv_elem(string_params[p_num], ',', get_param(p_num));
    String sval = get_label(p_num) + s;
    int num_blanks = 20 - sval.length();
    for (int i = 0; i < num_blanks; i++)
    {
        sval += " ";
    }
    ui.printLine(sval, ui.lines[line_num], 1); // is 10 trailing spaces enough?
}

void Greenface_gadget::print_string_var(uint16_t p_num, uint16_t line_num)
{
    EEPROM_String *string_var = &string_vars[get_param(p_num)];
    String label = get_label(p_num);
    int label_len = label.length();
    String pad;
    int num_blanks = get_chars_per_line() - (label_len + (*string_var).length());
    for (int i = 0; i < num_blanks; i++)
    {
        pad.concat(' ');
    }
    ui.printLine(label + (*string_var).get() + pad, ui.lines[line_num], 1);
}

void Greenface_gadget::print_param(uint16_t p_num, uint16_t line_num)
{
    uint8_t param_type = get_param_type(p_num);
    String label = get_label(p_num);
    String output = "";
    int dp;
    int frac_part, int_part;
    String sign;
    char intgr[5];
    char fract[5];
    char fmt[10];
    int places;
    uint8_t ndigs = get_num_digits(p_num);

    if (alt_values)
    {
        String alt_value = alt_values[p_num];
        if (alt_value.length() > 0)
        {
            ui.printLine(label + alt_value, ui.lines[line_num], 1);
            return;
        }
    }

    switch (param_type)
    {
    case SPANK_STRING_PARAM_TYPE:
        print_string_param(p_num, line_num);
        break;
    case SPANK_STRING_VAR_TYPE:
        print_string_var(p_num, line_num);
        break;
    case SPANK_FIXED_POINT_TYPE:
        dp = (pow(10, decimal_places[p_num]));
        int_part = abs(get_max_w_offset(p_num)) / dp;
        sign = get_param_w_offset(p_num) >= 0 ? "+" : "-";
        if (int_part == 0)
        {
            places = 1;
        }
        else
        {
            places = int(log10(float(int_part)));
        }
        places++;
        // ui.terminal_debug(" places: " + String(places) + " int_part: " + String(int_part));
        // fmt = "%0+";
        strcpy(fmt, "%0");
        strcat(fmt, String(places).c_str());
        strcat(fmt, "d");
        // Serial.println("FMT Int part: " + String(fmt) + " int_part: " + String(int_part) + " dp: " + String(dp));

        int_part = abs(get_param_w_offset(p_num)) / dp;
        sprintf(intgr, fmt, int_part, places);

        frac_part = abs(get_param_w_offset(p_num)) % dp;
        memset(fmt, '\0', sizeof(fmt));
        strcpy(fmt, "%0");
        strcat(fmt, String(decimal_places[p_num]).c_str());
        strcat(fmt, "d");
        // Serial.println("FMT Frac part: " + String(fmt) + " frac_part: " + String(frac_part));
        sprintf(fract, fmt, frac_part);
        output = sign + String(intgr) + "." + String(fract);

        ui.printLine(label + output, ui.lines[line_num], 1);

        break;
    default:
        bool is_offset = false;
        if (offsets)
        {
            if (offsets[p_num] != 0)
                is_offset = true;
        }
        String fmt = calc_format(ndigs, is_offset);
        const char *format = fmt.c_str();
        int param = get_param(p_num);
        if (offsets)
        {
            param += offsets[p_num];
        }
        // ui.terminal_debug("Param: " + String(param) + " ndigs: " + String(ndigs) + " fmt: " + fmt + " is offset: " + String(is_offset));
        ui.printParam(get_label(p_num), param, ndigs, format, line_num);
        break;
    }
}

void Greenface_gadget::exe_update_fxn()
{
    if (update_fxns)
    {
        if (update_fxns[param_num] != nullptr)
        {
            (*update_fxns[param_num])();
        }
    }
}

// TODO fix data type to allow negative numbers
void Greenface_gadget::put_param_w_offset(int val, int8_t _param_num)
{
    uint16_t param;

    if (_param_num != -1)
    {
        param_num = _param_num;
    }
    if (offsets)
    {
        val -= offsets[param_num];
    }
    param = (uint16_t)val;
    put_param(param, param_num);
}

void Greenface_gadget::param_put(uint16_t val, int8_t _param_num)
{
    params.put(val, _param_num);
}

void Greenface_gadget::put_param(uint16_t val, int8_t _param_num)
{
    String s;
    int save_param_num = param_num;
    // Serial.println("Val: " + String(val));
    // Serial.println("Max: " + String(get_max()));
    // Serial.println("Min: " + String(get_min()));
    if (_param_num != -1)
    {
        param_num = _param_num;
    }
    if (val > get_max())
        val = get_max();
    // ui.terminal_debug("Param num: " + String(_param_num) + " Val: " + String(val));
    if (val < get_min())
        val = get_min();
    // Serial.print("final val: ");
    // Serial.println(val);
    if (check_params)
        val = check_param(val);

    param_put(val, param_num);
    printParam();
    exe_update_fxn();
    param_num = save_param_num;
}

bool Greenface_gadget::param_is_stringvar()
{
    return string_params[param_num] == STRING_TOKEN;
}

void Greenface_gadget::set_cursor_to_param()
{
    uint8_t dig_offset = get_label(param_num).length();
    int char_num = digit_num + dig_offset + 1;
    String row = String(min(2, param_num) * 2 + display_offset + 8);
    // ui.terminal_debug("param_num: " + String(param_num) + " offset: " + String(display_offset));
    ui.terminal_setCursor(row, String(char_num));
    ui.terminal_clrToEOL();
}

void Greenface_gadget::hilight_param()
{
    if (enable_hilight)
    {
        uint8_t dig_offset = get_label(param_num).length();
        int char_num = digit_num + dig_offset;
        ui.underline_char(char_num, min(2, param_num + display_offset), 1, 8);
        // ui.terminal_debug("param_num: " + String(param_num) + " offset: " + String(display_offset));
        // ui.hiliteChar(min(2, param_num + display_offset), char_num);
        //  Serial.print ("Dig offset: "+String(dig_offset));
        //  Serial.print (" Digit #: "+String(digit_num));
        //  Serial.println(" Char Num: "+String(char_num));
    }
}

void Greenface_gadget::put_param_num(int val)
{
    int _val = val;
    _val = max(0, _val);
    _val = min(num_params - 1, _val);
    param_num = _val;
    if (get_param_type(param_num) == SPANK_STRING_VAR_TYPE)
    {
        digit_num = 0;
    }
    else
    {
        digit_num = get_num_digits(param_num) - 1;
    }
    // ui.terminal_debug("Dig# " + String(digit_num) + " param num: " + String(param_num) + " type: " + String(get_param_type(param_num)));
}

void Greenface_gadget::inc_param_num_by(int val)
{
    if (param_num + val < 0)
    {
        val = num_params - 1;
    }
    if (param_num + val > num_params - 1)
    {
        param_num = val = 0;
    }
    put_param_num(param_num + val);
    display();
    // ui.terminal_debug("Inc param num by: " + String(val));
}

void Greenface_gadget::insert_char(char c)
{
    uint8_t param_type = get_param_type(param_num);
    if (param_type == SPANK_STRING_VAR_TYPE)
    {
        String s = string_vars[get_param(param_num)].get();
        string_vars[get_param(param_num)].put(s.substring(0, digit_num) + String(c) + s.substring(digit_num));
        printParam();
    }
}

void Greenface_gadget::remove_char()
{
    uint8_t param_type = get_param_type(param_num);
    if (param_type == SPANK_STRING_VAR_TYPE)
    {
        String s = string_vars[get_param(param_num)].get();
        s.remove(digit_num - 1, 1);
        string_vars[get_param(param_num)].put(s);
        inc_dig_num_by(-1);
        printParam();
    }
}

void Greenface_gadget::put_dig_num(int val)
{
    // uint8_t num_digs = log10(get_max(param_num))+1;
    uint16_t num_digs = get_num_digits(param_num);
    bool sign_skipped = false;
    if (val < 0)
        val = 0;
    // Serial.println("Dig# "+String(digit_num)+" Digs "+String(num_digs));
    if (offsets)
    {
        if (offsets[param_num] && val < num_digs - 1)
        {
            val++;
            sign_skipped = true;
        }
    }
    if (decimal_places)
    {
        if (decimal_places[param_num])
        {
            if (val < num_digs - 1 && !sign_skipped)
            {
                val++;
                sign_skipped = true;
            }
            if (val >= (num_digs - 1) - decimal_places[param_num])
                val++; // skip decimal point
        }
    }
    if (val > num_digs - 1)
        val = num_digs - 1;
    digit_num = val;
    printParam();
    hilight_param();
}

void Greenface_gadget::inc_dig_num_by(int val)
{
    uint8_t num_digs = get_num_digits(param_num, NUMERIC_DIGITS_ONLY);
    uint8_t param_type = get_param_type(param_num);

    // ui.terminal_debug("Dig# " + String(digit_num) + " Num Digs " + String(num_digs) + " param num: " + String(param_num) + " max: " + String(get_max_w_offset(param_num)));
    if (get_max() >= 10 || param_type == SPANK_STRING_VAR_TYPE)
    {
        if (digit_num + val < 0)
        {
            digit_num = num_digs - 1;
        }
        else
        {
            digit_num += val;
        }
        if (param_type == SPANK_FIXED_POINT_TYPE)
        {
            num_digs++; // account for dp
            // Serial.println("Fixed point type, num_digs: " + String(num_digs));
            if (digit_num == 0 || digit_num > num_digs)
                digit_num = 1;
            if (digit_num == num_digs - decimal_places[param_num])
            {
                if (val < 0)
                {
                    digit_num--;
                }
                else
                {
                    digit_num++;
                }
                // digit_num += val;
                // Serial.println("Skipping decimal point: Dig# " + String(digit_num) + " Num Digs " + String(num_digs));
            }
        }
        else
        {
            int end_dig = num_digs - 1;
            if (offsets)
            {
                if (offsets[param_num] < 0)
                {
                    end_dig = num_digs;
                }
            }
            if (digit_num > end_dig)
                digit_num = 0;
        }
        if (offsets)
        {
            if (offsets[param_num] < 0 && digit_num == 0)
            {
                digit_num++;
                // Serial.println("Bumping: Dig# " + String(digit_num));
            }
        }
        // Serial.println("Final, num_digs: " + String(num_digs));
        // Serial.println("Final, digit_num: " + String(digit_num));
        printParam();
        hilight_param();
    }
}

String Greenface_gadget::get_label(int label_num)
{
    return labels[label_num];
}

void Greenface_gadget::printParam(int8_t _param_num)
{
    int save_param_num = param_num;
    if (_param_num != -1)
    {
        param_num = _param_num;
    }
    print_param(param_num, min(2, param_num + display_offset));
    param_num = save_param_num;
}

void Greenface_gadget::printParams()
{
    ui.fill(BLACK, 16 + (16 * display_offset));
    if (ui.terminal_mirror)
    {
        ui.t.clrDown(display_offset == 1 ? "10" : "8");
    }
    hilight_param();
    // Serial.println("Num params: "+String(num_params) + " Param# "+String(param_num));
    int start = max(0, param_num - (2 - display_offset));
    int end = min(3 - display_offset, num_params);
    for (int i = 0; i < end; i++)
    {
        print_param(i + start, i + display_offset);
    }
}

String Greenface_gadget::params_toJSON()
{
    String out = "";
    String label;
    uint8_t param_type;
    bool alt_done;
    String tempval;

    for (int i = 0; i < num_params; i++)
    {
        if (out > "")
            out += ",";
        out += "{ ";
        label = get_label(i);
        out += toJSON(F("label"), label);
        out += ", ";

        alt_done = false;
        if (alt_values)
        {
            String alt_value = alt_values[i];
            if (alt_value.length() > 0)
            {
                out += toJSON(F("type"), F("text"));
                out += ", ";
                out += toJSON(F("value"), alt_value);
                alt_done = true;
            }
        }

        if (!alt_done)
        {
            param_type = get_param_type(i);
            switch (param_type)
            {
            case SPANK_STRING_PARAM_TYPE:
                out += toJSON(F("type"), F("select"));
                out += ", ";
                out += toJSON(F("values"), string_params[i]);
                out += ", ";
                out += toJSON(F("value"), csv_elem(string_params[i], ',', get_param(i)));
                break;
            case SPANK_STRING_VAR_TYPE:
                out += toJSON(F("type"), F("text"));
                out += ", ";
                out += toJSON(F("value"), get_param_as_string_var(i));
                break;
            default:
                out += toJSON(F("type"), F("number"));
                out += ", ";
                tempval = String(get_param_w_offset(i));
                out += toJSON(F("value"), tempval);
                break;
            }
        }

        out += ", ";
        out += toJSON(F("numeric_val"), String(get_param_w_offset(i)));
        out += ", ";
        out += toJSON(F("param_num"), String(i));
        out += ", ";
        out += toJSON(F("min"), String(get_min_w_offset(i)));
        out += ", ";
        out += toJSON(F("max"), String(get_max_w_offset(i)));
        out += ", ";
        int num_digits = get_num_digits(i);
        if (get_min_w_offset(i) < 0)
        {
            num_digits += -1;
        }
        out += toJSON(F("num_digits"), String(num_digits));
        out += ", ";
        out += toJSON(F("selected"), user_entering_param == 1 && enable_hilight && param_num == i ? "true" : "false");
        out += ", ";
        out += toJSON(F("dp"), decimal_places ? String(decimal_places[i]) : "0");
        out += " }";
    }
    return "[" + out + "]";
}

void Greenface_gadget::default_display()
{
    ui.newFxn(name);
    printParams();
}

void Greenface_gadget::display()
{
    if (display_fxn == &noop)
    {
        default_display();
    }
    display_fxn();
}

void Greenface_gadget::copy_to(Greenface_gadget &target)
{
    for (int i = 0; i < 6; i++)
    {
        target.params.put(0, i);
        target.labels[i] = "";
        target.mins[i] = 0;
        target.maxs[i] = 0;
    }
    for (int i = 0; i < num_params; i++)
    {
        target.params.put(get_param(i), i);
        target.labels[i] = get_label(i);
        target.mins[i] = get_min(i);
        target.maxs[i] = get_max(i);
    }
    target.name = name;
    target.trigger_fxn = trigger_fxn;
    target.num_params = num_params;
    target.string_params = string_params;
    target.check_params = check_params;
    // Serial.println("Copying: " + target.name + " Num params: " + String(target.num_params));
}

void Greenface_gadget::init()
{
    // ui.terminal_debug("Initializing: " + name);
    Serial.println("\nInitializing: " + name);
    param_num = 0;
    user_entering_param = 1;
    digit_num = 0;
    triggered = false;
    for (int i = 0; i < num_params; i++)
    {
        Serial.println(get_label(i) + String(init_vals[i]));
        params.put(init_vals[i], i);
    }
}

void Greenface_gadget::debug()
{
    Serial.println("Howdy I'm " + name);
    Serial.println("Initial Delay: " + String(get_param(INITIAL_DELAY)));
}

void Greenface_gadget::trigger(uint8_t by, bool toggle)
{
    triggered_by = by;
    triggered = toggle ? !triggered : true;
    // triggered = !triggered;
    // if (triggered)
    // {
    //     // triggered = true;
    //     // Serial.println(name + " was triggered!");
    // }
    // else
    // {
    //     // Serial.println(name + " was un-triggered!");
    // }
}

void Greenface_gadget::clear_trigger(uint8_t by)
{
    if (triggered_by == by)
        triggered = false;
}

// private fxns
uint8_t Greenface_gadget::get_num_digits(int this_param_num, bool numeric_only)
{
    // Serial.println("Getting ndigs: "+String(this_param_num));
    int param_type = get_param_type(this_param_num);
    uint8_t dps = 0;
    bool is_offset = false;
    if (decimal_places)
    {
        dps = decimal_places[this_param_num];
    }
    if (offsets)
    {
        is_offset = offsets[this_param_num] < 0; // it has a negative sign
    }

    switch (param_type)
    {
    case SPANK_STRING_PARAM_TYPE:
        return 1;
        break;
    case SPANK_STRING_VAR_TYPE:
        return string_vars[get_param(this_param_num)].size;
        break;
    default:
        uint16_t dig_log = log10(abs(get_max_w_offset(this_param_num)));
        if (numeric_only)
        {
            return dig_log + 1;
        }
        int pad = (dps > 0 || is_offset) ? 2 : 1;
        if (dps > 0)
            pad++; // it has both a dp and a + sign
        return dig_log + pad;
        break;
    }
}

String Greenface_gadget::calc_format(uint8_t ndigs, bool is_offset)
{
    String plus_sign = is_offset ? "+" : "";
    return "%0" + plus_sign + String(ndigs) + "d";
}
