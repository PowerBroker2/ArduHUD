#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include "BluetoothSerial.h"
#include "SerialTransfer.h"
#include "ELMduino.h"




BluetoothSerial SerialBT;
#define DEBUG_PORT      Serial
#define LED_DRIVER_PORT Serial2
#define ELM_PORT        SerialBT




const char *ssid = "HUDuino";
const char MAIN_page[] PROGMEM = "<!DOCTYPE html>\n<html>\n<style>\nbody, html {\n  height: 100%;\n  margin: 0;\n  font-family: Arial;\n}\n\n/* Style tab links */\n.tablink {\n  background-color: #555;\n  color: white;\n  float: left;\n  border: none;\n  outline: none;\n  cursor: pointer;\n  padding: 14px 16px;\n  font-size: 17px;\n  width: 33.33%;\n}\n\n.tablink:hover {\n  background-color: #777;\n}\n\n.tabcontent {\n  color: white;\n  display: none;\n  padding: 100px 20px;\n  height: 100%;\n}\n\n#Home {background-color: white;}\n#PIDs {background-color: white;}\n#Console {background-color: white;}\n\n.card\n{\n    max-width: 400px;\n     min-height: 250px;\n     background: #02b875;\n     padding: 30px;\n     box-sizing: border-box;\n     color: #FFF;\n     margin:20px;\n     box-shadow: 0px 2px 18px -4px rgba(0,0,0,0.75);\n}\n\n#myProgress\n{\n  width: 100%;\n  background-color: #ddd;\n}\n\n#myBar\n{\n  width: 1%;\n  height: 30px;\n  background-color: #4CAF50;\n}\n\n/* Style the form - display items horizontally */\n.form-inline {\n  display: flex;\n  flex-flow: row wrap;\n  align-items: center;\n}\n\n/* Add some margins for each label */\n.form-inline label {\n  margin: 5px 10px 5px 0;\n}\n\n/* Style the input fields */\n.form-inline input {\n  vertical-align: middle;\n  margin: 5px 10px 5px 0;\n  padding: 10px;\n  background-color: #fff;\n  border: 1px solid #ddd;\n}\n\n/* Style the submit button */\n.form-inline button {\n  padding: 10px 20px;\n  background-color: dodgerblue;\n  border: 1px solid #ddd;\n  color: white;\n}\n\n.form-inline button:hover {\n  background-color: royalblue;\n}\n\n/* Add responsiveness - display the form controls vertically instead of horizontally on screens that are less than 800px wide */\n@media (max-width: 800px) {\n  .form-inline input {\n    margin: 10px 0;\n  }\n\n  .form-inline {\n    flex-direction: column;\n    align-items: stretch;\n  }\n}\n</style>\n\n<body>\n<button class=\"tablink\" onclick=\"openPage('Home', this, 'red')\" id=\"defaultOpen\">Home</button>\n<button class=\"tablink\" onclick=\"openPage('PIDs', this, 'green')\">PIDs</button>\n<button class=\"tablink\" onclick=\"openPage('Console', this, 'blue')\">Console</button>\n\n<div id=\"Home\" class=\"tabcontent\">\n    <h3>Home</h3>\n    <div class=\"card\">\n        <h4>ArduHUD</h4><br>\n        <h1>Speed: <span id=\"speed\">0</span></h1>\n        <div id=\"myProgress\">\n            <div id=\"myBar\"></div>\n        </div><br>\n    </div>\n</div>\n\n<div id=\"PIDs\" class=\"tabcontent\">\n    <h3>PIDs</h3>\n    <button type=\"standardPID\">SUPPORTED_PIDS_1_20</button><br>\n    <button type=\"standardPID\">MONITOR_STATUS_SINCE_DTC_CLEARED</button><br>\n    <button type=\"standardPID\">FREEZE_DTC</button><br>\n    <button type=\"standardPID\">FUEL_SYSTEM_STATUS</button><br>\n    <button type=\"standardPID\">ENGINE_LOAD</button><br>\n    <button type=\"standardPID\">ENGINE_COOLANT_TEMP</button><br>\n    <button type=\"standardPID\">SHORT_TERM_FUEL_TRIM_BANK_1</button><br>\n    <button type=\"standardPID\">LONG_TERM_FUEL_TRIM_BANK_1</button><br>\n    <button type=\"standardPID\">SHORT_TERM_FUEL_TRIM_BANK_2</button><br>\n    <button type=\"standardPID\">LONG_TERM_FUEL_TRIM_BANK_2</button><br>\n    <button type=\"standardPID\">FUEL_PRESSURE</button><br>\n    <button type=\"standardPID\">INTAKE_MANIFOLD_ABS_PRESSURE</button><br>\n    <button type=\"standardPID\">ENGINE_RPM</button><br>\n    <button type=\"standardPID\">VEHICLE_SPEED</button><br>\n    <button type=\"standardPID\">TIMING_ADVANCE</button><br>\n    <button type=\"standardPID\">INTAKE_AIR_TEMP</button><br>\n    <button type=\"standardPID\">MAF_FLOW_RATE</button><br>\n    <button type=\"standardPID\">THROTTLE_POSITION</button><br>\n    <button type=\"standardPID\">COMMANDED_SECONDARY_AIR_STATUS</button><br>\n    <button type=\"standardPID\">OXYGEN_SENSORS_PRESENT_2_BANKS</button><br>\n    <button type=\"standardPID\">OXYGEN_SENSOR_1_A</button><br>\n    <button type=\"standardPID\">OXYGEN_SENSOR_2_A</button><br>\n    <button type=\"standardPID\">OXYGEN_SENSOR_3_A</button><br>\n    <button type=\"standardPID\">OXYGEN_SENSOR_4_A</button><br>\n    <button type=\"standardPID\">OXYGEN_SENSOR_5_A</button><br>\n    <button type=\"standardPID\">OXYGEN_SENSOR_6_A</button><br>\n    <button type=\"standardPID\">OXYGEN_SENSOR_7_A</button><br>\n    <button type=\"standardPID\">OXYGEN_SENSOR_8_A</button><br>\n    <button type=\"standardPID\">OBD_STANDARDS</button><br>\n    <button type=\"standardPID\">OXYGEN_SENSORS_PRESENT_4_BANKS</button><br>\n    <button type=\"standardPID\">AUX_INPUT_STATUS</button><br>\n    <button type=\"standardPID\">RUN_TIME_SINCE_ENGINE_START</button><br><br><br>\n    \n    <button type=\"standardPID\">SUPPORTED_PIDS_21_40</button><br>\n    <button type=\"standardPID\">DISTANCE_TRAVELED_WITH_MIL_ON</button><br>\n    <button type=\"standardPID\">FUEL_RAIL_PRESSURE</button><br>\n    <button type=\"standardPID\">FUEL_RAIL_GUAGE_PRESSURE</button><br>\n    <button type=\"standardPID\">OXYGEN_SENSOR_1_B</button><br>\n    <button type=\"standardPID\">OXYGEN_SENSOR_2_B</button><br>\n    <button type=\"standardPID\">OXYGEN_SENSOR_3_B</button><br>\n    <button type=\"standardPID\">OXYGEN_SENSOR_4_B</button><br>\n    <button type=\"standardPID\">OXYGEN_SENSOR_5_B</button><br>\n    <button type=\"standardPID\">OXYGEN_SENSOR_6_B</button><br>\n    <button type=\"standardPID\">OXYGEN_SENSOR_7_B</button><br>\n    <button type=\"standardPID\">OXYGEN_SENSOR_8_B</button><br>\n    <button type=\"standardPID\">COMMANDED_EGR</button><br>\n    <button type=\"standardPID\">EGR_ERROR</button><br>\n    <button type=\"standardPID\">COMMANDED_EVAPORATIVE_PURGE</button><br>\n    <button type=\"standardPID\">FUEL_TANK_LEVEL_INPUT</button><br>\n    <button type=\"standardPID\">WARM_UPS_SINCE_CODES_CLEARED</button><br>\n    <button type=\"standardPID\">DIST_TRAV_SINCE_CODES_CLEARED</button><br>\n    <button type=\"standardPID\">EVAP_SYSTEM_VAPOR_PRESSURE</button><br>\n    <button type=\"standardPID\">ABS_BAROMETRIC_PRESSURE</button><br>\n    <button type=\"standardPID\">OXYGEN_SENSOR_1_C</button><br>\n    <button type=\"standardPID\">OXYGEN_SENSOR_2_C</button><br>\n    <button type=\"standardPID\">OXYGEN_SENSOR_3_C</button><br>\n    <button type=\"standardPID\">OXYGEN_SENSOR_4_C</button><br>\n    <button type=\"standardPID\">OXYGEN_SENSOR_5_C</button><br>\n    <button type=\"standardPID\">OXYGEN_SENSOR_6_C</button><br>\n    <button type=\"standardPID\">OXYGEN_SENSOR_7_C</button><br>\n    <button type=\"standardPID\">OXYGEN_SENSOR_8_C</button><br>\n    <button type=\"standardPID\">CATALYST_TEMP_BANK_1_SENSOR_1</button><br>\n    <button type=\"standardPID\">CATALYST_TEMP_BANK_2_SENSOR_1</button><br>\n    <button type=\"standardPID\">CATALYST_TEMP_BANK_1_SENSOR_2</button><br>\n    <button type=\"standardPID\">CATALYST_TEMP_BANK_2_SENSOR_2</button><br><br><br>\n    \n    <button type=\"standardPID\">SUPPORTED_PIDS_41_60</button><br>\n    <button type=\"standardPID\">MONITOR_STATUS_THIS_DRIVE_CYCLE</button><br>\n    <button type=\"standardPID\">CONTROL_MODULE_VOLTAGE</button><br>\n    <button type=\"standardPID\">ABS_LOAD_VALUE</button><br>\n    <button type=\"standardPID\">FUEL_AIR_COMMANDED_EQUIV_RATIO</button><br>\n    <button type=\"standardPID\">RELATIVE_THROTTLE_POSITION</button><br>\n    <button type=\"standardPID\">AMBIENT_AIR_TEMP</button><br>\n    <button type=\"standardPID\">ABS_THROTTLE_POSITION_B</button><br>\n    <button type=\"standardPID\">ABS_THROTTLE_POSITION_C</button><br>\n    <button type=\"standardPID\">ACCELERATOR_PEDAL_POSITION_D</button><br>\n    <button type=\"standardPID\">ACCELERATOR_PEDAL_POSITION_E</button><br>\n    <button type=\"standardPID\">ACCELERATOR_PEDAL_POSITION_F</button><br>\n    <button type=\"standardPID\">COMMANDED_THROTTLE_ACTUATOR</button><br>\n    <button type=\"standardPID\">TIME_RUN_WITH_MIL_ON</button><br>\n    <button type=\"standardPID\">TIME_SINCE_CODES_CLEARED</button><br>\n    <button type=\"standardPID\">MAX_VALUES_EQUIV_V_I_PRESSURE</button><br>\n    <button type=\"standardPID\">MAX_MAF_RATE</button><br>\n    <button type=\"standardPID\">FUEL_TYPE</button><br>\n    <button type=\"standardPID\">ETHONOL_FUEL_PERCENT</button><br>\n    <button type=\"standardPID\">ABS_EVAP_SYS_VAPOR_PRESSURE</button><br>\n    <button type=\"standardPID\">EVAP_SYS_VAPOR_PRESSURE</button><br>\n    <button type=\"standardPID\">SHORT_TERM_SEC_OXY_SENS_TRIM_1_3</button><br>\n    <button type=\"standardPID\">LONG_TERM_SEC_OXY_SENS_TRIM_1_3</button><br>\n    <button type=\"standardPID\">SHORT_TERM_SEC_OXY_SENS_TRIM_2_4</button><br>\n    <button type=\"standardPID\">LONG_TERM_SEC_OXY_SENS_TRIM_2_4</button><br>\n    <button type=\"standardPID\">FUEL_RAIL_ABS_PRESSURE</button><br>\n    <button type=\"standardPID\">RELATIVE_ACCELERATOR_PEDAL_POS</button><br>\n    <button type=\"standardPID\">HYBRID_BATTERY_REMAINING_LIFE</button><br>\n    <button type=\"standardPID\">ENGINE_OIL_TEMP</button><br>\n    <button type=\"standardPID\">FUEL_INJECTION_TIMING</button><br>\n    <button type=\"standardPID\">ENGINE_FUEL_RATE</button><br>\n    <button type=\"standardPID\">EMISSION_REQUIREMENTS</button><br><br><br>\n    \n    <button type=\"standardPID\">SUPPORTED_PIDS_61_80</button><br>\n    <button type=\"standardPID\">DEMANDED_ENGINE_PERCENT_TORQUE</button><br>\n    <button type=\"standardPID\">ACTUAL_ENGINE_TORQUE</button><br>\n    <button type=\"standardPID\">ENGINE_REFERENCE_TORQUE</button><br>\n    <button type=\"standardPID\">ENGINE_PERCENT_TORQUE_DATA</button><br>\n    <button type=\"standardPID\">AUX_INPUT_OUTPUT_SUPPORTED</button><br><br><br>\n</div>\n\n<div id=\"Console\" class=\"tabcontent\">\n    <h3>Console</h3>\n    <form class=\"form-inline\" action=\"/console\">\n    <input type=\"Service\" id=\"Service\" placeholder=\"Enter Service\" name=\"Service\">\n    <input type=\"PID\" id=\"PID\" placeholder=\"Enter PID\" name=\"PID\">\n    <select id=\"cars\">\n        <option value=\"HEX\">HEX</option>\n        <option value=\"DEC\">DEC</option>\n    </select>\n    <button type=\"submit\">Submit</button>\n    </form>\n</div>\n</body>\n\n<script>\nfunction openPage(pageName, elmnt, color)\n{\n  // Hide all elements with class=\"tabcontent\" by default */\n  var i, tabcontent, tablinks;\n  tabcontent = document.getElementsByClassName(\"tabcontent\");\n  for (i = 0; i < tabcontent.length; i++)\n  {\n    tabcontent[i].style.display = \"none\";\n  }\n\n  // Remove the background color of all tablinks/buttons\n  tablinks = document.getElementsByClassName(\"tablink\");\n  for (i = 0; i < tablinks.length; i++)\n  {\n    tablinks[i].style.backgroundColor = \"\";\n  }\n\n  // Show the specific tab content\n  document.getElementById(pageName).style.display = \"block\";\n\n  // Add the specific color to the button used to open the tab content\n  elmnt.style.backgroundColor = color;\n}\n\n\ndocument.getElementById(\"defaultOpen\").click();\n\n\nsetInterval(function()\n{\n  getSpeedData();\n  getRpmData();\n}, 100);\n\n\nfunction getSpeedData()\n{\n  var xhttp = new XMLHttpRequest();\n  \n  xhttp.onreadystatechange = function()\n  {\n    if (this.readyState == 4 && this.status == 200)\n        {\n      document.getElementById(\"speed\").innerHTML = this.responseText;\n    }\n  };\n  \n  xhttp.open(\"GET\", \"speed\", true);\n  xhttp.send();\n}\n\n\nfunction move(width)\n{\n  var elem = document.getElementById(\"myBar\");\n  \n  function frame()\n  {\n    if (width > 100)\n    {\n      width = 100;\n    }\n    else if (width < 0)\n    {\n      width = 0;\n    }\n    \n    elem.style.width = width + \"%\";\n  }\n}\n\n\nfunction getRpmData()\n{\n  var xhttp = new XMLHttpRequest();\n  \n  xhttp.onreadystatechange = function()\n  {\n    if (this.readyState == 4 && this.status == 200)\n  {\n       move(parseInt(this.responseText));\n    }\n  };\n  \n  xhttp.open(\"GET\", \"rpm\", true);\n  xhttp.send();\n}\n</script>\n</html>";
const uint16_t MAX_RPM = 3500;
const uint16_t MIN_RPM = 700;
const uint8_t MSG_LEN = 16;




