#include <iostream>

int main(int ac, char **av)
{
    if (ac != 2) 
    {
        std::cerr << "Usage:./webserv [configuration file] " << std::endl;
        return (EXIT_FAILURE);
    }

    //parsing fichier config (validation et recuperation des données)

    //creation du serveur : socket et de epoll pour surveiler les sockets
        // gestion client (creer/ajouter nouveau socket a epoll)

    //boucle principale
        // requete
        // reponse

    //fermeture du serveur
    
    return (EXIT_SUCCESS);
}