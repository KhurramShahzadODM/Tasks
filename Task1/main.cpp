#include <curl/curl.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <thread>
#include <vector>
#include <math.h>


std::string buffer[3];
int progress_func(void* ptr, double TotalToDownload, double NowDownloaded,
                  double TotalToUpload, double NowUploaded)
{
    // ensure that the file to be downloaded is not empty
    // because that would cause a division by zero error later on
    if (TotalToDownload <= 0.0) {
        return 0;
    }

    // how wide you want the progress meter to be
    int totaldotz=40;
    double fractiondownloaded = NowDownloaded / TotalToDownload;
    // part of the progressmeter that's already "full"
    int dotz = (int) round(fractiondownloaded * totaldotz);

    // create the "meter"
    int ii=0;
    printf("%3.0f%% [",fractiondownloaded*100);
    // part  that's full already
    for ( ; ii < dotz;ii++) {
        printf("=");
    }
    // remaining part (spaces)
    for ( ; ii < totaldotz;ii++) {
        printf(" ");
    }
    // and back to line begin - do not forget the fflush to avoid output buffering problems!
    printf("]\r");
    fflush(stdout);
    // if you don't return 0, the transfer will be aborted - see the documentation
    return 0;
}
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

CURLcode curl_read(const std::string url, int i)
{
    CURLcode code(CURLE_FAILED_INIT);
    CURL* curl = curl_easy_init();

    if (curl)
    {
        if (CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WriteCallback))
            && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, false))
            && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, false))
            && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer[i]))
            && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_URL, url.c_str()))
            && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_func)));
        {
            code = curl_easy_perform(curl);
        }
        curl_easy_cleanup(curl);
    }
    return code;
}
int main()
{
    curl_global_init(CURL_GLOBAL_ALL);
    std::vector<std::thread> workers;
    std::string urls[3] ={"https://image.shutterstock.com/image-photo/mountains-under-mist-morning-amazing-260nw-1725825019.jpg","https://images.unsplash.com/photo-1621240569574-6cfce17d7e18?ixlib=rb-1.2.1&ixid=MnwxMjA3fDB8MHxwaG90by1wYWdlfHx8fGVufDB8fHx8&auto=format&fit=crop&w=1534&q=80","https://images.unsplash.com/photo-1618236507249-9960f2c6a53d?ixlib=rb-1.2.1&ixid=MnwxMjA3fDB8MHxwaG90by1wYWdlfHx8fGVufDB8fHx8&auto=format&fit=crop&w=1534&q=80"};
    for(int i = 0; i < 3; i++)
    {
        std::thread tobj(curl_read,urls[i], i);
        workers.push_back(std::move(tobj));
    }
    for(int i = 0; i < 3; i++)
    {
        workers[i].join();
    }
    for(int i = 0; i < 3; i++)
    {
        std::ofstream file(std::to_string(i)+".jpg");
        file << buffer [i];
    }
    curl_global_cleanup();
}