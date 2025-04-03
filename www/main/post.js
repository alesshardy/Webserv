// Sélectionner le formulaire et la boîte de résultat
const postForm = document.getElementById('postForm');
const postResult = document.getElementById('postResult');

// Ajouter un événement de soumission au formulaire
postForm.addEventListener('submit', (event) => {
    event.preventDefault(); // Empêche le rechargement de la page

    // Récupérer les données du formulaire
    const formData = new FormData(postForm);
    const name = formData.get('name');
    const message = formData.get('message');

    // Simuler une requête POST
    postResult.innerHTML = '<p>Chargement...</p>';
    setTimeout(() => {
        postResult.innerHTML = `<p>Requête POST réussie ! Données envoyées : {"name": "${name}", "message": "${message}"}</p>`;
    }, 2000);
});