WiFiServer server(80);
SerialTransfer myTransfer;
ELM327 myELM327;




enum fsm{get_speed, 
         get_rpm};
fsm state = get_rpm;

struct STRUCT {
  int8_t status;
  uint32_t rpm;
  float mph;
  char msg[MSG_LEN];
} carTelem;




void setup()
{
#if LED_BUILTIN
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
#endif

  DEBUG_PORT.begin(115200);
  LED_DRIVER_PORT.begin(115200);

  WiFi.softAP(ssid);
  IPAddress myIP = WiFi.softAPIP();
  DEBUG_PORT.println(myIP);
  server.begin();

  // Wait for ELM327 to init
  delay(3000);
  
  ELM_PORT.begin("ArduHUD", true);
  
  if (!ELM_PORT.connect("OBDII"))
  {
    DEBUG_PORT.println("Couldn't connect to OBD scanner");
    while(1);
  }
  
  myELM327.begin(ELM_PORT);
  myTransfer.begin(LED_DRIVER_PORT);

  while(!myELM327.connected)
  {
    myELM327.initializeELM();
    serverProcessing();
  }
}




void loop()
{
  switch(state)
  {
    case get_rpm:
    {
      float tempRPM = myELM327.rpm();

      carTelem.status = myELM327.status;
      memcpy(carTelem.msg, myELM327.payload, MSG_LEN);
      
      if(myELM327.status == ELM_SUCCESS)
        carTelem.rpm = tempRPM;
      else
        printError();
      
      state = get_speed;
      break;
    }

    case get_speed:
    {
      float tempMPH = myELM327.mph();

      carTelem.status = myELM327.status;
      memcpy(carTelem.msg, myELM327.payload, MSG_LEN);
      
      if(myELM327.status == ELM_SUCCESS)
        carTelem.mph = tempMPH;
      else
        printError();
      
      state = get_rpm;
      break;
    }
  }

  myTransfer.txObj(carTelem, sizeof(carTelem));
  myTransfer.sendData(sizeof(carTelem));

  serverProcessing();
}




