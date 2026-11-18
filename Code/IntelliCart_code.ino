#include <WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <TFT_eSPI.h>

#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// =====================================================
// WIFI
// =====================================================

#define WIFI_SSID       "Airtel_wifi"
#define WIFI_PASSWORD   "password"

// =====================================================
// FIREBASE
// =====================================================

#define API_KEY         "Apikey"
#define DATABASE_URL    "https://database_URL"

#define USER_EMAIL      "Mail_ID"
#define USER_PASSWORD   "Password"

// =====================================================
// RFID MFRC522
// =====================================================

#define RFID_SS_PIN     5
#define RFID_RST_PIN    22

MFRC522 rfid(RFID_SS_PIN, RFID_RST_PIN);

// =====================================================
// BUZZER
// =====================================================

#define BUZZER_PIN      27

// =====================================================
// TFT
// =====================================================

TFT_eSPI tft = TFT_eSPI();

// =====================================================
// FIREBASE OBJECTS
// =====================================================

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// =====================================================
// CART
// =====================================================

#define MAX_CART_ITEMS 15

struct CartItem
{
  String uid;
  String name;
  float price;
  int quantity;
};

CartItem cart[MAX_CART_ITEMS];

int cartCount = 0;

float totalPrice = 0.0;

// =====================================================
// FUNCTION DECLARATIONS
// =====================================================

void connectWiFi();
void initializeFirebase();

String getRFIDUID();

bool getProductFromFirebase(
  String uid,
  String &productName,
  float &productPrice
);

void addToCart(
  String uid,
  String productName,
  float productPrice
);

void calculateTotal();

void drawCart();

void beepSuccess();
void beepError();

void showMessage(
  String line1,
  String line2
);

// =====================================================
// SETUP
// =====================================================

void setup()
{
  Serial.begin(115200);

  // ---------------------------------------------------
  // Buzzer
  // ---------------------------------------------------

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // ---------------------------------------------------
  // TFT
  // ---------------------------------------------------

  tft.init();
  tft.setRotation(1);

  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  tft.setTextSize(2);

  tft.setCursor(80, 120);
  tft.println("INTELLI CART");

  tft.setCursor(100, 155);
  tft.println("Starting...");

  delay(1500);

  // ---------------------------------------------------
  // SPI
  // ---------------------------------------------------

  SPI.begin();

  // ---------------------------------------------------
  // RFID
  // ---------------------------------------------------

  rfid.PCD_Init();

  delay(100);

  Serial.println();
  Serial.println("RFID Reader Ready");

  // ---------------------------------------------------
  // WiFi
  // ---------------------------------------------------

  connectWiFi();

  // ---------------------------------------------------
  // Firebase
  // ---------------------------------------------------

  initializeFirebase();

  // ---------------------------------------------------
  // Initial screen
  // ---------------------------------------------------

  drawCart();

  Serial.println();
  Serial.println("================================");
  Serial.println("       INTELLI CART READY");
  Serial.println("================================");
  Serial.println("Scan a product...");
}

// =====================================================
// WIFI CONNECTION
// =====================================================

void connectWiFi()
{
  Serial.println();
  Serial.print("Connecting to WiFi");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);

    Serial.print(".");

  }

  Serial.println();

  Serial.println("WiFi Connected!");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// =====================================================
// FIREBASE INITIALIZATION
// =====================================================

void initializeFirebase()
{
  Serial.println();
  Serial.println("Connecting to Firebase...");

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);

  Firebase.reconnectWiFi(true);

  Serial.println("Firebase initialized.");
}

// =====================================================
// MAIN LOOP
// =====================================================

