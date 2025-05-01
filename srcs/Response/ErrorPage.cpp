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

    // Générer une page d'erreur standard avec un style minimaliste
    std::string page = "<!DOCTYPE html>";
    page += "<html lang=\"en\">";
    page += "<head>";
    page += "<meta charset=\"UTF-8\">";
    page += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
    page += "<title>Error " + toString(error_code) + " - " + errorMessage + "</title>";
    page += "<style>";
    page += "body {";
    page += "  font-family: Arial, sans-serif;";
    page += "  background-color: #f8f9fa;";
    page += "  color: #333;";
    page += "  display: flex;";
    page += "  justify-content: center;";
    page += "  align-items: center;";
    page += "  height: 100vh;";
    page += "  margin: 0;";
    page += "}";
    page += ".error-container {";
    page += "  text-align: center;";
    page += "  background: #fff;";
    page += "  padding: 20px;";
    page += "  border-radius: 10px;";
    page += "  box-shadow: 0 4px 10px rgba(0, 0, 0, 0.1);";
    page += "}";
    page += ".error-logo {";
    page += "  font-size: 50px;";
    page += "  color: #e74c3c;";
    page += "  margin-bottom: 20px;";
    page += "}";
    page += ".error-title {";
    page += "  font-size: 24px;";
    page += "  margin-bottom: 10px;";
    page += "}";
    page += ".error-message {";
    page += "  font-size: 16px;";
    page += "  color: #666;";
    page += "}";
    page += "</style>";
    page += "</head>";
    page += "<body>";
    page += "<div class=\"error-container\">";
    page += "  <div class=\"error-logo\">⚠️</div>";
    page += "  <div class=\"error-title\">Error " + toString(error_code) + " - " + errorMessage + "</div>";
    page += "  <div class=\"error-message\">Something went wrong. Please try again later.</div>";
    page += "</div>";
    page += "</body>";
    page += "</html>";

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