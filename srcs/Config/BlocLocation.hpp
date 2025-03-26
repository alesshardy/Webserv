#ifndef BLOCLOCATION_HPP
# define BLOCLOCATION_HPP

# include "Config.hpp"

class BlocLocation
{
    private:
            std::string                 _root;
            std::string                 _alias;
            std::vector<std::string>    _index;
            std::set<std::string>       _allowMethod; //une seul clef mais plusieur valeur en face
            //std::vector<std::string>    _cgiExtension; Bonus a voir plus tard
            std::string                 _uploadPath;
            bool                        _autoIndex;
            
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
            //void    addCgiExtension(const std::string &extension); Bonus a voir plus tard
            void    setUploadPath(const std::string &path) {_uploadPath = path;};
            void    setAutoIndex(bool autoIndex) { _autoIndex = autoIndex;};

            //Getters
            const std::string                   &getRoot() const {return _root;};
            const std::string                   &getAlias() const {return _alias;};
            const std::vector<std::string>      &getIndex() const {return _index;};
            const std::set<std::string>         &getAllowMethod() const {return _allowMethod;};
            //const std::vector<std::string> &getCgiExtension() const; Bonus a voir plus tard
            const std::string                   &getUploadPath() const {return _uploadPath;};
            bool                                getAutoIndex() const {return _autoIndex;};

            //Validation
            bool    validateLocation() const;

            //Print
};

#endif