#include <WiFiNINA.h>

#ifndef WIFI_Util_h
#define WIFI_Util_h

// WiFi
String ssid = ""; // your network SSID (name)
String pass = ""; // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0; // your network key Index number (needed only for WEP)
unsigned int wifi_chk_time = 0;

int wifi_status = WL_IDLE_STATUS;
WiFiServer server(80);

bool is_wifi_connected()
{
    return wifi_status == WL_CONNECTED;
}

void printWifiStatus()
{
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    Serial.print("Status: ");
    Serial.println(String(wifi_status));

    // print your board's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println(" dBm");
    // print where to go in a browser:
    Serial.print("To see this page in action, open a browser to http://");
    Serial.println(ip);
}

// int count = 0;
String getEncryptionType(int thisType)
{
    // read the encryption type and print out the name:
    switch (thisType)
    {
    case ENC_TYPE_WEP:
        return ("WEP");
        break;
    case ENC_TYPE_TKIP:
        return ("WPA");
        break;
    case ENC_TYPE_CCMP:
        return ("WPA2");
        break;
    case ENC_TYPE_NONE:
        return ("None");
        break;
    case ENC_TYPE_AUTO:
        return ("Auto");
        break;
    case ENC_TYPE_UNKNOWN:
    default:
        return ("Unknown");
        break;
    }
}

String getConnectionStatus(int thisType)
{
    // read the connection type and print out the name:
    switch (thisType)
    {
    case WL_NO_MODULE:
        return ("NO MODULE");
        break;
    case WL_IDLE_STATUS:
        return ("IDLE");
        break;
    case WL_NO_SSID_AVAIL:
        return ("SSID UNAVAIL");
        break;
    case WL_SCAN_COMPLETED:
        return ("SCAN COMPLETED");
        break;
    case WL_CONNECTED:
        return ("CONNECTED");
        break;
    case WL_CONNECT_FAILED:
        return ("CONNECT FAILED");
        break;
    case WL_CONNECTION_LOST:
        return ("CONNECTION LOST");
        break;
    case WL_DISCONNECTED:
        return ("DISCONNECTED");
        break;
    case WL_AP_LISTENING:
        return ("LISTENING");
        break;
    case WL_AP_CONNECTED:
        return ("AP CONNECTED");
        break;
    case WL_AP_FAILED:
        return ("AP FAILED");
        break;
    default:
        return ("Unknown");
        break;
    }
}

void printEncryptionType(int thisType)
{
    Serial.print(getEncryptionType(thisType));
}

void print2Digits(byte thisByte)
{
    if (thisByte < 0xF)
    {
        Serial.print("0");
    }
    Serial.print(thisByte, HEX);
}

void printMacAddress(byte mac[])
{
    for (int i = 5; i >= 0; i--)
    {
        if (mac[i] < 16)
        {
            Serial.print("0");
        }
        Serial.print(mac[i], HEX);
        if (i > 0)
        {
            Serial.print(":");
        }
    }
    Serial.println();
}

String getEnc(int thisNet)
{
    return getEncryptionType(WiFi.encryptionType(thisNet));
}

String getSsid(int thisNet, boolean show_encryption = true)
{
    String enc = "";
    String ssid = String(WiFi.SSID(thisNet));
    if (show_encryption)
    {
        enc = " (" + getEnc(thisNet) + ")";
        ssid = ssid.substring(0, 14);
    }
    return ssid + enc;
}

int numNetworks()
{
    return WiFi.scanNetworks();
}

void listNetworks()
{
    // scan for nearby networks:
    Serial.println("** Scan Networks **");
    int numSsid = WiFi.scanNetworks();
    if (numSsid == -1)
    {
        Serial.println("Couldn't get a WiFi connection");
        return;
    }

    // print the list of networks seen:
    Serial.print("number of available networks: ");
    Serial.println(numSsid);

    // print the network number and name for each network found:
    for (int thisNet = 0; thisNet < numSsid; thisNet++)
    {
        Serial.print(thisNet + 1);
        Serial.print(") ");
        Serial.print("Signal: ");
        Serial.print(WiFi.RSSI(thisNet));
        Serial.print(" dBm");
        Serial.print("\tChannel: ");
        Serial.print(WiFi.channel(thisNet));
        byte bssid[6];
        Serial.print("\t\tBSSID: ");
        printMacAddress(WiFi.BSSID(thisNet, bssid));
        Serial.print("\tEncryption: ");
        printEncryptionType(WiFi.encryptionType(thisNet));
        Serial.print("\t\tSSID: ");
        Serial.println(WiFi.SSID(thisNet));
        Serial.flush();
    }
    Serial.println();
}

void WIFI_setup()
{
    wifi_status = WiFi.status();
    // check for the WiFi module:
    if (wifi_status == WL_NO_MODULE)
    {
        Serial.println("Communication with WiFi module failed!");
        return;
    }

    String fv = WiFi.firmwareVersion();
    if (fv < WIFI_FIRMWARE_LATEST_VERSION)
    {
        Serial.println("Please upgrade the firmware");
    }

    // attempt to connect to Wifi network:
    if (wifi_status != WL_CONNECTED)
    {
        Serial.println("");                                    //
        Serial.println("wifi_status: " + String(wifi_status)); // print the status
        Serial.print("Attempting to connect to Network named: ");
        Serial.println(ssid); // print the network name (SSID);

        Serial.print("password: ");
        Serial.println(pass); // print the password;

        // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
        wifi_status = WiFi.begin(ssid.c_str(), pass.c_str());
        // wait 10 seconds for connection:
        wifi_chk_time = millis() + 10000;
        // delay(10000);
    }
    else
    {
        Serial.println("");                                         //
        Serial.println("Server beginning: " + String(wifi_status)); // print the status
        server.begin();                                             // start the web server on port 80
        printWifiStatus();                                          // you're connected now, so print out the status
        // listNetworks();
    }
}

#endif