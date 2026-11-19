# IntelliCart – Smart Shopping Trolley

### An RFID-Based Automated Shopping and Billing System Using ESP32 and Firebase

IntelliCart is a smart shopping trolley designed to simplify the traditional shopping experience through RFID-based product identification, automated billing and digital payment integration.

The system uses an ESP32 microcontroller to manage product scanning, shopping cart operations and bill calculation. By integrating RFID technology, a TFT touchscreen, Firebase Realtime Database and QR code generation, IntelliCart provides customers with a convenient and interactive shopping experience.

---

## Project Overview

Traditional shopping often involves manually checking product prices, waiting in long billing queues and depending on cashier-operated billing systems.

IntelliCart addresses these challenges by allowing customers to scan products directly using an RFID reader, view product details on an interactive touchscreen and calculate the total bill automatically.

The system also supports UPI and cash payment options, along with digital bill generation after payment.

## Objectives

- Reduce waiting time at traditional billing counters.
- Automate product identification using RFID technology.
- Provide real-time product information and bill calculation.
- Integrate cloud-based product management using Firebase.
- Provide an interactive touchscreen interface.
- Enable convenient payment and digital bill generation.

---

## Working Principle

### 1. System Initialization

The ESP32 initializes the connected hardware peripherals and establishes Wi-Fi connectivity for communication with Firebase.

### 2. RFID-Based Product Identification

When a customer scans a product, the RFID reader captures its unique RFID tag ID and sends it to the ESP32.

### 3. Product Information Retrieval

The ESP32 communicates with Firebase Realtime Database to retrieve the corresponding product information, including product name, price and quantity details.

### 4. Shopping Cart Management

The retrieved product information is displayed on the TFT touchscreen. Customers can add products to their cart and use the touchscreen removal option to remove unwanted items.

### 5. Automatic Bill Calculation

The ESP32 manages the shopping cart and calculates the total amount based on the products added.

### 6. Payment Selection

After completing shopping, customers can choose between two payment methods:

- **UPI Payment:** A payment QR code is generated using the configured UPI details. Customers can scan it using a supported UPI application.
- **Cash Payment:** Customers can choose cash payment and complete the transaction at the billing counter.

### 7. Digital Bill Generation

After payment, the system generates a digital bill through a web-based interface. A QR code is provided to customers to access and download their shopping bill.

---

## Hardware Components

| Component | Function |
|---|---|
| ESP32 Development Board | Main microcontroller and system controller |
| RFID Reader (MFRC522) | Product identification using RFID tags |
| RFID Tags | Unique identification of products |
| 3.5-inch TFT Touch Display | Customer interaction and shopping cart display |
| Buzzer | Audio alerts and notifications |
| Battery | Power supply for the system |

---

## Software and Technologies

| Technology | Application |
|---|---|
| Arduino C | ESP32 programming and system functionality |
| Arduino IDE | Firmware development and uploading |
| ESP32 | Embedded control and Wi-Fi connectivity |
| Firebase Realtime Database | Cloud-based product data storage and retrieval |
| TFT_eSPI | TFT display interface |
| MFRC522 Library | RFID communication |
| QR Code Library | QR code generation |
| HTML | Digital bill web interface |

---

## Key Features

- **RFID-Based Product Scanning:** Identifies products through unique RFID tags.
- **Cloud-Based Product Database:** Retrieves product information from Firebase.
- **Interactive Touchscreen:** Displays product information, cart items and billing details.
- **Add and Remove Products:** Allows customers to manage shopping cart items.
- **Automatic Bill Calculation:** Calculates the total shopping amount.
- **UPI QR Payment:** Provides QR-based UPI payment using configured UPI details.
- **Cash Payment Option:** Supports cash payment at the billing counter.
- **Digital Bill Generation:** Generates a downloadable digital bill after payment.
- **Wireless Connectivity:** Uses ESP32 Wi-Fi for cloud communication.

---

## Future Scope

The following improvements are planned for future development:

### 1. Automatic Product Addition

Develop a system that detects when a customer places a product inside the trolley and automatically identifies and adds the product to the shopping cart without requiring manual RFID scanning.

### 2. Automatic Product Removal

Implement automatic detection when a customer takes a product out of the trolley. The system will identify the removed product and automatically update the shopping cart and bill.

### 3. Smart Cart Security

Develop a security mechanism to monitor products entering and leaving the trolley. This will help prevent discrepancies between the physical cart contents and the digital bill and improve shopping security.

### 4. Inventory Management

Integrate stock monitoring and inventory management features for improved supermarket operations.

### 5. Enhanced Payment Verification

Implement automated payment verification and transaction management for a more reliable checkout process.

---

## Project Information

**Project Name:** IntelliCart – Smart Shopping Trolley

**Domain:** Embedded Systems | IoT | Smart Retail Automation

**Controller:** ESP32

**Database:** Firebase Realtime Database

**Development Platform:** Arduino IDE

**Project Type:** Academic Mini Project

---

## Acknowledgement

This project was developed as part of an academic mini-project with the objective of applying embedded systems, IoT communication and automation technologies to improve the traditional shopping experience.

---

If you find this project interesting, feel free to explore the repository and its implementation.
