// Trigger functions

// trigger is "private" to trigger_outputs
void trigger(int output_num, int trig_num)
{
    // digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    OutputData *outptr = &outputs[output_num];
    bool enabled = false;
    int led_pin;
    switch (trig_num)
    {
    case 0:
        enabled = (*bonk_outputs[output_num]).get_param(OUTPUT_ENABLE_T0);
        if (enabled)
            led_pin = T0_LED;
        break;
    case 1:
        enabled = (*bonk_outputs[output_num]).get_param(OUTPUT_ENABLE_T1);
        if (enabled)
            led_pin = T1_LED;
        break;
    case 2:
        enabled = (*bonk_outputs[output_num]).get_param(OUTPUT_ENABLE_T2);
        if (enabled)
            led_pin = T2_LED;
        break;
    case 3:
        enabled = (*bonk_outputs[output_num]).get_param(OUTPUT_ENABLE_T3);
        if (enabled)
            led_pin = T3_LED;
        break;
    }
    if (enabled)
    {
        (*bonk_outputs[output_num]).trigger(led_pin);
        outptr->repeat_count = (*bonk_outputs[output_num]).get_param(OUTPUT_REPEAT);
        outptr->next_time = (*bonk_outputs[output_num]).get_param(OUTPUT_DELAY) + millis();
        if (!(*bonk_outputs[output_num]).triggered)
        {
            outptr->current_index = 0;
            set_idle_value(output_num);
            // dac_out(output_num, (*bonk_outputs[output_num]).get_param(OUTPUT_DACVAL) + _offset_corrections[output_num]);
        }
        digitalWrite(led_pin, (*bonk_outputs[output_num]).triggered);
    }
}

void trigger_outputs(int trig_num)
{
    long msecs = millis();
    for (int output_num = 0; output_num < NUM_OUTPUTS; output_num++)
    {
        OutputData *outptr = &outputs[output_num];
        trigger(output_num, trig_num);
    }
}

void clear_trigger(int output_num, int trig_num)
{
    OutputData *outptr = &outputs[output_num];
    (*bonk_outputs[output_num]).clear_trigger(trig_num);
    outptr->repeat_count = (*bonk_outputs[output_num]).get_param(OUTPUT_REPEAT);
    outptr->next_time = (*bonk_outputs[output_num]).get_param(OUTPUT_DELAY) + millis();
}

void clear_outputs(int trig_num)
{
    for (int output_num = 0; output_num < NUM_OUTPUTS; output_num++)
    {
        clear_trigger(output_num, trig_num);
    }
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

void test_trig0()
{
    trig0.start();
}

void test_trig1()
{
    trig1.start();
}

void test_trig2()
{
    trig2.start();
}

void test_trig3()
{
    trig3.start();
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
        // selected_trigger->clear();
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
            (*triggers[i]).state = TRIGGER_IDLE;
        }
    }
    return triggered;
}

String check_output_triggers()
{
    for (int i = 0; i < NUM_TRIGGERS; i++)
    {
        if ((*triggers[i]).state == TRIGGER_ACTIVE)
        {
            trigger_outputs(i);
            (*triggers[i]).state = TRIGGER_IDLE;
        }
    }
    String out = "";
    for (int i = 0; i < NUM_OUTPUTS; i++)
    {
        uint8_t trig_num = bonk_output(i).triggered_by;
        String t = " T" + String(trig_num) + "  ";
        out += String(i) + ":" + (bonk_output(i).triggered ? t : "     ");
    }
    return out;
}

void triggers_begin()
{
    attachInterrupt(digitalPinToInterrupt(T0_PIN), trigger0, RISING);
    attachInterrupt(digitalPinToInterrupt(T1_PIN), trigger1, RISING);
    attachInterrupt(digitalPinToInterrupt(T2_PIN), trigger2, RISING);
    attachInterrupt(digitalPinToInterrupt(T3_PIN), trigger3, RISING);
}