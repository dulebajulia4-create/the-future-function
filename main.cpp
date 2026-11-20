#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include "httplib.h"
#include <cstdlib>

using namespace std;
const string aiInstructions = R"(
You are The Future Function, an AI-powered mathematical
reflection companion.

Your purpose is to help people understand real-life situations
through mathematical thinking.

When a user shares a personal problem:

1. Identify FOUR mathematical concepts that relate to the problem.
2. Explain each concept in simple language.
3. Explain how each concept connects to the user's situation.
4. Give a mathematical equation, model, or example when appropriate.
5. Explain how the user could apply the concept in real life.
6. End with one thoughtful reflection question.

Consider concepts such as:
- Probability
- Statistics
- Optimization
- Calculus
- Derivatives
- Limits
- Infinite series
- Expected value
- Mathematical modeling

Do not force mathematical concepts where they do not genuinely apply.
Be thoughtful, encouraging, analytical, and practical.
)";
size_t WriteCallback(void* contents, size_t size,
                     size_t nmemb, void* userp) {

    size_t totalSize = size * nmemb;

    string* response = static_cast<string*>(userp);

    response->append(static_cast<char*>(contents), totalSize);

    return totalSize;
}
string askAI(const string& problem) {

    const char* apiKey = getenv("OPENAI_API_KEY");

    if (apiKey == nullptr) {
        return "API key not found.";
    }

    CURL* curl = curl_easy_init();

    if (!curl) {
        return "Could not start the connection.";
    }

    nlohmann::json request = {
        {"model", "gpt-6-astra"},
        {"instructions", aiInstructions},
        {"input", problem}
    };

    string response;

    struct curl_slist* headers = nullptr;

    headers = curl_slist_append(
        headers, "Content-Type: application/json"
    );

    string authorization = "Authorization: Bearer " + string(apiKey);

    headers = curl_slist_append(
        headers, authorization.c_str()
    );

    string requestBody = request.dump();

    curl_easy_setopt(curl, CURLOPT_URL,
        "https://api.openai.com/v1/responses");

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS,
        requestBody.c_str());

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
        WriteCallback);

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode result = curl_easy_perform(curl);

    long statusCode = 0;

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &statusCode);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (result != CURLE_OK) {
        return "Connection error.";
    }

    if (statusCode < 200 || statusCode >= 300) {
        return "API error: " + response;
    }

    nlohmann::json answer = nlohmann::json::parse(response);

    for (const auto& item : answer["output"]) {
        if (item.value("type", "") == "message") {
            for (const auto& content : item["content"]) {
                if (content.value("type", "") == "output_text") {
                    return content["text"].get<string>();
                }
            }
        }
    }

    return "No text response was received.";
}
int main() {

    curl_global_init(CURL_GLOBAL_DEFAULT);

    httplib::Server server;

    // Serve our website files
    server.set_mount_point("/", "./website");

    // Receive questions from the website
    server.Post("/ask", [](const httplib::Request& req,
                           httplib::Response& res) {

        try {

            auto body = nlohmann::json::parse(req.body);

            string problem = body["problem"];

            string answer = askAI(problem);

            nlohmann::json response = {
                {"answer", answer}
            };

            res.set_content(
                response.dump(),
                "application/json"
            );

        }
        catch (const exception& e) {

            res.status = 400;

            res.set_content(
                R"({"error":"Something went wrong."})",
                "application/json"
            );

        }

    });

    cout << "====================================" << endl;
    cout << "       THE FUTURE FUNCTION          " << endl;
    cout << "====================================" << endl;

    cout << "\nWebsite running at:" << endl;
    cout << "http://localhost:8080" << endl;

    cout << "\nPress Ctrl+C to stop the server." << endl;

    const char* port_env = getenv("PORT");
    int port = port_env ? stoi(port_env) : 8080;

    server.listen("0.0.0.0", port);

    curl_global_cleanup();

    return 0;
}
