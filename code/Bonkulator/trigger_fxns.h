// Trigger functions

void clear_trigger(int output_num, int trig_num)
{
    OutputData *outptr = &outputs[output_num];
    (*bonk_outputs[output_num]).clear_trigger(trig_num);
    outptr->repeat_count = (*bonk_outputs[output_num]).get_param(OUTPUT_REPEAT);
    outptr->next_time = (*bonk_outputs[output_num]).get_param(OUTPUT_INIT_DELAY) + millis();
}

void clear_outputs(int trig_num)
{
    for (int output_num = 0; output_num < NUM_OUTPUTS; output_num++)
    {
        clear_trigger(output_num, trig_num);
    }
}

void select_trigger(int int_param)
{
    switch (int_param)
    {
    case 0:
        selected_trigger = &trig0;
        break;
    case 1:
        selected_trigger = &trig1;
        break;
    case 2:
        selected_trigger = &trig2;
        break;
    case 3:
        selected_trigger = &trig3;
        break;
    }
    selected_fxn->put_param_num(selected_trigger->trig_num + OUTPUT_ENABLE_T0);
    // selected_fxn->printParams();
}

void clear_all_triggers()
{
    int trigger_save = selected_trigger->trig_num;
    for (int i = 0; i < NUM_TRIGGERS; i++)
    {
        clear_outputs(i);
        // select_trigger(i);
        (*triggers[i]).clear();
    }
    select_trigger(trigger_save);
}

bool check_any_triggers()
{
    bool triggered = false;
    for (int i = 0; i < NUM_TRIGGERS; i++)
    {
        if ((*triggers[i]).state == TRIGGER_ACTIVE)
        {
            triggered = true;
            // (*triggers[i]).state = TRIGGER_IDLE;
            (*triggers[i]).clear();
        }
    }
    return triggered;
}

String check_output_triggers()
{
    String out = "";
    String t = "     ";
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
        if ((*bonk_outputs[i]).get_param(OUTPUT_CLOCK) == 1)
        {
            t = " X" + String(bonk_output(i).triggered_by) + "  ";
        }
        else
        {
            t = "     ";
            if ((*bonk_outputs[i]).triggered)
            {
                t = " T" + String(bonk_output(i).triggered_by) + "  ";
            }
        }
        out += String(i) + ":" + t;
    }
    return out;
}

void trigger0()
{
    trig0.trigger();
}

void trigger1()
{
    trig1.trigger();
}

void trigger2()
{
    trig2.trigger();
}

void trigger3()
{
    trig3.trigger();
}

void trigger_output(byte trig_num, int output_num)
{
    OutputData *outptr = &outputs[output_num];
    (*bonk_outputs[output_num]).trigger(trig_num);
    if ((*bonk_outputs[output_num]).triggered || (*bonk_outputs[output_num]).get_param(OUTPUT_WAVEFORM) == WAVEFORM_TOGGLE)
    {
        outptr->repeat_count = (*bonk_outputs[output_num]).get_param(OUTPUT_REPEAT);
        outptr->next_time = (*bonk_outputs[output_num]).get_param(OUTPUT_INIT_DELAY) + millis();
    }
    else
    {
        outptr->current_index = 0;
        send_idle_value(output_num);
    }
    // set led
    digitalWrite(trig_num, (*bonk_outputs[output_num]).triggered);
}

void trigger_report()
{
    ui.t.clrDown("15");
    ui.terminal_printRow("Trigger Report", "16");
    Serial.println(F("Output\tT0\tT1\tT2\tT3\tTRIG"));
    ui.t.printChars(48, "-");
    Serial.print("\r\n");
    // Serial.println(F("----------------------------------"));
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
        Serial.print(String(i));
        Serial.print("\t");
        Serial.print(String((*bonk_outputs[i]).get_param(OUTPUT_ENABLE_T0)));
        Serial.print("\t");
        Serial.print(String((*bonk_outputs[i]).get_param(OUTPUT_ENABLE_T1)));
        Serial.print("\t");
        Serial.print(String((*bonk_outputs[i]).get_param(OUTPUT_ENABLE_T2)));
        Serial.print("\t");
        Serial.print(String((*bonk_outputs[i]).get_param(OUTPUT_ENABLE_T3)));
        Serial.print("\t");
        Serial.println(String((*bonk_outputs[i]).triggered));
    }
}

void triggers_begin()
{
    attachInterrupt(digitalPinToInterrupt(T0_PIN), trigger0, RISING);
    attachInterrupt(digitalPinToInterrupt(T1_PIN), trigger1, RISING);
    attachInterrupt(digitalPinToInterrupt(T2_PIN), trigger2, RISING);
    attachInterrupt(digitalPinToInterrupt(T3_PIN), trigger3, RISING);
}
