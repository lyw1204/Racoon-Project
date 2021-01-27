//WIFI and DB section
#define FIREBASE_HOST "https://raccoonlock-default-rtdb.firebaseio.com/" //Without http:// or https:// schemes
#define API_KEY "AIzaSyD3QWwmDnP0kIMlcZbPzi0YY0ERE4Ch9kk"

#define WIFI_SSID "DAVID NEW"
#define WIFI_PASSWORD "c=3.00*10^8m/s"
#define USER_EMAIL "racoonlock1@gmail.com"
#define USER_PASSWORD "1234567890"

char ssid [32] = WIFI_SSID;
char password [63] = WIFI_PASSWORD;



void wifiConnect()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID); Serial.println(" ...");

  int teller = 0;
  while (WiFi.status() != WL_CONNECTED)
  {                                       // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++teller); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
}



FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void firebaseHello(){

  config.host = FIREBASE_HOST;
  config.api_key = API_KEY;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);


  if(Firebase.RTDB.setInt(&fbdo, "/time", 1))
  {
    //Success
     Serial.println("Set int data success");

  }else{
    //Failed?, get the error reason from fbdo

    Serial.print("Error in setInt, ");
    Serial.println(fbdo.errorReason());
  }
  }
