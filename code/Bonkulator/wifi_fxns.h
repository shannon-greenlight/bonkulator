#include <Wire.h> // For I2C comm, but needed for not getting compile error
#include <TerminalVT100.h>
#include <stdlib.h>
#include <WiFiNINA.h>

// ui fxns
EEPROM_String wifi_password(64);
EEPROM_String wifi_string_vars[] = {wifi_password};

EEPROM_String wifi_ssid(64);
EEPROM_Bool wifi_active = EEPROM_Bool(); // sizeof fxn val

uint16_t _wifi_params[1];
uint16_t _wifi_mins[] = {0};
uint16_t _wifi_maxs[] = {64};
uint16_t _wifi_init_vals[] = {0};
uint16_t *wifi_stuff[] = {_wifi_params, _wifi_mins, _wifi_maxs, _wifi_init_vals};
String wifi_labels[] = {"Password: "};
String wifi_string_params[] = {"$~"};
bool wifi_param_active[] = {0};
SPANK_fxn wifi_fxn("WiFi", wifi_labels, wifi_stuff, sizeof(_wifi_params) / sizeof(_wifi_params[0]), &ui);

void wifi_display();

String get_label(int i);
String get_control(String control);

uint8_t select_wifi_screen = 5;
int numSsid = 0;

bool wait_not_triggered = false;

void wifi_connect(void);
void wifi_connected(void);

void wifi_idle()
{
    // ui.clearDisplay();
    // ui.printText("WiFi:", 0, 0, 2);
    ui.newFxn("WiFi Idle");
    ui.printLine("Press trigger to scan", LINE_1, 1);
    WiFi.disconnect();
    wifi_active.reset();
    numSsid = 0;
}

void wifi_display_networks()
{
    String temps;
    wifi_fxn.num_params = numSsid;
    if (numSsid == 0)
    {
        // wifi_password.put("a");
        // ui.clearDisplay();
        // ui.printText("WiFi:", 0, 0, 2);
        ui.newFxn("Networks");
        ui.printLine("Scanning networks... Please wait.", LINE_1, 1);

        numSsid = numNetworks();
        // ui.clearDisplay();

        wifi_fxn.num_params = numSsid;
        if (numSsid == -1)
        {
            // ui.printText("WiFi:", 0, 0, 2);
            ui.printLine("Couldn't get a WiFi connection", LINE_1, 1);
            return;
            // Serial.println("Couldn't get a WiFi connection");
        }
        else
        {
            // print the list of networks seen:
            ui.printText("Found: " + String(numSsid), 0, 0, 2);
            // Serial.print("number of available networks: ");
            // Serial.println(numSsid);
            // Serial.println();
        }
    }
    else
    {
        ui.newFxn("Found: " + String(numSsid));
    }
    // print the network number and name for each network found:
    // Serial.println("Num params: " + String(wifi_fxn.num_params) + "  Param# " + String(wifi_fxn.param_num));
    ui.fill(BLACK, LINE_1);
    ui.hiliteLine(min(2, wifi_fxn.param_num));
    int start = max(0, wifi_fxn.param_num - 2);
    int end = max(2, wifi_fxn.param_num);
    int lineNum = 0;
    for (int i = start; i <= end; i++)
    {
        // int thisNet = i + wifi_fxn.param_num/3;
        temps = getSsid(i);
        // Serial.println(String(i) + ". " + temps);
        ui.printLine(temps, ui.lines[lineNum++], 1);
    }
}

void wifi_set_ssid()
{
    // Serial.println("Set SSID: "+String(wifi_fxn.param_num));
    ssid = getSsid(wifi_fxn.param_num, false);
    wifi_ssid.put(ssid);
    String enc = getEnc(wifi_fxn.param_num);
    if (enc == "None")
    {
        select_wifi_screen++;
        select_wifi_screen++;
        wifi_connect();
    }
    else
    {
        select_wifi_screen++;
        wifi_display();
    }
}

void wifi_display_password()
{
    ui.newFxn(ssid);
    wifi_fxn.param_num = 0;
    wifi_fxn.num_params = 1;
    wifi_fxn.printParams();
}

String IpAddress2String(const IPAddress &ipAddress)
{
    return String(ipAddress[0]) + String(".") +
           String(ipAddress[1]) + String(".") +
           String(ipAddress[2]) + String(".") +
           String(ipAddress[3]);
}

