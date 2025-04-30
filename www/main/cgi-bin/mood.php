
#!/usr/bin/env php
<?php

// Récupérer l'humeur envoyée via POST
$mood = isset($_POST['mood']) ? $_POST['mood'] : 'unknown';

// Définir un message, une couleur et une icône en fonction de l'humeur
$messages = [
    'happy' => [
        'message' => 'Vous êtes rayonnant aujourd\'hui ! 😊',
        'color' => '#FFD700',
        'icon' => '🌞'
    ],
    'sad' => [
        'message' => 'Un câlin virtuel pour vous réconforter. 🤗',
        'color' => '#87CEEB',
        'icon' => '🌧️'
    ],
    'excited' => [
        'message' => 'Votre énergie est contagieuse ! 🎉',
        'color' => '#FF4500',
        'icon' => '🎆'
    ],
    'angry' => [
        'message' => 'Respirez profondément, tout ira bien. 😡',
        'color' => '#FF6347',
        'icon' => '🔥'
    ],
    'relaxed' => [
        'message' => 'Profitez de ce moment de calme. 🧘‍♂️',
        'color' => '#98FB98',
        'icon' => '🍃'
    ],
    'unknown' => [
        'message' => 'Humeur inconnue, mais vous êtes génial ! 🤔',
        'color' => '#D3D3D3',
        'icon' => '❓'
    ]
];

$response = $messages[$mood] ?? $messages['unknown'];

// Générer la réponse HTML
echo "Content-Type: text/html; charset=UTF-8\r\n\r\n";
echo <<<HTML
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Votre Humeur</title>
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
        <p>Merci d'avoir partagé votre humeur avec nous !</p>
        <a href="../php-cgi.html">Retour</a>
    </div>
</body>
</html>
HTML;
?>