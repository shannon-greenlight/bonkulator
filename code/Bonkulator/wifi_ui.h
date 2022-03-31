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

void send_data_to_client(WiFiClient client, char cmd)
{
  if (cmd == '[')
    return;
  // client.print(toJSON("fxn", fxn_name()));
  // client.print(",");
  // client.print(toJSON("fxn_num", String(fxn.get())));
  // client.print(",");
  // if (cmd == ' ' || cmd == 'f' || cmd == '+' || cmd == '-')
  //   client.print(list_fxns());
  client.print(toJSON("device_name", settings_get_device_name()));
  client.print(",");
  client.print(toJSON("cmd", String(cmd)));
  client.print(",");
  client.print(toJSON("digit_num", String(selected_fxn->digit_num)));
  client.print(",");
  client.print(toJSON("param_num", String(selected_fxn->param_num)));
  client.print(",");

  // client.print(toJSON("dac_fs", String(DAC_FS)));
  // client.print(",");
  // client.print(toJSON("adc_fs", String(ADC_FS)));
  // client.print(",");

  if (wifi_ui_message > "" || true)
  {
    client.print(toJSON("message", wifi_ui_message));
    client.print(",");
  }
  wifi_ui_message = "";

  client.print("\"params\" : [");
  client.print(selected_fxn->params_toJSON());
  client.print("]");
}

void _do_server()
{
  char cmd;

  WiFiClient client = server.available(); // listen for incoming clients

  // ui.terminal_debug("Do server. Client: " + String(client));

  if (client)
  { // if you get a client,
    // Serial.println("new client"); // print a message out the serial port
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
              cmd = currentLine[6];
              if (cmd == '[')
              {
                esc_mode = true;
                cmd = currentLine[7];
              }

              // Serial.println("Command: " + String(cmd));
              // int c = cmd;
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
              // Serial.println("Receiving: " + in_str + "*");

              process_cmd(in_str);
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
  if (wifi_status == WL_CONNECTED)
    ui.printWiFiBars(WiFi.RSSI());
}

void do_server()
{
  if (wifi_enabled() && wifi_active.get())
  {
    _do_server();
  }
}
