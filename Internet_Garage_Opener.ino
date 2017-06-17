/* Open your garage door with an arduino
   Complete instructions can be found at 
   http://www.instructables.com/member/kschroeter/
*/

#include <Ethernet.h>
#include <SPI.h>

  // whether or not the client was connected previously
  boolean reading = false; 
  // if need to change the MAC address (Very Rare)
  byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
  // choose a static IP that is within range of your router
  byte ip[] = { X, X, X, X };
  // integers to work with the garage and switch
  int motorPin = 7;
  int buttonPin = 5;
  int buttonState = 0;
  //Port 80 is defualt for HTTP
  EthernetServer server = EthernetServer(80);

void setup(){
  /* To find an IP that will work comment in this chunk of code and comment
  out "byte ip[] = { X, X, X, X };" "Ethernet.begin(mac, ip);" then open 
  up your serial and use the IP it gives you into ""byte ip[] = { X, X, X, X };"
  comment this code back out and comment the others back in. 
  
  Serial.begin(9600);
  Ethernet.begin(mac);
  Serial.println(Ethernet.localIP());
  */
  
  //Initialize pins on the arduino
  pinMode(motorPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  //Inithialize Ethernet library and network settings
  Ethernet.begin(mac, ip);
  //Sever is now listening for connections
  server.begin();
}

void loop(){

  //listen for incoming clients, and process request.
  checkForClient();

}

void checkForClient(){

  EthernetClient client = server.available();
  
  //if the server is available...
  if (client == true) {

    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    boolean sentHeader = false;
    
    //as long as we have a connection...
    while (client.connected()) {
      //and if there are bytes available to be read...
      if (client.available()) {
        //this is currently not false (true)
        if(!sentHeader){
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          // now it is not true
          sentHeader = true;
        }
        
        // read your input
        char c = client.read();
        // don't read if its blank
        if(reading && c == ' ') reading = false;
        // find the ? to start reading
        if(c == '?') reading = true;
        
        // if the ? has been found
        if(reading){

           switch (c) {
             
            // type ?C to close the garage door
            case 'C':
              client.print("Closing your garage door ");
              client.print("<br>");
              triggerPin(motorPin, client);
              break;
            
            // type ?O to open the garage door  
            case 'O':
              client.print("Opening your garage door ");
              client.print("<br>");
              triggerPin(motorPin, client);
              break;
            
            // type ?R to read the state of your garage 
            case 'R':
              buttonState = digitalRead(buttonPin);
              if (buttonState == HIGH) {
                client.print("Your garage door is closed ");
                client.print("<br>"); 
              }
              else {
                client.print("Sorry, your request could not be handled ");
                client.print("<br>");
              }
              break;
          }

        }

        if (c == '\n' && currentLineIsBlank)  break;

        if (c == '\n') {
          currentLineIsBlank = true;
        }else if (c != '\r') {
          currentLineIsBlank = false;
        }

      }
    }

    delay(1); // give the web browser time to receive the data
    client.stop(); // close the connection:

  } 

}

void triggerPin(int pin, EthernetClient client){
//Secretly, close and open do the same thing just with a different reply :)
//EthernetClient client is only needed if you want to add an HTML response here
  digitalWrite(pin, HIGH);
  delay(600);
  digitalWrite(pin, LOW);
}