void serverProcessing()
{
  bool mainPage = false;
  bool speedCall = false;
  bool rpmCall = false;
  
  WiFiClient client = server.available();

  if (client)
  {
    String currentLine = "";
    
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        Serial.write(c);

        if (currentLine.endsWith("GET / HTTP"))
          mainPage = true;
        else if (currentLine.endsWith("GET /speed HTTP"))
          speedCall = true;
        else if (currentLine.endsWith("GET /rpm HTTP"))
          rpmCall = true;
        
        if (c == '\n')
        {
          if (!currentLine.length())
          {
            if (mainPage)
            {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println();
        
              client.print(MAIN_page);
              client.println();
      
              mainPage = false;
              break;
            }
            else if (speedCall)
            {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println();
        
              client.println(int(carTelem.mph + 0.5));
              client.println();
      
              speedCall = false;
              break;
            }
            else if (rpmCall)
            {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println();
        
              client.println(map(carTelem.rpm, MIN_RPM, MAX_RPM, 0, 100));
              client.println();
      
              rpmCall = false;
              break;
            }
            else
            {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println();
        
              client.print(MAIN_page);
              client.println();
              break;
            }
          }
          else
            currentLine = "";
        }
        else if (c != '\r')
          currentLine += c;
      }
    }

    client.stop();
  }
}




