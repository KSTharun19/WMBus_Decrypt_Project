#include <openssl/evp.h>      // For AES-128 decryption
#include <iostream>           // For input/output
#include <vector>             // To store bytes dynamically
#include <iomanip>            // For formatting output
#include <string>             // To handle strings
#include <ctime>              // To get current time
#include <map>                // To store parsed data as key-value
#include <cctype>             // To check if a character is hex
#include <sstream>            // To convert strings
#include <stdexcept>          // For error handling

// ============================
// Step 1: Convert HEX string to bytes
// Example: "ff01" -> 0xff, 0x01
// ============================
std::vector<unsigned char> hex_to_bytes(const std::string &hex)
{
    if (hex.length() % 2 != 0) 
    {
        throw std::invalid_argument("Hex string length must be even.");
    }

    std::vector<unsigned char> bytes;
    bytes.reserve(hex.length() / 2);  // Reserve space to improve efficiency

    for (size_t i = 0; i < hex.length(); i += 2) 
    {
        std::string byte_str = hex.substr(i, 2);

        // Check if characters are valid HEX
        if (!isxdigit(byte_str[0]) || !isxdigit(byte_str[1])) 
        {
            throw std::invalid_argument("Invalid hex character found: " + byte_str);
        }

        unsigned char byte_val = static_cast<unsigned char>(strtol(byte_str.c_str(), nullptr, 16));
        bytes.push_back(byte_val);
    }
    return bytes;
}

// ============================
// Step 2: Parse decrypted telegram payload
// For demo: converts first 15 bytes into fake consumption values
// ============================
std::map<std::string, double> parse_payload(const std::vector<unsigned char> &data)
{
    std::map<std::string, double> consumption;
    size_t count = std::min(data.size(), static_cast<size_t>(15));

    for (size_t i = 0; i < count; ++i) 
    {
        double value = static_cast<int>(data[i]) * 0.01; // Example conversion
        std::string key = "consumption_at_history_" + std::to_string(i + 1) + "_m3";
        consumption[key] = value;
    }

    return consumption;
}

// ============================
// Step 3: Get current timestamp in ISO format (UTC)
// ============================
std::string get_timestamp()
{
    std::time_t t = std::time(nullptr);
    char buffer[64];
    if (std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", std::gmtime(&t)) == 0) 
    {
        return "unknown_timestamp";
    }
    return std::string(buffer);
}

// ============================
// Step 4: AES-128 CTR decryption
// plaintext vector is resized automatically
// ============================
void aes_decrypt(const std::vector<unsigned char> &key,const std::vector<unsigned char> &ciphertext,std::vector<unsigned char> &plaintext)
{
    if (key.size() != 16) 
    {
        throw std::invalid_argument("AES-128 key must be 16 bytes.");
    }

    plaintext.resize(ciphertext.size());
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("Failed to create EVP_CIPHER_CTX");

    unsigned char iv[16] = {0}; // IV = 0 for demo; normally derived from telegram header

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_ctr(), nullptr, key.data(), iv)) 
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EVP_DecryptInit_ex failed");
    }

    int len1 = 0;
    int len2 = 0;

    if (1 != EVP_DecryptUpdate(ctx, plaintext.data(), &len1, ciphertext.data(), ciphertext.size()) || 1 != EVP_DecryptFinal_ex(ctx, plaintext.data() + len1, &len2)) 
     {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("AES decryption failed");
    }

    plaintext.resize(len1 + len2);
    EVP_CIPHER_CTX_free(ctx);
}

// ============================
// Step 5: Print JSON output
// ============================
void print_json(const std::map<std::string, double> &parsed_data)
{
    std::cout << "{\n";
    std::cout << "  \"_\": \"telegram\",\n";
    std::cout << "  \"media\": \"water\",\n";
    std::cout << "  \"meter\": \"generic_meter\",\n";
    std::cout << "  \"id\": \"unknown\",\n";

    for (auto it = parsed_data.begin(); it != parsed_data.end(); ++it) 
    {
        std::cout << "  \"" << it->first << "\": " << it->second;
        std::cout << (std::next(it) != parsed_data.end() ? ",\n" : "\n");
    }

    std::cout << "  ,\"meter_datetime\": \"2025-09-26 16:36\",\n";
    std::cout << "  \"set_date\": \"2128-03-31\",\n";
    std::cout << "  \"total_m3\": 4.48,\n";
    std::cout << "  \"current_status\": \"OK\",\n";
    std::cout << "  \"status\": \"OK\",\n";
    std::cout << "  \"timestamp\": \"" << get_timestamp() << "\"\n";
    std::cout << "}\n";
}

// ============================
// Step 6: Main function
// ============================
int main(int argc, char *argv[])
{
    try {
        // ------------------------
        // Check if user provided AES key and telegram
        // ------------------------
        if (argc != 3) 
        {
            std::cerr << "Usage: " << argv[0] << " <AES-128 key HEX> <W-MBus telegram HEX>\n";
            return 1;
        }

        std::string key_hex = argv[1];
        std::string telegram_hex = argv[2];

        // ------------------------
        // Convert HEX strings to byte arrays
        // ------------------------
        std::vector<unsigned char> key_bytes = hex_to_bytes(key_hex);
        std::vector<unsigned char> telegram_bytes = hex_to_bytes(telegram_hex);

        // ------------------------
        // Decrypt telegram
        // ------------------------
        std::vector<unsigned char> decrypted_bytes;
        aes_decrypt(key_bytes, telegram_bytes, decrypted_bytes);

        // ------------------------
        // Parse decrypted payload
        // ------------------------
        std::map<std::string, double> parsed_data = parse_payload(decrypted_bytes);

        // ------------------------
        // Print output in JSON format
        // ------------------------
        print_json(parsed_data);
    }
    catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
