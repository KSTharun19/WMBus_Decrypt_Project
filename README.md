# WMBus_Decrypt_Project
This project decrypts W-MBus telegrams from water meters using AES-128. It converts the encrypted hex data to bytes, decrypts it using AES-128 CTR mode, and prints sample meter readings in JSON format. The program shows meter status, total consumption, and timestamp in human-readable form.

Basic Terms Explained

Wireless M-Bus (wM-Bus) / W-M-Bus:
A radio protocol used by utility meters (water, gas, electricity) to send meter readings.

Telegram:
The full radio message sent by a meter. It has a header (addresses, counters), payload (meter data), and checksums.

AES-128:
A type of encryption. “128” means the key is 128 bits (16 bytes). Used to lock (encrypt) and unlock (decrypt) data.

Key / AES-128 decryption key (hexadecimal):
The secret 16-byte value used by AES to encrypt or decrypt data.

Hex / Byte / Byte-array / Binary file:

Hex: numbers 0-9 and letters A-F used to show bytes.

Byte:
8 bits, smallest unit of data in telegrams.

Byte-array:
a list of bytes.

Binary file:
a file containing bytes, not text.

Payload:
The actual meter data inside the telegram (e.g., readings, timestamp).

Metadata:
Extra fields used for decryption or interpretation (e.g., frame counter, address, manufacturer).

IV (Initialization Vector) / Nonce / Counter:
Extra data given to AES to make encryption secure. Used to make the same data look different each time.

AES Modes (CTR, CBC, CCM, GCM):
Different ways to use AES for messages larger than 16 bytes.

MAC / CMAC / Authentication tag:
A small code that proves the message was not changed and came from the correct sender.

KeyID / Application Security Profile (ASP):
A number that tells which key and AES mode was used to encrypt the message.

Link Layer / Extended Link Layer (ELL) / AFL:
Parts of the telegram protocol. They handle addressing, security, authentication, and fragmentation of messages.

Frame counter / Access number / Message counter:
A number that increases for each message to prevent replay attacks.

CRC / checksum:
A simple check to detect accidental errors in the message.

Plaintext / Ciphertext / Encrypt / Decrypt:

Plaintext: readable meter data.

Ciphertext: encrypted data.

Encrypt: convert readable data to secret code.

Decrypt: convert secret code back to readable data.

Padding (PKCS#7):
Extra bytes added to messages to make them fit block sizes (needed for some AES modes like CBC).

Libraries / Tools (OpenSSL, mbedTLS, ESP-IDF):
Software libraries used to do AES encryption and decryption.



W-MBus Telegram Structure:

A W-MBus telegram is like a small package sent by a utility meter. It has three main parts:
Header:
This is like the envelope of the package.
It contains important information such as the meter address, message counter (to know which number message this is), and manufacturer details.
The header helps the receiver know where the message comes from and how to process it.

Payload:
This is the actual content of the package, which contains the meter readings like water or electricity consumed.
The payload is often encrypted, which means it is scrambled using AES-128 so only authorized people or programs can read it.

Checksum/CRC:
This is like a small note inside the package to check for mistakes.
It helps detect if the message got damaged during transmission.


AES-128 Decryption Steps (OMS Volume 2):

Decryption is the process of turning scrambled data back into readable meter readings. Here are the steps explained simply:
Convert AES key from hexadecimal to 16 bytes:
The AES key is given as a string of hex numbers like 425579....
Each pair of hex digits represents one byte.
AES-128 needs exactly 16 bytes as the key, so we convert the hex string into 16 bytes of data.

Convert encrypted telegram from hexadecimal to byte array:
The telegram we get from the meter is also in hex.
We convert it into a byte array because computers work with bytes, not hex strings.

Initialize AES-128 in CTR mode using key and IV:
CTR mode is a method of AES that lets us decrypt any size of data without worrying about padding.
IV (Initialization Vector) is a special number used to make encryption safe. OMS Volume 2 tells us how to get IV from the telegram header.

Decrypt telegram to get plaintext payload:
Using the AES-128 key and IV, we convert the scrambled payload back into readable bytes.
Parse payload to extract meter readings:
The decrypted data still needs to be interpreted according to the meter’s format.

Parsing means extracting values like water consumed at different times and putting them in a readable format (like JSON).


How to Build and Run:

Requirements:
Linux, Windows, or macOS
C++ compiler (g++)
OpenSSL library

Build:
g++ -o wmbus_decrypt main.cpp -lssl -lcrypto

Run:
./wmbus_decrypt <AES-128 key HEX> <W-MBus telegram HEX>

Example:
input:

./wmbus_decrypt 4255794d3dccfd46953146e701b7db68 a144c5142785895070078c20607a9d00902537ca231fa2da5889Be8df367...

Example Output
{
  "_": "telegram",
  "media": "water",
  "meter": "generic_meter",
  "id": "unknown",
  "consumption_at_history_10_m3": 0.72,
  "consumption_at_history_11_m3": 0.24,
  "consumption_at_history_12_m3": 0.56,
  "consumption_at_history_13_m3": 1.83,
  "consumption_at_history_14_m3": 1.34,
  "consumption_at_history_15_m3": 1.26,
  "consumption_at_history_1_m3": 0.57,
  "consumption_at_history_2_m3": 2.26,
  "consumption_at_history_3_m3": 1.36,
  "consumption_at_history_4_m3": 1.53,
  "consumption_at_history_5_m3": 0.89,
  "consumption_at_history_6_m3": 1.87,
  "consumption_at_history_7_m3": 0.4,
  "consumption_at_history_8_m3": 1.49,
  "consumption_at_history_9_m3": 0.12,
  "meter_datetime": "2025-09-26 16:36",
  "set_date": "2128-03-31",
  "total_m3": 4.48,
  "current_status": "OK",
  "status": "OK",
  "timestamp": "2025-10-02T14:40:00Z"
}
tharun@LAPTOP-SDBGJLIB:~/Internshala$ 
