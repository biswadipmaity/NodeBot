#include "Arduino.h"

void cloudPrint(String msg)
{
  String url = "/Console/AppendMsg?msg="+ msg;
  url.replace(" ", "%20");
  client.get(url);
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();
}

void cloudPrintln(String msg)
{
  cloudPrint(msg+"\\n");
}
