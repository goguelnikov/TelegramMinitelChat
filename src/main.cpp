/******************************************************************************
    A telegram terminal for Minitel ESP32 for my kids ;-)
    hackaday : https://hackaday.io/project/192211-minitel-telegram-client
    by goguelnikov

    Minitel ESP32 by iodeo
    hackaday : https://hackaday.io/project/180473-minitel-esp32
    Tindie: https://www.tindie.com/products/iodeo/minitel-esp32-dev-board/?utm_source=hackadayio&utm_medium=link&utm_campaign=project_buynow
    github: https://github.com/iodeo/Minitel-ESP32 
    
    Minitel library by eserandour
    github: https://github.com/eserandour/Minitel1B_Hard
    
    Universal-Arduino-Telegram-Bot by Brian Lough
    github: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
    YouTube: https://www.youtube.com/brianlough
    Tindie: https://www.tindie.com/stores/brianlough/
    Twitter: https://twitter.com/witnessmenow
 ******************************************************************************/

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h> // see https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <Minitel1B_Hard.h>  // see https://github.com/eserandour/Minitel1B_Hard

// init Minitel ESP32
Minitel minitel(Serial2);    // ESP32 serial port 2

#define TITRE "Telegram"
String texte = "";
int nbCaracteres = 0;
const int PREMIERE_LIGNE = 2;
const int NB_LIGNES = 3;
const String VIDE = ".";
unsigned long touche;
String intro = "Moi - ";

// Wifi network station credentials
#define WIFI_SSID "XXXXXXXXXX"
#define WIFI_PASSWORD "XXXXXXXXXX"

// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "XXXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you

#define CHAT_ID "XXXXXXXXXXXXX"

// to make the bot react to a specific group
// - get the group ID by adding the @myidbot to the group
// - send /getgroupid command 
// - paste the ID in CHAT_ID variable
// - make sure to have Privacy mode disabled for the bot in bot settings accessible in @botfather /mybots

const unsigned long BOT_MTBS = 1000; // mean time between scan messages

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime; // last time messages' scan has been done

void champVide(int premiereLigne, int nbLignes)
{
  minitel.noCursor();

  //minitel.moveCursorDown(1);
  //minitel.moveCursorXY(1,premiereLigne);
  //minitel.clearScreenFromCursor();

  for (int j=0; j<nbLignes; j++) {
    minitel.attributs(CARACTERE_BLEU);
    minitel.print(VIDE);
    minitel.repeat(39);
    minitel.attributs(CARACTERE_BLANC);
  }

  for (int j=0; j<nbLignes; j++) {
    minitel.moveCursorUp(1);
  }

  //texte="";
  //nbCaracteres=0;

  //minitel.moveCursorXY(31,24);
  //minitel.print("puis ");
  //minitel.attributs(INVERSION_FOND);
  //minitel.print("ENVOI");
  //minitel.attributs(FOND_NORMAL);
  //minitel.moveCursorDown(premiereLigne-1);
  //minitel.moveCursorXY(1,premiereLigne);
  minitel.cursor();

}

void newPage(String titre) {
  minitel.newScreen();


  minitel.attributs(INVERSION_FOND);
  minitel.attributs(CARACTERE_BLEU);
  //minitel.attributs(CARACTERE_ROUGE);  
  for (int i=0; i<40; i++) {
    minitel.print(" ");
  }
  for (int i=0; i<16; i++) {
    minitel.print(" ");
  }
  minitel.print(titre);
  for (int i=0; i<16; i++) {
    minitel.print(" ");
  }
  for (int i=0; i<40; i++) {
    minitel.print(" ");
  }
  //minitel.println(" ");
  minitel.attributs(CARACTERE_BLANC);
  minitel.attributs(FOND_NORMAL);
  minitel.println();
  //for (int i=1; i<=40; i++) {
  //  minitel.writeByte(0x7E);
  //}
}

void eraseLines(int nbLine)
{
  for (int j=0; j<nbCaracteres/40; j++) {
    minitel.moveCursorUp(1);
  }
  for (int j=0; j<nbCaracteres; j++) {
    minitel.moveCursorLeft(1);
  }

  for (int j=0; j<nbLine; j++) {
    //minitel.attributs(CARACTERE_BLEU);
    minitel.print(" ");
    minitel.repeat(39);
    //minitel.attributs(CARACTERE_BLANC);
  }
}

