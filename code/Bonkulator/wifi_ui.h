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
  client.print("{");
  // ui.terminal_debug("Send data to client: " + String(selected_fxn->get_param(OUTPUT_WAVEFORM)));
  if (cmd == '[')
    return;

  client.print(globals_toJSON(cmd));
  client.print(messages_toJSON());
  client.print(triggers_toJSON());
  client.print(fxn_params_toJSON());

  client.print("}");

  // The HTTP response ends with another blank line:
  client.println();
}

void do_server()
{
  char cmd = '/';

  // ui.terminal_debug("Do server. Asking: ");

  WiFiClient client = server.available(); // listen for incoming clients

  if (client)
  { // if you get a client, and not released code
    if (!IS_RELEASE && false)
    {
      // status_string = ("Do server. Client: " + String(client));
      Serial.println("new client"); // print a message out the serial port
    }
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

            send_data_to_client(client, cmd);
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
              while (inchar != ' ' && i < 20000)
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
    wifi_RSSI = WiFi.RSSI();
    ui.printWiFiBars(wifi_RSSI);
    cnt = 0;
  }
}

String get_wifi_bars()
{
  int wifi_strength = wifi_RSSI / 10 + 8;
  wifi_strength = constrain(wifi_strength, 0, 4);
  // ui.terminal_debug("WiFi Strngth: " + String(wifi_strength));
  String bars = "";
  for (int i = 0; i < wifi_strength; i++)
  {
    bars += "|";
  }
  for (int i = wifi_strength; i <= 4; i++)
  {
    bars += " ";
  }
  return ("Wifi: " + bars);
}

String check_wifi()
{
  static int cnt = 5; // ask too frequently and crash may occur
  if (wifi_enabled() && wifi_active.get())
  {
    if (cnt-- == 0)
    {
      do_server();
      cnt = 5;
    }
    return get_wifi_bars();
  }
  else
  {
    return "";
  }
}