void loop()
{
  // ---------------------------------------------------
  // Check Firebase connection
  // ---------------------------------------------------

  if (Firebase.ready())
  {
    // Firebase is ready
  }

  // ---------------------------------------------------
  // Check RFID
  // ---------------------------------------------------

  if (!rfid.PICC_IsNewCardPresent())
  {
    return;
  }

  if (!rfid.PICC_ReadCardSerial())
  {
    return;
  }

  // ---------------------------------------------------
  // Get RFID UID
  // ---------------------------------------------------

  String uid = getRFIDUID();

  Serial.println();
  Serial.println("----------------------------");
  Serial.print("RFID UID: ");
  Serial.println(uid);

  // ---------------------------------------------------
  // Stop RFID communication
  // ---------------------------------------------------

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  // ---------------------------------------------------
  // Firebase Product Data
  // ---------------------------------------------------

  String productName;
  float productPrice;

  bool productFound = getProductFromFirebase(
    uid,
    productName,
    productPrice
  );

  // ---------------------------------------------------
  // Product not found
  // ---------------------------------------------------

  if (!productFound)
  {
    Serial.println("Product not found in Firebase.");

    showMessage(
      "PRODUCT NOT FOUND",
      uid
    );

    beepError();

    delay(1500);

    drawCart();

    return;
  }

  // ---------------------------------------------------
  // Product found
  // ---------------------------------------------------

  Serial.println("Product Found!");

  Serial.print("Name: ");
  Serial.println(productName);

  Serial.print("Price: Rs.");
  Serial.println(productPrice);

  // ---------------------------------------------------
  // Add to cart
  // ---------------------------------------------------

  addToCart(
    uid,
    productName,
    productPrice
  );

  // ---------------------------------------------------
  // Beep
  // ---------------------------------------------------

  beepSuccess();

  // ---------------------------------------------------
  // Display updated cart
  // ---------------------------------------------------

  drawCart();

  delay(1200);
}

// =====================================================
// GET RFID UID
// =====================================================

String getRFIDUID()
{
  String uid = "";

  for (byte i = 0; i < rfid.uid.size; i++)
  {
    if (rfid.uid.uidByte[i] < 0x10)
    {
      uid += "0";
    }

    uid += String(
      rfid.uid.uidByte[i],
      HEX
    );
  }

  uid.toUpperCase();

  return uid;
}

// =====================================================
// GET PRODUCT FROM FIREBASE
// =====================================================

bool getProductFromFirebase(
  String uid,
  String &productName,
  float &productPrice
)
{
  if (!Firebase.ready())
  {
    Serial.println("Firebase not ready.");

    return false;
  }

  // ---------------------------------------------------
  // Product path
  //
  // Products/<RFID UID>
  // ---------------------------------------------------

  String path = "/Products/" + uid;

  Serial.print("Firebase Path: ");
  Serial.println(path);

  // ---------------------------------------------------
  // Read product name
  // ---------------------------------------------------

  String namePath = path + "/name";

  if (!Firebase.RTDB.getString(
        &fbdo,
        namePath
      ))
  {
    Serial.print("Name read failed: ");
    Serial.println(fbdo.errorReason());

    return false;
  }

  productName = fbdo.stringData();

  // ---------------------------------------------------
  // Read product price
  // ---------------------------------------------------

  String pricePath = path + "/price";

  if (!Firebase.RTDB.getFloat(
        &fbdo,
        pricePath
      ))
  {
    Serial.print("Price read failed: ");
    Serial.println(fbdo.errorReason());

    return false;
  }

  productPrice = fbdo.floatData();

  return true;
}

// =====================================================
// ADD PRODUCT TO CART
// =====================================================

void addToCart(
  String uid,
  String productName,
  float productPrice
)
{
  // ---------------------------------------------------
  // Check whether product already exists
  // ---------------------------------------------------

  for (int i = 0; i < cartCount; i++)
  {
    if (cart[i].uid == uid)
    {
      cart[i].quantity++;

      Serial.println("Existing product.");

      Serial.print("New Quantity: ");
      Serial.println(cart[i].quantity);

      calculateTotal();

      return;
    }
  }

  // ---------------------------------------------------
  // Add new product
  // ---------------------------------------------------

  if (cartCount >= MAX_CART_ITEMS)
  {
    Serial.println("Cart is full!");

    return;
  }

  cart[cartCount].uid = uid;

  cart[cartCount].name = productName;

  cart[cartCount].price = productPrice;

  cart[cartCount].quantity = 1;

  cartCount++;

  Serial.println("New product added to cart.");

  calculateTotal();
}

