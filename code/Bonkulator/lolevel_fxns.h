// for when you just want to do nothing
void noop() {}

void code_red(bool on)
{
    analogWrite(LEDR, on ? 0 : 255);
    analogWrite(LEDB, 255);
    // analogWrite(LEDG, 255);
}

void red_alert()
{
    analogWrite(LEDR, 0);
    analogWrite(LEDB, 255);
    // analogWrite(LEDG, 255);
}

String csv_elem(String s, char delim, int instance)
{
    // vector <String> out;
    int indx = 0;
    int found = 0;
    for (unsigned int i = 0; i < s.length() + 1; i++)
    {
        if (s.charAt(i) == delim || i == s.length())
        {
            if (found == instance)
            {
                return s.substring(indx, i);
            }
            found++;
            indx = i + 1;
        }
    }
    return "CSV element not found!";
}

String enquote(String value)
{
    return "\"" + value + "\"";
}

String encase(String value, String container)
{
    return container + value + container;
}