void wifi_connect()
{
    // todo - why does it only work one time?
    int count = 0;
    wifi_active.reset();
    pass = wifi_fxn.get_param_as_string_var(0);
    ssid = wifi_ssid.get();
    ui.newFxn(ssid);
    ui.printLine("Status: ", LINE_1, 1);
    wifi_status = WL_IDLE_STATUS;
    wifi_chk_time = 0;
    do
    {
        if (wifi_status != WL_CONNECTED && millis() > wifi_chk_time)
            WIFI_setup();
        ui.printLine("Status: " + getConnectionStatus(server.status()), LINE_1, 1);
        // Serial.println("Status: " + String(wifi_status));
    } while (wifi_status != WL_CONNECTED && count++ < 15);
    if (wifi_status == WL_CONNECTED)
    {
        WIFI_setup(); // do it again! must be done to begin wifi
        select_wifi_screen = 4;
        wifi_display();
    }
    else
    {
        // ui.fill(BLACK, LINE_1);
        ui.printLine("Failed", LINE_2, 1);
        select_wifi_screen = 5;
    }
}

void wifi_attempt_connect(boolean force)
{
    if (wifi_enabled())
    {
        if (force)
        { // true forces WiFi
            wifi_ssid.put(SECRET_SSID);
            wifi_password.put(SECRET_PASS);
            wifi_active.set();
        }
        else
        {
            // wifi_active.reset();
        }
        if (wifi_active.get() && wifi_status != WL_CONNECTED)
        {
            wifi_connect();
            delay(2000);
        }
    }
}

void wifi_connected()
{
    wifi_active.set();
    ui.newFxn(wifi_ssid.get());
    ui.printLine("Status: " + getConnectionStatus(wifi_status), LINE_1, 1);
    ui.printLine("IP: " + IpAddress2String(WiFi.localIP()), LINE_2, 1);
    ui.printLine("Signal: " + String(WiFi.RSSI()) + " dBm", LINE_3, 1);
}

boolean in_wifi()
{
    return selected_fxn == &wifi_fxn;
}

void wifi_display()
{
    if (wifi_enabled())
    {
        switch (select_wifi_screen)
        {
        case 0:
            wifi_display_networks();
            break;
        case 1:
            wifi_set_ssid();
            break;
        case 2:
            wifi_display_password();
            break;
        case 3:
            wifi_connect();
            break;
        case 4:
            wifi_connected();
            break;
        case 5:
            wifi_idle();
            break;
        }
    }
    else
    {
        ui.clearDisplay();
        ui.printLine("WiFi:", 0);
        ui.printLine("Disabled", LINE_1, 1);
        ui.printLine("See Settings", LINE_2, 1);
    }
}

// trigger action
void wifi_activate()
{
    select_wifi_screen++;
    if (select_wifi_screen > 5)
        select_wifi_screen = 0;
    // ui.terminal_debug("WiFi Activate! screen: " + String(select_wifi_screen));
    wifi_display();
}

void wifi_new_fxn()
{
    select_wifi_screen = (wifi_status == WL_CONNECTED) ? 4 : 5;

    wifi_fxn.digit_num = numSsid = 0;
    wifi_fxn.param_num = 0;
    wifi_fxn.num_params = 1;
}

void wifi_init()
{
    wifi_fxn.init();
    wifi_password.put("a");
}

void wifi_begin()
{
    wifi_password.begin(false);
    wifi_ssid.begin(false);
    wifi_active.begin(false);
    wifi_password.xfer();
    wifi_ssid.xfer();
    wifi_active.xfer();
    // wifi_param = wifi_get_param(wifi_dig_num);

    wifi_fxn.begin();
    wifi_fxn.string_vars = wifi_string_vars;
    wifi_fxn.string_vars[0].begin(false);
    wifi_fxn.string_vars[0].xfer();
    wifi_fxn.string_params = wifi_string_params;
    // wifi_fxn.update_fxns = wifi_update_fxns;
    wifi_fxn.active_params = wifi_param_active;

    wifi_fxn.display_fxn = &wifi_display;
    wifi_fxn.param_num = 0;
    wifi_fxn.num_params = 1;

    // for testing
    // wifi_fxn.digit_num = 4;

    // connect if wifi is active
    // wifi_attempt_connect(!IS_RELEASE);
    wifi_attempt_connect(false);
    wifi_new_fxn();
}