void printError()
{
  DEBUG_PORT.print("Received: ");
  for (byte i = 0; i < PAYLOAD_LEN; i++)
    DEBUG_PORT.write(myELM327.payload[i]);
  DEBUG_PORT.println();
  
  if (myELM327.status == ELM_SUCCESS)
    DEBUG_PORT.println(F("\tELM_SUCCESS"));
  else if (myELM327.status == ELM_NO_RESPONSE)
    DEBUG_PORT.println(F("\tERROR: ELM_NO_RESPONSE"));
  else if (myELM327.status == ELM_BUFFER_OVERFLOW)
    DEBUG_PORT.println(F("\tERROR: ELM_BUFFER_OVERFLOW"));
  else if (myELM327.status == ELM_UNABLE_TO_CONNECT)
    DEBUG_PORT.println(F("\tERROR: ELM_UNABLE_TO_CONNECT"));
  else if (myELM327.status == ELM_NO_DATA)
    DEBUG_PORT.println(F("\tERROR: ELM_NO_DATA"));
  else if (myELM327.status == ELM_STOPPED)
    DEBUG_PORT.println(F("\tERROR: ELM_STOPPED"));
  else if (myELM327.status == ELM_TIMEOUT)
    DEBUG_PORT.println(F("\tERROR: ELM_TIMEOUT"));
  else if (myELM327.status == ELM_TIMEOUT)
    DEBUG_PORT.println(F("\tERROR: ELM_GENERAL_ERROR"));

  delay(100);
}


