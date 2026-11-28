#include <curl/curl.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

size_t write_cb(void* ptr, size_t size, size_t nmemb, void* userdata) {
    auto* data = static_cast<std::string*>(userdata);
    data->append(static_cast<char*>(ptr), size * nmemb);
    return size * nmemb;
}

json getLatestRow(const std::string& url, const std::string& key, const std::string& table) {
    CURL* curl = curl_easy_init();
    std::string response;
    std::string endpoint = url + "/rest/v1/" + table + "?select=*&order=created_at.desc&limit=1";

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("apikey: " + key).c_str());
    headers = curl_slist_append(headers, ("Authorization: Bearer " + key).c_str());

    curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_perform(curl);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    json rows = json::parse(response);
    return rows.empty() ? json::object() : rows[0];
}

int main() {
    json row = getLatestRow("https://nwdfvgnkyamlatzhzlbw.supabase.co", "sb_publishable_TG3krEs4FqGq7qXxlLrsHg_ispC5P-l", "runs");

    std::cout << row.dump(2) << "\n";        
    std::cout << row["curve"] << "\n";          
    std::cout << row["x_min"] << "\n";          
    std::cout << row["x_max"] << "\n";          
    std::cout << row["type"] << "\n";  
}