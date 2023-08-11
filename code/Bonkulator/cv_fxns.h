// cv functions
void update_cv(int cv_num)
{
    // this clears mods that were applied -- so we can start fresh
    OutputData *outptr = &outputs[selected_output.get()];

    outptr->offset = 0;
    outptr->scale = 1.0;
    update_idle_value();
    // update_waveform(); done by output_display()

    int cv_type = selected_fxn->get_param(cv_num);
    // ui.terminal_debug("Update CV: " + String(cv_type));
    switch (cv_num)
    {
    case OUTPUT_CV0:
        trig_cv0.set_output(selected_output.get(), cv_type == 6);
        break;
    case OUTPUT_CV1:
        trig_cv1.set_output(selected_output.get(), cv_type == 6);
        break;
    }
    output_display();
}

// condition cv from +/- 1650 (3300/2)
int16_t condition_cv(int16_t cv_val, unsigned int cv_num)
{
    float xn;
    float yn;
    static float xn0;
    static float yn0;
    static float xn1;
    static float yn1;
    float *x = cv_num == OUTPUT_CV0 ? &xn0 : &xn1;
    float *y = cv_num == OUTPUT_CV0 ? &yn0 : &yn1;

    cv_val *= 1.2412; // ads1015 full scale = 4.096. System FS = 3.3V, multiply by ratio
    xn = cv_val;
    yn = 0.969 * *y + 0.0155 * xn + 0.0155 * *x;
    // yn = (xn + xn1) / 2;
    *x = xn;
    *y = yn;
    return cv_val + 2048;

    // cv_val = constrain(cv_val, -2048, 2047);
    // cv_val += 2048;
    // return cv_val;
}

// scales a float value by adjusted linear. Range of adj: param/100 -> param*100, cv of ADC_FS_OVER_2 returns param*1
float cv_scale_pow(int cv, float param)
{
    float temp = ((float)cv / ADC_FS_OVER_4) - 2.0;
    return pow(10, temp) * param;
}

// simple scale -- 0 through 1*param
// (5/3.32 * 1241.2) + 2048 = 3917.247
// (5/3.01 * 1241.2) + 2048 = 4109.794
#define CV_FS 3917.247
float cv_scale(int cv, float param)
{
    // return ((float)cv / (float)ADC_FS) * param;
    return ((float)cv / CV_FS) * param;
}

int cv_time_inc(int cv, float param)
{
    int val = cv_scale_pow(cv, param);
    return constrain(val, TIME_INC_MIN, TIME_INC_MAX);
}

int cv_active_time(int cv, float param, int output)
{
    int val = cv_scale_pow(cv, param);
    val = constrain(val, ACTIVE_TIME_MIN, MAX_MS);
    // outputs[output].active_time = val;
    // set_waveform(output, (*bonk_outputs[output]).get_param(OUTPUT_WAVEFORM));
    // update_waveform();
    // ui.terminal_debug("CV: " + String(cv) + " Val: " + String(val) + " Output: " + String(output));
    return val;
}

int16_t cv_offset(int16_t cv_val)
{
    // status_string = "CV1: " + String(cv_val) + " ADC1: " + String(adc1);
    // ui.terminal_debug("cv_val: " + String(cv_val) + " ratio: " + String((float)cv_val / ADC_FS));
    return (((float)cv_val / ADC_FS) * 2 * DAC_FS) - DAC_FS;
    // return (((float)cv_val / CV_FS) * 2 * DAC_FS) - DAC_FS;
    // return cv_scale(cv_val, 2 * DAC_FS) - DAC_FS;
}

bool sole_cv(int cv_num, int cv_type, Greenface_gadget bonk_output)
{
    return ((cv_num == OUTPUT_CV0 && bonk_output.get_param(OUTPUT_CV1) != cv_type) || (cv_num == OUTPUT_CV1 && bonk_output.get_param(OUTPUT_CV0) != cv_type));
}

