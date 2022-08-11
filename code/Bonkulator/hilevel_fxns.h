void scale_int(int *val, float scale)
{
    *val *= scale;
    *val = min(DAC_FS, *val);
    // *val = max(0, *val);
}

String toJSON(String key, String value)
{
    String s1 = "\\";
    String s2 = "\\\\";
    value.replace(s1, s2);

    s1 = "\"";
    s2 = "\\\"";
    value.replace(s1, s2);

    // Serial.println(value);
    return "\"" + key + "\" :\"" + value + "\"";
}

String urlDecode(String str)
{
    String ret;
    char ch;
    int i, ii, len = str.length();

    for (i = 0; i < len; i++)
    {
        if (str[i] != '%')
        {
            if (false && str[i] == '+') // spank client must not use + for space
                ret += ' ';
            else
                ret += str[i];
        }
        else
        {
            sscanf(str.substring(i + 1, i + 3).c_str(), "%x", &ii);
            // Serial.println("UrlDecode char: " + str + ": " + String(ii) + ".");
            // ii = 32;
            ch = static_cast<char>(ii);
            ret += ch;
            i = i + 2;
        }
    }
    return ret;
}

bool in_settings()
{
    return selected_fxn == &settings_fxn;
}

// bool in_output_fxn()
// {
//     // return true if we are in an output fxn
//     return !in_settings();
// }

// bool in_output_fxn()
// {
//     // return true if we are in an output fxn
//     for (int i = 0; i < NUM_OUTPUTS; i++)
//     {
//         if (selected_fxn == bonk_outputs[i])
//             return true;
//     }
//     return false;
// }

bool in_output_fxn()
{
    return selected_fxn_num < SETTINGS_FXN;
}

void adjust_param(int encoder_val)
{
    if (in_settings())
    {
        settings_adjust_param(encoder_val, 0);
    }
    else
    {
        selected_fxn->adjust_param(encoder_val, 0);
    }
}

void check_rotary_encoder()
{
    if (encoder_is_adjusting())
    {
        reset_inactivity_timer();
        // ui.terminal_debug("Rotary: " + String(e.getEncoderValue()));
        adjust_param(getEncoderValue());
        resetEncoder();
        delay(25);
    }
}

void activate()
{
    if (in_settings())
    {
        settings_activate();
    }
    else if (in_output_fxn())
    {
        selected_trigger->trigger();
    }
    else if (in_user_waveform_fxn())
    {
        user_waveforms_activate();
    }
    else if (in_wifi)
    {
        wifi_activate();
    }
    else if (in_ask_init())
    {
        ask_init();
    }
}

void inc_param_num()
{
    selected_fxn->inc_param_num_by(1);
}

void dec_param_num()
{
    selected_fxn->inc_param_num_by(-1);
}

void inc_dig_num()
{
    selected_fxn->inc_dig_num_by(1);

    if (in_output_fxn() && (the_output)().param_num == OUTPUT_WAVEFORM)
    {
        graph_waveform(selected_output.get());
    }

    if (in_user_waveform_fxn() && selected_fxn->param_num == USER_WAVEFORMS_NAME)
    {
        user_waveforms_update_waveform();
    }
}

void dec_dig_num()
{
    selected_fxn->inc_dig_num_by(-1);
    if ((in_output_fxn() && (the_output)().param_num == OUTPUT_WAVEFORM) || (in_user_waveform_fxn() && selected_fxn->param_num == USER_WAVEFORMS_NAME))
    {
        selected_fxn->printParams();
    }
}

void new_output(SPANK_fxn *output)
{
    selected_fxn = output;
    apply_params_to_waveform();
    selected_fxn->display();
}

void gen_params_macro(SPANK_fxn *item, bool print_header = true)
{
    if (print_header)
    {
        ui.t.clrScreen();
        ui.terminal_debug("// Macro for: " + item->name);
    }
    uint8_t param_type;
    for (int i = 0; i < item->num_params; i++)
    {
        param_type = item->get_param_type(i);
        // Serial.println(item->labels[i] + String(item->get_param_w_offset(i)) + " type: " + String(param_type));
        switch (param_type)
        {
        case SPANK_INT_PARAM_TYPE:
        case SPANK_STRING_PARAM_TYPE:
            Serial.println("p" + String(i));
            Serial.println(String(item->get_param_w_offset(i)));
            break;
        case SPANK_STRING_VAR_TYPE:
            Serial.println("p" + String(i));
            Serial.println("$" + String(item->get_param_as_string_var(i)));
            break;
        }
    }
}
