
const char* RAW_HTML = "<html>\
  <head>\
    <meta http-equiv='refresh' content='30'/>\
    <title>VMC1 status</title>\
    <style>\
    </style>\
  </head>\
  <body>\
    <h1>VMC1 status</h1>\
    <p>wifi: %02d dBm</p>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <p>temp/hum %.2f:%.2f</p>\
    <p>FAN : %02d</p>\
    <form METHOD='POST' action='/status'>\
      <input type='text' name='vmcInput' value='%d'>\
      <input type='submit' value='VMC'>\
    </form>\
  </body>\
</html>"; 

void handleStatus() {
  digitalWrite(led, 0);
  char temp[400];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  int wifilevel = WiFi.RSSI();
 
  if ( server.hasArg("vmcInput")){
       String vmcInput = server.arg("vmcInput");
       Serial.println("VMC");
       Serial.println(vmcInput);
      fanLevel= vmcInput.toInt();
      manageFan();
  }
  
  snprintf(temp, 400,RAW_HTML,
           wifilevel,
           hr, min % 60, sec % 60,
           temperature,humid, 
           fanPower, 
           fanLevel
          );
  server.send(200, "text/html", temp);
  digitalWrite(led, 1);
}