void handleNewMessages(int numNewMessages) {
  //Serial.println(F("handleNewMessages"));
  Serial.print(F("NB message: "));
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    Serial.print(F("From Chat_ID: "));
    String chat_id = String(bot.messages[i].chat_id);
    Serial.println(chat_id);
    Serial.print(F("Message: "));
    String text = bot.messages[i].text;
    Serial.println(text);
    Serial.print(F("From_name: "));
    String from_name = bot.messages[i].from_name;

    if (chat_id == CHAT_ID){
      //bot.sendMessage(chat_id, text, "");
      // Print the received message on Minitel
      //minitel.moveCursorXY(14,24);  
    
      //eraseLines(text.length()/40);
      minitel.println();
      minitel.attributs(INVERSION_FOND);
      //minitel.attributs(CARACTERE_BLEU);
      minitel.attributs(CARACTERE_ROUGE);
      
      minitel.print(from_name);
      minitel.print(" - ");
      minitel.println(text);
      minitel.attributs(CARACTERE_BLANC);
      minitel.attributs(FOND_NORMAL);
      //minitel.bip();
      champVide(PREMIERE_LIGNE, NB_LIGNES);
      minitel.print(intro);
      minitel.print(texte);
      //minitel.moveCursorXY(1,10);
      //minitel.moveCursorReturn(1);

    } else {
      bot.sendMessage(chat_id, "Unauthorized", "");
    }



  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println();

  // attempt to connect to Wifi network:
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Retrieving time: ");
  configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);

  Serial.print("Initialising Minitel...");  
  minitel.changeSpeed(minitel.searchSpeed());
  //minitel.modeMixte();
  minitel.smallMode();
  minitel.scrollMode();
  newPage(TITRE);
  champVide(PREMIERE_LIGNE, NB_LIGNES);
  Serial.println(" done"); 

  //Serial.print("Mintel type: "); 
  //Serial.println(minitel.identifyDevice()); 
  //minitel.modeMixte();
  //minitel.echo(false);  
  //minitel.standardKeyboard(); //block arrows in editing mode  
  //minitel.smallMode(); //uncap
  //minitel.clearScreen();
  //minitel.moveCursorXY(1,1);
  //debugPrint("Done");
  
  minitel.print(intro);

}

void loop()
{
// Telegram Part
  if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      //Serial.println("Got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }

// Minitel Part
    // reading key
    touche = minitel.getKeyCode();

    if ((touche != 0) &&
        (touche != CONNEXION_FIN) &&
        (touche != SOMMAIRE) &&
        (touche != ANNULATION) &&
        (touche != RETOUR) &&
        (touche != REPETITION) &&
        (touche != GUIDE) &&
        (touche != CORRECTION) &&
        (touche != SUITE) &&
        (touche != ENVOI)) {

      if (nbCaracteres < 40 * NB_LIGNES) {
        nbCaracteres++;
        texte += char(touche);
      }

      if (nbCaracteres == 40 * NB_LIGNES) {
        minitel.moveCursorXY(40,(PREMIERE_LIGNE - 1) + NB_LIGNES);
      }

      //Serial.print("Current text: ");
      //Serial.println(texte);
      //Serial.print("nb Car: ");
      //Serial.println(nbCaracteres);

    }

    switch (touche) {
      case ENVOI :
        // send message
        bot.sendMessage(CHAT_ID, texte, "");
        texte = "";
        nbCaracteres = 0;
        //minitel.moveCursorXY(1,10);
        champVide(PREMIERE_LIGNE, NB_LIGNES);
        minitel.moveCursorReturn(1);
        minitel.print(intro);
        break;
      case CORRECTION :
        if ((nbCaracteres > 0) && (nbCaracteres <= 40 * NB_LIGNES)) {
          if (nbCaracteres != 40 * NB_LIGNES) { 
            minitel.moveCursorLeft(1); 
          }
          minitel.attributs(CARACTERE_BLEU);
          minitel.print(VIDE);
          minitel.attributs(CARACTERE_BLANC);
          minitel.moveCursorLeft(1);
          if (nbCaracteres == 40*(NB_LIGNES-1)+1) { 
            minitel.moveCursorLeft(1); 
            //currRow--;
          }
          texte = texte.substring(0,texte.length()-1);
          nbCaracteres--;
        }
        break;
      case SOMMAIRE : 
        break;
      case SUITE : 
        break;
            case ANNULATION :
        //newPage(TITRE);
        //champVide(PREMIERE_LIGNE, NB_LIGNES);
        break;
      case GUIDE :
        break;
      case REPETITION :
        break;
    }    


  // Message de fin
  //minitel.textMode();
  //minitel.attributs(CARACTERE_NOIR);
  //minitel.moveCursorXY(7,23);
  //minitel.print("Merci de votre participation");
  //minitel.noCursor();
  //delay(4000);

}

