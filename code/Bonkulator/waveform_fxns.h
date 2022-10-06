// waveform fxns
void dump_waveform(int output, bool dump_ref)
{
    byte data_len = outputs[output].data_len;
    uint16_t *waveform_data;
    if (dump_ref)
    {
        waveform_data = waveform_reference[output];
    }
    else
    {
        waveform_data = outputs[output].waveform_data;
    }

    Serial.println("");
    if (dump_ref)
    {
        Serial.print("Reference ");
    }
    Serial.println("Wave data for Output: " + String(output));
    for (int i = 0; i < data_len; i++)
    {
        if (i % 8 == 0)
        {
            Serial.println();
        }
        Serial.print(waveform_data[i]);
        Serial.print(" ");
    }
    Serial.println("");
}

// void randomize_waveform(int output, int randomness)
// {
//   uint16_t *waveform_data = outputs[output].waveform_data;
//   byte data_len = outputs[output].data_len;
//   for (int i = 0; i < data_len; i++)
//   {
//   }
// }

void correct_ref_waveform(int output)
{
    // int output = selected_output.get();

    // uint16_t *waveform_data = outputs[output].waveform_data;
    uint16_t *waveform_ref = waveform_reference[output];
    byte data_len = outputs[output].data_len;

    float scale_factor = output_scale_corrections.get(output) / 1000.;
    int offset_factor = output_offset_corrections.get(output);
    // int offset_adjust = DAC_MID - DAC_MID * scale_factor;

    // Serial.println("Scale: " + String(output_scale_corrections.get(output)));
    // Serial.println("Scale Factor: " + String(scale_factor));
    // Serial.println("Offset Factor: " + String(offset_factor));
    // Serial.println("Offset Adjust: " + String(offset_adjust));
    // Serial.println("Offset output: " + String(output));
    // Serial.println();
    // return;

    int temp;

    for (int i = 0; i < data_len; i++)
    {
        temp = ((waveform_ref[i] - DAC_MID) * scale_factor) + .5 + DAC_MID;
        temp += offset_factor;
        // temp += offset_adjust;

        temp = constrain(temp, 0, DAC_FS); // constrain output
        // temp = max(0, temp);      // constrain output
        // temp = min(DAC_FS, temp); // constrain output
        waveform_ref[i] = temp;
    }
}

void apply_params_to_waveform(int output)
{
    // int output = selected_output.get();
    float scale = (the_output)().get_param(OUTPUT_SCALE);
    int offset = (the_output)().get_param(OUTPUT_OFFSET);
    int randomness = (the_output)().get_param(OUTPUT_RANDOMNESS);

    uint16_t *waveform_data = outputs[output].waveform_data;
    uint16_t *waveform_ref = waveform_reference[output];
    byte data_len = outputs[output].data_len;

    float scale_factor = scale / 100;
    // int offset_factor = offset - (DAC_FS + 1) + output_offset_corrections.get(output);
    int offset_factor = ((offset + OUTPUT_OFFSET_OFFSET) / 100.0) * (DAC_FS + 1);
    int offset_adjust = DAC_FS * (100 - scale) / 200;

    set_randomness_factor(randomness, scale_factor, output);

    // ui.terminal_debug("Apply params -- Offset factor: " + String(offset_factor) + " offset: " + String(offset));
    // ui.terminal_debug("Apply params -- Randomness factor: " + String(randomness_factor) + " int: " + String((int)randomness_factor));

    int temp;

    for (int i = 0; i < data_len; i++)
    {
        temp = waveform_ref[i] * scale_factor;
        temp += offset_factor;
        temp += offset_adjust;

        waveform_data[i] = constrain(temp, 0, DAC_FS);
    }
}

void set_wifi_message()
{
    // OutputData *outptr;
    uint16_t *waveform_data;
    byte data_len;
    OutputData *outptr = &outputs[selected_output.get()];

    if (in_output_fxn())
    {
        waveform_data = outptr->waveform_data;
        data_len = outptr->data_len;
    }
    else if (in_user_waveform_fxn())
    {
        waveform_data = _user_waveforms_data[get_user_waveform_num()];
        data_len = WAVEFORM_PARTS;
    }
    else
    {
        return;
    }

    int index = 0; // index into source waveform data
    float index_inc = WAVEFORM_PARTS / float(data_len);
    uint16_t value;
    // int randomness_factor = (the_output)().get_param(OUTPUT_RANDOMNESS);

    for (int i = 0; i < WAVEFORM_PARTS; i++)
    {
        if (i > 0)
            wifi_ui_message += ", ";

        value = waveform_data[index];
        value = randomize_value(value, outptr->randomness_factor);
        value = quantize_value(value, selected_output.get());
        wifi_ui_message += String(value);

        if (i + 1 > (index + 1) * index_inc)
        {
            index++;
        }
    }
}