// sets output data with appropriate cv modifier value
void cv_set(int cv_num, int output, int16_t cv_val)
{

    cv_val = condition_cv(cv_val, cv_num);

    // if (cv_num == OUTPUT_CV1)
    //     ui.terminal_debug("cv_val: " + String(cv_val) + " cv_num: " + String(cv_num) + " output: " + String(output));

    OutputData *outptr = &outputs[output];
    Greenface_gadget bonk_output = (*bonk_outputs[output]);
    byte cv_type = bonk_output.get_param(cv_num);

    if (cv_type > 0)
    {
        int ui_active_time;
        int16_t *cv_mem;
        switch (cv_num)
        {
        case 0:
            cv_mem = &outptr->cv0_val[output];
            break;
        case 1:
            cv_mem = &outptr->cv1_val[output];
            break;
        default:
            cv_mem = &outptr->cv0_val[output];
        }

        if (abs(cv_val - *cv_mem) > 2)
        {
            // Serial.println("Active time: " + String(outptr->active_time));
            // Serial.println("CV val: " + String(cv_val) + " mem: " + String(*cv_mem));
            // int16_t toffset;
            static float temp_offset;
            static float temp_scale;
            static uint16_t temp_active_time;

            // now set value
            switch (cv_type)
            {
            case CV_SCALE:
                if (sole_cv(cv_num, CV_SCALE, bonk_output))
                {
                    outptr->scale = cv_scale_pow(cv_val, 1.0);
                }
                else
                {
                    switch (cv_num)
                    {
                    case OUTPUT_CV0:
                        temp_scale = cv_scale_pow(cv_val, 1.0);
                        break;
                    case OUTPUT_CV1:
                        outptr->scale = cv_scale_pow(cv_val, temp_scale);
                        break;
                    }
                }
                break;
            case CV_OFFSET:
                if (sole_cv(cv_num, CV_OFFSET, bonk_output))
                {
                    outptr->offset = cv_offset(cv_val);
                    // ui.terminal_debug("CV Val: " + String(cv_val) + " Offset: " + String(outptr->offset));
                }
                else
                {
                    switch (cv_num)
                    {
                    case OUTPUT_CV0:
                        temp_offset = cv_offset(cv_val);
                        break;
                    case OUTPUT_CV1:
                        outptr->offset = cv_offset(cv_val) + temp_offset;
                        break;
                    }
                }
                break;
            case CV_ACTIVE_TIME:
                ui_active_time = (*bonk_outputs[output]).get_param(OUTPUT_ACTIVE_TIME);
                if (sole_cv(cv_num, CV_ACTIVE_TIME, bonk_output))
                {
                    outptr->active_time = cv_active_time(cv_val, (float)ui_active_time, output);
                }
                else
                {
                    Serial.println();
                    switch (cv_num)
                    {
                    case OUTPUT_CV0:
                        temp_active_time = cv_active_time(cv_val, (float)ui_active_time, output);
                        break;
                    case OUTPUT_CV1:
                        outptr->active_time = cv_active_time(cv_val, (float)temp_active_time, output);
                        break;
                    }
                }
                set_waveform(output, (*bonk_outputs[output]).get_param(OUTPUT_WAVEFORM));
                break;
            case CV_IDLE_VALUE:
                set_idle_value(cv_val, output);
                break;
            case CV_RANDOMNESS:
                set_randomness_factor(cv_val * 100.0 / ADC_FS, bonk_output.get_param(OUTPUT_SCALE) / 100.0, output);
                break;
            case CV_TRIGGER:
                switch (cv_num)
                {
                case OUTPUT_CV0:
                    trig_cv0.trigger();
                    break;
                case OUTPUT_CV1:
                    trig_cv1.trigger();
                    break;
                }
                // trigger_output(4, output);
                // ui.terminal_debug("CV Trigger: " + String(bonk_output.triggered) + " ms " + String(millis()));
                break;
            }
        }

        *cv_mem = cv_val;
    }
}

// this is an interrupt fxn!
// called by TimerHandler
void apply_cv(int output_num, int cv_num, OutputData *outptr)
{
    int cv_type = (*bonk_outputs[output_num]).get_param(cv_num);
    int val = outptr->output_value; // using int gives us headroom for math
    int offset_adjust = DAC_FS * (1 - outptr->scale) / 2;

    switch (cv_type)
    {
    case CV_SCALE:
        val *= outptr->scale;
        val += offset_adjust;
        outptr->output_value = constrain(val, 0, DAC_FS);
        break;
    case CV_OFFSET:
        val += outptr->offset;
        outptr->output_value = constrain(val, 0, DAC_FS);
        break;
    }
}
