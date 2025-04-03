// Sélectionner le bouton et la boîte de résultat
const getButton = document.getElementById('getButton');
const getResult = document.getElementById('getResult');

// Ajouter un événement au clic sur le bouton
getButton.addEventListener('click', () => {
    // Simuler une requête GET
    getResult.innerHTML = '<p>Chargement...</p>';
    setTimeout(() => {
        getResult.innerHTML = '<p>Requête GET réussie ! Voici vos données : {"message": "Hello, Webserv!"}</p>';
    }, 2000);
});