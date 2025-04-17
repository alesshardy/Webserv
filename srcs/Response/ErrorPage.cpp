#include "ErrorPage.hpp"


std::string ErrorPage::getErrorPage(int error_code, const std::map<int, std::string>& error_pages)
{
    LogManager::log(LogManager::DEBUG, "Error code: %d", error_code);

   
    

    std::map<int, std::string>::const_iterator it = error_pages.find(error_code);
    
    if (it != error_pages.end())
    {
        std::string file_path = it->second;
        std::string page;
        std::ifstream file(file_path.c_str());
        if (file.is_open())
        {
            std::string line;
            while (std::getline(file, line))
            {
                page += line + "\n";
            }
            file.close();
        }
        else
        {
            LogManager::log(LogManager::ERROR, "Error page file not found: %s", file_path.c_str());
            return "<html><body><h1>Error " + toString(error_code) + "</h1></body></html>";
        }
        // Add HTTP headers
        std::ostringstream oss;
        oss << page.size();
        std::string headers = "HTTP/1.1 " + toString(error_code) + " Error\r\n";
        headers += "Content-Type: text/html\r\n";
        headers += "Content-Length: " + oss.str() + "\r\n";
        headers += "Connection: close\r\n";
        headers += "\r\n";
        return headers + page;
    }
    else
    {
        LogManager::log(LogManager::ERROR, "Error page not found for error code: %d", error_code);
        std::string page = "<html><body><h1>Error " + toString(error_code) + "</h1></body></html>";

        // Add HTTP headers
        std::ostringstream oss;
        oss << page.size();
        std::string headers = "HTTP/1.1 " + toString(error_code) + " Error\r\n";
        headers += "Content-Type: text/html\r\n";
        headers += "Content-Length: " + oss.str() + "\r\n";
        headers += "Connection: close\r\n";
        headers += "\r\n";
        return headers + page;
    }
}