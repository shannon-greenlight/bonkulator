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
    if (e.is_adjusting())
    {
        reset_inactivity_timer();
        // ui.terminal_debug("Rotary: " + String(e.getEncoderValue()));
        adjust_param(e.getEncoderValue());
        e.resetEncoder();
    }
}

void activate()
{
    if (in_settings())
    {
        if (settings_fxn.param_num == SETTINGS_WIFI)
        {
            select_wifi_fxn();
            select_wifi_ok();
        }
    }
}

uint16_t get_param(int p_num)
{
    if (in_wifi)
    {
        return wifi_get_param(p_num);
    }
    else
    {
        return selected_fxn->get_param(p_num);
    }
}

int inc_dig_num()
{
    if (in_wifi)
    {
        wifi_inc_dig_num_by(1);
        select_wifi_fxn();
    }
    else
    {
        selected_fxn->inc_dig_num_by(1);
    }
}

int dec_dig_num()
{
    if (in_wifi)
    {
        wifi_inc_dig_num_by(-1);
        select_wifi_fxn();
    }
    else
    {
        selected_fxn->inc_dig_num_by(-1);
    }
}
