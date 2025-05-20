#!/usr/bin/env python3
import cgi
import cv2
import os

print("Content-Type: text/html\n")

form = cgi.FieldStorage()
face_file = form['face']
filter_type = form.getvalue('filter')  # Récupérer le filtre sélectionné

# Définir la taille maximale pour l'image (en pixels)
MAX_WIDTH = 800
MAX_HEIGHT = 800

if face_file.filename:
    try:
        # Définir les chemins
        script_dir = os.path.dirname(os.path.abspath(__file__))  # Répertoire du script
        temp_dir = os.path.join(script_dir, "tmp")  # Dossier temporaire local au script
        input_path = os.path.join(temp_dir, face_file.filename)
        output_path = os.path.join(temp_dir, f"{filter_type}_{face_file.filename}")

        # Créer le dossier temporaire s'il n'existe pas
        if not os.path.exists(temp_dir):
            os.makedirs(temp_dir)

        # Sauvegarder l'image uploadée
        with open(input_path, 'wb') as f:
            f.write(face_file.file.read())

        # Charger l'image
        img = cv2.imread(input_path)

        # Redimensionner l'image si elle dépasse les dimensions maximales
        height, width = img.shape[:2]
        if width > MAX_WIDTH or height > MAX_HEIGHT:
            scaling_factor = min(MAX_WIDTH / width, MAX_HEIGHT / height)
            new_width = int(width * scaling_factor)
            new_height = int(height * scaling_factor)
            img = cv2.resize(img, (new_width, new_height), interpolation=cv2.INTER_AREA)

        # Appliquer le filtre sélectionné
        if filter_type == "cartoon":
            gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
            gray = cv2.medianBlur(gray, 5)
            edges = cv2.adaptiveThreshold(
                gray, 255, cv2.ADAPTIVE_THRESH_MEAN_C, cv2.THRESH_BINARY, 9, 9)
            color = cv2.bilateralFilter(img, 9, 300, 300)
            result = cv2.bitwise_and(color, color, mask=edges)
        elif filter_type == "grayscale":
            result = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        elif filter_type == "sketch":
            gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
            inv_gray = cv2.bitwise_not(gray)
            result = cv2.divide(gray, 255 - inv_gray, scale=256)
        elif filter_type == "invert":
            result = cv2.bitwise_not(img)
        elif filter_type == "edges":
            result = cv2.Canny(img, 100, 200)
        elif filter_type == "pencil_sketch":
            gray, color = cv2.pencilSketch(img, sigma_s=60, sigma_r=0.07, shade_factor=0.05)
            result = color  # Utilisez `gray` pour un effet en niveaux de gris
        elif filter_type == "oil_painting":
            result = cv2.xphoto.oilPainting(img, 7, 1)
        elif filter_type == "thermal":
            result = cv2.applyColorMap(img, cv2.COLORMAP_JET)
        elif filter_type == "pixel_art":
            small = cv2.resize(img, (32, 32), interpolation=cv2.INTER_LINEAR)
            result = cv2.resize(small, (img.shape[1], img.shape[0]), interpolation=cv2.INTER_NEAREST)
        elif filter_type == "cartoon_glow":
            gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
            edges = cv2.Canny(gray, 100, 200)
            edges_colored = cv2.applyColorMap(edges, cv2.COLORMAP_HOT)
            result = cv2.addWeighted(img, 0.8, edges_colored, 0.2, 0)
        else:
            raise ValueError("Filtre non valide sélectionné.")

        # Sauvegarder l'image transformée
        cv2.imwrite(output_path, result)

        # Afficher le résultat
        print(f"""
        <!DOCTYPE html>
        <html lang="fr">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Résultat de la Transformation</title>
            <style>
                body {{
                    font-family: 'Comic Sans MS', sans-serif;
                    background: linear-gradient(135deg, #ff9a9e, #fad0c4);
                    color: #333;
                    display: flex;
                    justify-content: center;
                    align-items: center;
                    height: 100vh;
                    margin: 0;
                }}
                .container {{
                    text-align: center;
                    background: #fff;
                    padding: 20px;
                    border-radius: 10px;
                    box-shadow: 0 4px 10px rgba(0, 0, 0, 0.2);
                    animation: pop 0.5s ease-in-out;
                }}
                h1 {{
                    font-size: 2.5rem;
                    color: #ff6f61;
                }}
                p {{
                    font-size: 1.2rem;
                }}
                img {{
                    margin-top: 20px;
                    max-width: 100%;
                    border-radius: 10px;
                    box-shadow: 0 4px 10px rgba(0, 0, 0, 0.2);
                }}
                a {{
                    display: inline-block;
                    margin-top: 20px;
                    padding: 10px 20px;
                    background-color: #ff6f61;
                    color: #fff;
                    text-decoration: none;
                    border-radius: 5px;
                    font-size: 1rem;
                    transition: background-color 0.3s ease;
                }}
                a:hover {{
                    background-color: #e74c3c;
                }}
                @keyframes pop {{
                    0% {{
                        transform: scale(0.8);
                        opacity: 0;
                    }}
                    100% {{
                        transform: scale(1);
                        opacity: 1;
                    }}
                }}
            </style>
        </head>
        <body>
            <div class="container">
                <h1>✨ Transformation Réussie ! ✨</h1>
                <p>Votre image avec le filtre "<strong>{filter_type}</strong>" est prête :</p>
                <img src="tmp/{filter_type}_{face_file.filename}" alt="Image transformée">
                <a href="tmp/{filter_type}_{face_file.filename}" download>Télécharger l'image</a>
            </div>
        </body>
        </html>
        """)
    except Exception as e:
        print(f"<h1>Erreur</h1><p>{str(e)}</p>")
else:
    print("<h1>Erreur</h1><p>Aucune image sélectionnée.</p>")