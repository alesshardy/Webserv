#include "ErrorPage.hpp"

std::string ErrorPage::getErrorPage(int error_code, const std::map<int, std::string>& error_pages)
{
    LogManager::log(LogManager::DEBUG, "Generating error page for code: %d", error_code);

    // Vérifier si une page d'erreur personnalisée existe pour ce code
    std::map<int, std::string>::const_iterator it = error_pages.find(error_code);
    if (it != error_pages.end())
    {
        const std::string& file_path = it->second;
        std::ifstream file(file_path.c_str());
        if (file.is_open())
        {
            // Lire le contenu de la page d'erreur personnalisée
            std::string page((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            file.close();

            // Construire les en-têtes HTTP
            std::ostringstream oss;
            oss << page.size();
            std::string headers = "HTTP/1.1 " + toString(error_code) + " " + getStatusCodeMessage(error_code) + "\r\n";
            headers += "Content-Type: text/html\r\n";
            headers += "Content-Length: " + oss.str() + "\r\n";
            headers += "Connection: close\r\n";
            headers += "\r\n";

            return headers + page;
        }
        else
        {
            LogManager::log(LogManager::ERROR, "Custom error page file not found: %s", file_path.c_str());
        }
    }

    // Si aucune page personnalisée n'est trouvée ou si le fichier est introuvable, retourner une page standard
    LogManager::log(LogManager::DEBUG, "Using standard error page for code: %d", error_code);
    return generateStandardErrorPage(error_code);
}

std::string ErrorPage::generateStandardErrorPage(int error_code)
{
    // Obtenir le message correspondant au code d'erreur
    std::string errorMessage = getStatusCodeMessage(error_code);

    // Générer une page d'erreur standard
    std::string page = "<html><head><title>Error " + toString(error_code) + " - " + errorMessage + "</title></head>";
    page += "<body><h1>Error " + toString(error_code) + " - " + errorMessage + "</h1>";
    page += "<p>Something went wrong. Please try again later.</p></body></html>";

    // Construire les en-têtes HTTP
    std::ostringstream oss;
    oss << page.size();
    std::string headers = "HTTP/1.1 " + toString(error_code) + " " + errorMessage + "\r\n";
    headers += "Content-Type: text/html\r\n";
    headers += "Content-Length: " + oss.str() + "\r\n";
    headers += "Connection: close\r\n";
    headers += "\r\n";

    return headers + page;
}