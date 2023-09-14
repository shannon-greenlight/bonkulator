// waveform fxns
#include <Wire.h>

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

void apply_params_to_waveform(int output)
{
    // int output = selected_output.get();
    float scale = (the_output)().get_param_w_offset(OUTPUT_SCALE);
    int offset = (the_output)().get_param_w_offset(OUTPUT_OFFSET);
    int randomness = (the_output)().get_param(OUTPUT_RANDOMNESS);
    int wave_type = (the_output)().get_param(OUTPUT_WAVEFORM);

    float scale_correction = output_scale_corrections.get(output) / 1000.;
    int offset_correction = output_offset_corrections.get(output);

    uint16_t *waveform_data = outputs[output].waveform_data;
    uint16_t *waveform_ref = waveform_reference[output];
    byte data_len = outputs[output].data_len;

    float scale_factor;
    // int offset_factor = offset - (DAC_FS + 1) + output_offset_corrections.get(output);
    int offset_factor;
    int offset_adjust;

    switch (wave_type)
    {
    case WAVEFORM_PULSE:
        scale_factor = scale / 200;
        scale_factor += .5;
        offset_factor = offset_adjust = 0;
        // set_randomness_factor(randomness, abs(scale - 100) / 100.0, output);
        break;
    default:
        scale_factor = scale / 100;
        // offset_factor = ((offset + OUTPUT_OFFSET_OFFSET) / 100.0) * (DAC_FS + 1);
        offset_factor = (offset / 100.0) * (DAC_FS + 1);
        offset_adjust = DAC_FS * (100 - scale) / 200;
        // set_randomness_factor(randomness, scale_factor, output);
        break;
    }

    set_randomness_factor(randomness, abs(scale) / 100.0, output);

    // ui.terminal_debug("Apply params -- Offset factor: " + String(offset_factor) + " offset: " + String(offset));
    // ui.terminal_debug("Apply params -- Randomness factor: " + String(randomness_factor) + " int: " + String((int)randomness_factor));
    // ui.terminal_debug("Apply params -- Scale factor: " + String(scale_factor) + " scale: " + String((int)scale));
    // ui.terminal_debug("Apply params -- Output: " + String(output) + " offset corr: " + String(offset_correction));

    int temp;

    for (int i = 0; i < data_len; i++)
    {
        temp = waveform_ref[i] * scale_factor;
        temp *= scale_correction;
        temp += offset_factor;
        temp += offset_adjust;
        temp += offset_correction;
        waveform_data[i] = constrain(temp, 0, DAC_FS);
    }
}

void recv_user_waveform(String in_str)
{
    // set index
    int waveform_num = get_user_waveform_num();
    (*user_waveforms[waveform_num]).param_put((csv_elem(in_str, ',', 0)).substring(1).toInt(), USER_WAVEFORMS_INDEX);
    for (int i = 0; i < WAVEFORM_PARTS; i++)
    {
        user_waveforms_put_val(waveform_num, (csv_elem(in_str, ',', i + 1)).toInt(), i);
    }
}

