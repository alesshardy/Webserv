
<?php
#!/usr/bin/env php

// Récupérer l'humeur envoyée via POST
$mood = isset($_POST['mood']) ? $_POST['mood'] : 'unknown';

// Définir un message, une couleur et une icône en fonction de l'humeur

$messages = [
    'happy' => [
        'message' => 'Tu es rayonnant(e) aujourd\'hui ! 😊',
        'color' => '#FFD700',
        'icon' => '🌞'
    ],
    'sad' => [
        'message' => 'Un câlin virtuel pour te réconforter. 🤗',
        'color' => '#87CEEB',
        'icon' => '🌧️'
    ],
    'excited' => [
        'message' => 'Ton énergie est contagieuse ! 🎉',
        'color' => '#FF4500',
        'icon' => '🎆'
    ],
    'angry' => [
        'message' => 'Respire profondément, tout ira bien. 😡',
        'color' => '#FF6347',
        'icon' => '🔥'
    ],
    'relaxed' => [
        'message' => 'Profite de ce moment de calme. 🧘‍♂️',
        'color' => '#98FB98',
        'icon' => '🍃'
    ],
    'bored' => [
        'message' => 'Peut-être un peu de musique ou un bon livre ? 🎶',
        'color' => '#D3D3D3',
        'icon' => '📚'
    ],
    'confused' => [
        'message' => 'Pas de souci, ça arrive à tout le monde ! 🤔',
        'color' => '#FFA500',
        'icon' => '❓'
    ],
    'tired' => [
        'message' => 'Un peu de repos ne te fera pas de mal. 😴',
        'color' => '#A9A9A9',
        'icon' => '💤'
    ],
    'motivated' => [
        'message' => 'Continue comme ça, tu es sur la bonne voie ! 💪',
        'color' => '#32CD32',
        'icon' => '🚀'
    ],
    'grateful' => [
        'message' => 'La gratitude est la clé du bonheur. 🙏',
        'color' => '#FF69B4',
        'icon' => '💖'
    ],
    'anxious' => [
        'message' => 'Prends un moment pour respirer profondément. 🌬️',
        'color' => '#FFB6C1',
        'icon' => '😟'
    ],
    'curious' => [
        'message' => 'La curiosité est le début de la sagesse. 🔍',
        'color' => '#8A2BE2',
        'icon' => '🧐'
    ],
    'hopeful' => [
        'message' => 'Chaque jour est une nouvelle opportunité. 🌈',
        'color' => '#FFDAB9',
        'icon' => '🌅'
    ],
    'lonely' => [
        'message' => 'Tu n\'es pas seul(e), nous sommes là pour toi. 💔',
        'color' => '#FFB0C4',
        'icon' => '🤝'
    ],
    'overwhelmed' => [
        'message' => 'Prends une pause, tout ira bien. 🌊',
        'color' => '#4682B4',
        'icon' => '🌪️'
    ],
    'indifferent' => [
        'message' => 'Parfois, il est bon de ne rien ressentir. 😐',
        'color' => '#B0C4DE',
        'icon' => '😶'
    ],
    'nostalgic' => [
        'message' => 'Les souvenirs sont précieux. 🌌',
        'color' => '#FFDEAD',
        'icon' => '🕰️'
    ],
    'inspired' => [
        'message' => 'Laisse libre cours à ta créativité ! 🎨',
        'color' => '#FF1493',
        'icon' => '✨'
    ],
    'disappointed' => [
        'message' => 'C\'est normal d\'avoir des hauts et des bas. 😞',
        'color' => '#CD5C5C',
        'icon' => '😔'
    ],
    'stressed' => [
        'message' => 'Prends un moment pour te détendre. 🧘‍♀️',
        'color' => '#FF8C00',
        'icon' => '😰'
    ],
    'playful' => [
        'message' => 'Amuse-toi bien, la vie est un jeu ! 🎈',
        'color' => '#FF4500',
        'icon' => '🎠'
    ],
    'neutral' => [
        'message' => 'Parfois, la neutralité est la meilleure option. 😌',
        'color' => '#C0C0C0',
        'icon' => '😐'
    ],
    'unknown' => [
        'message' => 'Humeur inconnue, mais tu es génial(e) ! 🤔',
        'color' => '#D3D3D3',
        'icon' => '❓'
    ]

];
$response = $messages[$mood] ?? $messages['unknown'];

// Générer la réponse HTML
//echo "Content-Type: text/html; charset=UTF-8\r\n\r\n";
echo <<<HTML
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Ton Humeur</title>
    <style>
        @import url('https://fonts.googleapis.com/css2?family=Inter:wght@400;700&display=swap');

        body {
            font-family: 'Inter', sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: {$response['color']};
            color: #333;
            animation: backgroundFade 3s infinite alternate;
        }

        @keyframes backgroundFade {
            from {
                filter: brightness(1);
            }
            to {
                filter: brightness(1.2);
            }
        }

        .container {
            text-align: center;
            background: #fff;
            padding: 30px;
            border-radius: 15px;
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.2);
            max-width: 400px;
            width: 90%;
            animation: popIn 1s ease-in-out;
        }

        @keyframes popIn {
            from {
                transform: scale(0.8);
                opacity: 0;
            }
            to {
                transform: scale(1);
                opacity: 1;
            }
        }

        h1 {
            font-size: 2.5rem;
            margin-bottom: 20px;
            color: {$response['color']};
            text-shadow: 2px 2px 5px rgba(0, 0, 0, 0.3);
        }

        .icon {
            font-size: 4rem;
            margin-bottom: 20px;
            animation: bounce 2s infinite;
        }

        @keyframes bounce {
            0%, 100% {
                transform: translateY(0);
            }
            50% {
                transform: translateY(-10px);
            }
        }

        p {
            font-size: 1.2rem;
            margin-bottom: 20px;
        }

        a {
            display: inline-block;
            text-decoration: none;
            background-color: {$response['color']};
            color: #fff;
            padding: 10px 20px;
            border-radius: 5px;
            font-weight: bold;
            transition: background-color 0.3s ease, transform 0.2s ease;
        }

        a:hover {
            background-color: #333;
            color: #fff;
            transform: scale(1.1);
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="icon">{$response['icon']}</div>
        <h1>{$response['message']}</h1>
        <p>Merci d'avoir partagé ton humeur avec nous !</p>
        <a href="../php-cgi.html">Retour</a>
    </div>
</body>
</html>
HTML;
?>