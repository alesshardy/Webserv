#!/bin/bash
echo "Content-Type: text/plain"
echo ""

# Lire le corps de la requête
read -r command
echo "Commande brute reçue : $command" >> /tmp/cgi_debug.log

# Extraire la commande depuis le corps encodé
command=$(echo "$command" | sed -n 's/^command=//p')
echo "Commande décodée : $command" >> /tmp/cgi_debug.log

# Vérifier si la commande est vide
if [[ -z "$command" ]]; then
    echo "Erreur : Commande vide"
    exit 0
fi

# Vérifier si la commande contient des caractères spéciaux dangereux
if echo "$command" | grep -q '[^a-zA-Z0-9_ -]'; then
    echo "Erreur : Commande invalide"
    exit 0
fi

# Liste blanche des commandes autorisées
allowed_commands=("ls" "pwd" "whoami" "date")

# Vérifier si la commande est autorisée
is_allowed=false
for cmd in "${allowed_commands[@]}"; do
    if [[ "$command" == "$cmd" ]]; then
        is_allowed=true
        break
    fi
done

if ! $is_allowed; then
    echo "Erreur : Commande non autorisée"
    exit 0
fi

# Exécuter la commande autorisée avec un timeout
output=$(timeout 5s bash -c "$command" 2>&1)
if [[ $? -eq 124 ]]; then
    echo "Erreur : Commande trop longue"
    exit 0
fi

# Afficher uniquement le résultat brut
echo "$output"