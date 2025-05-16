#include "../includes/Webserv.hpp"

bool running = true;

void signalHandler(int signum)
{
    (void)signum;
    
    LogManager::log(LogManager::INFO, "Interrupt signal received, shutting down...");
    running = false;
}

int main(int ac, char **av)
{
    LogManager::log(LogManager::DEBUG, "Starting main");
    signal(SIGPIPE, SIG_IGN); // Ignorer SIGPIPE
    // Capturer les signaux pour arrêter proprement le serveur
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    //Initialisation du gestionnaire d'arguments de la ligne de commande
    try
    {
        CommandLineParser cmdParser(ac, av);

        LogManager::log(LogManager::DEBUG, "CommandLineParser initialized");
    
        if (cmdParser.hasOption("--help"))
        {
            cmdParser.printHelp();
            return (EXIT_SUCCESS);
        }
    
        try
        {
            Config config;
            
            std::string configFile = cmdParser.getConfigFilePath();
            LogManager::log(LogManager::INFO, "Using configuration file: %s", configFile.c_str());
            config.parseConfigFile(configFile, config);
    
            if (LogManager::getDebugLogStatus())
                config.printConfig();
            
            // Initialiser le serveur
            Server server;
            server.set_config(config); // Assigner la configuration au serveur
            server.init();             // Initialiser le serveur
    
            LogManager::log(LogManager::INFO, "Server initialized successfully %d", server.get_socket());
    
            // Lancer le serveur
            server.run();
    
            // Arrêter le serveur
            server.stop();
           
            // Vérifiez si le programme doit quitter
            if (!running)
            {
                LogManager::log(LogManager::INFO, "Exiting program...");
                return EXIT_SUCCESS;
            }
        }
        catch (const std::exception &e)
        {
            LogManager::log(LogManager::ERROR, e.what());
            return EXIT_FAILURE;
        }
        //fermeture du serveur
        LogManager::log(LogManager::INFO, "Server is shutting down...");
        LogManager::cleanup();
        return (EXIT_SUCCESS);
    }
    catch (const std::exception &e)
    {
        LogManager::log(LogManager::ERROR, "Error: %s", e.what());
        return (EXIT_FAILURE);
    }
    catch (...)
    {
        LogManager::log(LogManager::ERROR, "Unknown error occurred");
        return (EXIT_FAILURE);
    }
    LogManager::log(LogManager::INFO, "Exiting program...");
    LogManager::cleanup();
    return (EXIT_SUCCESS);


}

