#include <iostream>
#include <fstream>
#include <string>
#include <cctype> // Pour isspace

void parseConfig(const std::string &filename) {
    std::ifstream file(filename.c_str());
    if (!file) {
        std::cerr << "Erreur : impossible d'ouvrir le fichier " << filename << std::endl;
        return;
    }

    char c;
    std::string token;
    bool inComment = false;
    int depth = 0; // Niveau d'imbrication
    int semicolonCount = 0; // Compteur de points-virgules par ligne
    bool contentAfterSemicolon = false; // Indique si du contenu suit un point-virgule sur la même ligne
    bool isKey = true; // Indique si le mot actuel est une clé (premier mot d'une ligne ou d'un bloc)

    while (file.get(c)) {
        // Gestion des commentaires
        if (inComment) {
            if (c == '\n') {
                inComment = false; // Fin du commentaire
                semicolonCount = 0; // Réinitialiser le compteur pour la nouvelle ligne
                contentAfterSemicolon = false;
                isKey = true; // Réinitialiser pour la nouvelle ligne
            }
            continue;
        }

        if (c == '#') {
            inComment = true; // Début d’un commentaire
            continue;
        }

        // Gestion des sauts de ligne
        if (c == '\n') {
            if (!token.empty()) {
                if (isKey) {
                    std::cout << "Clé trouvée : " << token << std::endl;
                } else {
                    std::cout << "Argument trouvé : " << token << std::endl;
                }
                token.clear(); // Vider le token à la fin de la ligne
            }
            semicolonCount = 0; // Réinitialiser le compteur pour la nouvelle ligne
            contentAfterSemicolon = false;
            isKey = true; // Réinitialiser pour la nouvelle ligne
        } 
        // Gestion des blocs
        else if (c == '{') {
            std::cout << "Début de bloc" << std::endl;
            depth++;
            isKey = true; // Réinitialiser pour le début d'un bloc
        } else if (c == '}') {
            std::cout << "Fin de bloc" << std::endl;
            depth--;
            isKey = true; // Réinitialiser pour la fin d'un bloc
        } 
        // Gestion des points-virgules
        else if (c == ';') {
            semicolonCount++;
            if (semicolonCount > 1) {
                std::cerr << "Erreur : plus d'une requête par ligne détectée." << std::endl;
                return;
            }
            if (!token.empty()) {
                if (isKey) {
                    std::cout << "Clé trouvée : " << token << std::endl;
                } else {
                    std::cout << "Argument trouvé : " << token << std::endl;
                }
                token.clear(); // Vider le token après un point-virgule
            }
            isKey = true; // Réinitialiser pour que le prochain mot soit une clé
        } 
        // Gestion des espaces
        else if (isspace(c)) {
            if (!token.empty()) {
                if (isKey) {
                    std::cout << "Clé trouvée : " << token << std::endl;
                    isKey = false; // Le prochain mot sera un argument
                } else {
                    std::cout << "Argument trouvé : " << token << std::endl;
                }
                token.clear();
            }
        } 
        // Construction des mots-clés
        else {
            token += c; // Construire le mot clé
        }
    }

    file.close();
}

int main(int ac, char **av) 
{
    (void)ac;
    parseConfig(av[1]);
    return 0;
}