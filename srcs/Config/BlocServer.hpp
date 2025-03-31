#ifndef BLOCSERVER_HPP
# define BLOCSERVER_HPP

# include "Config.hpp"
# include "Listen.hpp"

# include "BlocLocation.hpp"

# define DEFAULT_CLIENT_MAX_BODY_SIZE 1048576 // 1MB

class BlocLocation;
class Listen;

class BlocServer
{
    private:
            std::vector<Listen>            _listen;
            std::vector<std::string>    _serverName;
            std::string                 _root;
            std::vector<std::string>    _index;
            std::map<int, std::string>  _errorPage;
            unsigned long long                   _clientMaxBodySize;
            std::map<std::string, BlocLocation> _location;

            // pour stocker error_page
            int _tmpErrorCode;
            bool _hasTmpErrorCode;
    
    public:
            BlocServer();
            BlocServer(const BlocServer &copy);
            ~BlocServer();

            BlocServer &operator=(const BlocServer &assign);

            //Setters
            void    addListen(const Listen &listen) {_listen.push_back(listen);};
            void    addServerName(const std::string &name) {_serverName.push_back(name);};
            void    setRoot(const std::string &root) {_root = root;};
            void    addIndex(const std::string &index) {_index.push_back(index);};
            void    addErrorPage(int code, const std::string &path) {_errorPage[code] = path;};
            void    setClientMaxBodySize(long long size) {_clientMaxBodySize = size;};
            void    addLocation(const std::string &path, const BlocLocation &location);

            //Getters
            const std::vector<Listen>                   &getListen() const {return _listen;};
            const std::vector<std::string>              &getServerName() const {return _serverName;};
            const std::string                           &getRoot() const {return _root;};
            const std::vector<std::string>              &getIndex() const {return _index;};
            const std::map<int, std::string>            &getErrorPage() const {return _errorPage;};
            long long                                   getClientMaxBodySize() const {return _clientMaxBodySize;};
            const std::map<std::string, BlocLocation>   &getLocation() const {return _location;};

            //Validation
            bool    validateServer() const;

            //pour stocker error_page
            void        setTmpErrorCode(int code) {_tmpErrorCode = code; _hasTmpErrorCode = true;};
            int         getTmpErrorCode() const {return _tmpErrorCode;};
            bool        hasTmpErrorCode() const {return _hasTmpErrorCode;};
            void        clearTmpErrorCode() {_hasTmpErrorCode = false;};
            
            // check double location
            bool        locationExists(const std::string &locationPath) const;


            //Print
};

#endif