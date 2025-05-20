#!/usr/bin/env python3
import cgi
import random
import html

print("Content-Type: text/html\n")

form = cgi.FieldStorage()
name = form.getfirst("name", "").strip()
if not name:
    name = "Visiteur Mystère"

compliments = [
    "a un sourire qui illumine la pièce 😁",
    "est plus cool qu'un pingouin sur un surf 🐧",
    "a un cerveau aussi puissant qu'un supercalculateur 🤓",
    "est la raison pour laquelle le soleil brille aujourd'hui ☀️",
    "a un style inimitable 👑",
    "fait rire même les robots 🤖",
    "est la personne la plus inspirante du jour ✨",
    "a un talent caché pour la danse du canard 🦆",
    "est plus motivé(e) qu'un lundi matin avec du café ☕️",
    "mérite une standing ovation 👏"
]

compliment = random.choice(compliments)

print(f"""<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Compliment du Jour</title>
    <style>
        body {{
            background: linear-gradient(135deg, #ffecd2, #fcb69f, #a1c4fd, #c2e9fb);
            background-size: 400% 400%;
            animation: gradientBG 10s ease infinite;
            font-family: 'Comic Sans MS', cursive, sans-serif;
            color: #333;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            min-height: 100vh;
            margin: 0;
        }}
        @keyframes gradientBG {{
            0% {{background-position: 0% 50%;}}
            50% {{background-position: 100% 50%;}}
            100% {{background-position: 0% 50%;}}
        }}
        .compliment-box {{
            background: rgba(255,255,255,0.95);
            border-radius: 20px;
            box-shadow: 0 8px 32px 0 rgba(31,38,135,0.17);
            padding: 40px 30px;
            margin: 30px;
            text-align: center;
            max-width: 400px;
        }}
        h1 {{
            font-size: 2.2rem;
            margin-bottom: 10px;
        }}
        .compliment {{
            font-size: 1.5rem;
            margin: 20px 0;
            color: #ff6f61;
            animation: bounce 1.5s infinite;
        }}
        .emoji {{
            font-size: 3rem;
            margin-bottom: 10px;
        }}
        @keyframes bounce {{
            0%, 100% {{ transform: translateY(0);}}
            50% {{ transform: translateY(-15px);}}
        }}
        a {{
            display: inline-block;
            margin-top: 20px;
            color: #fff;
            background: #21d4fd;
            padding: 10px 20px;
            border-radius: 10px;
            text-decoration: none;
            font-weight: bold;
            transition: background 0.3s;
        }}
        a:hover {{
            background: #b721ff;
        }}
    </style>
</head>
<body>
    <div class="compliment-box">
        <div class="emoji">🦄</div>
        <h1>Hey {html.escape(name)} !</h1>
        <div class="compliment">{html.escape(name)} {compliment}</div>
        <a href="../compliment_cgi.html">🔁 Rejouer</a>
    </div>
</body>
</html>
""")