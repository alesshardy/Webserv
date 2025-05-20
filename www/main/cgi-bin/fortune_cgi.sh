#!/bin/bash
echo "Content-Type: text/html"
echo ""

# Fonction pour décoder l'URL
urldecode() { : "${*//+/ }"; echo -e "${_//%/\\x}"; }

if [ "$REQUEST_METHOD" = "POST" ]; then
    read -r POST_DATA
    NAME=$(echo "$POST_DATA" | sed -n 's/^name=//p' | sed 's/%20/ /g')
    NAME=$(urldecode "$NAME")
    MESSAGE="Merci pour ton POST, $NAME ! Voici ta fortune du jour :"
else
    # Pour GET, récupérer le nom dans QUERY_STRING
    NAME=$(echo "$QUERY_STRING" | sed -n 's/^name=//p' | sed 's/%20/ /g')
    NAME=$(urldecode "$NAME")
    if [ -z "$NAME" ]; then
        NAME="Visiteur Mystère"
    fi
    MESSAGE="Bienvenue $NAME ! Voici ta fortune du jour :"
fi

# Quelques fortunes insolites
FORTUNES=(
    "Le bonheur est une direction, pas une destination."
    "Un bug par jour éloigne le développeur du repos."
    "42 est la réponse à tout."
    "Aujourd'hui, tout est possible (même un segfault)."
    "Tu vas croiser un canard en plastique qui résoudra ton bug."
    "Le café est la clé du succès."
    "Ton code va compiler du premier coup !"
    "Un commit inattendu va changer ta journée."
)
RANDOM_INDEX=$((RANDOM % ${#FORTUNES[@]}))
FORTUNE="${FORTUNES[$RANDOM_INDEX]}"

cat <<HTML
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Fortune CGI</title>
    <style>
        body {
            background: linear-gradient(135deg, #f7971e, #ffd200, #21d4fd, #b721ff);
            background-size: 400% 400%;
            animation: gradientBG 10s ease infinite;
            font-family: 'Comic Sans MS', cursive, sans-serif;
            color: #222;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            min-height: 100vh;
            margin: 0;
        }
        @keyframes gradientBG {
            0% {background-position: 0% 50%;}
            50% {background-position: 100% 50%;}
            100% {background-position: 0% 50%;}
        }
        .fortune-box {
            background: rgba(255,255,255,0.9);
            border-radius: 20px;
            box-shadow: 0 8px 32px 0 rgba(31,38,135,0.37);
            padding: 40px 30px;
            margin: 30px;
            text-align: center;
            max-width: 400px;
        }
        h1 {
            font-size: 2.2rem;
            margin-bottom: 10px;
        }
        .fortune {
            font-size: 1.3rem;
            margin: 20px 0;
            color: #b721ff;
        }
        .emoji {
            font-size: 3rem;
            margin-bottom: 10px;
            animation: bounce 1.5s infinite;
        }
        @keyframes bounce {
            0%, 100% { transform: translateY(0);}
            50% { transform: translateY(-15px);}
        }
        a {
            display: inline-block;
            margin-top: 20px;
            color: #fff;
            background: #21d4fd;
            padding: 10px 20px;
            border-radius: 10px;
            text-decoration: none;
            font-weight: bold;
            transition: background 0.3s;
        }
        a:hover {
            background: #b721ff;
        }
    </style>
</head>
<body>
    <div class="fortune-box">
        <div class="emoji">🧙‍♂️</div>
        <h1>$MESSAGE</h1>
        <div class="fortune">"$FORTUNE"</div>
        <a href="../fortune_cgi.html">↩️ Revenir au test CGI</a>
    </div>
</body>
</html>
HTML