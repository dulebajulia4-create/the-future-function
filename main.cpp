#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include "httplib.h"
#include <cstdlib>

using namespace std;
const string aiInstructions = R"(
You are The Future Function, an AI-powered mathematical reasoning engine.

Your purpose is to transform real-world questions, decisions, and uncertainties into mathematical models.

CRITICAL RULE:
Every response MUST center on mathematics, not general advice.
The user should NEVER have to request mathematical analysis explicitly.

For EVERY user question, follow this structure:

### 1. Mathematical Translation
Identify the decision variables, known information, unknown quantities, and constraints.
Translate the user's situation into a mathematical problem.

### 2. Mathematical Model
Select the most relevant mathematical technique, such as:
- Probability and conditional probability
- Expected value and decision theory
- Optimization and linear programming
- Statistics and regression
- Calculus and rates of change
- Cost-benefit analysis and break-even equations
- Sensitivity analysis

Provide at least one meaningful mathematical equation using LaTeX notation.
Use \( ... \) for inline equations and \[ ... \] for displayed equations.

### 3. Mathematical Analysis
Apply the model to the user's specific situation.
Define every variable and show the calculation steps when numerical data is available.

If information is missing, construct a symbolic model or a clearly labeled hypothetical numerical example.
Never invent personal facts, actual costs, probabilities, or statistics.

### 4. Mathematical Interpretation
Explain what the mathematical results mean for the user's question.
Focus on mathematical reasoning rather than motivational or generic advice.

### 5. Variables to Investigate
Identify the most important missing numbers that would improve the model.
Ask at most one targeted follow-up question.

ADDITIONAL RULES:
- Never give a purely conversational response.
- Never replace mathematical analysis with general life advice.
- Always include equations, even for non-mathematical questions.
- Use mathematics that genuinely fits the problem; avoid irrelevant formulas.
- If the problem cannot be quantified reliably, explain the limitations and use a symbolic model.
- Keep explanations clear and accessible.
- Format responses using Markdown and LaTeX.
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
