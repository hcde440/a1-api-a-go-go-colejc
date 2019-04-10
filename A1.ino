//A1


//We include libraries to allow for different functions in our code
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h> //provides the ability to parse and construct JSON objects

//We create a few variables to allow us to quickly change WiFi networks, and access API keys
//const char* ssid = "Samsung Galaxy S9_1914";
//const char* pass = "thga9095";
const char* ssid = "Sif";
const char* pass = "appletree3";
//const char* parkkey = "4OS246wdONnQXKftNsOxXxbg04uY3m24sM6IrvpB";
const char* timekey = "XkMPwXq63pHfG4VvIhDUmDBnrEO4lsAo";
//const char* parkCode = "acad";

//Two typedef structs, specialized to hold the information retrieved by API calls
typedef struct {
  String joke;
} ChuckJoke;
typedef struct {
  String link;
  String snippet;
} TimesData;

//The two instances of typedef structs we will use
ChuckJoke joke;
TimesData times;

void setup() {
  //These lines of code start up the process, and output the location of the file the machine is running
  //as well as the date and time of upload.
  Serial.begin(115200);
  delay(10);
  Serial.print("This board is running: ");
  Serial.println(F(__FILE__));
  Serial.print("Compiled: ");
  Serial.println(F(__DATE__ " " __TIME__));

  //We output a "connecting to ___" message, and attempt to connect to WiFi. While we aren't connected,
  //a while loop outputs a "." every half second so we know the machine is in the process of connecting.
  Serial.print("Connecting to "); Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //Once we connect, we output a confirmation and the device IP address.
  Serial.println(); Serial.println("WiFi connected"); Serial.println();
  Serial.print("Your ESP has been assigned the internal IP address ");
  Serial.println(WiFi.localIP());

  //Call our two methods which will contact APIs and fill in our typedef structs
  getJoke();
  getTimes();

  //The output of this code, containing the information retrieved from each API
  Serial.println();
  Serial.println("-----------------------------------");
  Serial.println();
  Serial.println("Here's what's going on in the news: " + times.link);
  Serial.println();
  Serial.println(times.snippet);
  Serial.println();
  Serial.println("And here's a bad joke: ");
  Serial.println();
  Serial.println(joke.joke);
  Serial.println();
  Serial.println("-----------------------------------");
}

void getJoke() {
  //This method calls a Chuck Norris joke API for a random joke.
  HTTPClient theClient;
  Serial.println("Making HTTP request");
  String apiCall = "http://api.icndb.com/jokes/random";

  //Collect the information from the URL, and check to make sure we haven't encountered any errors.
  theClient.begin(apiCall);
  int httpCode = theClient.GET();
  if (httpCode > 0) {
    if (httpCode == 200) {

      //Print a line that lets us know the correct payload has been received, and organize the information
      //we've grabbed from online in JSON format in the same way we did for getIP().
      Serial.println("Received HTTP payload.");
      DynamicJsonBuffer jsonBuffer;
      String payload = theClient.getString();
      Serial.println("Parsing...");
      JsonObject& root = jsonBuffer.parse(payload);

      // Test if parsing succeeds, and if it didn't, output a notice.
      if (!root.success()) {
        Serial.println("parseObject() failed");
        Serial.println(payload);
        return;
      }

      //Fill in the joke String in the joke instance
      joke.joke = root["value"]["joke"].as<String>();

    //Output a line in case httpCode is the incorrect value.
    } else {
      Serial.println("Something went wrong with connecting to the endpoint.");
    }
  }
}

void getTimes() {
  //This method grabs a top story from the NYT API.
  HTTPClient theClient;
  Serial.println("Making HTTP request #2");
  String apiCall = "http://api.nytimes.com/svc/search/v2/articlesearch.json?q=&api-key=";
  apiCall += timekey;

  //Collect the information from the URL, and check to make sure we haven't encountered any errors.
  theClient.begin(apiCall);
  int httpCode = theClient.GET();
  if (httpCode > 0) {
    if (httpCode == 200) {

      //Print a line that lets us know the correct payload has been received, and organize the information
      //we've grabbed from online in JSON format in the same way we did for getIP().
      Serial.println("Received HTTP payload.");
      DynamicJsonBuffer jsonBuffer;
      String payload = theClient.getString();
      Serial.println("Parsing...");
      JsonObject& root = jsonBuffer.parse(payload);

      // Test if parsing succeeds, and if it didn't, output a notice.
      if (!root.success()) {
        Serial.println("parseObject() failed");
        return;
      }

      //Fill in the link to the article and the attached snippet, to be output later.
      times.link = root["response"]["docs"][0]["web_url"].as<String>();
      times.snippet = root["response"]["docs"][0]["snippet"].as<String>();

    //Output a line in case httpCode is the incorrect value.
    } else {
      Serial.println("Something went wrong with connecting to the endpoint.");
    }
  }
}






void loop() {
  //Don't loop anything.
}

//This code grabs our IP address and returns it.
String getIP() {
  //Create variables and grab our IP address listed inside inside a web address, written in JSON.
  HTTPClient theClient;
  String ipAddress;
  theClient.begin("http://api.ipify.org/?format=json");
  int httpCode = theClient.GET();

  //We make sure we don't have an error code by checking if httpCode is greater than zero, and then if
  //it is equal to 200.
  if (httpCode > 0) {
    if (httpCode == 200) {

      //We create jsonBuffer and a string from the web address called payload. We then parse through this
      //string and organize it in JSON format, and fill in "ipAddress" with the given IP using the JSON
      //data structure.
      DynamicJsonBuffer jsonBuffer;
      String payload = theClient.getString();
      JsonObject& root = jsonBuffer.parse(payload);
      ipAddress = root["ip"].as<String>();

    //If we made an error, print something to let us know.
    } else {
      Serial.println("Something went wrong with connecting to the endpoint.");
      return "error";
    }
  }
  //Send the IP address back to any line that called getIP().
  return ipAddress;
}
