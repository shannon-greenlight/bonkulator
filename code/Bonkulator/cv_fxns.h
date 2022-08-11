// cv functions
void update_cv(int cv_num)
{
    // this clears mods that were applied -- so we can start fresh
    OutputData *outptr = &outputs[selected_output.get()];

    outptr->offset = 0;
    outptr->scale = 1.0;
    update_waveform();
}

// condition cv from +/- 1650 (3300/2)
int16_t condition_cv(int16_t cv_val)
{
    cv_val *= 1.2412; // ads1015 full scale = 4.096. System FS = 3.3V, multiply by ratio
    cv_val = constrain(cv_val, -2048, 2047);
    cv_val += 2048;
    return cv_val;
}

// scales a float value by adjusted linear. Range of adj: param/100 -> param*100, cv of ADC_FS_OVER_2 returns param*1
float cv_scale_pow(int cv, float param)
{
    float temp = ((float)cv / ADC_FS_OVER_4) - 2.0;
    return pow(10, temp) * param;
}

// simple scale -- 0 through 1*param
// (5/3.32 * 1.2412) + 2048 = 3817.2
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

int cv_period(int cv, float param, SPANK_fxn bonk_output)
{
    int val = cv_scale_pow(cv, param);
    return constrain(val, PERIOD_MIN, PERIOD_MAX);
}

int16_t cv_offset(int16_t cv_val)
{
    // status_string = "CV1: " + String(cv_val) + " ADC1: " + String(adc1);
    return (((float)cv_val / ADC_FS) * 2 * DAC_FS) - DAC_FS;
    // return (((float)cv_val / CV_FS) * 2 * DAC_FS) - DAC_FS;
    // return cv_scale(cv_val, 2 * DAC_FS) - DAC_FS;
}

bool sole_cv(int cv_num, int cv_type, SPANK_fxn bonk_output)
{
    return ((cv_num == OUTPUT_CV0 && bonk_output.get_param(OUTPUT_CV1) != cv_type) || (cv_num == OUTPUT_CV1 && bonk_output.get_param(OUTPUT_CV0) != cv_type));
}

// sets output data with appropriate cv modifier value
void cv_set(int cv_num, int output, int16_t cv_val)
{

    cv_val = condition_cv(cv_val);
    // if (cv_num == OUTPUT_CV0)
    //   ui.terminal_debug("cv_val: " + String(cv_val));

    // cv_val *= 1.2412; // ads1015 full scale = 4.096. System FS = 3.3V, multiply by ratio
    // cv_val = constrain(cv_val, -2048, 2047);
    // cv_val += 2048;
    // cv_val = ADC_FS - cv_val;

    OutputData *outptr = &outputs[output];
    SPANK_fxn bonk_output = (*bonk_outputs[output]);
    byte cv_type = bonk_output.get_param(cv_num);

    // int16_t toffset;
    static float temp_offset;
    static float temp_scale;
    static uint16_t temp_time_inc;

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
    case CV_PERIOD:
        if (sole_cv(cv_num, CV_PERIOD, bonk_output))
        {
            outptr->time_inc = cv_time_inc(cv_val, (float)calculated_time_inc);
        }
        else
        {
            switch (cv_num)
            {
            case OUTPUT_CV0:
                temp_time_inc = cv_time_inc(cv_val, (float)calculated_time_inc);
                break;
            case OUTPUT_CV1:
                outptr->time_inc = cv_time_inc(cv_val, temp_time_inc);
                break;
            }
        }
        break;
    }
}

// this is an interrupt fxn!
// called by TimerHandler
void apply_cv(int output_num, int cv_num, OutputData *outptr)
{
    int cv_type = (*bonk_outputs[output_num]).get_param(cv_num);
    int val = outptr->output_value; // using int givces us headroom for math
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
