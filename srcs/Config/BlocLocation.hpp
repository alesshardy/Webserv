#ifndef BLOCLOCATION_HPP
# define BLOCLOCATION_HPP

# include "Config.hpp"

class BlocLocation
{
    private:
            std::string                         _root;
            std::string                         _alias;
            std::vector<std::string>            _index;
            std::set<std::string>               _allowMethod; //une seul clef mais plusieur valeur en face
            std::map<std::string, std::string>  _cgiExtension;
            std::string                         _uploadPath;
            bool                                _autoIndex;
            std::map<int, std::string>          _returnDirectives;

            // pour stocker cgi
            std::string _tmpCgiExtension;
            bool        _hasTmpCgiExtension;

            // pour stocker les return directive
            int         _tmpReturnDirective;
            bool        _hasTmpReturnDirective;

    public:
            BlocLocation();
            BlocLocation(const BlocLocation &copy);
            ~BlocLocation();

            BlocLocation &operator=(const BlocLocation &assign);

            //Setters
            void    setRoot(const std::string &root) {_root = root;};
            void    setAlias(const std::string &alias) {_alias = alias;};
            void    addIndex(const std::string &index) {_index.push_back(index);};
            void    addAllowMethod(const std::string &method) {_allowMethod.insert(method);};
            void    addCgiExtension(const std::string &extension, const std::string &path) {_cgiExtension[extension] = path;};
            void    setUploadPath(const std::string &path) {_uploadPath = path;};
            void    setAutoIndex(bool autoIndex) { _autoIndex = autoIndex;};
            void    addReturnDirectives(int code, const std::string &path) {_returnDirectives[code] = path;};

            //Getters
            const std::string                           &getRoot() const {return _root;};
            const std::string                           &getAlias() const {return _alias;};
            const std::vector<std::string>              &getIndex() const {return _index;};
            const std::set<std::string>                 &getAllowMethod() const {return _allowMethod;};
            const std::map<std::string, std::string>    &getCgiExtensions() const {return _cgiExtension;}
            const std::string                           &getUploadPath() const {return _uploadPath;};
            bool                                        getAutoIndex() const {return _autoIndex;};
            const std::map<int, std::string>            &getReturnDirectives() const {return _returnDirectives;};


            //pour stocker cgi
            void                setTmpCgiExtension(std::string extension) {_tmpCgiExtension = extension; _hasTmpCgiExtension = true;};
            std::string         getTmpCgiExtension() const {return _tmpCgiExtension;};
            bool                hasTmpCgiExtension() const {return _hasTmpCgiExtension;};
            void                clearTmpCgiExtension() {_hasTmpCgiExtension = false;};

            // pour stocker les return directive
            void                setTmpReturnDirective(int code) {_tmpReturnDirective = code; _hasTmpReturnDirective = true;};
            int                 getTmpReturnDirective() const {return _tmpReturnDirective;};
            bool                hasTmpReturnDirective() const {return _hasTmpReturnDirective;};
            void                clearTmpReturnDirective() {_hasTmpReturnDirective = false;};

            //Validation
            bool    validateLocation() const;

            //Print
};

#endif