void graph_waveform(int output_num)
{
    OutputData *outptr = &outputs[selected_output.get()];
    uint16_t *input_waveform = outputs[output_num].waveform_data;
    uint16_t waveform_data[WAVEFORM_PARTS];
    for (int i = 0; i < outptr->data_len; i++)
    {
        waveform_data[i] = input_waveform[i];
        waveform_data[i] = randomize_value(waveform_data[i], outptr->randomness_factor);
        waveform_data[i] = quantize_value(waveform_data[i], selected_output.get());
    }
    // ui.terminal_debug("Graph waveform - len: " + String(outputs[output_num].data_len));
    ui.graph_waveform(waveform_data, outputs[output_num].data_len, DAC_FS);
}

uint16_t get_waveform_parts(int output)
{
    // uint16_t period = 268;
    uint16_t period = (outputs[output]).period;
    // uint16_t period = (*bonk_outputs[output]).get_param(OUTPUT_PERIOD);
    uint16_t leftover = period % WAVEFORM_PARTS;
    float ratio = (float)period / WAVEFORM_PARTS;
    if (leftover == 0)
    {
        calculated_time_inc = outputs[output].time_inc = period / WAVEFORM_PARTS;
        return WAVEFORM_PARTS;
    }
    else if (ratio < 1.0)
    {
        calculated_time_inc = outputs[output].time_inc = 1;
        return period;
    }
    else
    {
        calculated_time_inc = outputs[output].time_inc = (int)ratio * 2;
        return period / outputs[output].time_inc;
    }
}

uint16_t get_time_inc(int output)
{
    return outputs[output].time_inc;
    // return (*bonk_outputs[output]).get_param(OUTPUT_TIME_INC);
}

void set_waveform(int output, int waveform)
{
    // temp fix
    // waveform = min(5, waveform);

    int temp;
    uint16_t *waveform_ref = waveform_reference[output];
    uint16_t *waveform_data = outputs[output].waveform_data;
    uint16_t waveform_parts = get_waveform_parts(output); // also sets time_inc

    int user_wavenum = waveform - NUM_STD_WAVEFORMS;
    EEPROM_Arr16 user_waveform = (*user_waveforms_data[user_wavenum]);

    outputs[output].data_len = waveform_parts;
    float user_inc_factor = (WAVEFORM_PARTS / (float)waveform_parts);

    // ui.terminal_debug("Set Waveform output: " + String(output) + " waveform: " + String(waveform));
    // ui.terminal_debug("Set Waveform user_inc_factor: " + String(user_inc_factor) + " parts: " + String(waveform_parts) + " userlen: " + String(user_waveform.length()));

    for (int i = 0; i < waveform_parts; i++)
    {
        switch (waveform)
        {
        case WAVEFORM_SINE:
        case WAVEFORM_INV_SINE:
            temp = (sin(6.28319 * (float)i / waveform_parts) * 512) + 512;
            break;
        case WAVEFORM_RAMP_UP:
        case WAVEFORM_RAMP_DN:
            temp = i * (DAC_FS + 1) / waveform_parts;
            break;

        case WAVEFORM_PULSE:
        case WAVEFORM_TOGGLE:
            temp = DAC_FS;
            break;
        case WAVEFORM_INV_PULSE:
            temp = 0;
            break;
        default:
            temp = user_waveform.get(user_inc_factor * i);
            break;
        }

        // deal with inverse waveforms
        switch (waveform)
        {
        case WAVEFORM_INV_SINE:
        case WAVEFORM_RAMP_DN:
            temp = DAC_FS - temp;
            break;

        default:
            break;
        }

        waveform_data[i] = waveform_ref[i] = temp;
        // Serial.println(temp);
    }
    // dump_waveform(selected_output.get(), true);
    correct_ref_waveform(output);
    // dump_waveform(selected_output.get(), true);
    apply_params_to_waveform(output);
    // graph_waveform(selected_output.get());
}
