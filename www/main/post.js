// Sélectionner le formulaire et la boîte de résultat
const postForm = document.getElementById('postForm');
const postResult = document.getElementById('postResult');

// Ajouter un événement de soumission au formulaire
postForm.addEventListener('submit', (event) => {
    // event.preventDefault(); // Empêche le rechargement de la page

    // Récupérer les données du formulaire
    const formData = new FormData(postForm);
    const name = formData.get('name');
    const message = formData.get('message');

    // Créer un nouvel élément de message
    const newMessage = document.createElement('div');
    newMessage.classList.add('message');
    newMessage.innerHTML = `<strong>${name}</strong>: ${message}`;
    // Ajouter le message à la boîte de résultat
    postResult.appendChild(newMessage);
    // Réinitialiser le formulaire
    postForm.reset();
});