void set_wifi_message()
{
    // OutputData *outptr;
    // uint16_t *waveform_ref = waveform_reference[selected_output.get()];
    uint16_t *waveform_data;
    byte data_len;
    OutputData *outptr = &outputs[selected_output.get()];

    if (in_output_fxn())
    {
        waveform_data = outptr->waveform_data;
        // waveform_data = waveform_ref;
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

    int offset_correction = output_offset_corrections.get(selected_output.get());
    ui.terminal_debug("offset_correction: " + String(offset_correction));
    int index = 0; // index into source waveform data
    float index_inc = WAVEFORM_PARTS / float(data_len);
    uint16_t value;
    // int randomness_factor = (the_output)().get_param(OUTPUT_RANDOMNESS);

    for (int i = 0; i < WAVEFORM_PARTS; i++)
    {
        if (i > 0 && i != WAVEFORM_PARTS)
            waveform_values += ", ";

        value = waveform_data[index] - offset_correction;
        if (!in_user_waveforms())
            value = randomize_value(value, outptr->randomness_factor);
        value = quantize_value(value, selected_output.get());
        waveform_values += String(value);

        if (i + 1 >= (index + 1) * index_inc)
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
    // uint16_t active_time = 268;
    uint16_t active_time = (outputs[output]).active_time;
    // uint16_t active_time = (*bonk_outputs[output]).get_param(OUTPUT_ACTIVE_TIME);
    uint16_t leftover = active_time % WAVEFORM_PARTS;
    float ratio = (float)active_time / WAVEFORM_PARTS;
    if (leftover == 0)
    {
        calculated_time_inc = outputs[output].time_inc = active_time / WAVEFORM_PARTS;
        return WAVEFORM_PARTS;
    }
    else if (ratio < 1.0)
    {
        calculated_time_inc = outputs[output].time_inc = 1;
        return active_time;
    }
    else
    {
        calculated_time_inc = outputs[output].time_inc = (int)ratio * 2;
        return active_time / outputs[output].time_inc;
    }
}

uint16_t get_time_inc(int output)
{
    return outputs[output].time_inc;
    // return (*bonk_outputs[output]).get_param(OUTPUT_TIME_INC);
}

int calc_wave_value(int i, int waveform, uint16_t waveform_parts)
{
    int temp;
    int pyramid_index;
    switch (waveform)
    {
    case WAVEFORM_SINE:
        temp = (sin((6.28319 * (float)i / waveform_parts)) * DAC_MID) + DAC_MID;
        break;
    case WAVEFORM_HAYSTACK:
        temp = (sin(3.14159 * (float)i / waveform_parts) * DAC_FS);
        break;
    case WAVEFORM_RAMP:
        temp = i * (DAC_FS + 1) / waveform_parts;
        break;
    case WAVEFORM_PYRAMID:
        pyramid_index = (i < waveform_parts / 2) ? i : waveform_parts - i;
        temp = 2 * pyramid_index * (DAC_FS + 1) / waveform_parts;
        break;
    case WAVEFORM_PULSE:
    case WAVEFORM_TOGGLE:
        temp = DAC_FS;
        break;
    case WAVEFORM_MAYTAG:
        temp = random(0, DAC_FS);
        break;
    }
    return temp;
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

    // Serial.println("Set Waveform output: " + String(output) + " waveform: " + String(waveform));
    // Serial.println("Set Waveform user_inc_factor: " + String(user_inc_factor) + " parts: " + String(waveform_parts) + " userlen: " + String(user_waveform.length()) + " active time: " + String(outputs[output].active_time));
    // int pyramid_index;
    for (int i = 0; i < waveform_parts; i++)
    {
        if (waveform > WAVEFORM_MAYTAG) // is this a user wave?
        {
            waveform_data[i] = waveform_ref[i] = user_waveform.get(user_inc_factor * i);
        }
        else
        {
            waveform_data[i] = waveform_ref[i] = calc_wave_value(i, waveform, waveform_parts);
        }
        // switch (waveform)
        // {
        // case WAVEFORM_SINE:
        //     temp = (sin((6.28319 * (float)i / waveform_parts)) * DAC_MID) + DAC_MID;
        //     break;
        // case WAVEFORM_HAYSTACK:
        //     temp = (sin(3.14159 * (float)i / waveform_parts) * DAC_FS);
        //     break;
        // case WAVEFORM_RAMP:
        //     temp = i * (DAC_FS + 1) / waveform_parts;
        //     break;
        // case WAVEFORM_PYRAMID:
        //     pyramid_index = (i < waveform_parts / 2) ? i : waveform_parts - i;
        //     temp = 2 * pyramid_index * (DAC_FS + 1) / waveform_parts;
        //     break;
        // case WAVEFORM_PULSE:
        // case WAVEFORM_TOGGLE:
        //     temp = DAC_FS;
        //     break;
        // case WAVEFORM_MAYTAG:
        //     temp = random(0, DAC_FS);
        //     break;
        // default:
        //     temp = user_waveform.get(user_inc_factor * i);
        //     break;
        // }

        // waveform_data[i] = waveform_ref[i] = temp;
        // Serial.println(temp);
    }
    // dump_waveform(selected_output.get(), true);
    apply_params_to_waveform(output);
    // graph_waveform(selected_output.get());
}