// =====================================================
// CALCULATE TOTAL
// =====================================================

void calculateTotal()
{
  totalPrice = 0.0;

  for (int i = 0; i < cartCount; i++)
  {
    totalPrice +=
      cart[i].price *
      cart[i].quantity;
  }

  Serial.print("TOTAL = Rs.");
  Serial.println(totalPrice);
}

// =====================================================
// DRAW CART ON TFT
// =====================================================

void drawCart()
{
  tft.fillScreen(TFT_BLACK);

  // ---------------------------------------------------
  // HEADER
  // ---------------------------------------------------

  tft.setTextColor(TFT_CYAN, TFT_BLACK);

  tft.setTextSize(2);

  tft.setCursor(10, 10);

  tft.println("INTELLI CART");

  // ---------------------------------------------------
  // LINE
  // ---------------------------------------------------

  tft.drawFastHLine(
    10,
    38,
    460,
    TFT_WHITE
  );

  // ---------------------------------------------------
  // CART ITEMS
  // ---------------------------------------------------

  int y = 50;

  tft.setTextSize(2);

  if (cartCount == 0)
  {
    tft.setTextColor(
      TFT_WHITE,
      TFT_BLACK
    );

    tft.setCursor(100, 130);

    tft.println("SCAN PRODUCT");

    tft.setCursor(115, 160);

    tft.println("TO ADD");

  }
  else
  {
    for (int i = 0; i < cartCount; i++)
    {
      // ------------------------------------------------
      // Product name
      // ------------------------------------------------

      tft.setTextColor(
        TFT_WHITE,
        TFT_BLACK
      );

      tft.setCursor(10, y);

      String displayName =
        cart[i].name;

      if (displayName.length() > 15)
      {
        displayName =
          displayName.substring(0, 15);
      }

      tft.print(displayName);

      // ------------------------------------------------
      // Quantity
      // ------------------------------------------------

      tft.setCursor(220, y);

      tft.print("x");
      tft.print(cart[i].quantity);

      // ------------------------------------------------
      // Price
      // ------------------------------------------------

      tft.setCursor(300, y);

      float itemTotal =
        cart[i].price *
        cart[i].quantity;

      tft.print("Rs.");
      tft.print(itemTotal, 2);

      y += 30;

      // ------------------------------------------------
      // Prevent overflow
      // ------------------------------------------------

      if (y > 220)
      {
        break;
      }
    }
  }

  // ---------------------------------------------------
  // TOTAL AREA
  // ---------------------------------------------------

  tft.drawFastHLine(
    10,
    245,
    460,
    TFT_WHITE
  );

  tft.setTextColor(
    TFT_YELLOW,
    TFT_BLACK
  );

  tft.setTextSize(3);

  tft.setCursor(10, 260);

  tft.print("TOTAL");

  tft.setCursor(240, 260);

  tft.print("Rs.");

  tft.print(totalPrice, 2);

  // ---------------------------------------------------
  // FOOTER
  // ---------------------------------------------------

  tft.setTextSize(1);

  tft.setTextColor(
    TFT_GREEN,
    TFT_BLACK
  );

  tft.setCursor(10, 305);

  tft.println("Scan RFID to add product");
}

// =====================================================
// DISPLAY MESSAGE
// =====================================================

void showMessage(
  String line1,
  String line2
)
{
  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(
    TFT_RED,
    TFT_BLACK
  );

  tft.setTextSize(2);

  tft.setCursor(70, 100);

  tft.println(line1);

  tft.setTextColor(
    TFT_WHITE,
    TFT_BLACK
  );

  tft.setCursor(100, 140);

  tft.println(line2);
}

// =====================================================
// SUCCESS BEEP
// =====================================================

void beepSuccess()
{
  digitalWrite(
    BUZZER_PIN,
    HIGH
  );

  delay(100);

  digitalWrite(
    BUZZER_PIN,
    LOW
  );
}

// =====================================================
// ERROR BEEP
// =====================================================

void beepError()
{
  digitalWrite(
    BUZZER_PIN,
    HIGH
  );

  delay(300);

  digitalWrite(
    BUZZER_PIN,
    LOW
  );
}