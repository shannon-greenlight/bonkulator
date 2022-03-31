// inactivity timer routines
volatile int inactivity_timer = 0;
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
    if (inactivity_timer >= settings_get_inactivity_timeout() && ui.display_is_on) // Screen Saver
    {
        // todo make this stuff a ui method
        ui.screen_saver();
    }
}

void restore_display()
{
    keypress = 0;
    ui.display_on();
    // new_fxn();
    reset_inactivity_timer();
}

void check_display_restore()
{
    if (fp_key_pressed && !ui.display_is_on && keypress != '!')
    {
        restore_display();
    }
}
