#include <Wire.h> // For I2C comm, but needed for not getting compile error
#include <TerminalVT100.h>
#include <stdlib.h>
#include <WiFiNINA.h>

String wifi_params_toJSON()
{
  String out = "";
  out += "{ ";
  out += toJSON("label", "Connected to: ");
  out += ", ";
  out += toJSON("type", "text");
  out += ", ";
  out += toJSON("value", String(wifi_ssid.get()));
  out += ", ";
  out += toJSON("selected", "false");
  out += " }";
  out += ",";
  out += "{ ";
  out += toJSON("label", "Status");
  out += ", ";
  out += toJSON("type", "text");
  out += ", ";
  out += toJSON("value", getConnectionStatus(wifi_status));
  out += ", ";
  out += toJSON("selected", "false");
  out += " }";
  out += ",";
  out += "{ ";
  out += toJSON("label", "IP");
  out += ", ";
  out += toJSON("type", "text");
  out += ", ";
  out += toJSON("value", IpAddress2String(WiFi.localIP()));
  out += ", ";
  out += toJSON("selected", "false");
  out += " }";
  out += ",";
  out += "{ ";
  out += toJSON("label", "Signal");
  out += ", ";
  out += toJSON("type", "text");
  out += ", ";
  out += toJSON("value", String(WiFi.RSSI()) + " dBm");
  out += ", ";
  out += toJSON("selected", "false");
  out += " }";
  return "[" + out + "]";
}

String params_toJSON()
{
  return selected_fxn->params_toJSON();
}

String list_fxns()
{
  String out = "\"fxns\" : [";
  out += list_outputs();
  out += ", " + enquote("Settings");
  out += "],";
  return out;
}

void send_data_to_client(WiFiClient client, char cmd)
{
  // ui.terminal_debug("Send data to client: " + String(cmd));
  if (cmd == '[')
    return;
  client.print(toJSON("fxn", selected_fxn->name));
  client.print(",");
  client.print(toJSON("fxn_num", String(remembered_fxn.get())));
  client.print(",");
  if (cmd == ' ' || cmd == 'f' || cmd == '+' || cmd == '-')
    client.print(list_fxns());
  client.print(toJSON("device_name", settings_get_device_name()));
  client.print(",");
  client.print(toJSON("cmd", String(cmd)));
  client.print(",");
  uint8_t digit_num = selected_fxn->digit_num;
  if (selected_fxn->get_min_w_offset() < 0)
  {
    digit_num--;
  }
  client.print(toJSON("digit_num", String(digit_num)));
  client.print(",");
  client.print(toJSON("param_num", String(selected_fxn->param_num)));
  client.print(",");
  client.print(toJSON("param_active", String(selected_fxn->get_param_active())));
  client.print(",");

  client.print(toJSON("fs_volts", output_get_fs()));
  client.print(",");
  client.print(toJSON("offset_max", "100"));
  client.print(",");
  client.print(toJSON("offset_min", "-100"));
  client.print(",");
  client.print(toJSON("scale_max", "100"));
  client.print(",");
  client.print(toJSON("dac_fs", String(DAC_FS)));
  client.print(",");
  client.print(toJSON("adc_fs", String(ADC_FS)));
  client.print(",");

  if (selected_fxn != &settings_fxn)
  {
    set_wifi_message();
    client.print(toJSON("message", wifi_ui_message));
    client.print(",");
  }
  wifi_ui_message = "";

  client.print("\"triggers\" : [");
  for (int trig_num = 0; trig_num < NUM_TRIGGERS; trig_num++)
  {
    if (trig_num > 0)
    {
      client.print(",");
    }
    // ui.terminal_debug((*triggers[trig_num]).params_toJSON());
    client.print((*triggers[trig_num]).params_toJSON());
  }
  client.print("],");

  client.print("\"params\" : [");
  client.print(selected_fxn->params_toJSON());
  client.print("]");
}

void do_server()
{
  char cmd = '/';

  // ui.terminal_debug("Do server. Asking: ");

  WiFiClient client = server.available(); // listen for incoming clients

  if (client)
  { // if you get a client,
    // Serial.println("new client"); // print a message out the serial port
    status_string = ("Do server. Client: " + String(client));
    String currentLine = ""; // make a String to hold incoming data from the client
    while (client.connected())
    { // loop while the client's connected
      if (client.available())
      {                         // if there's bytes to read from the client,
        char c = client.read(); // read a byte, then
        // Serial.write(c);        // print it out the serial monitor
        if (c == '\n')
        { // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0)
          {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            // client.println("Content-type:application/json");
            client.println("Access-Control-Allow-Origin: *");
            client.println("Access-Control-Allow-Methods: *");
            client.println();

            client.print("{");
            send_data_to_client(client, cmd);
            client.print("}");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else
          { // if you got a newline, then clear currentLine:
            if (strstr(currentLine.c_str(), "POST"))
            {
              currentLine.replace("%23", "#");
              cmd = currentLine[6];
              if (cmd == '[')
              {
                cmd = currentLine[7];
              }
              String in_str = "";
              int i = 0;
              char inchar = currentLine[i + 6];
              while (inchar != ' ' && i < 20)
              {
                in_str += inchar;
                i++;
                inchar = currentLine[i + 6];
              }
              if (in_str == "")
              {
                in_str = " ";
              }
              // Serial.println("Receiving: " + currentLine + "*");

              if (in_str.startsWith("-Control"))
              {
                currentLine = "";
              }
              else
              {
                process_cmd(in_str);
              }

              // Serial.println("Command: " + in_str);
              // Serial.println("Command: " + String(cmd));
              // int c = cmd;
            }
            currentLine = "";
          }
        }
        else if (c != '\r')
        {                   // if you got anything else but a carriage return character,
          currentLine += c; // add it to the end of the currentLine
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);

    // close the connection:
    client.stop();
    // Serial.println("client disonnected");
  }

  //  if (!triggered && fxn.get() != LFO_FXN && wifi_status == WL_CONNECTED)
  static int cnt = 0;
  if (wifi_status == WL_CONNECTED && cnt++ > 10)
  {
    ui.printWiFiBars(WiFi.RSSI());
    cnt = 0;
  }
}

void check_wifi()
{
  if (wifi_enabled() && wifi_active.get())
  {
    do_server();
  }
}
