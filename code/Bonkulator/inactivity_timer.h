// inactivity timer routines
// todo maybe make this stuff a ui method

void reset_inactivity_timer()
{
    inactivity_timer = 0;
}

void inc_inactivity_timer()
{
    if ((inactivity_timer < settings_get_inactivity_timeout()))
    {
        inactivity_timer++;
    }
}

void check_inactivity_timer()
{
    unsigned long now = millis();
    if (now > inactivity_timer_last_reading)
    {
        inactivity_timer_last_reading = now;
        inactivity_timer++;
    }
    if (inactivity_timer >= settings_get_inactivity_timeout() && ui.display_is_on) // Screen Saver
    {
        ui.screen_saver();
        inactivity_timer = 0;
    }
}

void restore_display()
{
    ui.all_on();
    // ui.display_on();
    selected_fxn->display();
    reset_inactivity_timer();
    terminal_print_status(true);
}
